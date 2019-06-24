/*
 * Copyright (C) 2018-2019 LEIDOS.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */

#include "trajectory_executor/trajectory_executor.hpp"
#include <ros/ros.h>
#include <utility>
#include <cav_msgs/SystemAlert.h>

namespace trajectory_executor 
{
    cav_msgs::TrajectoryPlan trimFirstPoint(const cav_msgs::TrajectoryPlan plan) {
        cav_msgs::TrajectoryPlan out(plan);
        out.trajectory_points.erase(out.trajectory_points.begin());
        return out;
    }

    TrajectoryExecutor::TrajectoryExecutor(int traj_frequency) :
        _min_traj_publish_tickrate_hz(traj_frequency)
    {
    }

    TrajectoryExecutor::TrajectoryExecutor() :
        _min_traj_publish_tickrate_hz(10)
    {
    }

    std::map<std::string, std::string> TrajectoryExecutor::queryControlPlugins()
    {
        // Hard coded stub for MVP since plugin manager won't be developed yet
        // TODO: Query plugin manager to receive actual list of plugins and their corresponding topics
        ROS_DEBUG("Executing stub behavior for plugin discovery MVP...");
        std::map<std::string, std::string> out;

        // TODO: Replace with temporary path to wherever pure_pursuit is mapped
        out["pure_pursuit"] = "/carma/guidance/control_plugins/pure_pursuit/trajectory";
        return out;
    }
    
    void TrajectoryExecutor::onNewTrajectoryPlan(cav_msgs::TrajectoryPlan msg)
    {
        std::unique_lock<std::mutex> lock(_cur_traj_mutex); // Acquire lock until end of this function scope
        ROS_DEBUG("Received new trajectory plan!");
        ROS_DEBUG_STREAM("New Trajectory plan ID: " << msg.trajectory_id);
        ROS_DEBUG_STREAM("New plan contains " << msg.trajectory_points.size() << " points");

        _cur_traj = std::unique_ptr<cav_msgs::TrajectoryPlan>(new cav_msgs::TrajectoryPlan(msg));
        ROS_DEBUG_STREAM("Successfully swapped trajectories!");

        // TODO: Force traj emit
        // TODO: Reset timer state
    }

    void TrajectoryExecutor::onTrajEmitTick(const ros::TimerEvent& te)
    {
        std::unique_lock<std::mutex> lock(_cur_traj_mutex);
        ROS_DEBUG("TrajectoryExecutor tick start!");
        

        if (_cur_traj != nullptr) {
            if (!_cur_traj->trajectory_points.empty()) {
                if (_timesteps_since_last_traj > 0) {
                    _cur_traj = std::unique_ptr<cav_msgs::TrajectoryPlan>(new cav_msgs::TrajectoryPlan(trimFirstPoint(*_cur_traj)));
                }
                std::string control_plugin = _cur_traj->trajectory_points[0].controller_plugin_name;
                std::map<std::string, ros::Publisher>::iterator it = _traj_publisher_map.find(control_plugin);
                if (it != _traj_publisher_map.end()) {
                    ROS_DEBUG("Found match for control plugin %s at point %d in current trajectory!",
                        control_plugin.c_str(),
                        _timesteps_since_last_traj);
                    it->second.publish(*_cur_traj);
                } else {
                    ROS_ERROR("No match found for control plugin %s at point %d in current trajectory!",
                        control_plugin.c_str(),
                        _timesteps_since_last_traj);
                    cav_msgs::SystemAlert system_alert;
                    system_alert.type = system_alert.FATAL;
                    std::ostringstream description_builder;
                    description_builder << "No match found for control plugin " 
                        << control_plugin << " at point " 
                        << _timesteps_since_last_traj << " in current trajectory!";
                    system_alert.description = description_builder.str();
                    ROS_FATAL("Publishing FATAL error on system alert topic!");
                    _default_nh->publishSystemAlert(system_alert);
                }
                _timesteps_since_last_traj++;
            } else {
                    ROS_ERROR("Ran out of trajetory data to consume!");
                    cav_msgs::SystemAlert system_alert;
                    system_alert.type = system_alert.FATAL;
                    std::ostringstream description_builder;
                    description_builder << "Ran out of trajectory data to consume!";
                    system_alert.description = description_builder.str();
                    ROS_FATAL("Publishing FATAL error on system alert topic!");
                    _default_nh->publishSystemAlert(system_alert);
            }
        } else {
            ROS_DEBUG("Awaiting initial trajectory publication...");
        }
        ROS_DEBUG("TrajectoryExecutor tick completed succesfully!");
    }

    void TrajectoryExecutor::run()
    {
        ROS_DEBUG("Starting operations for TrajectoryExecutor component...");
        _timer = _timer_nh->createTimer(
            ros::Duration(ros::Rate(this->_min_traj_publish_tickrate_hz)),
            &TrajectoryExecutor::onTrajEmitTick, 
            this);

        ros::AsyncSpinner timer_spinner(1, &_timer_callbacks);
        timer_spinner.start();
        ROS_DEBUG("Timer thread started!");

        ros::AsyncSpinner msg_spinner(1, &_msg_callbacks);
        msg_spinner.start();

        ROS_DEBUG("TrajectoryExecutor component started succesfully! Starting to spin.");

        ros::Rate r(_default_spin_rate);
        while (ros::ok())
        {
            ros::spinOnce();
            r.sleep();
        }

        timer_spinner.stop();
        msg_spinner.stop();

        ros::shutdown();
    }

    bool TrajectoryExecutor::init()
    {
        ROS_DEBUG("Initializing TrajectoryExecutor node...");
    
        _default_nh = std::unique_ptr<ros::CARMANodeHandle>(new ros::CARMANodeHandle);
        _timer_nh = std::unique_ptr<ros::CARMANodeHandle>(new ros::CARMANodeHandle);
        _timer_nh->setCallbackQueue(&_timer_callbacks);
        _msg_nh = std::unique_ptr<ros::CARMANodeHandle>(new ros::CARMANodeHandle);
        _msg_nh->setCallbackQueue(&_msg_callbacks);
        ROS_DEBUG("Initialized all node handles");

        _default_nh->param("default_spin_rate", _default_spin_rate, 10);
        _default_nh->param("trajectory_publish_rate", _min_traj_publish_tickrate_hz, 10);

        this->_plan_sub = this->_msg_nh->subscribe<cav_msgs::TrajectoryPlan>("trajectory", 1000, &TrajectoryExecutor::onNewTrajectoryPlan, this);
        this->_cur_traj = std::unique_ptr<cav_msgs::TrajectoryPlan>();
        ROS_DEBUG("Subscribed to inbound trajectory plans.");

        ROS_DEBUG("Setting up publishers for control plugin topics...");

        std::map<std::string, ros::Publisher> control_plugin_topics;
        auto discovered_control_plugins = queryControlPlugins();
        for (auto it = discovered_control_plugins.begin(); it != discovered_control_plugins.end(); it++)
        {
            ROS_DEBUG("Trajectory executor discovered control plugin %s listening on topic %s.", it->first.c_str(), it->second.c_str());
            ros::Publisher control_plugin_pub = _msg_nh->advertise<cav_msgs::TrajectoryPlan>(it->second, 1000);
            control_plugin_topics.insert(std::make_pair(it->first, control_plugin_pub));
        }

        this->_traj_publisher_map = control_plugin_topics;
        ROS_DEBUG("TrajectoryExecutor component initialized succesfully!");

        return true;
    }
}

/*
 * Copyright (C) 2019 LEIDOS.
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

#include "autoware_plugin.h"
#include <gtest/gtest.h>
#include <ros/ros.h>

TEST(AutowarePluginTest, testGetWaypointsInTimeBoundary1)
{
    // compose a list of waypoints spanning 8 seconds
    std::vector<autoware_msgs::Waypoint> waypoints;
    autoware_msgs::Waypoint wp_1;
    wp_1.twist.twist.linear.x = 2.0;
    wp_1.pose.pose.position.x = 0.0;
    autoware_msgs::Waypoint wp_2;
    wp_2.twist.twist.linear.x = 4.0;
    wp_2.pose.pose.position.x = 6.0;
    autoware_msgs::Waypoint wp_3;
    wp_3.twist.twist.linear.x = 8.0;
    wp_3.pose.pose.position.x = 24.0;
    autoware_msgs::Waypoint wp_4;
    wp_4.twist.twist.linear.x = 8.0;
    wp_4.pose.pose.position.x = 40.0;
    autoware_msgs::Waypoint wp_5;
    wp_5.twist.twist.linear.x = 8.0;
    wp_5.pose.pose.position.x = 48.0;
    waypoints.push_back(wp_1);
    waypoints.push_back(wp_2);
    waypoints.push_back(wp_3);
    waypoints.push_back(wp_4);
    waypoints.push_back(wp_5);
    autoware_plugin::AutowarePlugin ap;
    std::vector<autoware_msgs::Waypoint> res = ap.get_waypoints_in_time_boundary(waypoints, 6.0);
    EXPECT_EQ(4, res.size());
    EXPECT_NEAR(2.0, res[0].twist.twist.linear.x, 0.01);
    EXPECT_NEAR(0.0, res[0].pose.pose.position.x, 0.01);
    EXPECT_NEAR(4.0, res[1].twist.twist.linear.x, 0.01);
    EXPECT_NEAR(6.0, res[1].pose.pose.position.x, 0.01);
    EXPECT_NEAR(8.0, res[2].twist.twist.linear.x, 0.01);
    EXPECT_NEAR(24.0, res[2].pose.pose.position.x, 0.01);
    EXPECT_NEAR(8.0, res.back().twist.twist.linear.x, 0.01);
    EXPECT_NEAR(40.0, res.back().pose.pose.position.x, 0.01);
}

TEST(AutowarePluginTest, testGetWaypointsInTimeBoundary2)
{
    // compose a list of waypoints spaning less than 6 seconds
    std::vector<autoware_msgs::Waypoint> waypoints;
    autoware_msgs::Waypoint wp_1;
    wp_1.twist.twist.linear.x = 2.0;
    wp_1.pose.pose.position.x = 0.0;
    autoware_msgs::Waypoint wp_2;
    wp_2.twist.twist.linear.x = 4.0;
    wp_2.pose.pose.position.x = 6.0;
    waypoints.push_back(wp_1);
    waypoints.push_back(wp_2);
    autoware_plugin::AutowarePlugin ap;
    std::vector<autoware_msgs::Waypoint> res = ap.get_waypoints_in_time_boundary(waypoints, 6.0);
    EXPECT_EQ(2, res.size());
    EXPECT_NEAR(2.0, res[0].twist.twist.linear.x, 0.01);
    EXPECT_NEAR(0.0, res[0].pose.pose.position.x, 0.01);
    EXPECT_NEAR(4.0, res[1].twist.twist.linear.x, 0.01);
    EXPECT_NEAR(6.0, res[1].pose.pose.position.x, 0.01);
}

TEST(AutowarePluginTest, testGetWaypointsInTimeBoundary3)
{
    // compose a list of waypoints spanning exactly 5 seconds
    std::vector<autoware_msgs::Waypoint> waypoints;
    autoware_msgs::Waypoint wp_1;
    wp_1.twist.twist.linear.x = 2.0;
    wp_1.pose.pose.position.x = 0.0;
    autoware_msgs::Waypoint wp_2;
    wp_2.twist.twist.linear.x = 4.0;
    wp_2.pose.pose.position.x = 6.0;
    autoware_msgs::Waypoint wp_3;
    wp_3.twist.twist.linear.x = 8.0;
    wp_3.pose.pose.position.x = 24.0;
    waypoints.push_back(wp_1);
    waypoints.push_back(wp_2);
    waypoints.push_back(wp_3);
    autoware_plugin::AutowarePlugin ap;
    std::vector<autoware_msgs::Waypoint> res = ap.get_waypoints_in_time_boundary(waypoints, 5.0);
    EXPECT_EQ(3, res.size());
    EXPECT_NEAR(2.0, res[0].twist.twist.linear.x, 0.01);
    EXPECT_NEAR(0.0, res[0].pose.pose.position.x, 0.01);
    EXPECT_NEAR(4.0, res[1].twist.twist.linear.x, 0.01);
    EXPECT_NEAR(6.0, res[1].pose.pose.position.x, 0.01);
    EXPECT_NEAR(8.0, res[2].twist.twist.linear.x, 0.01);
    EXPECT_NEAR(24.0, res[2].pose.pose.position.x, 0.01);
}

TEST(AutowarePluginTest, testCreateUnevenTrajectory1)
{
    // compose a list of waypoints, uneven spaced
    std::vector<autoware_msgs::Waypoint> waypoints;
    autoware_msgs::Waypoint wp_1;
    wp_1.twist.twist.linear.x = 2.0;
    wp_1.pose.pose.position.x = 0.0;
    autoware_msgs::Waypoint wp_2;
    wp_2.twist.twist.linear.x = 4.0;
    wp_2.pose.pose.position.x = 0.5;
    autoware_msgs::Waypoint wp_3;
    wp_3.twist.twist.linear.x = 2.0;
    wp_3.pose.pose.position.x = 1.3;
    autoware_msgs::Waypoint wp_4;
    wp_4.twist.twist.linear.x = 4.0;
    wp_4.pose.pose.position.x = 1.4;
    autoware_msgs::Waypoint wp_5;
    wp_5.twist.twist.linear.x = 4.0;
    wp_5.pose.pose.position.x = 2.0;
    waypoints.push_back(wp_1);
    waypoints.push_back(wp_2);
    waypoints.push_back(wp_3);
    waypoints.push_back(wp_4);
    waypoints.push_back(wp_5);
    autoware_plugin::AutowarePlugin ap;
    // create pose message to indicate that the current location is on top of the starting waypoint
    ap.pose_msg_.reset(new geometry_msgs::PoseStamped());
    std::vector<cav_msgs::TrajectoryPlanPoint> traj = ap.create_uneven_trajectory_from_waypoints(waypoints);
    EXPECT_EQ(5, traj.size());
    EXPECT_NEAR(0.0, traj[0].target_time, 0.01);
    EXPECT_NEAR(0.0, traj[0].x, 0.01);
    EXPECT_NEAR(0.1667, traj[1].target_time / 1e9, 0.001);
    EXPECT_NEAR(0.5, traj[1].x, 0.01);
    EXPECT_NEAR(0.4333, traj[2].target_time / 1e9, 0.001);
    EXPECT_NEAR(1.3, traj[2].x, 0.01);
    EXPECT_NEAR(0.4666, traj[3].target_time / 1e9, 0.001);
    EXPECT_NEAR(1.4, traj[3].x, 0.01);
    EXPECT_NEAR(0.6166, traj[4].target_time / 1e9, 0.001);
    EXPECT_NEAR(2.0, traj[4].x, 0.01);
}

TEST(AutowarePluginTest, testCreateUnevenTrajectory2)
{
    // compose a list of waypoints, uneven spaced
    std::vector<autoware_msgs::Waypoint> waypoints;
    autoware_msgs::Waypoint wp_1;
    wp_1.twist.twist.linear.x = 2.0;
    wp_1.pose.pose.position.x = 0.0;
    autoware_msgs::Waypoint wp_2;
    wp_2.twist.twist.linear.x = 4.0;
    wp_2.pose.pose.position.x = 0.5;
    autoware_msgs::Waypoint wp_3;
    wp_3.twist.twist.linear.x = 2.0;
    wp_3.pose.pose.position.x = 1.3;
    autoware_msgs::Waypoint wp_4;
    wp_4.twist.twist.linear.x = 4.0;
    wp_4.pose.pose.position.x = 1.4;
    autoware_msgs::Waypoint wp_5;
    wp_5.twist.twist.linear.x = 4.0;
    wp_5.pose.pose.position.x = 2.0;
    waypoints.push_back(wp_1);
    waypoints.push_back(wp_2);
    waypoints.push_back(wp_3);
    waypoints.push_back(wp_4);
    waypoints.push_back(wp_5);
    autoware_plugin::AutowarePlugin ap;
    // create pose message to indicate that the current location is not near the starting waypoint
    geometry_msgs::PoseStamped pose;
    pose.pose.position.x = -1.0;
    ap.pose_msg_.reset(new geometry_msgs::PoseStamped(pose));
    std::vector<cav_msgs::TrajectoryPlanPoint> traj = ap.create_uneven_trajectory_from_waypoints(waypoints);
    EXPECT_EQ(6, traj.size());
    EXPECT_NEAR(0.0, traj[0].target_time / 1e9, 0.01);
    EXPECT_NEAR(-1.0, traj[0].x, 0.01);
    EXPECT_NEAR(1.0, traj[1].target_time / 1e9, 0.01);
    EXPECT_NEAR(0.0, traj[1].x, 0.01);
    EXPECT_NEAR(1.1667, traj[2].target_time / 1e9, 0.001);
    EXPECT_NEAR(0.5, traj[2].x, 0.01);
    EXPECT_NEAR(1.4333, traj[3].target_time / 1e9, 0.001);
    EXPECT_NEAR(1.3, traj[3].x, 0.01);
    EXPECT_NEAR(1.4666, traj[4].target_time / 1e9, 0.001);
    EXPECT_NEAR(1.4, traj[4].x, 0.01);
    EXPECT_NEAR(1.6166, traj[5].target_time / 1e9, 0.001);
    EXPECT_NEAR(2.0, traj[5].x, 0.01);
}

TEST(AutowarePluginTest, testEvenTrajectory)
{
    // compose a trajectory
    std::vector<cav_msgs::TrajectoryPlanPoint> uneven_traj;
    cav_msgs::TrajectoryPlanPoint tp_1;
    tp_1.target_time = 0.0;
    tp_1.x = 0.0;
    cav_msgs::TrajectoryPlanPoint tp_2;
    tp_2.target_time = 0.1667 * 1e9;
    tp_2.x = 0.5;
    cav_msgs::TrajectoryPlanPoint tp_3;
    tp_3.target_time = 0.4333 * 1e9;
    tp_3.x = 1.3;
    cav_msgs::TrajectoryPlanPoint tp_4;
    tp_4.target_time = 0.4666 * 1e9;
    tp_4.x = 1.4;
    cav_msgs::TrajectoryPlanPoint tp_5;
    tp_5.target_time = 0.6166 * 1e9;
    tp_5.x = 2.0;
    uneven_traj.push_back(tp_1);
    uneven_traj.push_back(tp_2);
    uneven_traj.push_back(tp_3);
    uneven_traj.push_back(tp_4);
    uneven_traj.push_back(tp_5);
    autoware_plugin::AutowarePlugin ap;
    std::vector<cav_msgs::TrajectoryPlanPoint> res = ap.even_trajectory(uneven_traj, 0.1);
    EXPECT_EQ(7, res.size());
    EXPECT_NEAR(0.0, res[0].target_time / 1e9, 0.001);
    EXPECT_NEAR(0.0, res[0].x, 0.001);
    EXPECT_NEAR(0.1, res[1].target_time / 1e9, 0.001);
    EXPECT_NEAR(0.2999, res[1].x, 0.01);
    EXPECT_NEAR(0.2, res[2].target_time / 1e9, 0.001);
    EXPECT_NEAR(0.5999, res[2].x, 0.01);
    EXPECT_NEAR(0.3, res[3].target_time / 1e9, 0.001);
    EXPECT_NEAR(0.9, res[3].x, 0.01);
    EXPECT_NEAR(0.4, res[4].target_time / 1e9, 0.001);
    EXPECT_NEAR(1.2001, res[4].x, 0.01);
    EXPECT_NEAR(0.5, res[5].target_time / 1e9, 0.001);
    EXPECT_NEAR(1.5336, res[5].x, 0.01);
    EXPECT_NEAR(0.6, res[6].target_time / 1e9, 0.001);
    EXPECT_NEAR(1.9336, res[6].x, 0.01);
}

TEST(AutowarePluginTest, testEvenTrajectory2)
{
    // compose a trajectory
    std::vector<cav_msgs::TrajectoryPlanPoint> uneven_traj;
    cav_msgs::TrajectoryPlanPoint tp_1;
    tp_1.target_time = 0.0;
    tp_1.x = 84.8083;
    tp_1.y = -40.2181;
    cav_msgs::TrajectoryPlanPoint tp_2;
    tp_2.target_time = 253700845;
    tp_2.x = 84.8916;
    tp_2.y = -39.4143;
    cav_msgs::TrajectoryPlanPoint tp_3;
    tp_3.target_time = 836997889;
    tp_3.x = 86.5672;
    tp_3.y = -40.2168;
    uneven_traj.push_back(tp_1);
    uneven_traj.push_back(tp_2);
    uneven_traj.push_back(tp_3);
    autoware_plugin::AutowarePlugin ap;
    std::vector<cav_msgs::TrajectoryPlanPoint> res = ap.even_trajectory(uneven_traj, 0.1);
    for(int i = 1; i < res.size(); ++i)
    {
        cav_msgs::TrajectoryPlanPoint tpp = res[i - 1]; 
        cav_msgs::TrajectoryPlanPoint tpp2 = res[i];
        double delta_x = tpp.x - tpp2.x;
        double delta_y = tpp.y - tpp2.y;
        double delta_pos = sqrt(delta_x * delta_x + delta_y * delta_y);
        double delta_t_second = (double)abs(tpp2.target_time - tpp.target_time) / 1e9;
        double speed = delta_pos / delta_t_second;
        std::cerr << "speed" << i << " : " << speed << std::endl;
    }

}


// Run all the tests
int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}



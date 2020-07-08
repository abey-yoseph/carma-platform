/*
 * Copyright (C) 2020 LEIDOS.
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

#include <functional>
#include <mutex>
#include <carma_wm_ctrl/WMBroadcaster.h>
#include <carma_wm_ctrl/MapConformer.h>
#include <lanelet2_extension/utility/message_conversion.h>
#include <lanelet2_extension/projection/local_frame_projector.h>
#include <lanelet2_core/primitives/Lanelet.h>
#include <lanelet2_core/geometry/Lanelet.h>
#include <type_traits>

namespace carma_wm_ctrl
{
using std::placeholders::_1;

WMBroadcaster::WMBroadcaster(PublishMapCallback map_pub, std::unique_ptr<TimerFactory> timer_factory)
  : map_pub_(map_pub), scheduler_(std::move(timer_factory))
{
  scheduler_.onGeofenceActive(std::bind(&WMBroadcaster::addGeofence, this, _1));
  scheduler_.onGeofenceInactive(std::bind(&WMBroadcaster::removeGeofence, this, _1));
};

void WMBroadcaster::baseMapCallback(const autoware_lanelet2_msgs::MapBinConstPtr& map_msg)
{
  std::lock_guard<std::mutex> guard(map_mutex_);

  static bool firstCall = true;
  // This function should generally only ever be called one time so log a warning if it occurs multiple times
  if (firstCall)
  {
    firstCall = false;
    ROS_INFO("WMBroadcaster::baseMapCallback called for first time with new map message");
  }
  else
  {
    ROS_WARN("WMBroadcaster::baseMapCallback called multiple times in the same node");
  }

  lanelet::LaneletMapPtr new_map(new lanelet::LaneletMap);

  lanelet::utils::conversion::fromBinMsg(*map_msg, new_map);

  base_map_ = new_map;  // Store map

  lanelet::MapConformer::ensureCompliance(base_map_);  // Update map to ensure it complies with expectations

  // Publish map
  autoware_lanelet2_msgs::MapBin compliant_map_msg;
  lanelet::utils::conversion::toBinMsg(base_map_, &compliant_map_msg);
  map_pub_(compliant_map_msg);
};

/*TODO Replace geofence parameter with message type here once defined*/
void WMBroadcaster::geofenceCallback(const Geofence& gf)
{
  std::lock_guard<std::mutex> guard(map_mutex_);

  scheduler_.addGeofence(gf);  // Add the geofence to the schedule
  ROS_INFO_STREAM("New geofence message received by WMBroadcaster with id" << gf.id_);
};

void WMBroadcaster::addGeofence(const Geofence& gf)
{
  std::lock_guard<std::mutex> guard(map_mutex_);
  ROS_INFO_STREAM("Adding active geofence to the map with geofence id: " << gf.id_);
  // TODO Add implementation for adding a geofence
};

void WMBroadcaster::removeGeofence(const Geofence& gf)
{
  std::lock_guard<std::mutex> guard(map_mutex_);
  ROS_INFO_STREAM("Removing inactive geofence to the map with geofence id: " << gf.id_);
  // TODO Add implementation for removing a geofence
};
  
void  WMBroadcaster::routeCallbackMessage(const cav_msgs::RouteConstPtr& route_msg)
{
  auto path = lanelet::ConstLanelets(); 
  for(auto id : route_msg->route_path_lanelet_ids) 
  {
    auto laneLayer = world_model_->getMap()->laneletLayer.get(id);
    path.push_back(laneLayer);
  }
  if(path.size() == 0) return;
  
   /*logic to determine route bounds*/
  std::vector<lanelet::Lanelet> llt; 
  std::vector<BoundingBox> pathBox; 
  float minX = 99999;
  float minY = 99999;
  float minZ = 99999;
  float maxX = 0;
  float maxY = 0;
  float maxZ = 0;

  while (path.size() != 0) //Continue until there are no more lanelet elements in path
  {
      llt.push_back(path.back); //Add a lanelet to the vector
      pathBox.push_back(geometry::boundingBox2d(llt.back)); //Create a bounding box of the added lanelet and add it to the vector

      if (pathBox.back().BottomLeft.x() < minX) 
         minX = pathBox.back().BottomLeft.x(); //minimum x-value
    
      if (pathBox.back().BottomLeft.y() < minY) 
         minY = pathBox.back().BottomLeft.y(); //minimum y-value
  
      if (pathBox.back().BottomLeft.z() < minZ) 
         minZ = pathBox.back().BottomLeft.z(); //minimum z-value

      if (pathBox.back().TopRight.x() > maxX)
         maxX = pathBox.back().TopRight.x(); //maximum x-value

      if (pathBox.back().TopRight.y() > maxY)
         maxY = pathBox.back().TopRight.y(); //maximum y-value
  
      if (pathBox.back().TopRight.z() > maxZ)
         maxZ = pathBox.back().TopRight.z(); //maximum Z-value

      path.pop_back(); //remove the added lanelet from path an reduce pack.size() by 1
  }


  lanelet::projection::MGRSProjector projector;
  lanelet::BasicPoint3d localRoute;

  localRoute.x()= minX;
  localRoute.y()= minY;
  localRoute.z()= minZ; 

  lanelet::GPSPoint gpsRoute = projector.reverse(localRoute); //If the appropriate library is included, the reverse() function can be used instead of making a new one

  cav_msgs::ControlRequest cR; /*Fill the latitude value in message cB with the value of lat */
  cav_msgs::ControlBounds cB; /*Fill the longitude value in message cB with the value of lon*/

  cB.latitude = gpsRoute.lat;
  cB.longitude = gpsRoute.lon;

  msg_callBack.publish(cR); /*Publish the message containing the route info (latitude & longitude) this line is a placeholder since 
                            publishing is handled by WMBroadcasterNode.h*/

  ros::Timer timer = nh.createTimer(ros::Duration(0.1), routeCallbackMessage);/*Sleep for 10 seconds before making another request*/
}

}  // namespace carma_wm_ctrl

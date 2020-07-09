#pragma once

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
#include <lanelet2_core/LaneletMap.h>
#include <autoware_lanelet2_msgs/MapBin.h>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/date_defs.hpp>
#include <boost/icl/interval_set.hpp>
#include <unordered_set>
#include "ros/ros.h"
#include <lanelet2_core/LaneletMap.h>
#include <lanelet2_core/geometry/Lanelet.h>
#include <lanelet2_core/primitives/Lanelet.h>
#include <autoware_lanelet2_msgs/MapBin.h>
#include <lanelet2_extension/utility/message_conversion.h>
#include <lanelet2_extension/projection/local_frame_projector.h>
#include <carma_wm_ctrl/GeofenceScheduler.h>
#include <lanelet2_core/geometry/BoundingBox.h>
#include <lanelet2_core/primitives/BoundingBox.h>
#include <carma_wm/WMListener.h>
#include <carma_wm/WorldModel.h>
#include <cav_msgs/Route.h>
#include <cav_msgs/ControlRequest.h>
#include <cav_msgs/ControlBounds.h>
#include <lanelet2_extension/projection/mgrs_projector.h>
#include <autoware_lanelet2_msgs/MapBin.h>
#include "MapConformer.h"

namespace carma_wm_ctrl
{
/*!
 * \brief Class which provies exposes map publication and carma_wm update logic
 *
 * The WMBroadcaster handles updating the lanelet2 base map and publishing the new versions to the rest of the CARMA
 * Platform ROS network. The broadcaster also provides functions for adding or removing geofences from the map and
 * notifying the rest of the system.
 *
 */
class WMBroadcaster
{
public:
  using PublishMapCallback = std::function<void(const autoware_lanelet2_msgs::MapBin&)>;

  /*!
   * \brief Constructor
   */
  WMBroadcaster(PublishMapCallback map_pub, std::unique_ptr<TimerFactory> timer_factory);

  /*!
   * \brief Callback to set the base map when it has been loaded
   *
   * \param map_msg The map message to use as the base map
   *
   */
  void baseMapCallback(const autoware_lanelet2_msgs::MapBinConstPtr& map_msg);

  /*!
   * \brief Callback to add a geofence to the map
   *
   * \param geofence_msg The geofence to add. TODO replace with actual message type once defined
   */
  void geofenceCallback(const Geofence& gf);

  /*!
   * \brief Adds a geofence to the current map
   */
  void addGeofence(const Geofence& gf);

  /*!
   * \brief Removes a geofence from the current map
   */
  void removeGeofence(const Geofence& gf);
  
  /*!
  * \brief Pulls vehicle information from CARMA Cloud at startup by providing its selected route in a ControlRequest message that is published after a route is selected.
  * During operation at ~10s intervals the vehicle will make another control request for the remainder of its route.
  * \param route_msg The message containing route information
  */
  void routeCallbackMessage(const cav_msgs::RouteConstPtr& route_msg);


private:
  lanelet::LaneletMapPtr base_map_;
  lanelet::LaneletMapPtr current_map_;
  std::vector<lanelet::LaneletMapPtr> cached_maps_;
  std::mutex map_mutex_;
  PublishMapCallback map_pub_;
  GeofenceScheduler scheduler_;
};
}  // namespace carma_wm_ctrl

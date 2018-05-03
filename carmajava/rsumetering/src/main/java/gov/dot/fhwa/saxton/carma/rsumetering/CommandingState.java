/*
 * Copyright (C) 2018 LEIDOS.
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

package gov.dot.fhwa.saxton.carma.rsumetering;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import cav_msgs.MobilityOperation;
import cav_msgs.MobilityRequest;
import cav_msgs.MobilityResponse;
import gov.dot.fhwa.saxton.carma.rosutils.SaxtonLogger;

/**
 * In this state the merging vehicle is being directly commanded by the rsu
 * At the moment the logic is very simple.
 * Set the target vehicle speed to the platoon speed and activate the lane change indicator when needed
 */
public class CommandingState extends RSUMeteringStateBase {
  protected final static String EXPECTED_OPERATION_PARAMS = "STATUS|METER_DIST:%.2f,MERGE_DIST:%.2f,SPEED:%.2f,LANE:%d";
  protected final static String STATUS_TYPE_PARAM = "STATUS";
  protected final static List<String> OPERATION_PARAMS = new ArrayList<>(Arrays.asList("METER_DIST", "MERGE_DIST", "SPEED", "LANE"));
  protected final double vehLagTime;
  protected final double vehMaxAccel;
  protected final String vehicleId;
  protected final String planId;
  protected double distToMerge;

  /**
   * Constructor
   * 
   * @param worker The worker being represented by this state
   * @param log A logger
   * @param vehicleId The static id of the vehicle being controlled
   * @param vehLagTime The lag time of the controlled vehicle's response
   * @param vehMaxAccel The maximum acceleration limit allowed by the controlled vehicle
   * @param distToMerge The distance to the merge point of the controlled vehicle. This value can be negative
   */
  public CommandingState(RSUMeterWorker worker, SaxtonLogger log, String vehicleId, String planId,
   double vehLagTime, double vehMaxAccel, double distToMerge) {
    super(worker, log, worker.getCommandPeriod());
    this.vehLagTime = vehLagTime;
    this.vehMaxAccel = vehMaxAccel;
    this.distToMerge = distToMerge;
    this.vehicleId = vehicleId;
    this.planId = planId;
  }

  @Override
  public boolean onMobilityRequestMessage(MobilityRequest msg) {
    // Do nothing. We should not be getting requests in this state
    return false;
  }

  @Override
  public void onMobilityOperationMessage(MobilityOperation msg) {

    // Check this message is for the current merge plan
    if (!msg.getHeader().getSenderId().equals(vehicleId)
     || !msg.getHeader().getPlanId().equals(planId)) {
      return;
    }
    // Extract params
    List<String> params;
    try {
      params = worker.extractStrategyParams(msg.getStrategyParams(), STATUS_TYPE_PARAM, OPERATION_PARAMS);
    } catch (IllegalArgumentException e) {
      log.warn("Received operation message with bad params. Exception: " + e);
      return;
    }

    double meterDist = Double.parseDouble(params.get(0));
    double mergeDist = Double.parseDouble(params.get(1));
    double speed = Double.parseDouble(params.get(2));
    int lane = Integer.parseInt(params.get(3));
    // Simply updating the command speed to the platoon speed may be enough to make this work
    // If it is not more complex logic can be added
    PlatoonData platoon = worker.getNextPlatoon();
    
    // Target steering command
    double targetSteer = 0;

    // If we are not in our target lane and we are in the merge area, apply steering command
    if (lane != worker.getTargetLane() && mergeDist < 0 && mergeDist < worker.getMergeLength()) {
      // With fake lateral control a positive value results in right lane change and negative in left lane change
      targetSteer = lane - worker.getTargetLane();
    }
    // Update vehicle commands
    updateCommands(platoon.getSpeed(), vehMaxAccel, targetSteer);
  }

  @Override
  public void onMobilityResponseMessage(MobilityResponse msg) {

    if (!msg.getHeader().getSenderId().equals(vehicleId)
      || !msg.getHeader().getPlanId().equals(planId)) {
        return;
    }

    if (!msg.getIsAccepted()) {
      log.warn("NACK received from vehicle: " + vehicleId + " for plan: " + planId);
      worker.setState(new StandbyState(worker, log));
    }
  }

  @Override
  protected void onLoop() {
    publishSpeedCommand(vehicleId, planId);
  }
}
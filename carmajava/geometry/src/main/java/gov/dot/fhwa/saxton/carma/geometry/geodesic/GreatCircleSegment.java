/*
 * TODO: Copyright (C) 2017 LEIDOS.
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

package gov.dot.fhwa.saxton.carma.geometry.geodesic;

/**
 * Represents a line between two locations.
 * It is not necessarily straight in the traditional sense as the distance is calculated along the surface of the earth according to a curved earth model.
 * TODO: Implement this class. Currently just a placeholder to allow other classes to compile.
 */
public class GreatCircleSegment {
  protected Location loc1;
  protected Location loc2;
  /**
   * Constructor initializes this earth segment with the provided locations
   * @param loc1 First gps location
   * @param loc2 Second gps location
   */
  public GreatCircleSegment(Location loc1, Location loc2) {
    this.loc1 = loc1;
    this.loc2 = loc2;
  }
}

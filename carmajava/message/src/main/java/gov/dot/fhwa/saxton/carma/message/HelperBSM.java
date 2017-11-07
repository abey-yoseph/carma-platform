package gov.dot.fhwa.saxton.carma.message;

import java.util.Arrays;

import cav_msgs.*;

/**
 * This is the helper class for encoding BSM.
 * All fields' unit in this class match the units in J2735 message.
 */
public class HelperBSM {
	
	protected static final int UINT8_MAX = 255;
	protected static final int MSG_COUNT_MAX = 127;
	protected static final int MSG_COUNT_MIN = 0;
	protected static final int ID_MAX = 255;
	protected static final int ID_MIN = 0;
	protected static final int DSECOND_MAX = 65535;
	protected static final int DSECOND_MIN = 0;
	protected static final int LATITUDE_UNAVAILABLE = 900000001;
	protected static final int LATITUDE_MAX = 900000000;
	protected static final int LATITUDE_MIN = -900000000;
	protected static final int LONGITUDE_UNAVAILABLE = 1800000001;
	protected static final int LONGITUDE_MAX = 1800000000;
	protected static final int LONGITUDE_MIN = -1799999999;
	protected static final int ELEVATION_UNAVAILABLE = -4096;
	protected static final int ELEVATION_MAX = 61439;
	protected static final int ELEVATION_MIN = -4095;
	protected static final int ACCURACY_UNAVAILABLE = 255;
	protected static final int ACCURACY_MAX = 254;
	protected static final int ACCURACY_MIN = 0;
	protected static final int ACCURACY_ORIENTATION_UNAVAILABLE = 65535;
	protected static final int ACCURACY_ORIENTATION_MAX = 65534;
	protected static final int ACCURACY_ORIENTATION_MIN = 0;
	protected static final int TRANSMISSION_UNAVAILABLE = 7;
	protected static final int SPEED_UNAVAILABLE = 8191;
	protected static final int SPEED_MAX = 8190;
	protected static final int SPEED_MIN = 0;
	protected static final int HEADING_UNAVAILABLE = 28800;
	protected static final int HEADING_MAX = 28799;
	protected static final int HEADING_MIN = 0;
	protected static final int STEER_WHEEL_ANGLE_UNAVAILABLE = 127;
	protected static final int STEER_WHEEL_ANGLE_MAX = 126;
	protected static final int STEER_WHEEL_ANGLE_MIN = -126;
	protected static final int ACCELERATION_UNAVAILABLE = 2001;
	protected static final int ACCELERATION_MAX = 2000;
	protected static final int ACCELERATION_MIN = -2000;
	protected static final int ACCELERATION_VERTICAL_UNAVAILABLE = -127;
	protected static final int ACCELERATION_VERTICAL_MAX = 127;
	protected static final int ACCELERATION_VERTICAL_MIN = -126;
	protected static final int YAWRATE_UNAVAILABLE = 0;
	protected static final int YAWRATE_MAX = 32767;
	protected static final int YAWRATE_MIN = -32767;
	protected static final int BRAKES_STATUS_UNAVAILABLE = 16;
	protected static final int BRAKES_NOT_APPLIED = 0;
	protected static final int BRAKES_APPLIED = 15;
	protected static final int VEHICLE_WIDTH_MAX = 1023;
	protected static final int VEHICLE_LENGTH_MAX = 4095;
	protected static final int VEHICLE_SIZE_UNAVAILABLE = 0;
	protected static final int VEHICLE_SIZE_MIN = 1;
	
	private int msgCnt = MSG_COUNT_MIN;
	private int[] id = {ID_MIN, ID_MIN, ID_MIN, ID_MIN};
	private int secMark = DSECOND_MIN;
	private int lat = LATITUDE_UNAVAILABLE;
	private int lon = LONGITUDE_UNAVAILABLE;
	private int elev = ELEVATION_UNAVAILABLE;
	private int[] accuracy = {ACCURACY_UNAVAILABLE, ACCURACY_UNAVAILABLE, ACCURACY_ORIENTATION_UNAVAILABLE};
	private int transmission = TransmissionState.UNAVAILABLE;
	private int speed = SPEED_UNAVAILABLE;
	private int heading = HEADING_UNAVAILABLE;
	private int angle = STEER_WHEEL_ANGLE_UNAVAILABLE;
	private int[] acceleration = {ACCELERATION_UNAVAILABLE, ACCELERATION_UNAVAILABLE, ACCELERATION_VERTICAL_UNAVAILABLE, YAWRATE_UNAVAILABLE};
	private int wheel_brakes = BRAKES_STATUS_UNAVAILABLE;
	private int traction = TractionControlStatus.UNAVAILABLE;
	private int abs = AntiLockBrakeStatus.UNAVAILABLE;
	private int scs = StabilityControlStatus.UNAVAILABLE;
	private int bba = BrakeBoostApplied.UNAVAILABLE;
	private int aux = AuxiliaryBrakeStatus.UNAVAILABLE;
	private int[] vehicle_size = {VEHICLE_SIZE_UNAVAILABLE, VEHICLE_SIZE_UNAVAILABLE};
	
	/**
	 * This is the constructor for HelperBSM.
	 * @param bsm_core Take ros message as the input and set all fields in HelperBSM after necessary validations
	 */
	public HelperBSM(BSMCoreData bsm_core) {
		this.setMsgCnt(bsm_core.getMsgCount());
		byte[] temp_ID = new byte[4];
		for(int i = 0; i < bsm_core.getId().capacity(); i++) {
			temp_ID[i] = bsm_core.getId().getByte(i);
		}
		this.setId(temp_ID);
		this.setSecMark(bsm_core.getSecMark());
		this.setLat(bsm_core.getLatitude());
		this.setLon(bsm_core.getLongitude());
		this.setElev(bsm_core.getElev());
		this.setAccuracy(bsm_core.getAccuracy().getSemiMajor(), bsm_core.getAccuracy().getSemiMinor(), bsm_core.getAccuracy().getOrientation());
		this.setTransmission(bsm_core.getTransmission().getTransmissionState());
		this.setSpeed(bsm_core.getSpeed());
		this.setHeading(bsm_core.getHeading());
		this.setAngle(bsm_core.getAngle());
		this.setAcceleration(bsm_core.getAccelSet().getLateral(), bsm_core.getAccelSet().getLongitudinal(),
				bsm_core.getAccelSet().getVert(), bsm_core.getAccelSet().getYawRate());
		this.setWheel_brakes(bsm_core.getBrakes().getWheelBrakes().getBrakeAppliedStatus());
		this.setTraction(bsm_core.getBrakes().getTraction().getTractionControlStatus());
		this.setAbs(bsm_core.getBrakes().getAbs().getAntiLockBrakeStatus());
		this.setScs(bsm_core.getBrakes().getScs().getStabilityControlStatus());
		this.setBba(bsm_core.getBrakes().getBrakeBoost().getBrakeBoostApplied());
		this.setAux(bsm_core.getBrakes().getAuxBrakes().getAuxiliaryBrakeStatus());
		this.setVehicle_size(bsm_core.getSize().getVehicleWidth(), bsm_core.getSize().getVehicleLength());
	}

	protected void setMsgCnt(byte msgCnt_input) {
		if(msgCnt_input >= MSG_COUNT_MIN && msgCnt_input <= MSG_COUNT_MAX) {
			this.msgCnt = msgCnt_input;
		}
	}

	protected void setId(byte[] id_input) {
		for(int i = 0; i < this.id.length; i++) {
			int temp_id;
			if(id_input[i] < 0) {
				temp_id = 1 + UINT8_MAX + id_input[i]; 
			} else {
				temp_id = id_input[i];
			}
			if(temp_id >= ID_MIN && temp_id <= ID_MAX) {
				this.id[i] = temp_id;
			}
		}
	}

	protected void setSecMark(int secMark_input) {
		if(secMark_input >= DSECOND_MIN && secMark_input <= DSECOND_MAX) {
			this.secMark = secMark_input;
		}
	}

	protected void setLat(double lat_input) {
		if(lat_input == BSMCoreData.LATITUDE_UNAVAILABLE) {
			return;
		}
		int integer_lat_input = (int) (lat_input * 10000000);
		if(integer_lat_input >= LATITUDE_MIN && integer_lat_input <= LATITUDE_MAX) {
			this.lat = integer_lat_input;
		}
	}

	protected void setLon(double lon_input) {
		if(lon_input == BSMCoreData.LONGITUDE_UNAVAILABLE) {
			return;
		}
		int integer_lon_input = (int) (lon_input * 10000000);
		if(integer_lon_input >= LONGITUDE_MIN && integer_lon_input <= LONGITUDE_MAX) {
			this.lon = integer_lon_input;
		}
		
	}

	protected void setElev(float elev_input) {
		if(elev_input == BSMCoreData.ELEVATION_UNAVAILABLE) {
			return;
		}
		int integer_elev_input = (int) (elev_input * 10);
		if(integer_elev_input >= ELEVATION_MIN && integer_elev_input <= ELEVATION_MAX) {
			this.elev = integer_elev_input;
		}
	}

	protected void setAccuracy(float semimajor_input, float semiminor_input, double orientation_input) {
		if(semimajor_input != PositionalAccuracy.ACCURACY_UNAVAILABLE) {
			int integer_semimajor_input = (int) (semimajor_input * 20);
			if(integer_semimajor_input >= ACCURACY_MAX) {
				this.accuracy[0] = ACCURACY_MAX;
			} else if(integer_semimajor_input >= ACCURACY_MIN) {
				this.accuracy[0] = integer_semimajor_input;
			}
		}
		if(semiminor_input != PositionalAccuracy.ACCURACY_UNAVAILABLE) {
			int integer_semiminor_input = (int) (semiminor_input * 20);
			if(integer_semiminor_input >= ACCURACY_MAX) {
				this.accuracy[1] = ACCURACY_MAX;
			} else if(integer_semiminor_input >= ACCURACY_MIN) {
				this.accuracy[1] = integer_semiminor_input;
			}
		}
		if(orientation_input != PositionalAccuracy.ACCURACY_ORIENTATION_UNAVAILABLE) {
			int integer_orientation_input = (int) (orientation_input / 0.0054932479);
			if(integer_orientation_input >= ACCURACY_ORIENTATION_MIN && integer_orientation_input <= ACCURACY_ORIENTATION_MAX) {
				this.accuracy[2] = integer_orientation_input;
			}
		}
	}

	protected void setTransmission(byte transmission_input) {
		this.transmission = transmission_input;
	}

	protected void setSpeed(float speed_input) {
		if(speed_input == BSMCoreData.SPEED_UNAVAILABLE) {
			return;
		}
		int integer_speed_input = (int) (speed_input * 50);
		if(integer_speed_input >= SPEED_MIN && integer_speed_input <= SPEED_MAX) {
			this.speed = integer_speed_input;
		}
	}

	protected void setHeading(float heading_input) {
		if(heading_input == BSMCoreData.HEADING_UNAVAILABLE) {
			return;
		}
		int integer_heading_input = (int) (heading_input * 80);
		if(integer_heading_input >= HEADING_MIN && integer_heading_input <= HEADING_MAX) {
			this.heading = integer_heading_input;
		}
	}

	protected void setAngle(float angle_input) {
		if(angle_input == BSMCoreData.STEER_WHEEL_ANGLE_UNAVAILABLE) {
			return;
		}
		int integer_angle_input = (int) (angle_input / 1.5);
		if(integer_angle_input >= STEER_WHEEL_ANGLE_MAX) {
			this.angle = STEER_WHEEL_ANGLE_MAX; 
		} else if(integer_angle_input <= STEER_WHEEL_ANGLE_MIN) {
			this.angle = STEER_WHEEL_ANGLE_MIN;
		} else {
			this.angle = integer_angle_input;
		}
	}

	protected void setAcceleration(float acceleration_lat_input, float acceleration_lon_input, float acceleration_vert_input, float yaw_rate_input) {
		if(acceleration_lat_input != AccelerationSet4Way.ACCELERATION_UNAVAILABLE) {
			int integer_acceleration_lat_input = (int) (acceleration_lat_input * 100);
			if(integer_acceleration_lat_input >= ACCELERATION_MAX) {
				this.acceleration[0] = ACCELERATION_MAX;
			} else if(integer_acceleration_lat_input <= ACCELERATION_MIN) {
				this.acceleration[0] = ACCELERATION_MIN;
			} else {
				this.acceleration[0] = integer_acceleration_lat_input;
			}
		}
		if(acceleration_lon_input != AccelerationSet4Way.ACCELERATION_UNAVAILABLE) {
			int integer_acceleration_lon_input = (int) (acceleration_lon_input * 100);
			if(integer_acceleration_lon_input >= ACCELERATION_MAX) {
				this.acceleration[1] = ACCELERATION_MAX;
			} else if(integer_acceleration_lon_input <= ACCELERATION_MIN) {
				this.acceleration[1] = ACCELERATION_MIN;
			} else {
				this.acceleration[1] = integer_acceleration_lon_input;
			}
		}
		if(acceleration_vert_input != AccelerationSet4Way.ACCELERATION_VERTICAL_UNAVAILABLE) {
			int integer_acceleration_vert_input = (int) (acceleration_vert_input / (0.02 * 9.807));
			if(integer_acceleration_vert_input >= ACCELERATION_VERTICAL_MAX) {
				this.acceleration[2] = ACCELERATION_VERTICAL_MAX;
			} else if(integer_acceleration_vert_input <= ACCELERATION_VERTICAL_MIN) {
				this.acceleration[2] = ACCELERATION_VERTICAL_MIN;
			} else {
				this.acceleration[2] = integer_acceleration_vert_input;
			}
		}
		if(yaw_rate_input != AccelerationSet4Way.YAWRATE_UNAVAILABLE) {
			int integer_yaw_rate_input = (int) (yaw_rate_input * 100);
			if(integer_yaw_rate_input >= YAWRATE_MIN && integer_yaw_rate_input <= YAWRATE_MAX) {
				this.acceleration[3] = integer_yaw_rate_input; 
			}
		}
	}

	protected void setWheel_brakes(byte wheel_brakes_input) {
		if(wheel_brakes_input == BRAKES_APPLIED) {
			this.wheel_brakes = BRAKES_APPLIED;
		} else if(wheel_brakes_input == BRAKES_NOT_APPLIED) {
			this.wheel_brakes = BRAKES_NOT_APPLIED;
		}
	}

	protected void setTraction(byte traction_input) {
		this.traction = traction_input;
	}

	protected void setAbs(byte abs_input) {
		this.abs = abs_input;
	}

	protected void setScs(byte scs_input) {
		this.scs = scs_input;
	}

	protected void setBba(byte bba_input) {
		this.bba = bba_input;
	}

	protected void setAux(byte aux_input) {
		this.aux = aux_input;
	}

	protected void setVehicle_size(float vehicle_width_input, float vehicle_length_input) {
		if(vehicle_width_input != VehicleSize.VEHICLE_WIDTH_UNAVAILABLE) {
			int integer_vehicle_width_input = (int) (vehicle_width_input * 100);
			if(integer_vehicle_width_input >= VEHICLE_SIZE_MIN && integer_vehicle_width_input <= VEHICLE_WIDTH_MAX) {
				this.vehicle_size[0] = integer_vehicle_width_input;
			}
		}
		if(vehicle_length_input != VehicleSize.VEHICLE_WIDTH_UNAVAILABLE) {
			int integer_vehicle_length_input = (int) (vehicle_length_input * 100);
			if(integer_vehicle_length_input >= VEHICLE_SIZE_MIN && integer_vehicle_length_input <= VEHICLE_LENGTH_MAX) {
				this.vehicle_size[1] = integer_vehicle_length_input;
			}
		}
	}
	
	protected int getMsgCnt() {
		return msgCnt;
	}

	protected int[] getId() {
		return id;
	}

	protected int getSecMark() {
		return secMark;
	}

	protected int getLat() {
		return lat;
	}

	protected int getLon() {
		return lon;
	}

	protected int getElev() {
		return elev;
	}

	protected int[] getAccuracy() {
		return accuracy;
	}

	protected int getTransmission() {
		return transmission;
	}

	protected int getSpeed() {
		return speed;
	}

	protected int getHeading() {
		return heading;
	}

	protected int getAngle() {
		return angle;
	}

	protected int[] getAcceleration() {
		return acceleration;
	}

	protected int getWheel_brakes() {
		return wheel_brakes;
	}

	protected int getTraction() {
		return traction;
	}

	protected int getAbs() {
		return abs;
	}

	protected int getScs() {
		return scs;
	}

	protected int getBba() {
		return bba;
	}

	protected int getAux() {
		return aux;
	}

	protected int[] getVehicle_size() {
		return vehicle_size;
	}

	@Override
	public String toString() {
		return "HelperBSM [msgCnt=" + msgCnt + ", id=" + Arrays.toString(id) + ", secMark=" + secMark + ", lat=" + lat
				+ ", Lon=" + lon + ", elev=" + elev + ", accuracy=" + Arrays.toString(accuracy) + ", transmission="
				+ transmission + ", speed=" + speed + ", heading=" + heading + ", angle=" + angle + ", acceleration="
				+ Arrays.toString(acceleration) + ", wheel_brakes=" + wheel_brakes + ", traction="
				+ traction + ", abs=" + abs + ", scs=" + scs + ", bba=" + bba + ", aux=" + aux + ", vehicle_size="
				+ Arrays.toString(vehicle_size) + "]";
	}
	
}
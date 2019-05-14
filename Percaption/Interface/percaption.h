/*
 * percaption_information.h
 *
 *  Created on: 2019年1月9日
 *      Author: zhuguohua
 */

#ifndef INTERFACE_PERCAPTION_INFORMATION_H_
#define INTERFACE_PERCAPTION_INFORMATION_H_

#include "derivative.h"
#include "property.h"
#include "vehilce_config.h"

#include "Ultrasonic.h"
#include "Interface/vehicle_state.h"
// math
#include "math.h"
#include "vector_2d.h"

typedef struct _ObstacleInformationPacket
{
	Vector2d First_Position;
	Vector2d Second_Position;
	float    Length;
}ObstacleInformationPacket;

typedef struct _ObstacleDistancePacket
{
	float            distance;
	UltrasonicStatus status;
}ObstacleDistancePacket;

class Percaption {
public:
	Percaption();
	virtual ~Percaption();

	void Init(void);

//	virtual uint8_t Work(Ultrasonic *u,VehicleState *v) = 0;
	// wheel speed
	float getPositionX();
	void  setPositionX(float value);
	Property<Percaption,float,READ_WRITE> PositionX;

	float getPositionY();
	void  setPositionY(float value);
	Property<Percaption,float,READ_WRITE> PositionY;

	float getAttitudeYaw();
	void  setAttitudeYaw(float value);
	Property<Percaption,float,READ_WRITE> AttitudeYaw;

	float getParkingLength();
	void  setParkingLength(float value);
	Property<Percaption,float,READ_WRITE> ParkingLength;

	float getParkingWidth();
	void  setParkingWidth(float value);
	Property<Percaption,float,READ_WRITE> ParkingWidth;

	bool getDetectParkingStatus();
	void setDetectParkingStatus(bool value);
	Property<Percaption,bool,READ_WRITE> DetectParkingStatus;

	uint8_t getCommand();
	void setCommand(uint8_t value);
	Property<Percaption,uint8_t,READ_WRITE> Command;

	ObstacleInformationPacket getValidParkingPosition();
	void    setValidParkingPosition(ObstacleInformationPacket value);
	Property<Percaption,ObstacleInformationPacket,READ_WRITE> ValidParkingPosition;

	ObstacleDistancePacket getObstacleDistance();
	void    setObstacleDistance(ObstacleDistancePacket value);
	Property<Percaption,ObstacleDistancePacket,READ_WRITE> ObstacleDistance;
protected:
	// 最终输出的障碍物重新定位的库位信息
	ObstacleInformationPacket _valid_parking_position;

	// 输出障碍物离车辆边沿的距离
	ObstacleDistancePacket _obstacle_distance;
private:
	float _position_x;
	float _position_y;
	float _attitude_yaw;
	float _parking_length;
	float _parking_width;
	bool  _detect_parking_status;
	uint8_t _command;
};

#endif /* INTERFACE_PERCAPTION_INFORMATION_H_ */

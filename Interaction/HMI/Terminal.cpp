/*
 * terminal.cpp
 *
 *  Created on: January 8 2018
 *      Author: Guohua Zhu
 */
/*****************************************************************************/
/* FILE NAME: terminal.cpp                        COPYRIGHT (c) Motovis 2018 */
/*                                                       All Rights Reserved */
/* DESCRIPTION: Interaction terminal            					         */
/*****************************************************************************/
/* REV      AUTHOR        DATE              DESCRIPTION OF CHANGE            */
/* ---   -----------    ----------------    ---------------------            */
/* 1.0	 Guohua Zhu     January 8 2019      Initial Version                  */
/*****************************************************************************/

#include "Terminal.h"

Terminal::Terminal() {
	_terminal_frame = FirstHead1;
	_frame_err_cnt = 0;
	_push_active = 0;
	// ACK Valid
	AckValid.setContainer(this);
	AckValid.getter(&Terminal::getAckValid);
	AckValid.setter(&Terminal::setAckValid);

	// ACK Valid
	Command.setContainer(this);
	Command.getter(&Terminal::getCommand);
	Command.setter(&Terminal::setCommand);

	// Push Active
	PushActive.setContainer(this);
	PushActive.getter(&Terminal::getPushActive);
	PushActive.setter(&Terminal::setPushActive);
}

Terminal::~Terminal() {

}

/// AckValid
uint8_t Terminal::getAckValid()             {return _ack_valid ;}
void    Terminal::setAckValid(uint8_t value){_ack_valid = value;}
uint8_t Terminal::getCommand()             {return _command ;}
void    Terminal::setCommand(uint8_t value){_command = value;}
uint8_t Terminal::getPushActive()             {return _push_active ;}
void    Terminal::setPushActive(uint8_t value){_push_active = value;}
/**************************************************************************************/
void Terminal::Parse(vuint32_t id,vuint8_t dat[],VehicleController *ctl)
{
	uint8_t i,check_sum;
	switch(id)
	{
		case 0x516://eps status
			check_sum =0 ;
			for(i=0;i<7;i++){
				check_sum += dat[i];
			}
			check_sum = check_sum ^ 0xFF;
			if(check_sum == dat[7])
			{
				ctl->GearEnable 		=  dat[0]       & 0x01;
				ctl->AccelerationEnable = (dat[0] >> 2) & 0x01;
				ctl->DecelerationEnable = (dat[0] >> 4) & 0x01;
				ctl->TorqueEnable       = (dat[0] >> 5) & 0x01;
				ctl->VelocityEnable     = (dat[0] >> 3) & 0x01;
				if( (0 == ctl->SteeringEnable) || (0 == ((dat[0] >> 1) & 0x01)))
				{
					ctl->SteeringEnable 	= (dat[0] >> 1) & 0x01;
				}
				ctl->Gear 				= (uint8_t)dat[1];
				ctl->SteeringAngle 		= (float)(((int16_t)((dat[3] << 8) | dat[2])) * 0.1);
				ctl->SteeringAngleRate 	= (float)(((uint16_t)((dat[5] << 8) | dat[4])) * 0.01);
				AckValid = 0xa5;
			}
			break;

		case 0x517://eps status
			check_sum =0 ;
			for(i=0;i<7;i++){
				check_sum += dat[i];
			}
			check_sum = check_sum ^ 0xFF;
			if(check_sum == dat[7])
			{
				ctl->Acceleration	= (float)(((int16_t )((dat[1] << 8) | dat[0])) * 0.001);
				ctl->Deceleration	= (float)(((int16_t )((dat[3] << 8) | dat[2])) * 0.001);
				ctl->Velocity		= (float)(((uint16_t)((dat[5] << 8) | dat[4])) * 0.001);
				ctl->Torque			= (float)(dat[6] * 0.5);
			}
			break;

		case 0x518:
			ctl->SteeringAngle 		= ((int16_t)((dat[1] << 8) | dat[0])) * 0.1f;
			ctl->SteeringAngleRate 	= dat[2] * 4.0f;
			ctl->Velocity		    = dat[3] * 0.1f;
			ctl->Distance           = ((uint16_t)((dat[5] << 8) | dat[4])) * 0.001f;
			ctl->Gear 				= (uint8_t)dat[6];
			ctl->APAEnable          = (uint8_t)dat[7];
			break;
		default:

			break;
	}
}

void Terminal::Parse(vuint32_t id,vuint8_t dat[],Ultrasonic *u)
{
	switch(id)
	{
        case 0x508://传感器9
        case 0x509://传感器10
        case 0x50A://传感器11
        case 0x50B://传感器12
        	Ultrasonic_Packet ultrasonic_packet;
        	ultrasonic_packet.Distance1 = (float)(((uint16_t )((dat[1] << 8) | dat[0])) * u->Compensation(0));
        	ultrasonic_packet.Distance2 = (float)(((uint16_t )((dat[3] << 8) | dat[2])) * u->Compensation(0));
        	ultrasonic_packet.Level = dat[4] * LEVEL_RATIO;
        	ultrasonic_packet.Width = dat[5] * WIDTH_RATIO;
        	ultrasonic_packet.status = dat[6];
        	u->setUltrasonicPacket(id & 0x00f,ultrasonic_packet);
			break;
		default:

			break;
	}
}

void Terminal::Parse(vuint32_t id,vuint8_t dat[],MessageManager *msg)
{
	Byte2Int temp_int;
	switch(id)
	{
        case 0x510:
        	temp_int.b[1] = dat[2];
        	temp_int.b[0] = dat[3];
        	msg->SteeringAngle = temp_int.i16 * 0.1;
        	break;

        case 0x520:
        	temp_int.b[1] = dat[4];
        	temp_int.b[0] = dat[5];
        	msg->WheelSpeedRearLeft = temp_int.u16 * 0.001;
        	temp_int.b[1] = dat[6];
        	temp_int.b[0] = dat[7];
        	msg->WheelSpeedRearRight = temp_int.u16 * 0.001;
        	break;
		default:

			break;
	}
}

void Terminal::Parse(vuint32_t id,vuint8_t dat[],Percaption *pi,Planning *pp)
{
	Byte2Int temp_int;
	switch(id)
	{
        case 0x540:
        	temp_int.b[1] = dat[0];
        	temp_int.b[0] = dat[1];
        	pi->PositionX = temp_int.i16 * 0.01;
        	temp_int.b[1] = dat[2];
        	temp_int.b[0] = dat[3];
        	pi->PositionY = temp_int.i16 * 0.01;
        	temp_int.b[1] = dat[4];
        	temp_int.b[0] = dat[5];
        	pi->AttitudeYaw = temp_int.i16 * 0.01;
        	temp_int.b[1] = dat[6];
        	temp_int.b[0] = dat[7];
        	pp->OuterMarginMove = temp_int.i16 * 0.01;
        	break;

        case 0x541:
        	temp_int.b[1] = dat[0];
        	temp_int.b[0] = dat[1];
        	pi->ParkingLength = temp_int.u16 * 0.001;
        	temp_int.b[1] = dat[2];
        	temp_int.b[0] = dat[3];
        	pi->ParkingWidth = temp_int.u16 * 0.001;
        	temp_int.b[1] = dat[4];
        	temp_int.b[0] = dat[5];
        	pp->FrontMarginBoundary = temp_int.u16 * 0.001;
        	temp_int.b[1] = dat[6];
        	temp_int.b[0] = dat[7];
        	pp->RearMarginBoundary = temp_int.u16 * 0.001;
        	break;
		default:

			break;
	}
}

void Terminal::Parse(vuint32_t id,vuint8_t dat[],Planning *msg)
{
	switch(id)
	{
        case 0x532:
        	msg->Command = (uint8_t)dat[0];
        	break;

        case 0x560:
        	msg->PlanningBrakingAcc      = ((int8_t)dat[0])*0.1f;
        	msg->PlanningBrakingAccR     = 1/fabs(msg->PlanningBrakingAcc)/0.8f;
        	msg->PlanningBrakingAeb      = ((int8_t)dat[1])*0.1f;
        	msg->TurnningFeedforwardTime = ((uint8_t)dat[2])*0.01f;
        	msg->AccDisableTime          = (uint8_t)(50 * ((uint8_t)dat[3])*0.01f);
        	msg->PositionMin             = ((uint8_t)dat[4])*0.01f;
        	msg->PositionMax             = ((uint8_t)dat[5])*0.01f;
        	msg->ParkingMargin           = ((uint8_t)dat[6])*0.01f;
        	msg->KpYaw                   = (uint8_t)dat[7];
        	break;
        default:
        	break;
	}
}

void Terminal::Parse(vuint32_t id,vuint8_t dat[])
{
	switch(id)
	{
        case 0x530:
        	Command = (uint8_t)dat[0];
        	break;

        default:
        	break;
	}
}
/**************************************************************************************/
void Terminal::Push(ChangAnMessage *msg)
{
	CAN_Packet m_CAN_Packet;
	int16_t temp_int16;
	uint16_t temp_uint16;

	m_CAN_Packet.id = 0x410;
	m_CAN_Packet.length = 8;

	m_CAN_Packet.data[0] = ( (msg->EMS_QEC_ACC & 0x01) << 2) | ( (msg->ESP_QDC_ACC & 0x01) << 1) | (msg->APA_EPAS_Failed & 0x01);
	m_CAN_Packet.data[1] = 0;

	temp_int16 = (int16_t)(msg->SteeringAngle * 10);
	m_CAN_Packet.data[2] = temp_int16 & 0xff;
	m_CAN_Packet.data[3] = (temp_int16 >> 8) & 0xff;

	temp_uint16 = (uint16_t)(msg->SteeringAngleRate * 100);
	m_CAN_Packet.data[4] = temp_uint16 & 0xff;
	m_CAN_Packet.data[5] = (temp_uint16 >> 8) & 0xff;

	temp_int16 = (int16_t)(msg->SteeringTorque * 100);
	m_CAN_Packet.data[6] = temp_int16 & 0xff;
	m_CAN_Packet.data[7] = (temp_int16 >> 8) & 0xff;

	CAN2_TransmitMsg(m_CAN_Packet);
}

void Terminal::Push(VehicleController *msg)
{
	CAN_Packet m_CAN_Packet;
	int16_t temp_int16;
	uint16_t temp_uint16;

	m_CAN_Packet.id = 0x411;
	m_CAN_Packet.length = 8;

	m_CAN_Packet.data[0] = 	 msg->AccelerationEnable 	   |
							(msg->DecelerationEnable << 1) |
							(msg->TorqueEnable       << 2) |
							(msg->VelocityEnable     << 3) |
							(msg->SteeringEnable     << 4) |
							(msg->GearEnable         << 6) ;
	m_CAN_Packet.data[1] = msg->Gear ;

	temp_int16 = (int16_t)(msg->Acceleration * 100);
	m_CAN_Packet.data[2] = temp_int16 & 0xff ;
	m_CAN_Packet.data[3] = (temp_int16 >> 8) & 0xff ;

	temp_int16 = (int16_t)(msg->Deceleration * 100);
	m_CAN_Packet.data[4] = temp_int16 & 0xff ;
	m_CAN_Packet.data[5] = (temp_int16 >> 8) & 0xff ;

	temp_uint16 = (uint16_t)(msg->Velocity * 100);
	m_CAN_Packet.data[6] =  temp_uint16 & 0xff ;
	m_CAN_Packet.data[7] = (temp_uint16 >> 8) & 0xff ;

	CAN2_TransmitMsg(m_CAN_Packet);
}

void Terminal::Push(VehicleState *msg)
{
	CAN_Packet m_CAN_Packet;
	Byte2Int temp_int;
	m_CAN_Packet.id = 0x442;
	m_CAN_Packet.length = 8;

	temp_int.i16 = (int16_t) (msg->getPosition().X * 100);
	m_CAN_Packet.data[0] = temp_int.b[1];
	m_CAN_Packet.data[1] = temp_int.b[0];
	temp_int.i16 = (int16_t) (msg->getPosition().Y * 100);
	m_CAN_Packet.data[2] = temp_int.b[1];
	m_CAN_Packet.data[3] = temp_int.b[0];
	m_CAN_Packet.data[4] = ((int16_t) (msg->Yaw * 100)) & 0xff;
	m_CAN_Packet.data[5] = (((int16_t)(msg->Yaw * 100)) >> 8) & 0xff;
	m_CAN_Packet.data[6] = ((int16_t) (msg->TurnningRadius * 100)) & 0xff;
	m_CAN_Packet.data[7] = (((int16_t)(msg->TurnningRadius * 100)) >> 8) & 0xff;
	CAN2_TransmitMsg(m_CAN_Packet);
}

void Terminal::Push(Ultrasonic *u)
{
#if ULTRASONIC_PACKET == 1

#if ULTRASONIC_SCHEDULE_MODO == 2
	switch(u->ReadStage)
	{
		case 0:
			UltrasonicSend(1,u->UltrasonicPacket);
			UltrasonicSend(7,u->UltrasonicPacket);
			break;

		case 1:
		case 5:
			UltrasonicSend(8,u->UltrasonicPacket);
			UltrasonicSend(11,u->UltrasonicPacket);
			break;

		case 2:
		case 6:
			UltrasonicSend(9,u->UltrasonicPacket);
			UltrasonicSend(10,u->UltrasonicPacket);
			break;

		case 3:
			UltrasonicSend(3,u->UltrasonicPacket);
			UltrasonicSend(5,u->UltrasonicPacket);
			break;

		case 4:
			UltrasonicSend(0,u->UltrasonicPacket);
			UltrasonicSend(6,u->UltrasonicPacket);
			break;

		case 7:
			UltrasonicSend(2,u->UltrasonicPacket);
			UltrasonicSend(4,u->UltrasonicPacket);
			break;

		default:
			break;
	}
#endif

#if ULTRASONIC_SCHEDULE_MODO == 3
	switch(u->ReadStage)
	{
		case 0:
			UltrasonicSend(1,u->UltrasonicPacket);
			UltrasonicSend(6,u->UltrasonicPacket);

			UltrasonicLocationSend(1,u->AbstaclePositionDirect[1]);
			UltrasonicLocationSend(6,u->AbstaclePositionDirect[6]);

			UltrasonicLocationSend(0,u->UltrasonicLocationPacket);
			UltrasonicLocationSend(1,u->UltrasonicLocationPacket);
			UltrasonicLocationSend(2,u->UltrasonicLocationPacket);
			UltrasonicLocationSend(9,u->UltrasonicLocationPacket);
			UltrasonicLocationSend(10,u->UltrasonicLocationPacket);
			UltrasonicLocationSend(11,u->UltrasonicLocationPacket);


			break;

		case 1:
			UltrasonicLocationSend(12,u->AbstaclePositionTriangle[0],u->AbstaclePositionTriangle[1]);
			UltrasonicLocationSend(15,u->AbstaclePositionTriangle[6],u->AbstaclePositionTriangle[7]);

			UltrasonicGroundLocationSend(12,u->AbstacleGroundPositionTriangle[0],u->AbstacleGroundPositionTriangle[1]);
			UltrasonicGroundLocationSend(15,u->AbstacleGroundPositionTriangle[6],u->AbstacleGroundPositionTriangle[7]);

			UltrasonicSend(8 ,u->UltrasonicPacket);
			UltrasonicSend(10,u->UltrasonicPacket);

			UltrasonicLocationSend(8,u->AbstaclePositionDirect[8]);
			UltrasonicLocationSend(10,u->AbstaclePositionDirect[10]);
			break;

		case 5:
			UltrasonicLocationSend(13,u->AbstaclePositionTriangle[2],u->AbstaclePositionTriangle[3]);
			UltrasonicLocationSend(14,u->AbstaclePositionTriangle[4],u->AbstaclePositionTriangle[5]);

			UltrasonicGroundLocationSend(13,u->AbstacleGroundPositionTriangle[2],u->AbstacleGroundPositionTriangle[2]);
			UltrasonicGroundLocationSend(14,u->AbstacleGroundPositionTriangle[4],u->AbstacleGroundPositionTriangle[5]);

			UltrasonicSend(8 ,u->UltrasonicPacket);
			UltrasonicSend(10,u->UltrasonicPacket);

			UltrasonicLocationSend(8,u->AbstaclePositionDirect[8]);
			UltrasonicLocationSend(10,u->AbstaclePositionDirect[10]);
			break;

		case 2:
		case 6:
			UltrasonicSend(9,u->UltrasonicPacket);
			UltrasonicSend(11,u->UltrasonicPacket);

			UltrasonicLocationSend(9,u->AbstaclePositionDirect[9]);
			UltrasonicLocationSend(11,u->AbstaclePositionDirect[11]);
			break;

		case 3:
			UltrasonicSend(3,u->UltrasonicPacket);
			UltrasonicSend(4,u->UltrasonicPacket);

			UltrasonicLocationSend(3,u->AbstaclePositionDirect[3]);
			UltrasonicLocationSend(4,u->AbstaclePositionDirect[4]);
			break;

		case 4:
			UltrasonicSend(2,u->UltrasonicPacket);
			UltrasonicSend(5,u->UltrasonicPacket);

			UltrasonicLocationSend(2,u->AbstaclePositionDirect[2]);
			UltrasonicLocationSend(5,u->AbstaclePositionDirect[5]);

			UltrasonicLocationSend(3,u->UltrasonicLocationPacket);
			UltrasonicLocationSend(4,u->UltrasonicLocationPacket);
			UltrasonicLocationSend(5,u->UltrasonicLocationPacket);
			UltrasonicLocationSend(6,u->UltrasonicLocationPacket);
			UltrasonicLocationSend(7,u->UltrasonicLocationPacket);
			UltrasonicLocationSend(8,u->UltrasonicLocationPacket);
			break;

		case 7:
			UltrasonicSend(0,u->UltrasonicPacket);
			UltrasonicSend(7,u->UltrasonicPacket);

			UltrasonicLocationSend(0,u->AbstaclePositionDirect[0]);
			UltrasonicLocationSend(7,u->AbstaclePositionDirect[7]);
			break;

		default:
			break;
	}
#endif

#else
#if ULTRASONIC_SCHEDULE_MODO == 2
	switch(u->ReadStage)
	{
		case 0:
			UltrasonicSend(1,u->UltrasonicDatas);
			UltrasonicSend(7,u->UltrasonicDatas);
			break;

		case 1:
		case 5:
			UltrasonicSend(8,u->UltrasonicDatas);
			UltrasonicSend(11,u->UltrasonicDatas);
			break;

		case 2:
		case 6:
			UltrasonicSend(9,u->UltrasonicDatas);
			UltrasonicSend(10,u->UltrasonicDatas);
			break;

		case 3:
			UltrasonicSend(3,u->UltrasonicDatas);
			UltrasonicSend(5,u->UltrasonicDatas);
			break;

		case 4:
			UltrasonicSend(0,u->UltrasonicDatas);
			UltrasonicSend(6,u->UltrasonicDatas);
			break;

		case 7:
			UltrasonicSend(2,u->UltrasonicDatas);
			UltrasonicSend(4,u->UltrasonicDatas);
			break;

		default:
			break;
	}
#endif

#if ULTRASONIC_SCHEDULE_MODO == 3
	switch(u->ReadStage)
	{
		case 0:
			UltrasonicSend(1,u->UltrasonicDatas);
			UltrasonicSend(6,u->UltrasonicDatas);
			UltrasonicLocationSend(0,u->UltrasonicLocationDatas);
			UltrasonicLocationSend(1,u->UltrasonicLocationDatas);
			UltrasonicLocationSend(2,u->UltrasonicLocationDatas);
			UltrasonicLocationSend(9,u->UltrasonicLocationDatas);
			UltrasonicLocationSend(10,u->UltrasonicLocationDatas);
			UltrasonicLocationSend(11,u->UltrasonicLocationDatas);
			break;

		case 1:
		case 5:
			UltrasonicSend(8,u->UltrasonicDatas);
			UltrasonicSend(10,u->UltrasonicDatas);
			break;

		case 2:
		case 6:
			UltrasonicSend(9,u->UltrasonicDatas);
			UltrasonicSend(11,u->UltrasonicDatas);
			break;

		case 3:
			UltrasonicSend(3,u->UltrasonicDatas);
			UltrasonicSend(4,u->UltrasonicDatas);
			break;

		case 4:
			UltrasonicSend(2,u->UltrasonicDatas);
			UltrasonicSend(5,u->UltrasonicDatas);
			UltrasonicLocationSend(3,u->UltrasonicLocationDatas);
			UltrasonicLocationSend(4,u->UltrasonicLocationDatas);
			UltrasonicLocationSend(5,u->UltrasonicLocationDatas);
			UltrasonicLocationSend(6,u->UltrasonicLocationDatas);
			UltrasonicLocationSend(7,u->UltrasonicLocationDatas);
			UltrasonicLocationSend(8,u->UltrasonicLocationDatas);
			break;

		case 7:
			UltrasonicSend(0,u->UltrasonicDatas);
			UltrasonicSend(7,u->UltrasonicDatas);
			break;

		default:
			break;
	}
#endif
#endif
}

void Terminal::Push(Percaption *p)
{
	CAN_Packet m_CAN_Packet;
	Byte2Int temp_int;
	Vector2d temp_v;
	m_CAN_Packet.id = 0x441;
	m_CAN_Packet.length = 8;

	temp_int.u16 = (uint16_t)(p->ParkingLength * 1000);
	m_CAN_Packet.data[0] = temp_int.b[1];
	m_CAN_Packet.data[1] = temp_int.b[0];
	temp_int.u16 = (uint16_t)(p->ParkingWidth * 1000);
	m_CAN_Packet.data[2] = temp_int.b[1];
	m_CAN_Packet.data[3] = temp_int.b[0];

	m_CAN_Packet.data[4] = 0;
	m_CAN_Packet.data[5] = 0;
	m_CAN_Packet.data[6] = 0;
	m_CAN_Packet.data[7] = 0;
	CAN2_TransmitMsg(m_CAN_Packet);

	m_CAN_Packet.id = 0x440;
	m_CAN_Packet.length = 8;

	temp_int.i16 = (int16_t)(p->PositionX * 100);
	m_CAN_Packet.data[0] = temp_int.b[1];
	m_CAN_Packet.data[1] = temp_int.b[0];
	temp_int.i16 = (int16_t)(p->PositionY * 100);
	m_CAN_Packet.data[2] = temp_int.b[1];
	m_CAN_Packet.data[3] = temp_int.b[0];
	temp_int.i16 = (int16_t)(p->AttitudeYaw * 100);
	m_CAN_Packet.data[4] = temp_int.b[1];
	m_CAN_Packet.data[5] = temp_int.b[0];

	m_CAN_Packet.data[6] = p->DetectParkingStatus;
	m_CAN_Packet.data[7] = 0;
	CAN2_TransmitMsg(m_CAN_Packet);
}

void Terminal::Push(Planning *p)
{
	CAN_Packet m_CAN_Packet;
	Vector2d temp_v;
	m_CAN_Packet.id = 0x448;
	m_CAN_Packet.length = 8;

	m_CAN_Packet.data[0] = p->ParkingStatus;
	m_CAN_Packet.data[1] = 0;
	m_CAN_Packet.data[2] = 0;
	m_CAN_Packet.data[3] = 0;

	m_CAN_Packet.data[4] = 0;
	m_CAN_Packet.data[5] = 0;
	m_CAN_Packet.data[6] = 0;
	m_CAN_Packet.data[7] = 0;
	CAN2_TransmitMsg(m_CAN_Packet);
}
/**************************************************************************************/
void Terminal::UltrasonicSend(uint8_t id,LIN_RAM *msg)
{
	CAN_Packet m_CAN_Packet;
	m_CAN_Packet.id = 0x400 | id;
	m_CAN_Packet.length = 8;
	if(id < 8)
	{

		m_CAN_Packet.data[0] =  msg[id].STP318.TOF       & 0xff;
		m_CAN_Packet.data[1] = (msg[id].STP318.TOF >> 8) & 0xff;
		m_CAN_Packet.data[2] = 0;
		m_CAN_Packet.data[3] = 0;
		m_CAN_Packet.data[4] = 0;
		m_CAN_Packet.data[5] = 0;
		m_CAN_Packet.data[6] =  msg[id].STP318.Status;
		m_CAN_Packet.data[7] = 0;
	}
	else
	{
		m_CAN_Packet.data[0] =  msg[id].STP313.TOF1       & 0xff;
		m_CAN_Packet.data[1] = (msg[id].STP313.TOF1 >> 8) & 0xff;
		m_CAN_Packet.data[2] =  msg[id].STP313.TOF2       & 0xff;
		m_CAN_Packet.data[3] = (msg[id].STP313.TOF2 >> 8) & 0xff;
		m_CAN_Packet.data[4] =  msg[id].STP313.Level;
		m_CAN_Packet.data[5] =  msg[id].STP313.Width;
		m_CAN_Packet.data[6] =  msg[id].STP313.Status;
		m_CAN_Packet.data[7] =  0;
	}
	CAN2_TransmitMsg(m_CAN_Packet);
}

void Terminal::UltrasonicLocationSend(uint8_t id,LIN_RAM *msg)
{
	CAN_Packet m_CAN_Packet;
	m_CAN_Packet.id = 0x470 | id;
	m_CAN_Packet.length = 8;
	m_CAN_Packet.data[0] =  msg[id].STP318.TOF       & 0xff;
	m_CAN_Packet.data[1] = (msg[id].STP318.TOF >> 8) & 0xff;
	m_CAN_Packet.data[2] = 0;
	m_CAN_Packet.data[3] = 0;
	m_CAN_Packet.data[4] = 0;
	m_CAN_Packet.data[5] = 0;
	m_CAN_Packet.data[6] =  msg[id].STP318.Status;
	m_CAN_Packet.data[7] = 0;
	CAN2_TransmitMsg(m_CAN_Packet);
}

void Terminal::UltrasonicSend(uint8_t id,Ultrasonic_Packet *msg_pk)
{
	CAN_Packet m_CAN_Packet;
	m_CAN_Packet.id = 0x400 | id;
	m_CAN_Packet.length = 8;
	if(id < 8)
	{
		m_CAN_Packet.data[0] = ((uint8_t)(msg_pk[id].Distance1*100))      & 0xff;
		m_CAN_Packet.data[1] = ((uint8_t)(msg_pk[id].Distance1*100) >> 8) & 0xff;
		m_CAN_Packet.data[2] = 0;
		m_CAN_Packet.data[3] = 0;
		m_CAN_Packet.data[4] = 0;
		m_CAN_Packet.data[5] = 0;
		m_CAN_Packet.data[6] =  msg_pk[id].status;
		m_CAN_Packet.data[7] = 0;
	}
	else
	{
		m_CAN_Packet.data[0] = ((uint8_t)(msg_pk[id].Distance1 * 100)    ) & 0xff;
		m_CAN_Packet.data[1] = ((uint8_t)(msg_pk[id].Distance1 * 100)>> 8) & 0xff;
		m_CAN_Packet.data[2] = ((uint8_t)(msg_pk[id].Distance2 * 100)    ) & 0xff;
		m_CAN_Packet.data[3] = ((uint8_t)(msg_pk[id].Distance2 * 100)>> 8) & 0xff;
		m_CAN_Packet.data[4] = ((uint8_t)(msg_pk[id].Level * 10     )    ) & 0xff;
		m_CAN_Packet.data[5] = (uint8_t)(msg_pk[id].Width);
		m_CAN_Packet.data[6] =  msg_pk[id].status;
		m_CAN_Packet.data[7] =  0;
	}
	CAN2_TransmitMsg(m_CAN_Packet);
}

void Terminal::UltrasonicLocationSend(uint8_t id,Ultrasonic_Packet *msg_pk)
{
	CAN_Packet m_CAN_Packet;
	m_CAN_Packet.id = 0x470 | id;
	m_CAN_Packet.length = 8;

	m_CAN_Packet.data[0] = ((uint8_t)(msg_pk[id].Distance1*100))      & 0xff;
	m_CAN_Packet.data[1] = ((uint8_t)(msg_pk[id].Distance1*100) >> 8) & 0xff;
	m_CAN_Packet.data[2] = 0;
	m_CAN_Packet.data[3] = 0;
	m_CAN_Packet.data[4] = 0;
	m_CAN_Packet.data[5] = 0;
	m_CAN_Packet.data[6] =  msg_pk[id].status;
	m_CAN_Packet.data[7] = 0;

	CAN2_TransmitMsg(m_CAN_Packet);
}

void Terminal::UltrasonicLocationSend(uint8_t id,Vector2d v1)
{
	CAN_Packet m_CAN_Packet;
	int16_t temp;
	m_CAN_Packet.id = 0x480 | id;
	m_CAN_Packet.length = 8;

	temp = v1.getX()*1000;
	m_CAN_Packet.data[0] = (uint8_t)((temp     ) & 0xff );
	m_CAN_Packet.data[1] = (uint8_t)((temp >> 8) & 0xff );
	temp = v1.getY()*1000;
	m_CAN_Packet.data[2] = (uint8_t)((temp     ) & 0xff );
	m_CAN_Packet.data[3] = (uint8_t)((temp >> 8) & 0xff );

	m_CAN_Packet.data[4] = 0;
	m_CAN_Packet.data[5] = 0;
	m_CAN_Packet.data[6] = 0;
	m_CAN_Packet.data[7] = 0;

	CAN2_TransmitMsg(m_CAN_Packet);
}

void Terminal::UltrasonicLocationSend(uint8_t id,Vector2d v1,Vector2d v2)
{
	CAN_Packet m_CAN_Packet;
	int16_t temp;
	m_CAN_Packet.id = 0x480 | id;
	m_CAN_Packet.length = 8;

	temp = v1.getX()*1000;
	m_CAN_Packet.data[0] = (uint8_t)((temp     ) & 0xff );
	m_CAN_Packet.data[1] = (uint8_t)((temp >> 8) & 0xff );
	temp = v1.getY()*1000;
	m_CAN_Packet.data[2] = (uint8_t)((temp     ) & 0xff );
	m_CAN_Packet.data[3] = (uint8_t)((temp >> 8) & 0xff );
	temp = v2.getX()*1000;
	m_CAN_Packet.data[4] = (uint8_t)((temp     ) & 0xff );
	m_CAN_Packet.data[5] = (uint8_t)((temp >> 8) & 0xff );
	temp = v2.getY()*1000;
	m_CAN_Packet.data[6] = (uint8_t)((temp     ) & 0xff );
	m_CAN_Packet.data[7] = (uint8_t)((temp >> 8) & 0xff );

	CAN2_TransmitMsg(m_CAN_Packet);
}

void Terminal::UltrasonicGroundLocationSend(uint8_t id,Vector2d v1,Vector2d v2)
{
	CAN_Packet m_CAN_Packet;
	int16_t temp;
	m_CAN_Packet.id = 0x490 | id;
	m_CAN_Packet.length = 8;

	temp = v1.getX()*1000;
	m_CAN_Packet.data[0] = (uint8_t)((temp     ) & 0xff );
	m_CAN_Packet.data[1] = (uint8_t)((temp >> 8) & 0xff );
	temp = v1.getY()*1000;
	m_CAN_Packet.data[2] = (uint8_t)((temp     ) & 0xff );
	m_CAN_Packet.data[3] = (uint8_t)((temp >> 8) & 0xff );
	temp = v2.getX()*1000;
	m_CAN_Packet.data[4] = (uint8_t)((temp     ) & 0xff );
	m_CAN_Packet.data[5] = (uint8_t)((temp >> 8) & 0xff );
	temp = v2.getY()*1000;
	m_CAN_Packet.data[6] = (uint8_t)((temp     ) & 0xff );
	m_CAN_Packet.data[7] = (uint8_t)((temp >> 8) & 0xff );

	CAN2_TransmitMsg(m_CAN_Packet);
}

void Terminal::Ack(void)
{
	CAN_Packet m_CAN_Packet;
	m_CAN_Packet.id = 0x416;
	m_CAN_Packet.length = 8;
	m_CAN_Packet.data[0] = 0x5A;
	m_CAN_Packet.data[1] = 0xA5;
	m_CAN_Packet.data[2] = 0;
	m_CAN_Packet.data[3] = 0;
	m_CAN_Packet.data[4] = 0;
	m_CAN_Packet.data[5] = 0;
	m_CAN_Packet.data[6] = 0;
	m_CAN_Packet.data[7] = 0;
	CAN2_TransmitMsg(m_CAN_Packet);
}

void Terminal::VehicleInitPositionSend(VehicleBody v)
{
	CAN_Packet m_CAN_Packet;
	Byte2Int temp_int;
	Vector2d temp_v;
	m_CAN_Packet.id = 0x440;
	m_CAN_Packet.length = 8;

	temp_v = v.Center;
	temp_int.i16 = (int16_t)(temp_v.getX() * 100);
	m_CAN_Packet.data[0] = temp_int.b[1];
	m_CAN_Packet.data[1] = temp_int.b[0];
	temp_int.i16 = (int16_t)(temp_v.getY() * 100);
	m_CAN_Packet.data[2] = temp_int.b[1];
	m_CAN_Packet.data[3] = temp_int.b[0];
	temp_int.i16 = (int16_t)(v.AttitudeYaw * 100);
	m_CAN_Packet.data[4] = temp_int.b[1];
	m_CAN_Packet.data[5] = temp_int.b[0];

	m_CAN_Packet.data[6] = 0;
	m_CAN_Packet.data[7] = 0;
	CAN2_TransmitMsg(m_CAN_Packet);
}

void Terminal::ParkingMsgSend(Percaption *p,float fm,float rm)
{
	CAN_Packet m_CAN_Packet;
	Byte2Int temp_int;
	Vector2d temp_v;
	m_CAN_Packet.id = 0x441;
	m_CAN_Packet.length = 8;

	temp_int.u16 = (uint16_t)(p->ParkingLength * 1000);
	m_CAN_Packet.data[0] = temp_int.b[1];
	m_CAN_Packet.data[1] = temp_int.b[0];
	temp_int.u16 = (uint16_t)(p->ParkingWidth * 1000);
	m_CAN_Packet.data[2] = temp_int.b[1];
	m_CAN_Packet.data[3] = temp_int.b[0];
	temp_int.u16 = (uint16_t)(fm * 1000);
	m_CAN_Packet.data[4] = temp_int.b[1];
	m_CAN_Packet.data[5] = temp_int.b[0];
	temp_int.u16 = (uint16_t)(rm * 1000);
	m_CAN_Packet.data[6] = temp_int.b[1];
	m_CAN_Packet.data[7] = temp_int.b[0];
	CAN2_TransmitMsg(m_CAN_Packet);
}

void Terminal::FrontTrialPositionSend(VehicleBody v,uint8_t cnt)
{
	CAN_Packet m_CAN_Packet;
	Byte2Int temp_int;
	Vector2d temp_v;
	m_CAN_Packet.id = 0x443;
	m_CAN_Packet.length = 8;

	temp_v = v.Center;
	temp_int.i16 = (int16_t)(temp_v.getX() * 100);
	m_CAN_Packet.data[0] = temp_int.b[1];
	m_CAN_Packet.data[1] = temp_int.b[0];
	temp_int.i16 = (int16_t)(temp_v.getY() * 100);
	m_CAN_Packet.data[2] = temp_int.b[1];
	m_CAN_Packet.data[3] = temp_int.b[0];
	temp_int.i16 = (int16_t)(v.AttitudeYaw * 100);
	m_CAN_Packet.data[4] = temp_int.b[1];
	m_CAN_Packet.data[5] = temp_int.b[0];

	m_CAN_Packet.data[6] = cnt;
	m_CAN_Packet.data[7] = 0;
	CAN2_TransmitMsg(m_CAN_Packet);
}

void Terminal::RearTrialPositionSend(VehicleBody v,uint8_t cnt)
{
	CAN_Packet m_CAN_Packet;
	Byte2Int temp_int;
	Vector2d temp_v;
	m_CAN_Packet.id = 0x444;
	m_CAN_Packet.length = 8;

	temp_v = v.Center;
	temp_int.i16 = (int16_t)(temp_v.getX() * 100);
	m_CAN_Packet.data[0] = temp_int.b[1];
	m_CAN_Packet.data[1] = temp_int.b[0];
	temp_int.i16 = (int16_t)(temp_v.getY() * 100);
	m_CAN_Packet.data[2] = temp_int.b[1];
	m_CAN_Packet.data[3] = temp_int.b[0];
	temp_int.i16 = (int16_t)(v.AttitudeYaw * 100);
	m_CAN_Packet.data[4] = temp_int.b[1];
	m_CAN_Packet.data[5] = temp_int.b[0];

	m_CAN_Packet.data[6] = cnt;
	m_CAN_Packet.data[7] = 0;
	CAN2_TransmitMsg(m_CAN_Packet);
}

void Terminal::EnterParkingPositionSend(VehicleBody v,uint8_t cnt,uint8_t s)
{
	CAN_Packet m_CAN_Packet;
	Byte2Int temp_int;
	Vector2d temp_v;
	m_CAN_Packet.id = 0x445;
	m_CAN_Packet.length = 8;

	temp_v = v.Center;
	temp_int.i16 = (int16_t)(temp_v.getX() * 100);
	m_CAN_Packet.data[0] = temp_int.b[1];
	m_CAN_Packet.data[1] = temp_int.b[0];
	temp_int.i16 = (int16_t)(temp_v.getY() * 100);
	m_CAN_Packet.data[2] = temp_int.b[1];
	m_CAN_Packet.data[3] = temp_int.b[0];
	temp_int.i16 = (int16_t)(v.AttitudeYaw * 100);
	m_CAN_Packet.data[4] = temp_int.b[1];
	m_CAN_Packet.data[5] = temp_int.b[0];

	m_CAN_Packet.data[6] = cnt;
	m_CAN_Packet.data[7] = s;
	CAN2_TransmitMsg(m_CAN_Packet);
}

void Terminal::TurnPointSend(Turn v,uint8_t cnt)
{
	CAN_Packet m_CAN_Packet;
	Byte2Int temp_int;
	Vector2d temp_v;
	m_CAN_Packet.id = 0x446;
	m_CAN_Packet.length = 8;

	temp_v = v.Point;
	temp_int.i16 = (int16_t)(temp_v.getX() * 100);
	m_CAN_Packet.data[0] = temp_int.b[1];
	m_CAN_Packet.data[1] = temp_int.b[0];
	temp_int.i16 = (int16_t)(temp_v.getY() * 100);
	m_CAN_Packet.data[2] = temp_int.b[1];
	m_CAN_Packet.data[3] = temp_int.b[0];
	temp_int.i16 = (int16_t)(v.SteeringAngle * 10);
	m_CAN_Packet.data[4] = temp_int.b[1];
	m_CAN_Packet.data[5] = temp_int.b[0];

	m_CAN_Packet.data[6] = cnt;
	m_CAN_Packet.data[7] = 0;
	CAN2_TransmitMsg(m_CAN_Packet);
}

void Terminal::ParkingCenterPointSend(Vector2d v)
{
	CAN_Packet m_CAN_Packet;
	Byte2Int temp_int;
	Vector2d temp_v;
	m_CAN_Packet.id = 0x447;
	m_CAN_Packet.length = 8;

	temp_v = v;
	temp_int.i16 = (int16_t)(temp_v.getX() * 100);
	m_CAN_Packet.data[0] = temp_int.b[1];
	m_CAN_Packet.data[1] = temp_int.b[0];
	temp_int.i16 = (int16_t)(temp_v.getY() * 100);
	m_CAN_Packet.data[2] = temp_int.b[1];
	m_CAN_Packet.data[3] = temp_int.b[0];

	m_CAN_Packet.data[4] = 0;
	m_CAN_Packet.data[5] = 0;

	m_CAN_Packet.data[6] = 0;
	m_CAN_Packet.data[7] = 0;
	CAN2_TransmitMsg(m_CAN_Packet);
}

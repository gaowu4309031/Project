#ifndef __USERTASK_H__
#define __USERTASK_H__

#include "types.h"
#include "bcomdef.h"


#define BLE_DATA_LEN		20

// User Base Task Events
#define USER_TASK1_EVENT0								(1 << 0)
#define USER_TASK1_EVENT1								(1 << 1)
#define USER_TASK1_EVENT2								(1 << 2)

#define USER_TASK2_EVENT0								(1 << 0)
#define USER_TASK2_EVENT1								(1 << 1)
#define USER_TASK2_EVENT2								(1 << 2)
#define USER_TASK2_EVENT3								(1 << 3)
#define USER_TASK2_EVENT4								(1 << 4)
#define USER_TASK2_EVENT5								(1 << 5)
#define USER_TASK2_EVENT6								(1 << 6)

#define USER_TASK3_EVENT0								(1 << 0)		//蓝牙串口数据事件
#define USER_TASK3_EVENT1								(1 << 1)
#define USER_TASK3_EVENT2								(1 << 2)

#define USER_TASK4_EVENT0								(1 << 0)		//蓝牙串口数据事件
#define USER_TASK4_EVENT1								(1 << 1)
#define USER_TASK4_EVENT2								(1 << 2)

// Profile Parameters
#define USERPROFILE_CHAR1                   0  // RW uint8 - Profile Characteristic 1 value 
#define USERPROFILE_CHAR2                   1  // RW uint8 - Profile Characteristic 2 value
#define USERPROFILE_CHAR3                   2  // RW uint8 - Profile Characteristic 3 value
#define USERPROFILE_CHAR4                   3  // RW uint8 - Profile Characteristic 4 value
#define USERPROFILE_CHAR5                   4  // RW uint8 - Profile Characteristic 4 value
#define USERPROFILE_CHAR6                   5  // RW uint8 - Profile Characteristic 4 value
#define USERPROFILE_CHAR7                   6  // RW uint8 - Profile Characteristic 4 value

// UUID
#define USERPROFILE_SERV_UUID               0xFFF7
#define USERPROFILE_CHAR1_UUID            	0x36F1
#define USERPROFILE_CHAR2_UUID            	0x36F2

#define IBEACON_ATT_LONG_PKT              	251//230//160

//ble cmd
#define BLE_CMD_AUTH				0x01
#define BLE_CMD_COPY				0x02
#define BLE_CMD_QUERY				0x03
#define BLE_CMD_EXIT				0x04
#define BLE_CMD_GET					0x05
#define BLE_CMD_CLEAR				0x06
#define BLE_CMD_MODE				0x07
#define BLE_CMD_REMOTE				0x08
#define BLE_CMD_UPDATE				0x10

//ble cmd response
#define BLE_RESPONSE_AUTH			0x81
#define BLE_RESPONSE_COPY			0x82
#define BLE_RESPONSE_QUERY			0x83
#define BLE_RESPONSE_EXIT			0x84
#define BLE_RESPONSE_GIT			0x85
#define BLE_RESPONSE_CLEAR			0x86
#define BLE_RESPONSE_MODE			0x87
#define BLE_RESPONSE_REMOTE			0x88
#define BLE_RESPONSE_UPDATE			0x90







void UserBase1_Init( uint8 task_id );
void UserBase2_Init( uint8 task_id );
void UserBase3_Init( uint8 task_id );
void UserBase4_Init( uint8 task_id );
uint16 UserBase1_ProcessEvent( uint8 task_id, uint16 events );
uint16 UserBase2_ProcessEvent( uint8 task_id, uint16 events );
uint16 UserBase3_ProcessEvent( uint8 task_id, uint16 events );
uint16 UserBase4_ProcessEvent( uint8 task_id, uint16 events );
bStatus_t UserProfile_AddService( uint32 services );
bStatus_t User_NotifyData(uint8_t *notiData, uint8_t notiLen);
void Data_Packaging(uint8_t cmd, uint8_t *pDdata, uint8_t len);


extern uint8 UserBase1_TaskID;
extern uint8 UserBase2_TaskID;
extern uint8 UserBase3_TaskID;

#endif	





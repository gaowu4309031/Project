#ifndef __USERTASK_H__
#define __USERTASK_H__

#include "types.h"

// User Base Task Events
#define USER_TASK1_EVENT0								(1 << 0)
#define USER_TASK1_EVENT1								(1 << 1)
#define USER_TASK1_EVENT2								(1 << 2)

#define USER_TASK2_EVENT0								(1 << 0)
#define USER_TASK2_EVENT1								(1 << 1)
#define USER_TASK2_EVENT2								(1 << 2)


























void UserBase1_Init( uint8 task_id );
void UserBase2_Init( uint8 task_id );
uint16 UserBase1_ProcessEvent( uint8 task_id, uint16 events );
uint16 UserBase2_ProcessEvent( uint8 task_id, uint16 events );

#endif	





#ifndef ATTACKERS_H
#define ATTACKERS_H
#include "cmsis_os.h"
#include <game_engine.h>
#include <global.h>

extern const osThreadAttr_t attackersTask_attributes;

osThreadId_t attackerTaskHandles_1;
osThreadId_t attackerTaskHandles_2;
osThreadId_t attackerTaskHandles_3;
osThreadId_t attackerTaskHandles_4;
osThreadId_t attackerTaskHandles_5;

void StartAttackerTask_1(void *argument);
void StartAttackerTask_2(void *argument);
void StartAttackerTask_3(void *argument);
void StartAttackerTask_4(void *argument);
void StartAttackerTask_5(void *argument);

#endif
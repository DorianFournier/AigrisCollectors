#ifndef EXPLORATORS_H
#define EXPLORATORS_H
#include "cmsis_os.h"
#include <game_engine.h>
#include <global.h>

#undef DEBUG_EX

extern const osThreadAttr_t exploratorsTask_attributes;

osThreadId_t exploratorTaskHandles_1;
osThreadId_t exploratorTaskHandles_2;

void StartExploratorTask_1(void *argument);
void StartExploratorTask_2(void *argument);

#endif
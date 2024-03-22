#ifndef GLOBAL_H
#define GLOBAL_H

#include "../include/constants.h"
#include "../include/hardware.h"
#include "cmsis_os.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define OS_DELAY 100

osMutexId_t uartMutex_M;

extern const osMutexAttr_t uartMutex_attributes;

bool uart_data_available();
bool send_command(const char *command, char *response_buffer);
char *getsMutex(char *text);
void putsMutex(char *text);
char *osStatusToString(osStatus_t status);

#endif
#include "OS_engine.h"
#include "../include/hardware.h"
#include "OS_types.h"
#include <constants.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool is_comptetion_started = false;
bool rx_command_received = false;
char rx_command_buffer[RX_COMMAND_BUFFER_SIZE] = {""};

static os_mutex_id uart_mutex_id;
static os_mutex_id game_data_mutex_id;
static os_mutex_id game_state_mutex_id;

void os_engine_init(void) {
  const os_mutex_attr uart_mutex_attributes = {
      .name = "uartMutex", osMutexPrioInherit, NULL, 0U};

  uart_mutex_id = os_create_mutex(uart_mutex_attributes);

  const os_mutex_attr game_data_mutex_attributes = {
      .name = "dataMutex", osMutexPrioInherit, NULL, 0U};

  game_data_mutex_id = os_create_mutex(game_data_mutex_attributes);

  const os_mutex_attr game_state_mutex_attributes = {
      .name = "gameState", osMutexPrioInherit, NULL, 0U};

  game_state_mutex_id = os_create_mutex(game_state_mutex_attributes);
}

void wait_start(void) {
  while (!is_comptetion_started) {
    osDelay(OS_DELAY);
  }
}

os_mutex_id os_create_mutex(const os_mutex_attr mutex_attribute) {
  os_mutex_id mutex = osMutexNew(&mutex_attribute);

  if (mutex == NULL)
    while (1)
      puts("ERROR CREATING MUTEX\n");

  return mutex;
}

void os_acquire_mutex(os_mutex_id mutex_id, uint32_t timeout) {
  osStatus_t aquire_status = osMutexAcquire(mutex_id, timeout);

  if (aquire_status != osOK)
    while (1) {
      puts("ERROR ACQUIRE\n");
    }
}

void os_release_mutex(os_mutex_id mutex_id) {
  osStatus_t release_status = osMutexRelease(mutex_id);

  if (release_status != osOK)
    while (1) {
      puts("ERROR RELEASE\n");
    }
}

void os_acquire_game_data_mutex(void) {
  os_acquire_mutex(game_data_mutex_id, os_wait_forever);
}

void os_release_game_data_mutex(void) { os_release_mutex(game_data_mutex_id); }

void os_acquire_game_state_mutex(void) {
  os_acquire_mutex(game_state_mutex_id, osWaitForever);
}

void os_release_game_state_mutex(void) {
  os_release_mutex(game_state_mutex_id);
}

void send_command(char *command, char *response_buffer) {
  os_acquire_mutex(uart_mutex_id, osWaitForever);
  puts(command);

  while (rx_command_received == false) {
    osDelay(OS_DELAY); // Let some time
  }

  if (strstr(rx_command_buffer, "OK") != NULL) {
    rx_command_received = false;
    memset(rx_command_buffer, 0, sizeof(rx_command_buffer));
    os_release_mutex(uart_mutex_id);
  } else if (strstr(rx_command_buffer, "KO") != NULL) {
    rx_command_received = false;
    memset(rx_command_buffer, 0, sizeof(rx_command_buffer));
    os_release_mutex(uart_mutex_id);
  } else {
    rx_command_received = false;

    strncpy(response_buffer, rx_command_buffer, sizeof(rx_command_buffer) - 1);
    response_buffer[sizeof(rx_command_buffer) - 1] = '\0';

    memset(rx_command_buffer, 0, sizeof(rx_command_buffer));
    os_release_mutex(uart_mutex_id);
  }
  osDelay(OS_DELAY); // TODO Why ?
}

// TODO remove os_mutex_id parameters for putsMutex
void putsMutex(char *text) {
  os_acquire_mutex(uart_mutex_id, osWaitForever);
  puts(text);
  os_release_mutex(uart_mutex_id);
}

char *getsMutex(char *text) {
  char *original_str = text;
  os_acquire_mutex(uart_mutex_id, osWaitForever);
  gets(text);
  os_release_mutex(uart_mutex_id);
  return original_str;
}

void read_game_data_mutex() {
  os_acquire_mutex(game_data_mutex_id, osWaitForever);
}
uint32_t getFreeStackSpace(os_thread_id thread_id) {
  return osThreadGetStackSpace(thread_id);
}

uint32_t getStackSize(os_thread_id thread_id) {
  uint32_t task_size = TASKS_SIZES;
  if (strstr(osThreadGetName(thread_id), "exploratorsTask") != NULL) {
    task_size = TASKS_SIZES_EXPLORER;
  }
  return task_size;
}

uint32_t getUsedStackSpace(os_thread_id thread_id) {
  return (getStackSize(thread_id) - getFreeStackSpace(thread_id));
}

void printTaskInformation(os_thread_id thread_id) {

  const char *thread_name =
      osThreadGetName(thread_id); // Get the thread name pointer

  char buff_information[30] = {0};
  sprintf(buff_information,
          // "%s : free stack space : %ld/%ld \n"
          "%s : %ld/%ld \n", thread_name, getUsedStackSpace(thread_id),
          getStackSize(thread_id));
  putsMutex(buff_information);
}

void printTotalRAMUsage(void) {
  char buff_information[200] = {0};
  sprintf(buff_information, "TOTAL RAM USAGE : %d/%d \n",
          (configTOTAL_HEAP_SIZE - xPortGetFreeHeapSize()),
          configTOTAL_HEAP_SIZE);
  putsMutex(buff_information);
}

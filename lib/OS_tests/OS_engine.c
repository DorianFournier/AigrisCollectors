#include "OS_engine.h"
#include <stdio.h>

void send_command(char *command, char *response_buffer) {
  printf("%s", command);
}

void os_acquire_mutex(os_mutex_id mutex_id, uint32_t timeout) {}

void os_release_mutex(os_mutex_id mutex_id) {}

void os_acquire_game_data_mutex(void) {}

void os_release_game_data_mutex(void) {}

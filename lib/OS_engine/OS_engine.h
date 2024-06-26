#ifndef OS_ENGINE_H
#define OS_ENGINE_H

#include "OS_types.h"
#include <constants.h>
#include <stdbool.h>

extern bool is_comptetion_started;
extern bool rx_command_received;
extern char rx_command_buffer[RX_COMMAND_BUFFER_SIZE];

void os_puts_mutex(char *text);
char *getsMutex(char *text);

void os_delay(uint32_t os_delay);

void send_command(char *command);
void send_command_radar(char *command, char *response_buffer);
void wait_start(void);

void os_engine_init(void);
os_mutex_id os_create_mutex(const os_mutex_attr mutex_attribute);
void os_acquire_mutex(os_mutex_id mutex_id, uint32_t timeout);
void os_release_mutex(os_mutex_id mutex_id);
void os_firing_timer_start(os_timer_id timer_id, uint32_t ticks);
os_timer_id os_timer_new(uint8_t ship_id, void *os_firing_timer_callback,
                         uint8_t attacker_id, void *game_data);
void release_game_data_mutex(void);
void aquire_game_data_mutex(void);

uint32_t getFreeStackSpace(os_thread_id os_thread_id);
void getUsedStackSpace(os_thread_id os_thread_id);
uint32_t getStackSize(os_thread_id os_thread_id);

#endif

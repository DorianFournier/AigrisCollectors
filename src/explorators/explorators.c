#include "explorators.h"

uint8_t nb_planets = 0;
uint8_t nb_ship = 0;

const osThreadAttr_t exploratorsTask_attributes = {
    .name = "exploratorsTask",
    .priority = (osPriority_t)osPriorityNormal,
    .stack_size = TASKS_SIZES,
};

void StartExploratorTask_1(void *argument) {
  while (1) {
    if (is_comptetion_started == true) {
    }
    osDelay(OS_DELAY);
  }
}

void StartExploratorTask_2(void *argument) {
  while (1) {
    if (is_comptetion_started == true) {
    }
    osDelay(OS_DELAY);
  }
}
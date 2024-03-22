#include "attackers.h"

const osThreadAttr_t attackersTask_attributes = {
    .name = "attackersTask",
    .priority = (osPriority_t)osPriorityNormal,
    .stack_size = TASKS_SIZES,
};

void StartAttackerTask_1(void *argument) {
  while (1) {
    if (is_comptetion_started == true) {
    }
    osDelay(OS_DELAY);
  }
}

void StartAttackerTask_2(void *argument) {
  while (1) {
    if (is_comptetion_started == true) {
    }
    osDelay(OS_DELAY);
  }
}

void StartAttackerTask_3(void *argument) {
  while (1) {
    if (is_comptetion_started == true) {
    }
    osDelay(OS_DELAY);
  }
}

void StartAttackerTask_4(void *argument) {
  while (1) {
    if (is_comptetion_started == true) {
      // buffer_attacker_4 = test_ship(4, buffer_attacker_4);
      // putsMutex(buffer_attacker_4);
    }
    osDelay(OS_DELAY);
  }
}

void StartAttackerTask_5(void *argument) {
  while (1) {
    if (is_comptetion_started == true) {
      // buffer_attacker_5 = test_ship(5, buffer_attacker_5);
      // putsMutex(buffer_attacker_5);
    }
    osDelay(OS_DELAY);
  }
}
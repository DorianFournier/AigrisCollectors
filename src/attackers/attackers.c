#include "attackers.h"
#include <OS_engine.h>

const osThreadAttr_t attackersTask_attributes = {
    .name = "attackersTask",
    .priority = (osPriority_t)osPriorityNormal,
    .stack_size = TASKS_SIZES_ATTACKER,
};

void StartAttackerTask_1(void *argument) {
  wait_start();
  attacker_manager(ATTACKER_1);
}

void StartAttackerTask_2(void *argument) {
  wait_start();
  attacker_manager(ATTACKER_2);
}

void StartAttackerTask_3(void *argument) {
  wait_start();
  attacker_manager(ATTACKER_3);
}

void StartAttackerTask_4(void *argument) {
  wait_start();
  attacker_manager(ATTACKER_4);
}

void StartAttackerTask_5(void *argument) {
  wait_start();
  attacker_manager(ATTACKER_5);
}

#include "explorators.h"
#include <OS_engine.h>

const osThreadAttr_t exploratorsTask_attributes = {
    .name = "exploratorsTask",
    .priority = (osPriority_t)osPriorityNormal,
    .stack_size = TASKS_SIZES_EXPLORER,
};

void StartExploratorTask_1(void *argument) {
  wait_start();
  explorer_manager(EXPLORER_1);
}

void StartExploratorTask_2(void *argument) {
  wait_start();
  explorer_manager(EXPLORER_2);
}
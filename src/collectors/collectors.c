#include "collectors.h"
#include <OS_engine.h>

const osThreadAttr_t collectorsTask_attributes = {
    .name = "collectorsTask",
    .priority = (osPriority_t)osPriorityNormal,
    .stack_size = TASKS_SIZES_COLLECTOR,
};

void StartCollectorTask_1(void *argument) {
  wait_start();
  collector_manager(COLLECTOR_1);
}

void StartCollectorTask_2(void *argument) {
  wait_start();
  collector_manager(COLLECTOR_2);
}
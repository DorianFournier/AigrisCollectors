#include "collectors.h"

const osThreadAttr_t collectorsTask_attributes = {
    .name = "collectorsTask",
    .priority = (osPriority_t)osPriorityNormal,
    .stack_size = TASKS_SIZES,
};

void StartCollectorTask_1(void *argument) {
  while (1) {
    if (is_comptetion_started == true) {
      // buffer_collector_1 = test_ship(8, buffer_collector_1);
      // putsMutex(buffer_collector_1);
    }
    osDelay(OS_DELAY);
  }
}

void StartCollectorTask_2(void *argument) {
  T_point collector_ship;
  T_point planet_coo;
  char *buffer = create_buffer(BUFFER_SIZE);

  while (1) {
    if (is_comptetion_started == true) {
      collector_ship.pos_X = 4000;
      collector_ship.pos_Y = 5000;
      planet_coo.pos_X = 13000;
      planet_coo.pos_Y = 8000;
      putsMutex(move(9, test(collector_ship, planet_coo), 1000, buffer));
    }
    osDelay(OS_DELAY);
  }

  free_buffer(buffer);
}
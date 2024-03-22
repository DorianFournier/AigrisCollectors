#include "cmsis_os.h"
#include "hardware.h"
#include <math.h>
#include <stdbool.h>

#include <global.h>

#include "attackers/attackers.h"
#include "collectors/collectors.h"
#include "explorators/explorators.h"

osThreadId_t mainTaskHandle;
void StartMainTask(void *argument);

int main(void) {
  hardware_init();
  osKernelInitialize();

  const osThreadAttr_t mainTask_attributes = {
      .name = "mainTask",
      .priority = (osPriority_t)osPriorityNormal1,
      .stack_size = TASKS_SIZES,
  };
  mainTaskHandle = osThreadNew(StartMainTask, NULL, &mainTask_attributes);

  attackerTaskHandles_1 =
      osThreadNew(StartAttackerTask_1, NULL, &attackersTask_attributes);
  attackerTaskHandles_2 =
      osThreadNew(StartAttackerTask_2, NULL, &attackersTask_attributes);
  attackerTaskHandles_3 =
      osThreadNew(StartAttackerTask_3, NULL, &attackersTask_attributes);
  attackerTaskHandles_4 =
      osThreadNew(StartAttackerTask_4, NULL, &attackersTask_attributes);
  attackerTaskHandles_5 =
      osThreadNew(StartAttackerTask_5, NULL, &attackersTask_attributes);

  exploratorTaskHandles_1 =
      osThreadNew(StartExploratorTask_1, NULL, &exploratorsTask_attributes);
  exploratorTaskHandles_2 =
      osThreadNew(StartExploratorTask_2, NULL, &exploratorsTask_attributes);

  collectorTaskHandles_1 =
      osThreadNew(StartCollectorTask_1, NULL, &collectorsTask_attributes);
  collectorTaskHandles_2 =
      osThreadNew(StartCollectorTask_2, NULL, &collectorsTask_attributes);

  uartMutex_M = osMutexNew(&uartMutex_attributes);

  char *buffer = create_buffer(BUFFER_SIZE);

  while (is_comptetion_started != true) {
    gets(buffer);

    if (strstr(buffer, "START") != NULL) {
#ifdef DEBUG
      puts("BOOTED");
#endif
      break;
    }
  }

  is_comptetion_started = true;
  free_buffer(buffer);
  osKernelStart();
}

void StartMainTask(void *argument) {
#ifdef DEBUG
  putsMutex("Main Thread Task started\n");
#endif
  while (1) {
    osDelay(1);
  }
}
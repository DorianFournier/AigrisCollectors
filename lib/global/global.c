#include "global.h"
#include "stm32f4xx_hal_uart.h"

const osMutexAttr_t uartMutex_attributes = {
    .name = "uartMutex", osMutexPrioInherit, NULL, 0U};

void putsMutex(char *text) {
  if (uartMutex_M != NULL) {
    osStatus_t aquire_status = osMutexAcquire(uartMutex_M, osWaitForever);
    if (aquire_status != osOK) {
#ifdef DEBUG
      puts("osMutexAcquire failed :");
      puts(osStatusToString(aquire_status));
#endif
      return;
    }
    puts(text);
    osStatus_t release_status = osMutexRelease(uartMutex_M);
    if (release_status != osOK) {
#ifdef DEBUG
      puts("osMutexrelease failed :");
      puts(osStatusToString(release_status));
#endif
      return;
    }
  }
}

char *getsMutex(char *text) {
  char *original_str = text;
  if (uartMutex_M != NULL) {
    osStatus_t aquire_status = osMutexAcquire(uartMutex_M, osWaitForever);
    if (aquire_status != osOK) {
#ifdef DEBUG
      puts("osMutexAcquire failed :");
      puts(osStatusToString(aquire_status));
#endif
      return NULL;
    }

    gets(text);

    osStatus_t release_status = osMutexRelease(uartMutex_M);
    if (release_status != osOK) {
#ifdef DEBUG
      puts("osMutexrelease failed :");
      puts(osStatusToString(release_status));
#endif
      return NULL;
    }
  }
  return original_str;
}

bool uart_data_available() {
  return (__HAL_UART_GET_FLAG(&huart2, UART_FLAG_RXNE) != RESET);
}

bool send_command(const char *command, char *response_buffer) {
  if (uartMutex_M != NULL) {
    osStatus_t aquire_status = osMutexAcquire(uartMutex_M, osWaitForever);
    if (aquire_status != osOK)
      return false;

    if (puts(command) < 0) {
#ifdef DEBUG
      puts("Error sending command");
#endif
      return false;
    }

    char *response = gets(response_buffer);
    if (response == NULL) {
#ifdef DEBUG
      puts("Error receiving response");
#endif
      return false;
    }
    osStatus_t release_status = osMutexRelease(uartMutex_M);
    if (release_status != osOK) {
#ifdef DEBUG
      puts("osMutexRelease failed :");
      puts(osStatusToString(aquire_status));
#endif
      return false;
    }
  }
  return true;
}

char *osStatusToString(osStatus_t status) {
  switch (status) {
  case osOK:
    return "osOK";
  case osError:
    return "osError";
  case osErrorTimeout:
    return "osErrorTimeout";
  case osErrorResource:
    return "osErrorResource";
  case osErrorParameter:
    return "osErrorParameter";
  case osErrorNoMemory:
    return "osErrorNoMemory";
  case osErrorISR:
    return "osErrorISR";
  case osStatusReserved:
    return "osStatusReserved";
  default:
    return "Unknown Status";
  }
}

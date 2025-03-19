#ifndef GLOBALS_H
#define GLOBALS_H

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

extern SemaphoreHandle_t i2c_mutex;

#endif // GLOBALS_H
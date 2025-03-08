#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>


// Tracing tag
#define TAG "mutex_helper.h"

// Max delay to wait for mutex
#define MAX_DELAY 1000

static SemaphoreHandle_t display_mutex = nullptr;

inline bool init_mutex() {
  if (display_mutex == nullptr) {
    display_mutex = xSemaphoreCreateMutex();
    if (display_mutex != nullptr) {
      ESP_LOGI(TAG, "Mutex created successfully");
      return true;
    } else {
      ESP_LOGE(TAG, "Failed to create mutex");
    }
  }
  return false;
}

inline bool lock_mutex() {
  ESP_LOGI(TAG, "Locking mutex...");
  if (display_mutex != nullptr) {
    if (xSemaphoreTake(display_mutex, pdMS_TO_TICKS(MAX_DELAY))) {
      ESP_LOGI(TAG, "Mutex locked");
      return true;
    } else {
      ESP_LOGI(TAG, "Mutex could not be locked - timeout");
    }
  } else {
    ESP_LOGE(TAG, "Mutex is null");
  }
  return false;
}

inline bool unlock_mutex() {
  ESP_LOGI(TAG, "Unlocking mutex...");
  if (display_mutex != nullptr) {
    xSemaphoreGive(display_mutex);
    ESP_LOGI(TAG, "Mutex unlocked");
    return true;
  } else {
    ESP_LOGE(TAG, "Mutex is null");
  }
  return false;
}

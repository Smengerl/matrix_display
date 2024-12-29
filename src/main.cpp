/*
 * LED Matrix clock based on ESP32 with Wifi+NTP sync
 */
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <WiFiManager.h>
#include <WiFi.h>
#include "ESPDateTime.h"
#include "config.h"
#include "esp_bt.h"


// ***********************************************************
// NTP SETTINGS
// ***********************************************************
// Servername NTP
#define NTP_SERVER "pool.ntp.org"

// Timezone
#define TIMEZONE "CET-1CEST,M3.5.0,M10.5.0/3"

// Adapt to a reasonable timeout to try to sync with NTP server (full roundtrip time),
// however, in current simple implementation, this affects framerate of display refresh during sync
#define NTP_SYNC_TIMEOUT 400


// ***********************************************************
// DISPLAY SETTINGS
// ***********************************************************
// LED matrix intensity from 0...0x0f (brightest)
#define LED_INTENSITY 0x00
// LED matrix inverted?
#define INVERT false

// Animation for old time to disappear
#define OUT_ANIMATION_SPEED 50
#define OUT_ANIMATION_MODE PA_SCROLL_UP

// Animaton for new time to appear
#define IN_ANIMATION_SPEED 50
#define IN_ANIMATION_MODE PA_SCROLL_UP

// Animation for all messages to user on display
#define DEFAULT_ANIMATION_SPEED 50
#define DEFAULT_ANIMATION_STOP 1500


// ***********************************************************
// HW SETTINGS
// ***********************************************************
// GPIO-Pins
#define PIN_NUM_MOSI GPIO_NUM_23
#define PIN_NUM_CLK  GPIO_NUM_5
#define PIN_NUM_CS   GPIO_NUM_21

// HW setup for 4 digit blue LED matrix
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW 
#define MAX7219_NUM_DEVICES 4 

// Time in ms for sleep cycle
#define DEEP_SLEEP_CYCLE 1000






MD_Parola* ledMatrix;




void setupMatrixDisplay() {
  // initialize the object
  ledMatrix = new MD_Parola(HARDWARE_TYPE, PIN_NUM_MOSI, PIN_NUM_CLK, PIN_NUM_CS, MAX7219_NUM_DEVICES);
  ledMatrix->begin();         
  ledMatrix->setIntensity(LED_INTENSITY); // set the brightness of the LED matrix display (from 0 to 15)
  ledMatrix->setInvert(INVERT);
  ledMatrix->displayClear();  // clear led matrix display
/*
  // Add custom characters to the MD_Parola instance
  ledMatrix->addChar('(', battery_begin);
  ledMatrix->addChar(')', battery_end);
  ledMatrix->addChar('_', battery_segment_empty);
  ledMatrix->addChar('|', battery_segment_full);
  ledMatrix->setCharSpacing(0);
*/
}

/*
void connectWiFi() {
  ledMatrix->displayText("WiFi Init", PA_CENTER, 50, 1000, PA_SCROLL_LEFT, PA_FADE);
  Serial.print("WiFi connecting... ");

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    if (ledMatrix->displayAnimate()) {
      ledMatrix->displayReset();
    }
    Serial.print(".");
//    vTaskDelay(pdMS_TO_TICKS(100));
  }
  Serial.println(" connected.\n");
}
*/

void setupDateTime() {
  ledMatrix->displayText("NTP Sync", PA_CENTER, DEFAULT_ANIMATION_SPEED, DEFAULT_ANIMATION_STOP, PA_SCROLL_LEFT, PA_FADE);
  ledMatrix->displayAnimate();

  Serial.print("NTP synching, please wait... ");

  DateTime.setServer(NTP_SERVER);
  DateTime.setTimeZone(TIMEZONE);

  while (!DateTime.begin(NTP_SYNC_TIMEOUT)) {
    if (ledMatrix->displayAnimate()) {
      ledMatrix->displayReset();
    }
    Serial.print(".");
  }
  Serial.println("done");

  Serial.printf("Date Now is %s\n", DateTime.toISOString().c_str());
  Serial.printf("Timestamp is %ld\n", DateTime.now());
}


void setupWiFi() {
  //WiFiManager, Local variable sufficient as once its business is done, there is no need to keep it any longer
  WiFiManager wm;

  Serial.println("WiFi connecting... ");

  char buf[100];
  snprintf(buf, sizeof(buf), "To setup Wifi connect to: %s", wm.getDefaultAPName().c_str());
  ledMatrix->displayText(buf, PA_CENTER, DEFAULT_ANIMATION_SPEED, DEFAULT_ANIMATION_STOP, PA_SCROLL_LEFT, PA_FADE);

  // reset settings - wipe stored credentials for testing
  // these are stored by the esp library
  // wm.resetSettings();

  wm.setConfigPortalBlocking(false); // asynchroneous mode in order to display the message on the LED matrix
  if (!wm.autoConnect()) { // Try to reconnect to last WiFi or open non password protected AP with default name
    while (!wm.process()) {
      if (ledMatrix->displayAnimate()) {
        ledMatrix->displayReset();
      }
      Serial.print(".");
    }
    Serial.println("WiFi successfully connected (using manual config in AP portal)");
  } else {
    Serial.println("WiFi successfully connected (using saved credentials)");
  }
}

void disableWiFi() {
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  Serial.println("WiFi disabled");
}

// Function to disable Bluetooth
void disableBluetooth() {
  if (esp_bt_controller_get_status() == ESP_BT_CONTROLLER_STATUS_ENABLED) {
      esp_err_t ret = esp_bt_controller_disable();
      if (ret != ESP_OK) {
          Serial.printf("Bluetooth disable failed: %s\n", esp_err_to_name(ret));
          return;
      }
  }

  if (esp_bt_controller_get_status() == ESP_BT_CONTROLLER_STATUS_INITED) {
      esp_err_t ret = esp_bt_controller_deinit();
      if (ret != ESP_OK) {
          Serial.printf("Bluetooth deinit failed: %s\n", esp_err_to_name(ret));
          return;
      }
  }

  Serial.println("Bluetooth disabled");
}



const uint8_t battery_begin[] = {
  3,
  0b00111100,
  0b11111111,
  0b10000001,
};
const uint8_t battery_end[] = {
  1,
  0b01111110
};
const uint8_t battery_segment_charging[] = {
  4,
  0b10000001,
  0b10011101,
  0b10010001,
  0b10111001,
};
const uint8_t battery_segment_empty[] = {
  4,
  0b10000001,
  0b10000001,
  0b10000001,
  0b10000001,
};
const uint8_t battery_segment_full[] = {
  4,
  0b10111101,
  0b10111101,
  0b10111101,
  0b10000001,
};



void setup() {
  Serial.begin(115200);

  disableBluetooth();

  setupMatrixDisplay();

  // Sync with NTP only on initial startup. After that, power off Wifi to save energy
  setupWiFi();
  setupDateTime();
  disableWiFi();
}





void sleepForNextMinute() {
  // Sleep until next full minute
  time_t now = DateTime.now();
  struct tm *tm = localtime(&now);
  long sleepTime = (60 - tm->tm_sec) * 1000; // ms

  // Implement deep sleep functionality here
  Serial.printf("\nEntering sleep for %dms...\n", sleepTime);

  // Advise FreeRTOS to wait for up to this amount of time
  vTaskDelay(pdMS_TO_TICKS(sleepTime));

// Using deep sleep requires some rework as all initialization would need to be redone and setup routine is called again on wakeup
//  esp_sleep_enable_timer_wakeup( * 1000);
//  esp_deep_sleep_start();
}


int lastMin = -1;
int lastHour = -1;
char time_str[20] = ""; // HH:MM format


void loop() {
  time_t now = DateTime.now();
  struct tm *tm = localtime(&now);

  // Adjust for daylight saving time if necessary
  if (tm->tm_isdst > 0) {
    tm->tm_hour += 1;
    mktime(tm); // Normalize the time structure
  }


  if (tm->tm_min != lastMin || tm->tm_hour != lastHour ) {
    lastMin = tm->tm_min;
    lastHour = tm->tm_hour;

    ledMatrix->displayText(time_str, PA_CENTER, OUT_ANIMATION_SPEED, 0, PA_NO_EFFECT, OUT_ANIMATION_MODE);
    Serial.print("\nOut animation");
    do {
      Serial.print(".");
    } while (!ledMatrix->displayAnimate());


//    strftime(time_str, sizeof(time_str), "(|||____)", tm);
    strftime(time_str, sizeof(time_str), "%H:%M", tm);
    Serial.printf("\nCurrent time: %s", time_str);

    ledMatrix->displayText(time_str, PA_CENTER, IN_ANIMATION_SPEED, 0, IN_ANIMATION_MODE, PA_NO_EFFECT);
    Serial.print("\nIn animation");
    do {
      Serial.print(".");
    } while (!ledMatrix->displayAnimate());

  } else {
    Serial.print("Loop function called but no further minute has passed. Is sleep cycle correctly configured?\n");
  }

  sleepForNextMinute();
}


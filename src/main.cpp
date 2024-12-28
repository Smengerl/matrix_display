/*
 * LED Matrix clock based on ESP32 with Wifi+NTP sync
 */
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <WiFiManager.h>
#include <WiFi.h>
#include "ESPDateTime.h"
#include "config.h"


// Servername NTP
#define NTP_SERVER "pool.ntp.org"

// LED matrix intensity from 0...0x0f (brightest)
#define LED_INTENSITY 0x00
// LED matrix inverted?
#define INVERT false


// GPIO-Pins
#define PIN_NUM_MOSI GPIO_NUM_23
#define PIN_NUM_CLK  GPIO_NUM_5
#define PIN_NUM_CS   GPIO_NUM_21

// HW setup for 4 digit blue LED matrix
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW 
#define MAX7219_NUM_DEVICES 4 



#define TIMEZONE "CET-1CEST,M3.5.0,M10.5.0/3"


#define OUT_ANIMATION_SPEED 50
#define OUT_ANIMATION_MODE PA_SCROLL_UP

#define IN_ANIMATION_SPEED 50
#define IN_ANIMATION_MODE PA_SCROLL_UP


// Animation for all messages to user on display
#define DEFAULT_ANIMATION_SPEED 50
#define DEFAULT_ANIMATION_STOP 1500





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
  ledMatrix->displayText("NTP Synching, please wait", PA_CENTER, DEFAULT_ANIMATION_SPEED, DEFAULT_ANIMATION_STOP, PA_SCROLL_LEFT, PA_FADE);
  ledMatrix->displayAnimate();

  Serial.print("NTP synching... ");

  DateTime.setServer(NTP_SERVER);
  DateTime.setTimeZone(TIMEZONE);

  while (!DateTime.begin(DEFAULT_ANIMATION_SPEED)) {
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

  setupMatrixDisplay();
  setupWiFi();
  setupDateTime();
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
    Serial.print("\nIdling");
  }

  Serial.print(".");
  vTaskDelay(pdMS_TO_TICKS(1000));
}

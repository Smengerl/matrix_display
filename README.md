# Matrix_Clock

Matrix_Clock is a project that displays the current time on an 8x8 LED matrix display using an ESP32 microcontroller. The project uses the MD_Parola library to control the LED matrix and display custom characters, including a battery icon and digits.

## Features

- Displays the current time in HH:MM format
- 4 Digit LED matrix display (8x8 pixel)
- Customizable animations, dimming and 
- Automatic time synchronization using NTP

## Hardware Requirements

- ESP32 (housing designed for USB C version)
- 4 digit 8x8 LED matrix display based on MAX7219 (FC16)
- 3D printed housing
- 8 screws M3*10mm

## Software Requirements

- PlatformIO
- MD_Parola (via PIO)
- ESPDateTime (via PIO) 
- WiFiManager (via PIO)

## Installation

1. Clone the repository:
   ```sh
   git clone https://github.com/yourusername/Matrix_Clock.git
2. Open the project in PlatformIO.
3. Wait for PIO to configure and download the required libraries:
4. Connect the ESP32 to the 8x8 LED matrix display according to the wiring diagram.
5. Upload the project to your ESP32 microcontroller.

## Usage
- Power on the ESP32.
- On first usage only: Configure the ESP to your local WiFi. For this, connect to the ESP's access point and use the default configuration page to enter your WLAN SSID and password 
- Wait for NTP sync (may take up to a minute)
- The current time will be displayed on the LED matrix.

## License
This project is licensed under the MIT License. See the LICENSE file for details.

## Acknowledgements
MD_Parola library by MajicDesigns
pftime library

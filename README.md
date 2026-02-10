# Mousetrap

An ESP32-based mousetrap notification system. When a mouse is caught, the ESP32 wakes from deep sleep and sends an email notification with battery voltage information.

## Hardware

- ESP32 FireBeetle (DFRobot Firebeetle ESP32-D)
- Magnetic reed switch or pushbutton on GPIO 15 (wake-up trigger)
- Battery connected to ADC pin GPIO 36 (voltage divider, factor 2)
- Board voltage on ADC pin GPIO 34

## How it works

1. The ESP32 is in deep sleep, consuming ~13.7 µA
2. When the trap triggers, GPIO 15 goes LOW and wakes the ESP32
3. The ESP32 connects to WiFi, gets NTP time, and sends an email notification via Gmail SMTP
4. After sending (or failing), it goes back to deep sleep

## Setup

1. Install [Arduino IDE](https://www.arduino.cc/en/software) with ESP32 board support
2. Install the [ESP-Mail-Client](https://github.com/mobizt/ESP-Mail-Client) library
3. Copy `credentials.h.example` to `credentials.h` and fill in your WiFi and Gmail credentials
4. For Gmail, you need to create an [App Password](https://myaccount.google.com/apppasswords)
5. Select "ESP32 Dev Module" as board and upload

## License

MIT License - see [LICENSE](LICENSE) file.

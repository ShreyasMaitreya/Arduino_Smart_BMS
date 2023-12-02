# Battery Monitoring and Management System

## Overview
This repository contains Arduino code for a comprehensive Battery Monitoring and Management System (BMS). The system is designed to monitor multiple battery cells, calculate State of Charge (SoC) and State of Health (SoH), and perform data logging and remote data transmission.

## Features
- **SoC Calculation:** Measures and calculates the State of Charge of battery cells.
- **SoH Calculation:** Determines the State of Health based on the remaining capacity.
- **Data Logging:** Records parameters on an SD card.
- **Remote Data Transmission:** Sends data to a ThingSpeak channel for remote monitoring.
- **Cell Balancing:** Actively balances the cells to maintain uniform charge levels.
- **Coulomb Counting:** Essential for accurate SoH monitoring.

## Hardware Requirements
- Any Arduino compatible mucrocontroller
- SD card module for data logging
- Wi-Fi module for remote data transmission
- Voltage measurement setup for battery cells
- Digital pins for cell balancing control

## Dependencies
- Custom Scheduler Library: Download from [GitHub](https://github.com/mikaelpatel/Arduino-Scheduler)
- `ThingSpeak` library
- `WiFi` library for ESP8266/ESP32
- `SPI` and `SD` libraries for SD card operations
- `math.h` for mathematical functions

## Installation
1. Ensure all required libraries are installed in the Arduino IDE.
2. Connect the hardware as per the schematic (not provided in this README).
3. Configure the Wi-Fi credentials and ThingSpeak API details in the code.
4. Upload the code to your microcontroller.

## Configuration
- Update the `network_name` and `network_password` variables with your Wi-Fi credentials.
- Set the `channel_id` and `API_Key` for your ThingSpeak channel.
- Adjust `V1`, `V2`, `V3`, etc., pins according to your setup.
- Modify `capacity`, `Time`, and other constants as per your battery specifications.

## Usage
- The `setup()` function initializes the Wi-Fi connection, SD card, and sets up the pins.
- The `loop()` function manages the task scheduling for continuous monitoring and balancing of cells.
- Data is logged on the SD card and sent to ThingSpeak for remote monitoring.

## Contributing
Contributions to improve this project are welcome. Feel free to fork this repository and submit pull requests with your enhancements.

## Disclaimer
This code is provided "as is" and without any warranty. The author is not responsible for any damage caused by the use of this code.

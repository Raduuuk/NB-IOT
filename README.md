# NB-IoT Arduino Monitoring Project

## Overview

This project explores the integration of Narrowband IoT (NB-IoT) technology with Arduino for precise voltage and current measurement applications. By leveraging NB-IoT, the project aims to provide a robust IoT solution that enables real-time data transfer to cloud platforms, specifically Thingspeak. The project highlights the potential of NB-IoT in enhancing remote monitoring systems in IoT applications, particularly for environments requiring efficient and reliable sensor-based systems.

## Project Structure
The system setup includes an Arduino Mega microcontroller and the DFR0763 NB-IoT module for wireless data transmission. The circuit is designed to read analog voltage and current values, process them on the Arduino, and transmit the data to Thingspeak for analysis and visualization. The project code also includes the necessary configurations for Thingspeak's API and real-time clock (RTC) integration to timestamp data.

## Key Components
- Arduino Mega: Microcontroller used for analog data collection and processing.
- DFR0763 NB-IoT Module: Module enabling low-power, long-range data transmission over cellular networks.
- Thingspeak API: Cloud platform for IoT data collection, with an API write key for secure data transfer.
- RTC Module: Ensures accurate timestamping of each measurement.
## Key Features
- Precise Measurements: Captures voltage and current readings at 4ms intervals, optimized for real-time applications.
- NB-IoT Communication: Enables reliable data transmission using cellular networks.
- Cloud Integration: Seamless integration with Thingspeak for data visualization and analysis.
- Extensibility: The system can be adapted to other IoT applications requiring remote monitoring.
## Installation
### Libraries Required:

- `DFRobot_SIM7000`
- `SoftwareSerial`
- `Wire`
- `RTClib`
### Upload the Code:

- Ensure that all libraries are installed in your Arduino IDE.
- Update the `THINGSPEAK_API_KEY` with your own API write key.

# -GARDEN-AUTOMATION-WITH-FREERTOS
Smart irrigation and monitoring system with ESP32. Collects data from humidity, rain, temperature and cistern sensors and displays them on an OLED display. Integrates with OpenMeteo API and SinricPro for remote control and automation of irrigation based on weather conditions.

Project: Smart Monitoring and Irrigation System
Description
This project consists of an environmental monitoring and irrigation control system using the ESP32 platform. It collects data from soil moisture, rain, temperature, and humidity sensors, and controls irrigation based on the collected data and weather conditions obtained from the OpenMeteo API. Additionally, the system integrates with the SinricPro platform for data sending and remote control.

Features
Sensor Readings:

DHT11 sensor for temperature and humidity.
Soil moisture and rain sensors.
Water level sensor in the cistern.
Irrigation Control:

Irrigation is automatically controlled based on soil moisture data, rain presence, and weather conditions.
Scheduling for irrigation using the OpenMeteo API.
Data Display:

OLED display for showing soil moisture, temperature, air humidity, rain condition, and cistern level information.
Integration with SinricPro:

Sending temperature and humidity events to the SinricPro platform for remote monitoring.
Connectivity:

Wi-Fi connection for access to the OpenMeteo API and integration with SinricPro.
Code Structure
Hardware Configuration: Initialization of pins, sensors, and OLED display.
Multithreading Tasks: Using FreeRTOS to create independent tasks for sensor readings and irrigation control.
Taskdht11: Reading the DHT11 sensor.
TaskSolo: Reading the soil moisture sensor.
Taskchuva: Reading the rain sensor.
Taskcisterna: Reading the cistern level sensor.
OpenMeteoAPI: Fetching weather data from the OpenMeteo API.
Irrigation: Controlling irrigation based on collected data.
SinricPro Integration: Configuring and sending events to the SinricPro platform.
OLED Display: Functions to display collected information on the OLED display.
Usage
Hardware:

Connect the DHT11, soil moisture, rain, and cistern level sensors to the specified pins in the code.
Connect the relay module for irrigation control.
Software Configuration:

Configure Wi-Fi credentials and OpenMeteo and SinricPro API keys in the code.
Upload the code to the ESP32 board using the Arduino IDE.
Execution:

After uploading the code, the system will start collecting sensor data.
Data will be displayed on the OLED display and sent to the SinricPro platform.
Irrigation will be automatically controlled based on configured conditions and collected data.
Dependencies
Libraries: DHT, WiFi, HTTPClient, ArduinoJson, SPI, Wire, Adafruit_GFX, Adafruit_SSD1306, SinricPro.
Platform: ESP32.
This project provides a complete solution for environmental monitoring and irrigation control, ideal for precision agriculture and automated gardening.

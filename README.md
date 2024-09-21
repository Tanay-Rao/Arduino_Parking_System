# Smart Parking Management System

## Overview
This project implements a Smart Parking Management System using Arduino and various sensors to automate parking space detection and boom barrier control. It provides real-time updates on parking space availability and efficiently manages vehicle entry and exit.

## Features
- **Parking Space Detection**: Utilizes multiple HC-SR04 ultrasonic sensors to monitor the occupancy of three parking spaces.
- **Boom Barrier Control**: Automates the opening and closing of a boom barrier based on vehicle presence at entry and exit points.
- **LCD Display**: Shows the number of available parking spaces and which slots are free.
- **Serial Monitor Logs**: Outputs sensor readings and parking status to the Serial Monitor for debugging and monitoring.
- **Compact Design**: All components are integrated into a compact system suitable for demonstration and practical use.

## Components Used
- Arduino UNO
- HC-SR04 Ultrasonic Sensors (5 units)
- Servo Motor for boom barrier control
- LCD Display (I2C interface)
- Buzzer for audio alerts (optional)
- Jumper wires, breadboard, and other miscellaneous components

## Digital Pin Configuration
| Component                 | Pin Number |
|---------------------------|------------|
| HC-SR04 Sensor 1 (Trig)  | 5          |
| HC-SR04 Sensor 1 (Echo)  | 6          |
| HC-SR04 Sensor 2 (Trig)  | 7          |
| HC-SR04 Sensor 2 (Echo)  | 8          |
| HC-SR04 Sensor 3 (Trig)  | 9          |
| HC-SR04 Sensor 3 (Echo)  | 10         |
| Boom Barrier Entry (Trig) | 13         |
| Boom Barrier Entry (Echo) | 12         |
| Boom Barrier Exit (Trig)  | 11         |
| Boom Barrier Exit (Echo)  | 4          |
| Servo Motor               | 3          |

## Libraries Used
- `Wire.h`: For I2C communication with the LCD.
- `LiquidCrystal_I2C.h`: To control the I2C LCD display.
- `Servo.h`: To control the servo motor for the boom barrier.

## Getting Started
1. **Wiring**: Connect the ultrasonic sensors and servo motor to the specified pins on the Arduino.
2. **Upload the Code**: Use the Arduino IDE to upload the provided code to your Arduino UNO.
3. **Power the System**: Ensure the Arduino is powered and observe the LCD for updates on parking availability.

## Code Explanation
The provided code includes:
- Functions to measure distance using the HC-SR04 sensors.
- Logic to track the status of parking spaces.
- Management of the boom barrier based on vehicle detection at entry and exit points.
- Real-time updates displayed on an LCD and logged to the Serial Monitor.

## Contribution
Feel free to fork the repository, make enhancements, or report issues. Contributions are welcome!

## License
This project is licensed under the MIT License.

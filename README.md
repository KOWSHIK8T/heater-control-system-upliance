# Heater Control System using ESP32

This project is a simple yet effective Heater Control System built using an ESP32 and a DS18B20 temperature sensor. It's designed to automatically maintain a target temperature, with built-in safety features to prevent overheating.

This repository contains the full source code and documentation for the project.

**Wokwi Simulation Link:** [Click here to see it run in a simulator!](https://wokwi.com/projects/436891097299493889)

**Hardware Demo Video:** [Watch the real prototype in action!](https://drive.google.com/file/d/1sPjoh1X56OnBt9ygG4l6JknpRuhZzQ5J/view?usp=drivesdk)

![Hardware Prototype](https://i.imgur.com/your-image-url.png)  <!-- Optional: Upload your prototype image to a service like Imgur and paste the link here -->

---

## Features

*   **Automatic Temperature Control**: Turns a heater on or off to maintain a set temperature.
*   **State Machine Logic**: Uses a clear and reliable state machine (IDLE, HEATING, STABILIZING, etc.) to manage its operations.
*   **Safety First**: Includes critical safety features like an overheat threshold that triggers an alarm and shuts the heater down.
*   **Hysteresis**: Prevents the heater from rapidly switching on and off around the target temperature.
*   **Visual & Audible Feedback**: Uses LEDs and a buzzer to clearly show the system's current status and any warnings.
*   **Sensor Health Checks**: Can detect if the temperature sensor is disconnected or faulty and enter a failsafe mode.

---

## Hardware Required

*   ESP32 Development Board
*   DS18B20 Temperature Sensor
*   1 x Heater element (or an LED to simulate it)
*   3 x LEDs (for status indicators: Heating, Target Reached, Overheat)
*   1 x Piezo Buzzer
*   1 x 4.7kΩ Resistor (as a pull-up for the DS18B20 sensor)
*   Breadboard and Jumper Wires

---

## Software & Libraries

This project was built using the Arduino IDE. You will need to install the following libraries:

*   **`OneWire`** by Paul Stoffregen
*   **`DallasTemperature`** by Miles Burton & others
*   **`Ticker`** (comes pre-installed with the ESP32 core for Arduino)

---

## How It Works

The system operates based on a state machine. The current state is determined by the temperature reading from the DS18B20 sensor.

1.  **`IDLE`**: The system is waiting. If the temperature drops too low, it moves to the `HEATING` state.
2.  **`HEATING`**: The heater is on. It stays in this state until the target temperature is reached, then moves to `STABILIZING`.
3.  **`STABILIZING`**: The heater is turned off and the system waits for 30 seconds to ensure the temperature doesn't drop too quickly. If it remains stable, it moves to `TARGET_REACHED`.
4.  **`TARGET_REACHED`**: The target temperature has been successfully reached and is stable. The system monitors the temperature and will go back to `HEATING` if it gets too cold.
5.  **`OVERHEAT`**: A failsafe state. If the temperature exceeds the safety threshold (42°C), the heater is immediately shut down and an alarm sounds.

---

## How to Set Up

1.  **Connect the Hardware**: Wire up the components on a breadboard according to the pin definitions at the top of the `.ino` file.
2.  **Install Libraries**: Open the Arduino IDE, go to `Sketch > Include Library > Manage Libraries...` and install the libraries listed above.
3.  **Open the Code**: Open the `Heater_Control_System.ino` file in the Arduino IDE.
4.  **Configure Board**: Select your ESP32 board from the `Tools > Board` menu.
5.  **Upload**: Connect your ESP32 to your computer, select the correct COM port, and click the "Upload" button.
6.  **Monitor**: Open the Serial Monitor (`Tools > Serial Monitor`) with the baud rate set to `115200` to see status updates and logs.

---

## Future Ideas

This project has a solid foundation. Here are some ideas for future improvements:

*   **Add a second sensor** for redundancy and better safety.
*   **Implement remote control** and monitoring via Wi-Fi or Bluetooth Low Energy (BLE).
*   **Create different heating profiles** (e.g., "Day," "Night," "Eco-Mode").
*   **Integrate with smart home systems** like Google Home or Alexa.

# ESP32 Ultrasonic Proximity Alert

An ESP32-C3 proximity monitoring system using an HC-SR04 ultrasonic sensor, a 0.96-inch SSD1306 OLED display, and an active buzzer.

The system measures the approximate distance of an object, displays the result on the OLED, and produces progressively faster warning sounds as the object moves closer.

## Project Overview

This project was created to practise embedded programming, ultrasonic sensor testing, GPIO control, OLED integration, voltage-level protection, alarm logic, and technical documentation.

The HC-SR04 measures distance using ultrasonic sound pulses. The ESP32 calculates the object distance from the echo duration and displays the result on the OLED.

## Features

- Measures object distance in centimetres
- Displays live distance on a 128 × 64 OLED
- Displays proximity status
- Shows a graphical distance bar
- Produces slow warning beeps when an object is near
- Produces fast warning beeps when an object is very near
- Produces a continuous alarm when an object is too close
- Filters readings to reduce display jumping
- Detects invalid or missing sensor readings
- Protects the ESP32 ECHO input with a voltage divider

## Hardware Used

- ESP32-C3 SuperMini
- HC-SR04 ultrasonic sensor
- 0.96-inch SSD1306 OLED display
- Active buzzer
- 1 kΩ resistor
- 2 kΩ resistor
- Breadboard
- Jumper wires
- USB cable

## Software and Libraries

- Arduino IDE
- Arduino C++
- Wire library
- Adafruit GFX Library
- Adafruit SSD1306
- Adafruit BusIO

## Wiring

### OLED Display

| OLED Pin | ESP32-C3 |
|---|---|
| VCC | 3.3V |
| GND | GND |
| SDA | GPIO 8 |
| SCL | GPIO 9 |

### HC-SR04 Ultrasonic Sensor

| HC-SR04 Pin | ESP32-C3 |
|---|---|
| VCC | 5V |
| GND | GND |
| TRIG | GPIO 4 |
| ECHO | GPIO 5 through voltage divider |

### Active Buzzer

| Buzzer Pin | ESP32-C3 |
|---|---|
| VCC | 3.3V |
| GND | GND |
| Signal | GPIO 6 |

## ECHO Voltage Divider

The HC-SR04 ECHO pin can output approximately 5V.

Because ESP32 GPIO pins use 3.3V logic, a voltage divider is used to reduce the ECHO voltage before it reaches GPIO 5.

Connections:

    HC-SR04 ECHO
          |
         1 kΩ
          |
          +------ ESP32 GPIO 5
          |
         2 kΩ
          |
         GND

The 1 kΩ resistor is placed between ECHO and GPIO 5.

The 2 kΩ resistor is placed between GPIO 5 and GND.

## Pin Configuration

| Function | ESP32-C3 Pin |
|---|---|
| OLED SDA | GPIO 8 |
| OLED SCL | GPIO 9 |
| HC-SR04 TRIG | GPIO 4 |
| HC-SR04 ECHO | GPIO 5 |
| Buzzer signal | GPIO 6 |

## Distance Warning Levels

| Distance | OLED Status | Buzzer Behaviour |
|---|---|---|
| Above 40 cm | CLEAR | Silent |
| 20–40 cm | NEAR | Slow beeping |
| 10–20 cm | VERY NEAR | Fast beeping |
| Below 10 cm | TOO CLOSE | Continuous alarm |

## How It Works

1. The ESP32 sends a short pulse from the HC-SR04 TRIG pin.
2. The ultrasonic sensor transmits a sound pulse.
3. The sound reflects from an object.
4. The HC-SR04 returns an ECHO pulse.
5. The ESP32 measures the ECHO duration.
6. The distance is calculated from the speed of sound.
7. The result is smoothed to reduce rapid changes.
8. The OLED displays distance and proximity status.
9. The buzzer behaviour changes according to the measured distance.

## Distance Calculation

The distance is calculated using:

    distance = echo duration × 0.0343 / 2

The value is divided by two because the ultrasonic pulse travels to the object and then returns to the sensor.

## OLED Output

Example:

    OBJECT DISTANCE
    ----------------

         24.7 cm

    Status: NEAR

    [distance bar]

## Installation

Install the required libraries through:

    Arduino IDE → Sketch → Include Library → Manage Libraries

Install:

- Adafruit GFX Library
- Adafruit SSD1306
- Adafruit BusIO

## Running the Project

1. Complete the wiring.
2. Verify the ECHO voltage divider before applying power.
3. Open `src/ultrasonic_proximity_alert.ino`.
4. Select `ESP32C3 Dev Module`.
5. Enable `USB CDC On Boot` if the option is available.
6. Select the correct USB port.
7. Upload the sketch.
8. Open Serial Monitor at 115200 baud.
9. Place a flat object in front of the sensor.
10. Move the object toward and away from the sensor.

## Testing Procedure

A flat object such as a book or piece of cardboard provides a reliable reflection.

Suggested tests:

| Test | Expected Result |
|---|---|
| Object above 40 cm | OLED shows CLEAR and buzzer remains silent |
| Object between 20 and 40 cm | Slow buzzer warning |
| Object between 10 and 20 cm | Fast buzzer warning |
| Object below 10 cm | Continuous alarm |
| No object detected | OLED displays sensor error |
| Object moves closer | Distance decreases and bar grows |

## Troubleshooting

### OLED remains blank

- Confirm OLED VCC is connected to 3.3V
- Confirm OLED GND is connected to GND
- Confirm SDA is connected to GPIO 8
- Confirm SCL is connected to GPIO 9
- Confirm the OLED address is 0x3C
- Run an I2C scanner if necessary

### Sensor always reports an error

- Check HC-SR04 VCC and GND
- Check TRIG is connected to GPIO 4
- Check ECHO reaches GPIO 5 through the resistor divider
- Confirm the 1 kΩ and 2 kΩ resistor junction is connected to GPIO 5
- Use a flat target surface
- Keep the target farther than approximately 2 cm

### Distance reading jumps

- Keep jumper wires short
- Keep the sensor steady
- Use a flat target
- Avoid soft materials that absorb sound
- Avoid testing at an angle
- Use measurement smoothing

### ESP32 resets

- Check for loose power connections
- Confirm the HC-SR04 is powered from 5V
- Confirm all grounds are connected
- Ensure no 5V line is connected directly to an ESP32 GPIO

### Buzzer works backwards

Some buzzer modules are active-low.

If the buzzer sounds when it should be silent, reverse the buzzer control logic in the code.

## Testing Performed

- Confirmed OLED initialization
- Confirmed ultrasonic distance readings
- Tested the voltage divider connection
- Tested distance display updates
- Tested slow warning beeps
- Tested fast warning beeps
- Tested continuous close-range alarm
- Tested invalid-reading detection
- Verified Serial Monitor output

## Skills Demonstrated

- ESP32 programming
- Arduino C++
- Ultrasonic sensor integration
- GPIO input and output
- I2C communication
- OLED display control
- Voltage-divider design
- 5V-to-3.3V signal protection
- Distance measurement
- Alarm logic
- Sensor testing
- Embedded troubleshooting
- Technical documentation

## Future Improvements

- Add minimum-distance tracking
- Add configurable alarm thresholds
- Add buttons for changing settings
- Add RGB LED status indicators
- Add Wi-Fi monitoring
- Add browser-based live distance display
- Add data logging
- Add battery power
- Design a custom PCB
- Create a 3D-printed enclosure
- Use the system as a parking-distance assistant

## Project Status

Prototype development and testing in progress.

## Author

Rubbal Sandhu

Computer Science student interested in embedded systems, IoT, robotics, AI, and laboratory prototyping.

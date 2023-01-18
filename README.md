# ESE5190 Fall22 Final Project: Tokyo Drifters

## Team Members
Ronil Synghal: https://github.com/ronils428

Aurunima Samaddar: https://github.com/Aurunima

## Functions
The Tokyo Drifters vehicle packs various desirable features in a tiny and inteligent form:
- Object detection and object avoidance using ultrasonic sensors
- Object detection and object avoidance using Arducam
- Line following using IR sensors
- Object avoidance and Line following (improvisation possible using ML)

## Features
Environment data is gathered by ultrasonic sensors, Arducam and IR sensors

Line presence is senses by IR sensors.

Using ML, the line following could be made smoother and more accurate.
Further, with the usage of ML, improvised performance in object detection and object avoidance could be achieved.


## Demo

https://github.com/Aurunima/ESE5190_Fall22_FinalProject_TokyoDrifters/blob/main/media/Ln_demo.gif

## Diagram
Image unavailable


## Components

Pico4ML

Ultrasonic sensors

Pololu 3Pi bot with expansion board

AVR programmer

Level shifter


## Workflow
1. Motor control using encoder of 3pi bot using AVR programmer
2. Ultrasonic sensor interface with Pi Pico
3. Arducam data integration
4. AVR programmer used to add serial slave feature to 3Pi bot
5. Commands from Pi Pico to 3Pi bot over UART
6. IR sensor in 3Pi bot and PID tuning
7. Line following and object avoidance demonstrated separately
8. Balancing ultrasonic sensors and Pico4ML on the bot

## Debugging
### Motor and encoder
Servo motor was initially used but the maximum speed was not good enough.

### UART communication
UART data from the Pi Pico was not being recognised by the Polol 3Pi bot. The voltage levels of those of the Pic4ML and those of the 3Pi bot are different, which needs to be considered while implementing the circuit.

### Line following
Line following was very jerky initially, which was resolved by introducing PID.
Furhter, to increase the accuracy of line following, ML could be used.

## Thoughts
UART communication between the Pico4ML and 3Pi bot was an issue. The commands from the Pico4ML would be of 3.3V signals, which we converted to 5V signals using level shifter, to send to the 3Pi bot receiver (PD0). Similarly, the TX (PD1) line from the 3Pi bot of 5V was converted to 3.3.V using level shifter to connect to the RX line of the Pico4ML.

## Code
https://github.com/Aurunima/ESE5190_Fall22_FinalProject_TokyoDrifters/blob/main/code/main.c

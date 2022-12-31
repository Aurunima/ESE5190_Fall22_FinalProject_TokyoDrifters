# ESE5190_Fall22_FinalProject_TokyoDrifters

## Team Members
Ronil Synghal: https://github.com/ronils428

Aurunima Samaddar: https://github.com/Aurunima

## Design
The Tokyo Drifters vehicle packs various desirable features in a tiny and inteligent form:
- Object detection and object avoidance using PIR sensors
- Object detection and object avoidance using arducam
- Line following using IR sensors
- Object avoidance and Line following using ML

## Feature
Environment data is gathered y PIR sensors, Arducam and IR sensors

Line presence is senses by IR sensors. 

Using ML, the line following is made smoother and more accurate.
Further, with the usage of ML, improvised performance in object detection and object avoidance is achieved.


## Demo

https://github.com/Aurunima/ESE5190_Fall22_FinalProject_TokyoDrifters/blob/main/media/demo/Ln_demo.gif

## Diagram
Image unavailable


## Materials

Pico4ml

PIR sensors

Pololu 3Pi bot with expansion board

AVR programmer

Level shifter


# Development Overview
1. Realize screen display
2. Realize temperature measuring
3. Realize time correction

## Troubleshooting
### Motor and encoder
Servo motor was initially used but the maximum speed was not good enough.

### UART communication
UART data from the Pi Pico was not being recognised by the Polol 3Pi bot. The voltage levels of those of the Pic4ML and those of the 3Pi bot are different, which needs to be considered while implementing the circuit.

### Line following
Line following was very jerky initially, which was resolved by introducing PID.
Furhter, to increase the accuracy of line following, ML is also used.

## Reflection
The vehicle

## Code
https://github.com/Aurunima/ESE5190_Fall22_FinalProject_TokyoDrifters/blob/main/code/main.c

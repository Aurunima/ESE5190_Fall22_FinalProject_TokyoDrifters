#include <OrangutanLEDs.h>
#include <OrangutanAnalog.h>
#include <OrangutanMotors.h>
#include <OrangutanLCD.h>
#include <OrangutanPushbuttons.h>
#include <OrangutanBuzzer.h>

/*
 * OrangutanMotorExample for the 3pi robot, Orangutan LV-168, Orangutan SV-xx8,
 *   and Baby Orangutan B
 *
 * This example uses the OrangutanMotors library to drive
 * motors in response to the position of user trimmer potentiometer
 * and blinks the red user LED at a rate determined by the trimmer
 * potentiometer position.  It uses the OrangutanAnalog library to measure
 * the trimpot position, and it uses the OrangutanLEDs library to provide
 * limited feedback with the red user LED.
 *
 * http://www.pololu.com/docs/0J17/5.e
 * http://www.pololu.com
 * http://forum.pololu.com
 */
/*
 * m1 -> left; m2 -> right
 * OxC1 -> m1 forward
 * OxC2 -> m2 backward
 * 0xC3 -> m1 & m2 forward
 * 0xC5 -> m2 forward
 * 0xC6 -> m2 backward
 * 0xC7 -> m1 & m2 backward
*/

OrangutanAnalog analog;
OrangutanLEDs leds;
OrangutanMotors motors;
OrangutanLCD lcd;
OrangutanPushbuttons pbutton;
OrangutanBuzzer buzzer;


#include <avr/pgmspace.h>
// A couple of simple tunes, stored in program space.
const char welcome[] PROGMEM = ">g32>>c32";
const char go[] PROGMEM = "L16 cdegreg4";

#define echopin 27 // attach pin D2 Arduino to pin Echo of HC-SR04
#define trigpin 28 //attach pin D3 Arduino to pin Trig of HC-SR04

#define uS_cm 0.034

void setup()               // run once, when the sketch starts
{
  Serial.begin(9600);
  pinMode(trigpin, OUTPUT);
  pinMode(echopin, INPUT);
  //buzzer.play("c32");
}

int m1spd = 0;
int m2spd = 0;
int max_spd = 255;
int nom_spd = max_spd/2;
int dur = 50;

void loop()                // run over and over again
{
  lcd.clear();
  // Send a 10us pulse to the trig pin to start the measurement
  digitalWrite(trigpin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigpin, LOW);

  // Measure the pulse width of the signal on the echo pin
  long duration = pulseIn(echopin, HIGH);

  // Calculate the distance to the object using the speed of sound
  // and the measured pulse width
  float distance = duration * uS_cm / 2;
  char c = (char)Serial.read();
  int cmd = c;
  //unsigned char button = pbutton.waitForButton(ALL_BUTTONS);
  
  switch(cmd)
  {
    case 0x6C: m1spd = 0; // l -> left
              m2spd = nom_spd;
              dur = 1000;
              lcd.print("left");
              break;
    case 0x6B: m1spd = 0; // k -> reverse right
              m2spd = -nom_spd;
              dur = 1000;
              lcd.print("rev right");
              break;
    case 0x66: m1spd = nom_spd; // f -> forward
              m2spd = nom_spd;
              dur = 1000;
              lcd.print("forward");
              break;
    case 0x72: m1spd = nom_spd; // r -> right
              m2spd = 0;
              dur = 1000;
              lcd.print("right");
              break;
    case 0x61: m1spd = -nom_spd; // a -> reverse left
              m2spd = 0;
              dur = 1000;
              lcd.print("rev left");
              break;
    case 0x62: m1spd = -nom_spd; // b -> backward
              m2spd = -nom_spd;
              dur = 1000;
              lcd.print("backward");
              break;
    case 0x73: m1spd = max_spd; // s -> spin
              m2spd = 0;
              dur = 1000;
              lcd.print("spin");
              break;
    case 0x70: m1spd = 0; // p -> pause
              m2spd = 0;
              dur = 1000;
              lcd.print("pause");
              break;                            
    default: lcd.print("Bad");
              lcd.gotoXY(0, 1);
              lcd.print(cmd);
              buzzer.play("! V8 cdefgab>cbagfedc");
              delay(100);
              buzzer.stopPlaying();
  }
  lcd.gotoXY(0, 1);
  lcd.print(cmd);
  lcd.gotoXY(6, 0);
  lcd.print((char)Serial.read());

  motors.setSpeeds(m1spd, m2spd);
  delay(dur);  
}

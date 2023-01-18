
#include <Pololu3piPlus32U4.h>
#include <PololuMenu.h>

using namespace Pololu3piPlus32U4;
OLED display;

Buzzer buzzer;
LineSensors lineSensors;
Motors motors;
ButtonA buttonA;
ButtonB buttonB;
ButtonC buttonC;

int16_t lastError = 0;

#define NUM_SENSORS 5
unsigned int lineSensorValues[NUM_SENSORS];
uint16_t flag1;
uint16_t maxSpeed;
int16_t minSpeed;
uint16_t baseSpeed;

uint16_t calibrationSpeed;

uint16_t proportional; // coefficient of the P term * 256
uint16_t derivative; // coefficient of the D term * 256

void selectHyper()
{
  motors.flipLeftMotor(true);
  motors.flipRightMotor(true);
  
  // encoders.flipEncoders(true);
  maxSpeed = 100;
  minSpeed = 0;
  baseSpeed = maxSpeed;
  calibrationSpeed = 50;
  proportional = 64; // P coefficient = 1/4
  derivative = 256; // D coefficient = 1
}

void selectStandard()
{
  maxSpeed = 200;
  minSpeed = 0;
  baseSpeed = maxSpeed;
  calibrationSpeed = 60;
  proportional = 64; // P coefficient = 1/4
  derivative = 256; // D coefficient = 1
}

void selectTurtle()
{
  maxSpeed = 400;
  minSpeed = 0;
  baseSpeed = 400;
  calibrationSpeed = 120;
  proportional = 64; // P coefficient = 1/4
  derivative = 256; // D coefficient = 1
}

PololuMenu<typeof(display)> menu;

void selectEdition()
{
  display.clear();
  display.print(F("Select"));
  display.gotoXY(0,1);
  display.print(F("edition"));
  delay(1000);

  static const PololuMenuItem items[] = {
    { F("Standard"), selectStandard },
    { F("Turtle"), selectTurtle },
    { F("Hyper"), selectHyper },
  };

  menu.setItems(items, 3);
  menu.setDisplay(display);
  menu.setBuzzer(buzzer);
  menu.setButtons(buttonA, buttonB, buttonC);

  while(!menu.select());

  display.gotoXY(0,1);
  display.print("OK!  ...");
}

void loadCustomCharacters()
{
  static const char levels[] PROGMEM = {
    0, 0, 0, 0, 0, 0, 0, 63, 63, 63, 63, 63, 63, 63
  };
  display.loadCustomCharacter(levels + 0, 0);  // 1 bar
  display.loadCustomCharacter(levels + 1, 1);  // 2 bars
  display.loadCustomCharacter(levels + 2, 2);  // 3 bars
  display.loadCustomCharacter(levels + 3, 3);  // 4 bars
  display.loadCustomCharacter(levels + 4, 4);  // 5 bars
  display.loadCustomCharacter(levels + 5, 5);  // 6 bars
  display.loadCustomCharacter(levels + 6, 6);  // 7 bars
}

void printBar(uint8_t height)
{
  if (height > 8) { height = 8; }
  const char barChars[] = {' ', 0, 1, 2, 3, 4, 5, 6, (char)255};
  display.print(barChars[height]);
}

void calibrateSensors()
{
  display.clear();

  delay(1000);
  for(uint16_t i = 0; i < 80; i++)
  {
    if (i > 20 && i <= 60)
    {
      motors.setSpeeds(-(int16_t)calibrationSpeed, calibrationSpeed);
    }
    else
    {
      motors.setSpeeds(calibrationSpeed, -(int16_t)calibrationSpeed);
    }

    lineSensors.calibrate();
  }
  motors.setSpeeds(0, 0);
}

void showReadings()
{
  display.clear();

  while(!buttonB.getSingleDebouncedPress())
  {
    uint16_t position = lineSensors.readLineBlack(lineSensorValues);

    display.gotoXY(0, 0);
    display.print(position);
    display.print("    ");
    display.gotoXY(0, 1);
    for (uint8_t i = 0; i < NUM_SENSORS; i++)
    {
      uint8_t barHeight = map(lineSensorValues[i], 0, 1000, 0, 8);
      printBar(barHeight);
    }

    delay(50);
  }
}

void setup()
{
  //motors.flipLeftMotor(true);
  //motors.flipRightMotor(true);

  loadCustomCharacters();

  buzzer.play(">g32>>c32");

  // selectHyper(), selectStandard(), or selectTurtle().
  selectEdition();

  // Wait for button B to be pressed and released.
  display.clear();
  display.print(F("Press B"));
  display.gotoXY(0, 1);
  display.print(F("to calib"));
  while(!buttonB.getSingleDebouncedPress());

  calibrateSensors();

  showReadings();

  // Play music and wait for it to finish before we start driving.
  display.clear();
  display.print(F("Go!"));
  buzzer.play("L16 cdegreg4");
  while(buzzer.isPlaying());
  flag1 = 0;
}

void loop()
{
  
  int16_t position = lineSensors.readLineBlack(lineSensorValues);
  flag1++;

  int16_t error = position - 2000;

  int16_t speedDifference = error * (int32_t)proportional / 256  + (error - lastError) * (int32_t)derivative / 256;

  lastError = error;

  int16_t leftSpeed = (int16_t)baseSpeed + speedDifference;
  int16_t rightSpeed = (int16_t)baseSpeed - speedDifference;

  leftSpeed = constrain(leftSpeed, minSpeed, (int16_t)maxSpeed);
  rightSpeed = constrain(rightSpeed, minSpeed, (int16_t)maxSpeed);
  if (flag1 >= 700) {
    leftSpeed = 0;
    rightSpeed = 0;
  }

  motors.setSpeeds(leftSpeed, rightSpeed);
}

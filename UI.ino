/* Libraries */
#include <SD.h>                   /** SD Library: https://www.arduino.cc/en/reference/SD */
#include <Adafruit_GFX.h>         /** Adafruit GFX Library: https://github.com/adafruit/Adafruit-GFX-Library */
#include <MCUFRIEND_kbv.h>        /** MCUFRIEND_kbv Library: https://github.com/prenticedavid/MCUFRIEND_kbv */
#include <TouchScreen.h>          /** TouchScreen Library: https://github.com/adafruit/Adafruit_TouchScreen */

/* Define constants */
#define MOTOR_OFF      25
#define MOTOR_ON       50
#define SPEED_UP       100
#define SPEED_DOWN     200

#define SD_CS          10
#define BLACK          0
#define GREY           21845
#define BLUE           31
#define RED            63488
#define GREEN          2016
#define DARK_GREEN     1472
#define CYAN           2047
#define MAGENTA        63519
#define YELLOW         65504
#define GOLD           56768
#define WHITE          65535
#define NOT_PRESSED    200

/* Touch screen configuration */
#define MIN_PRESSURE   200
#define MAX_PRESSURE   1000

/* Touch panel parameters */
const int XP = 8, XM = A2, YP = A3, YM = 9;  /** 320x480 ID=0x9486 */
const int TS_LEFT = 144, TS_RT = 887, TS_TOP = 936, TS_BOT = 87;

/* Define pin numbers */
const int STEP_PIN = 5;
const int DIR_PIN = 2;
const int EN_PIN = 8;

TouchScreen touchScreen = TouchScreen(XP, YP, XM, YM, 300);
TSPoint touchPoint;
bool isTouchDown;
int touchPixelX, touchPixelY;  /** Touch_getXY() updates global vars */

/* Variables */
int currentPage = 0, oldPage = -1;
bool motorState = false;

/* Objects */
MCUFRIEND_kbv tft;
int pressedButton = NOT_PRESSED;

/* Button calibration */
Adafruit_GFX_Button page1Button, page2Button;
Adafruit_GFX_Button offButton, onButton, speedUpButton, speedDownButton;

/* Button dimensions and position */
int margin = 5;
int buttonWidth = 100;
int buttonHeight = 40;
int buttonY = 200;

/* Motor speed */
int motorSpeed = 500;

void setup() 
{
  /* Initialize Serial USB */
  Serial.begin(9600);
  Serial.println(F("Initialize System"));
  Serial1.begin(9600);

  /* Initialize TFT screen */
  uint16_t displayID = tft.readID();
  if (displayID == 0xD3D3) displayID = 0x9486;  /** for 3.5" TFT LCD Shield, 0x9341 for 2.8" TFT LCD Shield */
  tft.begin(displayID);
  tft.setRotation(1);  /** 0-PORTRAIT 1-LANDSCAPE 2-REVERSE PORTRAIT 3-REVERSE LANDSCAPE */

  /* Uncomment if you are using SD */
  /*
  if (!SD.begin(SD_CS)) {
    Serial.println(F("Initialization failed!"));
    return;
  }
  */

  /* Set the two pins as Outputs */
  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(EN_PIN, OUTPUT);
  digitalWrite(EN_PIN, LOW);

  /* Initialize Serial communication */
  Serial.begin(9600);

  /* Set A0 as an Input */
  pinMode(A0, INPUT);

  drawPage1();
}

void loop() 
{
  RunMotor();
  isTouchDown = GetTouchCoordinates();
  if (isTouchDown) 
  {
    offButton.press(offButton.contains(touchPixelX, touchPixelY));
    onButton.press(onButton.contains(touchPixelX, touchPixelY));
    speedUpButton.press(speedUpButton.contains(touchPixelX, touchPixelY));
    speedDownButton.press(speedDownButton.contains(touchPixelX, touchPixelY));

    if (offButton.justPressed()) 
    {
      motorState = false;
      Serial1.println("MOTOR_OFF");
    } 
    else if (onButton.justPressed()) 
    {
      motorState = true;
      Serial1.println("MOTOR_ON");
    } 
    else if (speedUpButton.justPressed() && ((motorSpeed + 100) < 1023)) 
    {
      motorSpeed += 100;
      Serial1.println("SPEED_UP");
    } 
    else if (speedDownButton.justPressed() && ((motorSpeed - 100) > 0)) 
    {
      motorSpeed -= 100;
      Serial1.println("SPEED_DOWN");
    }

    /* Display current state and speed on the screen */
    DisplayCurrentState();
    DisplayCurrentSpeed();
  }
}

/* Draw the first page on the TFT screen (480x320 Resolution) */
void drawPage1() 
{
  tft.setRotation(1);
  tft.fillScreen(BLACK);

  /* Display current state on the screen */
  String currentStateLabel = "Current State: ";
  tft.setTextSize(3);
  tft.setCursor(tft.width() / 2 - currentStateLabel.length() * 3 * 3 - 3, 27);
  tft.setTextColor(WHITE, BLACK);
  tft.print(currentStateLabel);
  tft.setCursor(tft.width() / 2 + currentStateLabel.length() * 3 * 3 - 3, 27);
  tft.setTextColor(BLUE, BLACK);
  if (!motorState) 
  {
    tft.print("OFF");
  } 
  else 
  {
    tft.print("ON");
  }

  /* Display buttons on the screen */
  offButton.initButton(&tft, 51 + 70, 74 + 30, buttonWidth, buttonHeight, WHITE, GREEN, BLACK, "OFF", 2);
  onButton.initButton(&tft, 340, 74 + 30, buttonWidth, buttonHeight, WHITE, GREEN, BLACK, "ON", 2);
  offButton.drawButton(false);
  onButton.drawButton(false);

  /* Display current speed on the screen */
  DisplayCurrentSpeed();

  /* Display speed control buttons on the screen */
  speedDownButton.initButton(&tft, 51 + 70, 74 + 170, buttonWidth, buttonHeight, WHITE, GREEN, BLACK, "-", 3);
  speedUpButton.initButton(&tft, 340, 74 + 170, buttonWidth, buttonHeight, WHITE, GREEN, BLACK, "+", 3);
  speedUpButton.drawButton(false);
  speedDownButton.drawButton(false);
}

/* Utility function to get touch coordinates */
bool GetTouchCoordinates() 
{
  touchPoint = touchScreen.getPoint();
  pinMode(YP, OUTPUT);             /** restore shared pins */
  pinMode(XM, OUTPUT);
  digitalWrite(YP, HIGH);
  digitalWrite(XM, HIGH);
  bool pressed = (touchPoint.z > MIN_PRESSURE && touchPoint.z < MAX_PRESSURE);
  if (pressed) 
  {
    if (tft.width() <= tft.height()) {  /** Portrait */
      touchPixelX = map(touchPoint.x, TS_LEFT, TS_RT, 0, tft.width());
      touchPixelY = map(touchPoint.y, TS_TOP, TS_BOT, 0, tft.height());
    } 
    else 
    {
      touchPixelX = map(touchPoint.y, TS_TOP, TS_BOT, 0, tft.width());
      touchPixelY = map(touchPoint.x, TS_RT, TS_LEFT, 0, tft.height());
    }
  }
  return pressed;
}

void RunMotor(void) 
{
  if (motorState) 
  {
    for (int x = 0; x < 6400; x++) 
    {
      motorSpeed = map(motorSpeed, 0, 1023, 0, 255);

      /* Generate pulses to drive the stepper motor */
      digitalWrite(STEP_PIN, HIGH);
      delayMicroseconds(motorSpeed);
      digitalWrite(STEP_PIN, LOW);
      delayMicroseconds(motorSpeed);

      /** Wire connections: GND-8-2-5 */
    }
  } 
  else 
  {
    /* Do nothing */
  }
}

/* Display current state on the screen */
void DisplayCurrentState() 
{
  String currentStateLabel = "Current State: ";
  tft.setTextSize(3);
  tft.setCursor(tft.width() / 2 - currentStateLabel.length() * 3 * 3 - 3, 27);
  tft.setTextColor(WHITE, BLACK);
  tft.print(currentStateLabel);
  tft.setCursor(tft.width() / 2 + currentStateLabel.length() * 3 * 3 - 3, 27);
  tft.setTextColor(BLUE, BLACK);
  if (!motorState) 
  {
    tft.print("OFF");
  } 
  else 
  {
    tft.print("ON ");
  }
}

/* Display current speed on the screen */
void DisplayCurrentSpeed() 
{
  String currentSpeedLabel = "Current Speed: ";
  tft.setTextSize(3);
  tft.setCursor(tft.width() / 2 - currentSpeedLabel.length() * 3 * 3 - 3, 160);
  tft.setTextColor(WHITE, BLACK);
  tft.print(currentSpeedLabel);
  tft.setCursor(tft.width() / 2 + currentSpeedLabel.length() * 3 * 3 - 3, 160);
  tft.setTextColor(BLUE, BLACK);
  tft.print(motorSpeed);
}

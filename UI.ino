//Libraries
#include <SD.h>//https://www.arduino.cc/en/reference/SD
#include <Adafruit_GFX.h>//https://github.com/adafruit/Adafruit-GFX-Library
#include <MCUFRIEND_kbv.h>//https://github.com/prenticedavid/MCUFRIEND_kbv
#include <TouchScreen.h> //https://github.com/adafruit/Adafruit_TouchScreen

#define MOTOR_OFF   25
#define MOTOR_ON    50
#define SPEED_UP    100
#define SPEED_DOWN  200

//Constants
#define SD_CS 10
#define BLACK 0
#define GREY 21845
#define BLUE 31
#define RED 63488
#define GREEN 2016
#define DARKGREEN 1472
#define CYAN 2047
#define MAGENTA 63519
#define YELLOW 65504
#define GOLD 56768
#define WHITE 65535
#define NOT_PRESSED 200
//Touch screen configuration
#define MINPRESSURE 200
#define MAXPRESSURE 1000
// ALL Touch panels and wiring is DIFFERENT
// copy-paste results from TouchScreen_Calibr_native.ino
//3.5 Parameters
const int XP = 8, XM = A2, YP = A3, YM = 9; //320x480 ID=0x9486
const int TS_LEFT = 144, TS_RT = 887, TS_TOP = 936, TS_BOT = 87;
//2.8 Parameters
//const int XP = 8, XM = A2, YP = A3, YM = 9; //240x320 ID=0x9341
//const int TS_LEFT = 907, TS_RT = 120, TS_TOP = 74, TS_BOT = 913;

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
TSPoint p;
bool down;
int pixel_x, pixel_y; 				//Touch_getXY() updates global vars

//Variables
int currentPage 	= 0, oldPage = -1;
bool state = 0;
//Objects
MCUFRIEND_kbv tft;
int pressedButton = NOT_PRESSED;
// Button calibration
Adafruit_GFX_Button page1_btn, page2_btn;
Adafruit_GFX_Button offBtn, onBtn, speedUp, speedDown;

int margin = 5;
int btnWidth = 100;
int btnHeight = 40;
int btnY = 200;

int speed = 500;
void setup() {
 	//Init Serial USB
 	Serial.begin(9600);
 	Serial.println(F("Initialize System"));
 	Serial1.begin(9600);

 	//Init tft screen
 	uint16_t ID = tft.readID();
 	if (ID == 0xD3D3) ID = 0x9486; 	//for 3.5" TFT LCD Shield , 0x9341 for 2.8" TFT LCD Shield 	
 	tft.begin(ID);
 	tft.setRotation(1);//0-PORTRAIT 1-PAYSAGE 2-REVERSE PORTRAIT 3-REVERSE PAYSAGE
 	//Uncomment if you are using SD
 	/*if (!SD.begin(SD_CS)) {
 			Serial.println(F("initialization failed!"));
 			return;
 	}*/
  drawPage1();
}

void loop() {
 	down = Touch_getXY();
  if (down)
  {
    offBtn.press(offBtn.contains(pixel_x, pixel_y));
    onBtn.press(onBtn.contains(pixel_x, pixel_y));
    speedUp.press(speedUp.contains(pixel_x, pixel_y));
    speedDown.press(speedDown.contains(pixel_x, pixel_y));
    
    if (offBtn.justPressed())
    {
      state = 0;
      Serial1.println(MOTOR_OFF);
      
    }
    else if (onBtn.justPressed())
    {
      state = 1;
      Serial1.println(MOTOR_ON);
    }
    else if (speedUp.justPressed() && ((speed + 100) < 1023))
    {
        speed += 100;
        Serial1.println(SPEED_UP);
    } 
    else if (speedDown.justPressed() && ((speed - 100) > 0))
    {
        speed -= 100;
        Serial1.println(SPEED_DOWN);
    }
      String Current_State = "Current State: ";
 	tft.setTextSize(3);
 	tft.setCursor(tft.width() / 2 - Current_State.length() * 3 * 3 - 3, 27);
 	tft.setTextColor(WHITE, BLACK);
 	tft.print(Current_State); // Prints the string on the screen
 	tft.setCursor(tft.width() / 2 + Current_State.length() * 3 * 3 - 3, 27);
 	tft.setTextColor(BLUE, BLACK);
  if (state == 0)
  {
     	tft.print("OFF");
  }
  else
  {
    tft.print("ON ");
  }
 	//Button
  String Current_Speed = "Current Speed: ";
 	tft.setTextSize(3);
 	tft.setCursor(tft.width() / 2 - Current_State.length() * 3 * 3 - 3, 160);
 	tft.setTextColor(WHITE, BLACK);
 	tft.print(Current_Speed); // Prints the string on the screen
 	tft.setCursor(tft.width() / 2 + Current_State.length() * 3 * 3 - 3, 160);
 	tft.setTextColor(BLUE, BLACK);
  tft.print(speed);
  }
}

// 480x320 Resolution

void drawPage1() { /* function drawHomePage */
 	tft.setRotation(1);
 	tft.fillScreen(BLACK);
 	//Title
  String Current_State = "Current State: ";
 	tft.setTextSize(3);
 	tft.setCursor(tft.width() / 2 - Current_State.length() * 3 * 3 - 3, 27);
 	tft.setTextColor(WHITE, BLACK);
 	tft.print(Current_State); // Prints the string on the screen
 	tft.setCursor(tft.width() / 2 + Current_State.length() * 3 * 3 - 3, 27);
 	tft.setTextColor(BLUE, BLACK);
  if (state == 0)
  {
     	tft.print("OFF");
  }
  else
  {
    tft.print("ON");
  }
 	//Button
 	offBtn.initButton(&tft, 51+70, 74+30,  100, 61, WHITE, GREEN, BLACK, "OFF", 2);
  onBtn.initButton(&tft, 340, 74+30,  100, 61, WHITE, GREEN, BLACK, "ON", 2);
 	offBtn.drawButton(false);
  onBtn.drawButton(false);
  
  String Current_Speed = "Current Speed: ";
 	tft.setTextSize(3);
 	tft.setCursor(tft.width() / 2 - Current_State.length() * 3 * 3 - 3, 160);
 	tft.setTextColor(WHITE, BLACK);
 	tft.print(Current_Speed); // Prints the string on the screen
 	tft.setCursor(tft.width() / 2 + Current_State.length() * 3 * 3 - 3, 160);
 	tft.setTextColor(BLUE, BLACK);
  tft.print(speed);
 	speedDown.initButton(&tft, 51+70, 74+170,  100, 61, WHITE, GREEN, BLACK, "-", 3);
  speedUp.initButton(&tft, 340, 74+170,  100, 61, WHITE, GREEN, BLACK, "+", 3);
 	speedUp.drawButton(false);
  speedDown.drawButton(false);

}

/************************************************************************************
 			UTILITY FUNCTION
*************************************************************************************/
bool Touch_getXY(void)
{
 	p = ts.getPoint();
 	pinMode(YP, OUTPUT); 					//restore shared pins
 	pinMode(XM, OUTPUT);
 	digitalWrite(YP, HIGH);
 	digitalWrite(XM, HIGH);
 	bool pressed = (p.z > MINPRESSURE && p.z < MAXPRESSURE);
 	if (pressed) {
 			if (tft.width() <= tft.height()) { //Portrait
 					pixel_x = map(p.x, TS_LEFT, TS_RT, 0, tft.width()); //.kbv makes sense to me
 					pixel_y = map(p.y, TS_TOP, TS_BOT, 0, tft.height());
 			} else {
 					pixel_x = map(p.y, TS_TOP, TS_BOT, 0, tft.width());
 					pixel_y = map(p.x, TS_RT, TS_LEFT, 0, tft.height());
 			}
 	}
 	return pressed;
}


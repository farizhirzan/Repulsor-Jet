//based on FastLED DemoReel100 code and MPU6050 Sensor Arduino Tutorial by Supperb Tech
//modified by Fariz Hirzan


#include "Wire.h"       
#include "I2Cdev.h"     
#include "MPU6050.h"
#include <FastLED.h>
#include <Servo.h>


FASTLED_USING_NAMESPACE


#define DATA_PIN    3 //define data pin for LED WS2812
#define LED_TYPE    WS2812
#define COLOR_ORDER RGB
#define NUM_LEDS    24 //Number of LEDS used
CRGB leds[NUM_LEDS];


#define BRIGHTNESS 120 //define max brightness of the LED
#define FRAMES_PER_SECOND 120 //define LED FPS


MPU6050 mpu; //define MPU6050 sensor
int16_t ax, ay, az; //define acceleration in 3-axis
int16_t gx, gy, gz; //define gyroscope in 3-axis
const int servoPinA = 9; //define servo pin A
const int servoPinB = 10; //define servo pin B
const int switchPin = 12; //define switch pin
int switchState = 0; //variable for reading the button setup


struct MyData
{
  byte X; //define X-axis data
  byte Y; //define Y-axis data
  byte Z; //define Z-axis data
  byte GX;
  byte GY;
  byte GZ; 
};


MyData data;


Servo ServoA; //define servo A
Servo ServoB; //define servo B


void setup()
{
  Serial.begin(9600); //for serial monitor 9600 BAUD


  pinMode(switchPin, INPUT); //assign digital input pin 13 as input
  Wire.begin();
  mpu.initialize(); //initializing MPU6050


  ServoA.attach(servoPinA); //servo A setup
  ServoB.attach(servoPinB); //servo B setup
  delay(1000);  //startup delay
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS); //WS2812 LED setup
  FastLED.setBrightness(BRIGHTNESS); //set LED brightness
  FastLED.clear(); //clear data value from LED
  FastLED.show(); //write data value to LED
}


uint8_t gCurrentPatternNumber = 0; //Index number of which pattern is current
uint8_t gHue = 0; //rotating "base color" used by many of the patterns


typedef void (*SimplePatternList[])(); //define pattern list
SimplePatternList gPatterns = { standby, fly }; //patttern list


void loop()
{
  //switchState = digitalRead(switchPin); //check input status from switch
  //if (switchState == HIGH) //if switch is ON
  //{
    function(); //run function
  //}
  //else //if switch is OFF
  //{
    //fill_solid(leds, NUM_LEDS, CRGB::Black); //turn off LED 
    //ServoA.write(30); //turn servo A to 30 degrees
    //ServoB.write(30); //turn servo B to 30 degrees
    //FastLED.show(); //write data value to LED, in this case turn it off
  //}
}


void function() //run on the go function while the switch is on
{
  gPatterns[gCurrentPatternNumber](); //start with pattern number 0 as standby position


  FastLED.show(); //write data value to LED  
  FastLED.delay(1000/FRAMES_PER_SECOND); //insert a delay to keep the framerate modest


  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz); //get acceleration and gyroscope data from MPU6050
  data.X = map(ax, -17000, 17000, 0, 255 ); //X axis data
  data.Y = map(ay, -17000, 17000, 0, 255);  //Y axis data
  data.Z = map(az, -17000, 17000, 0, 255);  //Z axis data
  data.GX = map(gx, -17000, 17000, 0, 255 ); //X axis data
  data.GY = map(gy, -17000, 17000, 0, 255);  //Y axis data
  data.GZ = map(gz, -17000, 17000, 0, 255);  //Z axis data
  delay(500);
  Serial.print("Axis X = ");
  Serial.print(data.X); //show acceleration X axis data on serial monitor
  Serial.print("  ");
  Serial.print("Axis Y = "); 
  Serial.print(data.Y); //show acceleration Y axis data on serial monitor
  Serial.print("  ");
  Serial.print("Axis Z  = ");
  Serial.println(data.Z); //show acceleration Z axis data on serial monitor
  Serial.print("Gyro X = ");
  Serial.print(data.GX); //show gyro X axis data on serial monitor
  Serial.print("  ");
  Serial.print("Gyro Y = "); 
  Serial.print(data.GY); //show gyro Y axis data on serial monitor
  Serial.print("  ");
  Serial.print("Gyro Z  = ");
  Serial.println(data.GZ); //show gyro Z axis data on serial monitor


  nextPattern(data); //check data to modify LED pattern
}


#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))


void nextPattern(MyData data) //check data to modify LED pattern
{
  // add one to the current pattern number, and wrap around at the end
   if (data.X > 190 && data.Y > 150 && data.GX > 100 && data.GY > 100) //if hand are in flying position
   {
    gCurrentPatternNumber = 1; //LED light turn blue
    ServoA.write(90); //rotate servo A to 90 degrees position
    ServoB.write(0); //rotate servo B to 0 degrees position
   }
  else
  {
    gCurrentPatternNumber = 0; //LED light turn into standby pattern in magenta color
    ServoA.write(0); //rotate servo A to 0 degrees position
    ServoB.write(90); //rotate servo B to 90 degrees position
  }
}


void standby() //magenta speckles that blink in and fade smoothly
{
  fadeToBlackBy( leds, NUM_LEDS, 50); //turn the LED off
  int pos = random16(NUM_LEDS); //generate random speckles on the LED
  leds[pos] += CHSV( gHue + 103, 199, 235); //magenta color hex
}


void fly()
{
  fadeToBlackBy( leds, NUM_LEDS, 50); //turn the LED off
  for( int i = 0; i < NUM_LEDS; i++) { //turn all LED on constantly
    leds[i] = CHSV(gHue + 199, 103, 235); //blue color hex
  }
}
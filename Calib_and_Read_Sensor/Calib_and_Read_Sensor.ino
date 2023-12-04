#include "FastIMU.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "FS.h"
#include "SD.h"
#include "SPI.h"

#define IMU_ADDRESS 0x69
#define DISPLAY_ADDRESS 0x3C
#define PERFORM_CALIBRATION
#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels
BMI160 IMU;

calData calib = { 0 };
AccelData accelData;
GyroData gyroData;
MagData magData;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup() {
  Wire.begin();
  Wire.setClock(400000);
  Serial.begin(115200);
  while (!Serial){
    ;
  }
  // For debugging !!!
  // if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {  // Address 0x3D for 128x64
  //   Serial.println(F("SSD1306 allocation failed"));
  //   for (;;)
  //     ;
  // }
  display.begin(SSD1306_SWITCHCAPVCC, DISPLAY_ADDRESS);
  delay(2000);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(30, 10);
  display.println("Initialize");
  display.setCursor(20, 20);
  display.println("Accellerometor!");
  display.display();
  delay(1000);

  int err = IMU.init(calib, IMU_ADDRESS);
  if (err != 0) {
    display.clearDisplay();
    display.setCursor(0, 10);
    display.print("Error initializing IMU: ");
    display.println(err);
    display.display();
    // Serial.print("Error initializing IMU: ");
    // Serial.println(err);
    while (true) {
      ;
    }
  }else{
    display.clearDisplay();
    display.setCursor(0, 10);
    display.print("Initializing Success");
    // display.println(err);
    display.display();
    delay(1000);
    display.clearDisplay();
  }
  
#ifdef PERFORM_CALIBRATION
  
  // Serial.println("FastIMU calibration & data example");
  delay(2000);
  display.setCursor(16, 0);
  // Serial.println("Keep IMU level.");
  display.println("Begin Calibrate.");
  display.setCursor(18, 10);
  display.println("Keep IMU level.");
  display.display();
  delay(2000);
  display.clearDisplay();
  
  IMU.calibrateAccelGyro(&calib);
  display.setCursor(12, 0);
  display.println("Calibration done!");
  display.setCursor(6, 10);
  display.println("Accel biases X/Y/Z: ");
  display.setCursor(0, 20);
  display.print(calib.accelBias[0]);
  display.setCursor(45, 20);
  display.print(calib.accelBias[1]);
  display.setCursor(90, 20);
  display.println(calib.accelBias[2]);
  display.display();
  delay(5000);
  display.clearDisplay();
  IMU.init(calib, IMU_ADDRESS);
#endif

  //err = IMU.setGyroRange(500);      //USE THESE TO SET THE RANGE, IF AN INVALID RANGE IS SET IT WILL RETURN -1
  //err = IMU.setAccelRange(2);       //THESE TWO SET THE GYRO RANGE TO ±500 DPS AND THE ACCELEROMETER RANGE TO ±2g
  
  if (err != 0) {
    Serial.print("Error Setting range: ");
    Serial.println(err);
    while (true) {
      ;
    }
  }
}

void loop() {
  display.clearDisplay();
  IMU.update();
  IMU.getAccel(&accelData);
  display.setCursor(16, 10);
  display.print("X");
  display.setCursor(58, 10);
  display.print("Y");
  display.setCursor(100, 10);
  display.print("Z");
  display.setCursor(0, 20);
  display.print(accelData.accelX);
  display.setCursor(45, 20);
  display.print(accelData.accelY);
  display.setCursor(90, 20);
  display.print(accelData.accelZ);
  display.setCursor(25, 40);
  display.print("Loggin Data");
  display.display();
}

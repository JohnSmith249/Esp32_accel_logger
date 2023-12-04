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

String DataFileName = "DataLog.txt";

calData calib = { 0 };
AccelData accelData;
GyroData gyroData;
MagData magData;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void quickLog(fs::FS &fs, const char *path, float buf[]) {
  File file = fs.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("Open file operation failed !!!");
    return;
  }
  size_t i;
  for (i = 0; i < 512; i+=3) {
    file.print(buf[i]);
    file.print(' ');
    file.print(buf[i+1]);
    file.print(' ');
    file.println(buf[i+2]);
  }
  file.close();
  // For debug, DO NOT ACTIVATE !!!
  // Serial.println("done");
}

void setup() {
  Wire.begin();
  Wire.setClock(400000);
  Serial.begin(115200);

  while (!Serial) {
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
  display.setTextSize(1);
  display.setTextColor(WHITE);
  sdStatus();
  delay(2000);
  display.clearDisplay();
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
  } else {
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

  // *********************************** Testting area ***************************************************//
  // For debug purpose from creator CuongTran, contact trncuong249@gmail.com for further instructions.
  // DO NOT TRIGGER UNDER ANY CIRCUMSTANCES !!!
  // writeFile(SD, "/logFile_ID.txt", "0001");
  // readFile(SD, "/logFile_ID.txt");

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

void sdStatus() {
  display.clearDisplay();
  display.setCursor(5, 10);
  display.print("Initializing SD Card");
  display.display();
  delay(2000);
  if (!SD.begin()) {
    display.clearDisplay();
    display.setCursor(25, 20);
    display.print("SDCard module");
    display.setCursor(12, 40);
    display.print("Initialize failed");
    display.display();
    // Serial.println("SDCard module initialize failed");
    return;
  } else {
    display.clearDisplay();
    display.setCursor(25, 20);
    display.print("SDCard module");
    display.setCursor(12, 40);
    display.print("Initialize success");
    display.display();
    delay(2000);
  }
  uint8_t cardType = SD.cardType();

  if (cardType = CARD_NONE) {
    display.clearDisplay();
    display.setCursor(5, 20);
    display.print("No SD Card Detected");
    display.display();
    delay(2000);
    // Serial.println("No SD Card Detected");
    return;
  }

  // Serial.print("SD Card Type: ");
  display.clearDisplay();
  display.setCursor(15, 20);
  display.print("SD Card Type: ");
  display.setCursor(25, 40);
  if (cardType == CARD_MMC) {
    display.print("MMC");
  } else if (cardType == CARD_SD) {
    // Serial.println("SDSC");
    display.print("SDSC");
  } else if (cardType == CARD_SDHC) {
    // Serial.println("SDHC");
    display.print("SDHC");
  } else {
    // Serial.println("UNKNOWN");
    display.print("SDHC");
  }
  display.display();
  delay(2000);
  display.clearDisplay();
  display.setCursor(0, 20);
  display.print("SD Card Size: ");
  display.setCursor(85, 20);
  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);
  display.print(cardSize);
  display.display();
}

void readFile(fs::FS &fs, const char * path){
    Serial.printf("Reading file: %s\n", path);

    File file = fs.open(path);
    if(!file){
        Serial.println("Failed to open file for reading");
        return;
    }

    Serial.print("Read from file: ");
    while(file.available()){
        Serial.write(file.read());
    }
    file.close();
}

void writeFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Writing file: %s\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
        return;
    }
    if(file.print(message)){
        Serial.println("File written");
    } else {
        Serial.println("Write failed");
    }
    file.close();
}


// For advanced functions, DO NOT ACTIVATE !!!
void appendFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Appending to file: %s\n", path);

    File file = fs.open(path, FILE_APPEND);
    if(!file){
        Serial.println("Failed to open file for appending");
        return;
    }
    if(file.print(message)){
        Serial.println("Message appended");
    } else {
        Serial.println("Append failed");
    }
    file.close();
}

void createDir(fs::FS &fs, const char * path){
    Serial.printf("Creating Dir: %s\n", path);
    if(fs.mkdir(path)){
        Serial.println("Dir created");
    } else {
        Serial.println("mkdir failed");
    }
}


// For advanced functions, DO NOT ACTIVATE !!!
void removeDir(fs::FS &fs, const char * path){
    Serial.printf("Removing Dir: %s\n", path);
    if(fs.rmdir(path)){
        Serial.println("Dir removed");
    } else {
        Serial.println("rmdir failed");
    }
}

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// I2C communication libraries
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Temperature and humidity sensor library and initialization
#include <Adafruit_AM2320.h>

// Sensor input pins
#define POT_DMC 6
#define POT_DC 5
#define POT_WIND 4
#define SDA 8
#define SCL 9

// Degree of seriousness LED pins
#define LOW 15
#define MODERATE 16
#define HIGH 17
#define VERY_HIGH 18

// I2C communcation: Initialize 16x2 LCD (I2C address: 0x27, 16 columns, 2 rows)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Initialize AM2320 sensor
Adafruit_AM2320 am2320 = Adafruit_AM2320();

// Detection handles
TaskHandle_t Data_Read_Handle = NULL;

// FWI Calculation handle
TaskHandle_t FWI_Calc_Handle = NULL;

// Create queue handles
QueueHandle_t dataQueue;

// Detection task prototype
void Data_Read(void *pvParameter);

// FWI Calculation task prototype
void FWI_Calc(void *pvParameter);

// Blinking LED task prototype
void Blink_LED(void *pvParameter);

// ============================= Global Variables =============================
volatile int FWI = 0; // Fire Weather Index

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Advanced Fire Sensing and Mitigation System!");

  // Initialize pins
  pinMode(POT_DMC, INPUT);
  pinMode(POT_DC, INPUT);
  pinMode(POT_WIND, INPUT);

  // Initialize I2C communication
  Wire.begin();
  
  // Start the LCD
  lcd.init();
  lcd.backlight(); // Turn on backlight

  // Start the sensor
  am2320.begin();

  // Create tasks
  // TODO:...
  xTaskCreate(Data_Read, "Data Read", 4096, NULL, 1, &Data_Read_Handle);
  xTaskCreate(FWI_Calc, "FWI", 4096, NULL, 1, &FWI_Calc_Handle);
}

// Create Tasks Functions
// TODO:...
// Temperature and Humidity sensing
void Data_Read(void *pvParameter) {
  int temp;
  int humid;
  int dmc;
  int dc;
  int potWind;
  int wind;
  while (1) {  // Infinite loop to keep the task running
    temp = am2320.readTemperature();
    humid = am2320.readHumidity();
    potWind = analogRead(POT_WIND);
    wind = (potWind / 4095.0) * 50.0;   // Scale value between 0-50 km/h
    vTaskDelay(pdMS_TO_TICKS(500));  // Delay to prevent CPU overload
  }
}

void FWI_Calc(void *pvParameter) {
  while (1) {
    Serial.print("Wind: ");
    Serial.println(wind);

    Serial.print("Temp: ");
    Serial.println(temp);

    Serial.print("Humidity: ");
    Serial.println(humid);
    
    vTaskDelay(pdMS_TO_TICKS(500));
    FWI += 1;
  }
}

void Blink_LED(void *pvParameter) {
  // Green 0-5 low
  // Blue 5-15 moderate
  // Yellow 15-30 high
  // Red 30+ very high
  bool blink = true;
  while(1) {
    if (FWI < 6) {
      digitalWrite(LOW, blink);
      blink = !blink;
      vTaskDelay(pdMS_TO_TICKS(50));
    } else if (FWI < 16) {
      digitalWrite(MODERATE, blink);
      blink = !blink;
      vTaskDelay(pdMS_TO_TICKS(50));
    } else if (FWI < 31) {
      digitalWrite(HIGH, blink);
      blink = !blink;
      vTaskDelay(pdMS_TO_TICKS(50));
    } else {
      digitalWrite(VERY_HIGH, blink);
      blink = !blink;
      vTaskDelay(pdMS_TO_TICKS(50));
    }
  }
}

// ============================= ESP-NOW Wireless Communication =============================
// TODO:

void loop() {}
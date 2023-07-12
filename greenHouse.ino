#include <LiquidCrystal_I2C.h>

#include <ESP32Servo.h>
#include <analogWrite.h>
#include <ESP32Tone.h>
#include <ESP32PWM.h>

#include <DHT.h>
#include <DHT_U.h>

//light
#define LED_YELLOW 23
#define LED_LIGHT1 17
//#define LED_LIGHT2

//BUTTON
#define BUTTON_BLACK 16

//photocell
#define photocellPin 25
int photocellReading;

//SS HC SR04 (Speaker)
const int TRIGGER_PIN = 27;
const int ECHO_PIN = 26;
//define speed of sound in air in cm/us
#define SPEED_OF_SOUND 0.034

//servo moter
int SERVO_PIN_air = 18;
int SERVO_PIN_door = 14;
Servo airServo;
Servo doorServo;

//monitor
LiquidCrystal_I2C LCD(0x27, 16, 2);

//humidity & temperature
#define DHTTYPE DHT11
static const int DHT_PIN = 33;
DHT dht(DHT_PIN, DHTTYPE);

//water sensor
int WATER_PIN = 32;

//motor
const int MOTOR_PIN = 19;

void setup() {
  Serial.begin(115200);
  //setup LED
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_LIGHT1, OUTPUT);
  //pinMode(LED_LIGHT2, OUTPUT);
  //water sensor
  pinMode(WATER_PIN, INPUT);

  //dht
  dht.begin();

  //Speaker
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  //servo
  airServo.attach(SERVO_PIN_air);
  doorServo.attach(SERVO_PIN_door);
  airServo.write(0);
  doorServo.write(0);

  //setup button
  pinMode(BUTTON_BLACK, INPUT_PULLUP);

  //monitor
  LCD.init();
  delay(250);
  LCD.backlight();

  //photocell
  pinMode(photocellPin, INPUT);

  //motor
  pinMode(MOTOR_PIN, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  turnOnLED();
  waterMeter();
  showMonitor();  
  airFlow();
  if(digitalRead(BUTTON_BLACK) == 0){
    openDoor();
  }
  
}

bool lightMeter(){
  int bright = 1000;
  photocellReading = analogRead(photocellPin);

  //Serial.print("light");
  //Serial.println(photocellReading);

  if(photocellReading > bright){   //if bright
    
    return (false);
  }
  else{     //if dark
    return (true);
  }
}

void openDoor(){
  doorServo.write(90);
  delay(1000);
  doorServo.write(0);
  delay(1000);
}

void turnOnLED(){
  //light
  if(lightMeter()){
    digitalWrite(LED_LIGHT1, HIGH);
    //digitalWrite(LED_LIGHT2, HIGH);
  }
  else{
    digitalWrite(LED_LIGHT1, LOW);
    //digitalWrite(LED_LIGHT2, LOW);
  }
  //detect and turn on A WARNING LIGHT
  if(detectWildAnimals()){
    digitalWrite(LED_YELLOW, HIGH);
  }
  else{
    digitalWrite(LED_YELLOW, LOW);
  }
}

//SS HC SR04 (Speaker)
bool detectWildAnimals(){
  //reset trigger pin
  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(2);

  //send pulse by setting trigger pin on HIGH state for 10 microseconds
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);

  //read echo pin to get soundwave travel time, also in us
  long duration = pulseIn(ECHO_PIN, HIGH);

  //calculate and return the distance in cm
  long distance = duration * SPEED_OF_SOUND / 2;

  //Serial.println(distance);
  if(distance < 30){
    return (true);  //turn on warning light
  }
  else{
    return (false);
  }
}

void airFlow(){
  airServo.write(180);
  delay(200);
  airServo.write(0);
  delay(200);
}

void waterMeter(){
  int water = 100;
  int waterReading = analogRead(WATER_PIN);
  Serial.println(waterReading);
  if(waterReading < 100){
    //pump running
    digitalWrite(MOTOR_PIN, HIGH);
    delay(500);
  }
  else{
    digitalWrite(MOTOR_PIN, LOW);
    delay(500);
  }
}

//humidity & temp
static bool measure_env(float *temp, float *humidity){
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if(isnan(h) || isnan(t)){
    Serial.print("no env");
    return (false);
  }
  else{
    //Serial.println(t);
    //Serial.println(h);
    *temp = t;
    *humidity = h;
    return (true);
  }
}

void showMonitor(){   //show humidity and temperature
  float temperature;
  float humidity;

  if (measure_env(&temperature, &humidity)){
    LCD.setCursor(0, 0);
    LCD.print("humidity: ");
    LCD.setCursor(10, 0);
    LCD.print(humidity);
    LCD.setCursor(0, 1);
    LCD.print("temp: ");
    LCD.setCursor(10, 1);
    LCD.print(temperature);
  }
  else{
    LCD.setCursor(0, 0);
    LCD.print("no~~~~~~~~");
  }
  
}

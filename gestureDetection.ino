#include <LiquidCrystal_I2C.h> // Library for LCD
#include <Wire.h> // Library for IC2 communication
#include <dht.h> // Lbrary for temperature sensor
#include <NewPing.h> // Library for ultrasonic sensors 
// Pins for the HC-SR04 sensors
const int echoPinRgt = 5;
const int trigPinRgt = 6;
const int echoPinLft = 10;
const int trigPinLft = 11;
// Variables for gesture control
String gesture = "";
String gesture2 = "";
const int MAX_DISTANCE = 400; // HC-SR04-sensors maximum distance in cm //65
int iterations = 3; // For median calculationg //10, 3, 4, 5,
float durationLft, durationRgt, distanceLft, distanceRgt;
// Initialization for the NewPing-library usage
NewPing Rgt(trigPinRgt, echoPinRgt, MAX_DISTANCE);
NewPing Lft(trigPinLft, echoPinLft, MAX_DISTANCE);
// Initialization for LCD-display
LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
// variables and initialization for temperature sensor
#define DHT11_PIN 7
const int pwmPin = 3; // PulseWidthModulation for fan controll
int temperature;
dht DHT;

void setup()
{
    Serial.begin(2000000);
    Serial.setTimeout(250);
    pinMode(pwmPin, OUTPUT);
    lcd.begin(16, 2); // set up the LCD's number of columns and rows:
}

void loop()
{
    temperature = DHT.temperature;
    gesture = findOutGesture(); // findOutGesture-function returns string
    Serial.println(gesture);
    fanControl();
    display();
}

void fanControl()
{
    int pulseWidth;
    int chk = DHT.read11(DHT11_PIN);

    if (temperature > -999)
    {

        if ((temperature >= 35) && (temperature > 0))
        {
            pulseWidth = 240;
        }
        if ((temperature < 35) && (temperature > 0))
        {
            pulseWidth = 80;
        }
        analogWrite(pwmPin, pulseWidth);
    }
}

void display ()
{
    lcd.setCursor(0, 1);
    if (temperature > -999)
    {
        lcd.setCursor(0, 1);
        lcd.print("E:");
        lcd.print(temperature);
    }
    lcd.setCursor(5, 1);
    lcd.print(gesture);
    //if (Serial.available() > 0) {
    String s = Serial.readString();

    //if (s != gesture) {

    if (s.length() <= 16)
    {
        lcd.setCursor(0, 0);
        lcd.print(s);
    }
    else
    {
        lcd.setCursor(0,0);
        lcd.print(s.substring(0,13));
    }
    //}
    //}
}


void calculateDistance ()
{
    durationLft = Lft.ping_median(iterations);
    distanceLft = Lft.convert_cm(durationLft);
    if (distanceLft >= 400)
    {
        distanceLft = 0;   //65
    }
    durationRgt = Rgt.ping_median(iterations);
    distanceRgt = Rgt.convert_cm(durationRgt);
    if (distanceRgt >= 400)
    {
        distanceRgt = 0;   //65
    }
}

String findOutGesture ()
{
    String gestureIs;
    // Variables for operational distances
    int Max = 28;
    int Min = 2;
    // temporary variables
    unsigned long tempL;
    unsigned long tempR;
    // Variables for calibration
    int distanceMeasurmentCAL = 250;
    int isHandInFrontCAL = 8;
    int isHandRemovedCAL = 28;

    calculateDistance ();
    if (distanceRgt <= Max && distanceRgt >= Min)
    {
        tempR = millis(); // millis-function return the time since Arduino began running the program in ms

        while (millis() <= (tempR + distanceMeasurmentCAL))
            calculateDistance();

        if (distanceRgt <= Max && distanceRgt >= Min)
        {
            tempR = distanceRgt;

            while (distanceRgt <= isHandRemovedCAL || distanceRgt == 0)
            {
                calculateDistance();

                if ((tempR + isHandInFrontCAL) < distanceRgt)
                {
                    gestureIs = "mute      ";
                }
                if ((tempR - isHandInFrontCAL) < distanceRgt)
                {
                    gestureIs = "play/pause";
                }
            }
        }
        else
        {
            gestureIs = "next T     ";
        }
    }
    if (distanceLft <= Max && distanceLft >= Min)
    {
        tempL = millis();

        while (millis() <= (tempL + distanceMeasurmentCAL))
            calculateDistance();

        if (distanceLft <= Max && distanceLft >= Min)
        {
            tempL = distanceLft;

            while (distanceLft <= isHandRemovedCAL || distanceLft == 0)
            {
                calculateDistance();

                if ((tempL + isHandInFrontCAL) < distanceLft)
                {
                    gestureIs = "volume D   ";
                }
                if ((tempL - isHandInFrontCAL) < distanceLft)
                {
                    gestureIs = "volume U   ";
                }
            }
        }
        else
        {
            gestureIs = "previous T";
        }
    }
    return gestureIs;
}

//Necessary Libraries for the Project
#include <LiquidCrystal.h>
#include <Adafruit_NeoPixel.h>
#include <Servo.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

//Necessary definitions for Neopixel
#define PIN 6 // input pin Neopixel is attached to

#define NUMPIXELS 6 // number of neopixels in strip

// Variables for Servo
int pos = 0;

Servo servo_10;

int delayval = 20; // timing delay in milliseconds

// Variables for Neopixel
int redColor = 0;
int greenColor = 0;
int blueColor = 0;

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

//Assigning values to the variables used in calculating Distance
int Ultasonic_trig = 0;

int Ultrsonic_echo = 0;

int inches = 0;

int distance_cm = 0;

//For Pushbutton
int buttonState = 0;

//Assigning values to the variables used in calculating Temperature
int celsius = 0;

int temp = 0;

//Assigning values to the variables used to calculate peoples
int people_inside = 0;
int ppl_fever = 0;

//Messages to Display
char msg1[] = "Welcome to AutoSan!";
char msg2[] = "Your Temperature is High, Seek Medical";
char msg2_1[] = "Help ... and don't forget to Sanitiz";
char msg2_2[] = "e your hands ... ";
char msg3[] = "Your Temperature is Fine, Sanitize you";
char msg3_1[] = "r hands ...";

void setup()
{
    pinMode(A0, INPUT); // Temperature Sensor
    pinMode(9, OUTPUT); // Buzzer
    pinMode(7, OUTPUT); // Ultrasonic Trigg Pin
    pinMode(8, INPUT);  // Ultrasonic Echo Pin
    pinMode(13, INPUT); // Push Button
    servo_10.attach(10, 500, 2500);
    // Initialize the NeoPixel library.
    pixels.begin();
    // set up the LCD's number of columns and rows:
    lcd.begin(16, 2);

    Serial.begin(9600);   // Initializing  Serial Monitor
    Serial.println(msg1); // Welcome Message
}

void loop()
{
    buttonState = digitalRead(13);
    if (buttonState == HIGH)
    {
        Serial.print("People inside :");
        Serial.println(people_inside);
        Serial.print("People with high temperature :");
        Serial.println(ppl_fever);
    }
    neopixel('W');
    // set the cursor to column 0, line 1
    // (note: line 1 is the second row, since counting begins with 0):
    lcd.setCursor(0, 1);
    // print the number of people entered in room since last reset:
    lcd.print("No. of People- ");
    lcd.print(people_inside);
    // printing Welcome Message
    print(msg1);
    // formula to calculate distance
    distance_cm = 0.01723 * readUltrasonicDistance(7, 8);
    // checking for the condition to detect temperature

    if (distance_cm >= 10 and distance_cm <= 20)
    {
        Serial.print("Distance : ");
        Serial.print(distance_cm);
        Serial.println(" cm");
        Serial.print("People inside :");
        Serial.println(people_inside);
        temp = temperature(); // variable temp gets the temperature measured from sensor

        if (temp >= 36 and temp <= 38)
        {
            // normal temperature range of human body
            print_body_temp();
            neopixel('G');
            print(msg3);
            print(msg3_1);
            delay(1000);
            sanitizer();
            people_inside++;
        }
        else if (temp > 38 and temp < 41)
        { // checking for temperature range
            // body temperature in fever
            print_body_temp();
            neopixel('R');
            tone(9, 262); // turn on the buzzer
            print(msg2);
            print(msg2_1);
            print(msg2_2);
            delay(1000);
            sanitizer();
            noTone(9); // turn off the buzzer
            ppl_fever++;
        }
    }
}

long readUltrasonicDistance(int triggerPin, int echoPin)
{
    pinMode(triggerPin, OUTPUT); // Clear the trigger
    digitalWrite(triggerPin, LOW);
    delayMicroseconds(2);
    // Sets the trigger pin to HIGH state for 10 microseconds
    digitalWrite(triggerPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(triggerPin, LOW);
    pinMode(echoPin, INPUT);
    // Reads the echo pin, and returns the sound wave travel time in microseconds
    return pulseIn(echoPin, HIGH);
}

// calculating temperature in degree Celsius
int temperature()
{
    celsius = map(((analogRead(A0) - 20) * 3.04), 0, 1023, -40, 125);
    return celsius;
}

// Displaying neopixel according to detection of temperature
int neopixel(char color)
{
    if (color == 'R')
    {
        redColor = 255;
        greenColor = 0;
    }
    else if (color == 'G')
    {
        redColor = 0;
        greenColor = 255;
    }
    else
    {
        redColor = 0;
        greenColor = 0;
    }
    for (int i = 0; i < NUMPIXELS; i++)
    {
        // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
        pixels.setPixelColor(i, pixels.Color(redColor, greenColor, blueColor));

        // This sends the updated pixel colour to the hardware.
        pixels.show();

        // Delay for a period of time (in milliseconds).
        delay(delayval);
    }
}

// using servo to press the sanitizer and allow user to sanitize their hands
int sanitizer()
{
    // sweep the servo from 0 to 90 degrees in steps
    // of 1 degrees
    for (pos = 0; pos <= 90; pos += 1)
    {
        // tell servo to go to position in variable 'pos'
        servo_10.write(pos);
        // wait 15 ms for servo to reach the position
        delay(20); // Wait for 15 millisecond(s)
    }
    for (pos = 90; pos >= 0; pos -= 1)
    {
        // tell servo to go to position in variable 'pos'
        servo_10.write(pos);
        // wait 15 ms for servo to reach the position
        delay(20); // Wait for 15 millisecond(s)
    }
}

// printing message passed by argument
int print(char text[])
{
    unsigned int i = 0;
    // set the cursor to (0,0):
    lcd.setCursor(0, 0);

    while (text[i] != '\0')
    {
        lcd.print(text[i]);

        if (i >= 14)
        {
            lcd.command(0x18); //Scrolling text to Right
        }
        delay(100);
        i++;
    }
    // clear screen for the next loop:
    lcd.clear();
}

// prints the body temperature of user
void print_body_temp()
{
    Serial.print("Temperature of Body : ");
    Serial.print(temp);
    Serial.println(" °C");
    lcd.print("Temperature : ");
    lcd.print(temp);
    delay(500);
    lcd.clear();
}
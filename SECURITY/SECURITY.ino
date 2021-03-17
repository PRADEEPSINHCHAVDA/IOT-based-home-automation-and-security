/*
 Project Title: Home Security System
 Author:pradeepsinh chavda
,
*/

/////////////////////////////////////////////////////////////////
#include <LiquidCrystal.h>
#include <Wire.h>
#include <Password.h> //http://www.arduino.cc/playground/uploads/Code/Password.zip
#include <Keypad.h> //http://www.arduino.cc/playground/uploads/Code/Keypad.zip
#include <Servo.h> 
#include "RTClib.h"

//Servo
Servo myservo;        // create servo object to control a servo           
int pos = 90;         // variable to store the servo position 
int passwd_pos = 15;  // the postition of the password input

//Real Time Clock
RTC_DS1307 RTC; 

//Password
Password password = Password( "1234" );

const byte ROWS = 4; // Four rows
const byte COLS = 4; // Four columns

char keys[ROWS][COLS] = { // Define the Keymap
  {
    '1','2','3','A'      }
  ,
  {
    '4','5','6','B'      }
  ,
  {
    '7','8','9','C'      }
  ,
  {
    '*','0','#','D'      }
};

byte rowPins[ROWS] = {
  46, 47, 48, 49};     //connect to the row pinouts of the keypad
byte colPins[COLS] = {
  50, 51, 52, 53};     //connect to the column pinouts of the keypad

// Create the Keypad
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(7, 8, 9, 10, 11, 12); // Assignign arduino pins to LCD display module

//Stroke LED Lights
int ledDelay = 50; // delay by 50ms
int redPin = 29;
int bluePin = 31;

//constants for LEDs, inputs and outputs
//int blueLED = 36;
int greenLED = 37;
int redLED = 38;
int pirPin1 = 39;
int pirPin2 = 34;

int reedPin1 = 41;
int reedPin2 = 42;
int speakerPin = 35; 
//int relay1 = 3; // 
int relay2 = 4; // connected to 12V Blue LED strip
int relay3 = 5; // Cam
int relay4 = 6; // 

int alarmStatus = 0;
int zone = 0;
int alarmActive = 0;

void setup(){
  Serial.begin(9600);
  lcd.begin(20, 4);
  //Adding time 
  Wire.begin();
  RTC.begin();
  //If we remove the comment from the following line, we will set up the module time and date with the computer one
  RTC.adjust(DateTime(__DATE__, __TIME__));
  
  myservo.attach(2);  // attaches the servo on pin 2 to the servo object 

  displayCodeEntryScreen();

  //Police LED Lights
  pinMode(redPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  
  //setup and turn off both LEDs
  pinMode(redLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(speakerPin, OUTPUT);
  
  //pinMode(relay1, OUTPUT);  
  pinMode(relay2, OUTPUT);  //12V Blue LED lighting 
  pinMode(relay3, OUTPUT);  //camera, 5V external DC supply
  pinMode(relay4, OUTPUT);  //
 
  pinMode(pirPin1, INPUT);  //Bedroom 2
  pinMode(pirPin2, INPUT);  //Garage
  pinMode(reedPin1, INPUT); //Front door
  pinMode(reedPin2, INPUT); //Back door

  digitalWrite(redLED, LOW);
  digitalWrite(greenLED, HIGH);
  digitalWrite(speakerPin, LOW);
 
  //digitalWrite(relay1, LOW); // 
  digitalWrite(relay2, HIGH); // 12V Blue LED lighting 
  digitalWrite(relay3, HIGH); // camera, 5V external DC supply
  digitalWrite(relay4, LOW); // 

  keypad.addEventListener(keypadEvent); //add an event listener for this keypad
  myservo.write(pos);
}

void loop(){
  //display time and date
  DateTime now = RTC.now();

  //DATE
  lcd.setCursor(0,1);
  lcd.print(now.month(), DEC);
  lcd.print('/'); 
  //We print the day
  lcd.print(now.day(), DEC);
  lcd.print('/');  
  //We print the year
  lcd.print(now.year(), DEC);
  lcd.print(' ');

  //TIME
  lcd.setCursor(13,1);
  lcd.print(now.hour(), DEC);
  lcd.print(':');
  lcd.setCursor(16,1);
  lcd.print(now.minute(), DEC);
  //lcd.print(':');
  //lcd.print(now.second(), DEC);
  //delay(1000);

  keypad.getKey();
  //Serial.println(digitalRead(reedPin2));
  //Serial.println(digitalRead(pirPin));
  //Serial.println(digitalRead(pirPin2));
  if (alarmActive == 1){ 
    if (digitalRead(pirPin1) == HIGH)
    {
      zone = 0;
      alarmTriggered();
    }
    if (digitalRead(reedPin1) == LOW)
    {
      zone = 1;
      alarmTriggered();
    }
    if (digitalRead(reedPin2) == LOW)
    {
      zone = 2;
      alarmTriggered();
    }
    
    if (digitalRead(pirPin2) == HIGH)
     {
     zone = 3;
     alarmTriggered();
     }
   }
}

/////////////////////////  Functions  /////////////////////////////////
//take care of some special events
void keypadEvent(KeypadEvent eKey){
  switch (keypad.getState()){
  case PRESSED:
    if (passwd_pos - 15 >= 5) { 
      return ;
    }
    lcd.setCursor((passwd_pos++),0);
    switch (eKey){
    case '#':                 //# is to validate password 
      passwd_pos  = 15;
      checkPassword(); 
      break;
    case '*':                 //* is to reset password attempt
      password.reset(); 
      passwd_pos = 15;
   // TODO: clear the screen output 
      break;
    default: 
      password.append(eKey);
      lcd.print("*");
    }
  }
}

void alarmTriggered(){
  int expected_pos;
  int incr;
  digitalWrite(speakerPin, HIGH);
  digitalWrite(relay2, LOW);
  digitalWrite(relay3, LOW);
  digitalWrite(redPin, HIGH);
  
//
  password.reset();
  alarmStatus = 1;
  // alarmActive = 0;
  lcd.clear();
  lcd.setCursor(0,2);
  lcd.print("  SYSTEM TRIGGERED  ");
  lcd.setCursor(0,4);
  if (zone == 1)
  { 
    lcd.print("  Front Door Open  ");
    expected_pos = 65;
    delay(1000);
  }
   if(zone == 0){
    expected_pos = 40;
    lcd.print("Motion in Bedroom 1 ");
    delay(1000);
  }
  else if(zone == 2){
    expected_pos = 10;
    lcd.print("Backdoor Open");
    delay(1000);
  }
   else if(zone == 3){
     expected_pos = 145;
   lcd.print("Motion in Garage");
   delay(1000);
   }
   
   if (expected_pos > pos) {
     incr = 1;
   } else {
     incr = -1;
   }
   
   for (pos = pos; pos != expected_pos; pos += incr) {
    myservo.write(pos);                  // tell servo to go to position in variable 'pos' 
    delay(5);                            // waits 5ms for the servo to reach the position 
   }
   
   /*
   for(pos = 0; pos < angle; pos += 1)   // goes from 0 degrees to 180 degrees 
  {                                      // in steps of 1 degree 
    myservo.write(pos);                  // tell servo to go to position in variable 'pos' 
    delay(20);                           // waits 15ms for the servo to reach the position 
  } 
  for(pos = angle; pos>=1; pos-=1)       // goes from 180 degrees to 0 degrees 
  {                                
   myservo.write(pos);                   // tell servo to go to position in variable 'pos' 
   delay(20);                            // waits 15ms for the servo to reach the position 
  } 
  */
  
  {
   StrokeLight();
  }
}

void StrokeLight(){                                                      //Stroke LED Lights
    digitalWrite(redPin, HIGH);        // turn the red light on
    delay(ledDelay); // wait 50 ms
    digitalWrite(redPin, LOW);         // turn the red light off
    delay(ledDelay); // wait 50 ms
    digitalWrite(redPin, HIGH);        // turn the red light on
    delay(ledDelay); // wait 50 ms
    digitalWrite(redPin, LOW);         // turn the red light off
    delay(ledDelay); // wait 50 ms
    digitalWrite(redPin, HIGH);        // turn the red light on
    delay(ledDelay); // wait 50 ms
    digitalWrite(redPin, LOW);         // turn the red light off
    delay(ledDelay); // wait 50 ms
    delay(10); // delay midpoint by 100ms
    digitalWrite(bluePin, HIGH);       // turn the blue light on
    delay(ledDelay); // wait 50 ms
    digitalWrite(bluePin, LOW);        // turn the blue light off
    delay(ledDelay); // wait 50 ms
    digitalWrite(bluePin, HIGH);       // turn the blue light on
    delay(ledDelay); // wait 50 ms
    digitalWrite(bluePin, LOW);        // turn the blue light off
    delay(ledDelay); // wait 50 ms
    digitalWrite(bluePin, HIGH);       // turn the blue light on
    delay(ledDelay); // wait 50 ms
    digitalWrite(bluePin, LOW);        // turn the blue light off
    delay(ledDelay); // wait 50 ms
    }                                                          

void checkPassword(){                  // To check if PIN is corrected, if not, retry!
  if (password.evaluate())
  {  
    if(alarmActive == 0 && alarmStatus == 0)
    {
      activate();
    } 
    else if( alarmActive == 1 || alarmStatus == 1) {
      deactivate();
    }
  } 
  else {
    invalidCode();
  }
}  

void invalidCode()    // display meaasge when a invalid is entered
{
  password.reset();
  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("INVALID CODE! LOL!");
  lcd.setCursor(5,2);
  lcd.print("TRY AGAIN!");
  digitalWrite(greenLED, LOW);
  digitalWrite(redLED, HIGH);
  delay(2000);
  digitalWrite(redLED, LOW);
  delay(1000);
  displayCodeEntryScreen();
}

void activate()      // Activate the system if correct PIN entered and display message on the screen
{
  if((digitalRead(reedPin1) == HIGH) && (digitalRead(reedPin2) == HIGH)){
    digitalWrite(redLED, HIGH);
    digitalWrite(greenLED, LOW);
    digitalWrite(2, HIGH);
    lcd.setCursor(0,0);
    lcd.print("SYSTEM ACTIVE!"); 
    alarmActive = 1;
    password.reset();
    delay(2000);
  }
  else{
    deactivate();   // if PIN not corrected, run "deactivate" loop
  }
}

void deactivate()
{
  //digitalWrite(camera, LOW);
  alarmStatus = 0;
  digitalWrite(redLED, LOW);
  digitalWrite(greenLED, HIGH);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(" SYSTEM DEACTIVATED!");
  digitalWrite(speakerPin, LOW);
  alarmActive = 0;
  password.reset();
  delay(5000);
  digitalWrite(relay2, HIGH);
  digitalWrite(relay3, HIGH);


  displayCodeEntryScreen();
}

void displayCodeEntryScreen()    // Dispalying start screen for users to enter PIN
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Enter PIN:");
  lcd.setCursor(0,2);
  lcd.print("Home Security System");
  lcd.setCursor(0,3);
  lcd.print("By pradeepsinh chavda");
}

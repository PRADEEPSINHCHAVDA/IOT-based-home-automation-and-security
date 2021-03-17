#include <Servo.h>

Servo servo_9;

int counter;

void setup()
{
  servo_9.attach(9, 500, 2500);

  pinMode(13, OUTPUT);
}

void loop()
{
  for (counter = 0; counter < 10; ++counter) {
    servo_9.write(0);
    delay(1000); // Wait for 1000 millisecond(s)
    servo_9.write(180);
    delay(1000); // Wait for 1000 millisecond(s)
    servo_9.write(90);
    delay(1000); // Wait for 1000 millisecond(s)
    digitalWrite(13, HIGH);
    delay(1000); // Wait for 1000 millisecond(s)
    digitalWrite(13, LOW);
    delay(1000); // Wait for 1000 millisecond(s)
  }
}
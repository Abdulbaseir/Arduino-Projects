#include <Servo.h>  // Include the Servo library

// Create a Servo object
Servo myServo;

// Pin definitions
const int joystickXPin = A0;  // Analog pin connected to VRx of the joystick
const int servoPin = 9;       // Digital pin connected to the signal pin of the servo

void setup() {
  myServo.attach(servoPin);  // Attach the servo to the specified pin
  Serial.begin(9600);        // Start serial communication for debugging
}

void loop() {
  int joystickXValue = analogRead(joystickXPin);  // Read the X-axis value from the joystick

  // Map the joystick X-axis value (0-1023) to servo angle (0-180)
  int servoAngle = map(joystickXValue, 0, 1023, 0, 180);

  myServo.write(servoAngle);  // Set the servo position
  delay(15);                  // Allow time for the servo to reach the position

  // Print the values for debugging
  Serial.print("Joystick X: ");
  Serial.print(joystickXValue);
  Serial.print(" -> Servo Angle: ");
  Serial.println(servoAngle);
}

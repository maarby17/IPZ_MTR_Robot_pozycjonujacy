// First robot software on arduino for tests 

#include <RC5.h>

#define LED 13
#define BUZZER 10
#define PUSHBUTTON 2
#define L_MOTOR_DIR 4
#define L_MOTOR_PWM 5
#define R_MOTOR_DIR 9
#define R_MOTOR_PWM 6
#define MOTOR_PWM_MAX 150
#define TSOP_PIN 3
#define SR04_FRONT_ECHO 7
#define SR04_FRONT_TRIG 8

RC5 rc5(TSOP_PIN);

byte address;
byte command;
byte toggle;

 void setup() {
  // Setting up outputs
  pinMode(LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(L_MOTOR_DIR, OUTPUT);
  pinMode(L_MOTOR_PWM, OUTPUT);
  pinMode(R_MOTOR_DIR, OUTPUT);
  pinMode(R_MOTOR_PWM, OUTPUT);
  
  // Setting up inputs
  pinMode(PUSHBUTTON, INPUT);

  // Setting all outputs off
  digitalWrite(LED, LOW);
  digitalWrite(BUZZER, LOW);

  // Setting up serial
  Serial.begin(9600);

  // Test LED blink
  digitalWrite(LED, HIGH);
  delay(500);
  digitalWrite(LED, LOW);
  delay(500);

  //Setting up echo sensors
  pinMode(SR04_FRONT_ECHO, INPUT);
  pinMode(SR04_FRONT_TRIG, OUTPUT);
}

 // Function for setting up speed and direction of left motor
void leftMotorMove(int _iL){
  if (_iL >0) digitalWrite(L_MOTOR_DIR, 0);
  else {
    digitalWrite(L_MOTOR_DIR, 1);
     _iL = abs(_iL);
  }
  
  _iL = map(_iL, 0, 100, 0, MOTOR_PWM_MAX);
  analogWrite(L_MOTOR_PWM, _iL);
}

// Function for setting up speed and direction of right motor
void rightMotorMove(int _iR){
  if (_iR >0) digitalWrite(R_MOTOR_DIR, 0);
  else { 
    digitalWrite(R_MOTOR_DIR, 1);
    _iR = abs(_iR);
  }
 
  _iR = map(_iR, 0, 100, 0, MOTOR_PWM_MAX);
  analogWrite(R_MOTOR_PWM, _iR);
}

// Function for motors stop
void stopMotors(void){
   digitalWrite(L_MOTOR_DIR, 0);
   analogWrite(L_MOTOR_PWM, 0);
   digitalWrite(R_MOTOR_DIR, 0);
   analogWrite(R_MOTOR_PWM, 0);
}

// Function turning robot left by 90 degrees
void turnLeft (void){
  rightMotorMove(55); // 50 speed is minimum
  delay(550);
  stopMotors();
}

// Function turning robot right by 90 degrees
void turnRight (void){
  leftMotorMove(55); // 50 speed is minimum
  delay(550);
  stopMotors();
}

// Function moving robot forward
void forward (void){
  leftMotorMove(50); // 50 speed is minimum
  rightMotorMove(50);
  delay(200);
  stopMotors();
}

// Function moving robot Backward
void backward (void){
  leftMotorMove(-50); // 50 speed is minimum
  rightMotorMove(-50);
  delay(200);
  stopMotors();
}

int distance(void){
  long _lTime, _lDistance;

  digitalWrite(SR04_FRONT_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(SR04_FRONT_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(SR04_FRONT_TRIG, LOW);

  _lTime = pulseIn(SR04_FRONT_ECHO, HIGH);
  _lDistance = _lTime / 58;

  return _lDistance;
}

void loop() {
  Serial.print(distance());
  Serial.println(" cm");
   if(rc5.read(&toggle, &address, &command)){
      switch(command){
        case 2:
          forward();
        break;
        case 4:
          turnLeft();
        break;
        case 6:
          turnRight();
        break;
        case 8:
          backward();
        break;
      }
   }
}

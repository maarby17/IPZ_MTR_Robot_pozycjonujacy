// First version of robot software (only data)

#include <Wire.h>
#include <LSM303.h>

#define LED                 13
#define BUZZER              10
#define PUSHBUTTON           2
#define L_MOTOR_DIR          4
#define L_MOTOR_PWM          5
#define R_MOTOR_DIR          9
#define R_MOTOR_PWM          6
#define MOTOR_PWM_MAX      150
#define TSOP_PIN             3
#define SR04_FRONT_ECHO      7
#define SR04_FRONT_TRIG      8
#define SR04_BACK_ECHO      12
#define SR04_BACK_TRIG      13
#define SR04_RIGHT_ECHO     16
#define SR04_RIGHT_TRIG     17
#define SR04_LEFT_ECHO      14
#define SR04_LEFT_TRIG      15
#define LSM303D_ADDR      0x3a

LSM303 compass;

char report[80];

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
  pinMode(SR04_BACK_ECHO, INPUT);
  pinMode(SR04_BACK_TRIG, OUTPUT);
  pinMode(SR04_RIGHT_ECHO, INPUT);
  pinMode(SR04_RIGHT_TRIG, OUTPUT);
  pinMode(SR04_LEFT_ECHO, INPUT);
  pinMode(SR04_LEFT_TRIG, OUTPUT);

  // Setting up I2C
  Wire.begin(); 

  //Setting up compass
  compass.init();
  compass.enableDefault();
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

int distance(int _iPin_TRIG,int _iPin_ECHO){
  long _lTime, _lDistance;

  digitalWrite(_iPin_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(_iPin_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(_iPin_TRIG, LOW);

  _lTime = pulseIn(_iPin_ECHO, HIGH);
  _lDistance = _lTime / 58;

  return _lDistance;
}

void compass_ping(void){
  compass.read();
  float _a_x = compass.a.x * 2.0f / 32678.0f;
  float _a_y = compass.a.y * 2.0f / 32678.0f;
  float _a_z = compass.a.z * 2.0f / 32678.0f;
  
  //snprintf(report, sizeof(report), "A: %f %f %f   M: %6d %6d %6d", _a_x, _a_y, _a_z, compass.m.x, compass.m.y, compass.m.z);
  Serial.print("A: ");
  Serial.print(_a_x); Serial.print(" ");
  Serial.print(_a_y); Serial.print(" ");
  Serial.println(_a_z);
  Serial.print("M: ");
  Serial.print(compass.m.x); Serial.print(" ");
  Serial.print(compass.m.y); Serial.print(" ");
  Serial.println(compass.m.z); 
}


void loop() {
  Serial.print(distance(SR04_FRONT_TRIG, SR04_FRONT_ECHO));
  Serial.println(" cm front");
  Serial.print(distance(SR04_BACK_TRIG, SR04_BACK_ECHO));
  Serial.println(" cm back");
  Serial.print(distance(SR04_RIGHT_TRIG, SR04_RIGHT_ECHO));
  Serial.println(" cm right");
  Serial.print(distance(SR04_LEFT_TRIG, SR04_LEFT_ECHO));
  Serial.println(" cm left");

  compass_ping();
  //Serial.println(report); 
  
 
  // forward();
  // delay(500);
    
  // turnLeft();
  // delay(500);

  // turnRight();
  // delay(500);
    
  // backward();
  delay(5000);
   
}

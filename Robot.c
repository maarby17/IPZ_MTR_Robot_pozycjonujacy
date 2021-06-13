// First version of robot software with movement

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
#define ANGLE_NORTH        125
#define ANGLE_OFFSET         4
#define DISTANCE_OFFSET      5
#define LEFT_OFFSET          7
#define RIGHT_OFFSET         7
#define BACK_OFFSET         21

LSM303 compass;

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

  // Setting up echo sensors
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

  // Setting up compass
  compass.init();
  compass.enableDefault();

  // Compass calibration
  compass.m_min = (LSM303::vector<int16_t>){-2568, -2729, -2019}; // -2568, -2729, -2019
  compass.m_max = (LSM303::vector<int16_t>){+2887, +2629, +3849}; // +2887, +2629, +3849

  // Setting up Z asix as a reference for compass
  compass.heading((LSM303::vector<int>){0, 0, 1});
}

void leftMotorMove(int _iL){ // (F) Setting up speed and direction of left motor
  if (_iL >0) digitalWrite(L_MOTOR_DIR, 0);
  else {
    digitalWrite(L_MOTOR_DIR, 1);
     _iL = abs(_iL);
  }
  
  _iL = map(_iL, 0, 100, 0, MOTOR_PWM_MAX);
  analogWrite(L_MOTOR_PWM, _iL);
}

void rightMotorMove(int _iR){ // (F)  Setting up speed and direction of right motor
  if (_iR >0) digitalWrite(R_MOTOR_DIR, 0);
  else { 
    digitalWrite(R_MOTOR_DIR, 1);
    _iR = abs(_iR);
  }
 
  _iR = map(_iR, 0, 100, 0, MOTOR_PWM_MAX);
  analogWrite(R_MOTOR_PWM, _iR);
}

void stopMotors(void){ // (F) Motors stop
   digitalWrite(L_MOTOR_DIR, 0);
   analogWrite(L_MOTOR_PWM, 0);
   digitalWrite(R_MOTOR_DIR, 0);
   analogWrite(R_MOTOR_PWM, 0);
}

void turnLeft (int _iSpeed, int _iTime = 200){ // (F) Turn robot left with speed (50 - 100)
  rightMotorMove(_iSpeed);
  delay(_iTime);
  stopMotors();
}

void turnRight (int _iSpeed, int _iTime = 200){ // (F) Turns robot right with speed (50 - 100)
  leftMotorMove(_iSpeed); // 50 speed is minimum
  delay(_iTime);
  stopMotors();
}


void forward (int _iSpeed, int _iTime = 200){ // (F) Move robot forward
  leftMotorMove(_iSpeed); // 50 speed is minimum
  rightMotorMove(_iSpeed);
  delay(_iTime);
  stopMotors();
}

void backward (int _iSpeed, int _iTime = 200){ // (F) Move robot backward
  leftMotorMove(-_iSpeed); // 50 speed is minimum
  rightMotorMove(-_iSpeed);
  delay(_iTime);
  stopMotors();
}

int getDistance (int _iPin_TRIG,int _iPin_ECHO){ // (F) Get distance from echo sensor
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

void compassPing (void){
  compass.read();
  float _a_x = compass.a.x * 2.0f / 32678.0f;
  float _a_y = compass.a.y * 2.0f / 32678.0f;
  float _a_z = compass.a.z * 2.0f / 32678.0f;

  float heading = compass.heading();
  
  Serial.print("A: ");
  Serial.print(_a_x); Serial.print(" ");
  Serial.print(_a_y); Serial.print(" ");
  Serial.println(_a_z);
  Serial.println(heading);
}

void headNorth (void){
  int _iS;
  
  compass.read();
  float heading = compass.heading();

  while(heading > (ANGLE_NORTH + ANGLE_OFFSET) || heading < (ANGLE_NORTH - ANGLE_OFFSET)){
    if(heading > (ANGLE_NORTH + ANGLE_OFFSET)){
      _iS = map(heading, (ANGLE_NORTH + ANGLE_OFFSET), 360, 65, 100);
      turnLeft(_iS, 25);
    }
    else{
      if(heading < (ANGLE_NORTH - ANGLE_OFFSET)){
        _iS = map(heading, 0, (ANGLE_NORTH - ANGLE_OFFSET), 100, 65);
        turnRight(_iS, 25);
      }
    }
  compass.read();
  heading = compass.heading();
  }
}

void goalXY (int _iX, int _iY){
  int _iActualX, _iActualY, _iD, _iXg = 0, _iYg = 0;

  while(1){
    headNorth();
    delay(1000);
    
    _iActualY = getDistance(SR04_FRONT_TRIG, SR04_FRONT_ECHO);
    if(_iActualY > (_iY+DISTANCE_OFFSET) && _iYg == 0){
      _iD = map((_iActualY-_iY), 0, 230, 65, 100);
      forward(_iD);
   }
   else {
      if(_iActualY < (_iY-DISTANCE_OFFSET) && _iYg == 0){
         _iD = map((_iY-_iActualY), 0, 230, 65, 100);
        backward(_iD);
     }
     else _iYg = 1;
    }

    _iActualX = getDistance(SR04_LEFT_TRIG, SR04_LEFT_ECHO);
     if(_iActualX > (_iX+DISTANCE_OFFSET) && _iXg == 0){
      _iD = map((_iActualX-_iX), 0, 115, 65, 100);
      turnLeft(75);
      forward(_iD);
      turnRight(75);
   }
   else {
      if(_iActualX < (_iX-DISTANCE_OFFSET) && _iXg == 0){
         _iD = map((_iX-_iActualX), 0, 115, 65, 100);
       turnRight(75);
       forward(_iD);
       turnLeft(75);
     }
     else _iXg = 1;
    }

    if (_iXg == 1 && _iYg == 1) break;
  }
  
}


void loop() {

  //compassPing();

  goalXY(75, 50);
  
  delay(2000);
}

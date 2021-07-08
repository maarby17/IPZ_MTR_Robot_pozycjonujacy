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
#define MOTOR_PWM_MIN      100
#define TSOP_PIN             3
#define SR04_FRONT_ECHO      7
#define SR04_FRONT_TRIG      8
#define SR04_BACK_ECHO      12
#define SR04_BACK_TRIG      13
#define SR04_RIGHT_ECHO     16
#define SR04_RIGHT_TRIG     17
#define SR04_LEFT_ECHO      14
#define SR04_LEFT_TRIG      15
#define LSM303D_SDA         18
#define LSM303D_SCL         19
#define LSM303D_ADDR      0x3a
#define ESP_RX               0
#define ESP_TX               1
#define ANGLE_NORTH        127
#define ANGLE_OFFSET         1
#define DISTANCE_OFFSET      5
#define LEFT_OFFSET          7
#define RIGHT_OFFSET         7
#define BACK_OFFSET         21
#define COMPARE_OFFSET       5
#define MOVE_GAP            10
#define NUMBER_OF_POINTS   169

LSM303 compass;

int _gMapPoints[NUMBER_OF_POINTS][4]= { 
  {25,25,175,185},{35,25,165,185},{45,25,155,185},{55,25,145,185},{65,25,135,185},{75,25,125,185},{85,25,115,185},{95,25,105,185},{105,25,95,185},{115,25,85,185},{125,25,75,185},{135,25,65,185},{145,25,55,185},{155,25,45,185},{165,25,35,185},{175,25,25,185},
  {25,35,175,175},{35,35,165,175},{45,35,155,175},{55,35,145,175},{65,35,135,175},{75,35,125,175},{85,35,115,175},{95,35,105,175},{105,35,95,175},{115,35,85,175},{125,35,75,175},{135,35,65,175},{145,35,55,175},{155,35,45,175},{165,35,35,175},{175,35,25,175},
  {25,45,175,165},{35,45,165,165},{45,45,155,165},{55,45,145,165},{65,45,135,165},{75,45,125,165},{85,45,115,165},{95,45,105,165},{105,45,95,165},{115,45,85,165},{125,45,75,165},{135,45,65,165},{145,45,55,165},{155,45,45,165},{165,45,35,165},{175,45,25,165},
  {25,55,175,155},{35,55,165,155},{45,55,155,155},{55,55,145,155},{65,55,135,155},{75,55,125,155},{85,55,115,155},{95,55,105,155},{105,55,95,155},{115,55,85,155},{125,55,75,155},{135,55,65,155},{145,55,55,155},{155,55,45,155},{165,55,35,155},{175,55,25,155},
  {25,65,175,145},{35,65,165,145},{45,65,155,145},{55,65,145,145},{65,65,135,145},{75,65,125,145},{85,65,115,145},
  {25,75,165,135},{35,75,155,135},{45,75,145,135},{55,75,135,135},{65,75,125,135},{75,75,115,135},{85,75,105,135},
  {25,85,100,125},{35,85,90,125},{45,85,80,125},{55,85,70,125},{65,85,60,125},{75,85,50,125},{85,85,40,125},
  {25,95,100,115},{35,95,90,115},{45,95,80,115},{55,95,70,115},{65,95,60,115},{75,95,50,115},{85,95,40,115},
  {25,105,110,105},{35,105,110,105},{45,105,110,105},{55,105,110,105},{65,105,110,105},{75,105,110,105},{85,105,110,105},
  {25,115,110,95},{35,115,110,95},{45,115,110,95},{55,115,110,95},{65,115,110,95},{75,115,110,95},{85,115,110,95},
  {25,125,110,85},{35,125,110,85},{45,125,110,85},{55,125,110,85},{65,125,110,85},{75,125,110,85},{85,125,110,85},
  {25,135,110,75},{35,135,110,75},{45,135,110,75},{55,135,110,75},{65,135,110,75},{75,135,110,75},{85,135,110,75},
  {25,145,110,65},{35,145,110,65},{45,145,110,65},{55,145,110,65},{65,145,110,65},{75,145,110,65},{85,145,110,65},
  {25,155,110,55},{35,155,110,55},{45,155,110,55},{55,155,110,55},{65,155,110,55},{75,155,110,55},{85,155,110,55},
  {25,165,90,45},{35,165,90,45},{45,165,90,45},{55,165,90,45},{65,165,90,45},{75,165,90,45},{85,165,90,45},
  {25,175,90,35},{35,175,90,35},{45,175,90,35},{55,175,90,35},{65,175,90,35},{75,175,90,35},{85,175,90,35},
  {25,185,90,25},{35,185,90,25},{45,185,90,25},{55,185,90,25},{65,185,90,25},{75,185,90,25},{85,185,90,25},
  {25,195,90,15},{35,195,90,15},{45,195,90,15},{55,195,90,15},{65,195,90,15},{75,195,90,15},{85,195,90,15},
  {25,205,90,5},{35,205,90,5},{45,205,90,5},{55,205,90,5},{65,205,90,5},{75,205,90,5},{85,205,90,5},
  };
int _gPointsWeight[NUMBER_OF_POINTS];

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
  pinMode(ESP_TX, INPUT);
  pinMode(ESP_RX, OUTPUT);

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

  // Reseting points weights
  for(int i = 0; i++; i < NUMBER_OF_POINTS) _gPointsWeight[i] = 0; 
}

void leftMotorMove(int _iL){ // (F) Setting up speed and direction of left motor
  if (_iL > 0) digitalWrite(L_MOTOR_DIR, 0);
  else {
    digitalWrite(L_MOTOR_DIR, 1);
     _iL = abs(_iL);
  }
  
  _iL = map(_iL, 0, 100, MOTOR_PWM_MIN, MOTOR_PWM_MAX);
  analogWrite(L_MOTOR_PWM, _iL);
}

void rightMotorMove(int _iR){ // (F)  Setting up speed and direction of right motor
  if (_iR > 0) digitalWrite(R_MOTOR_DIR, 0);
  else { 
    digitalWrite(R_MOTOR_DIR, 1);
    _iR = abs(_iR);
  }
 
  _iR = map(_iR, 0, 100, MOTOR_PWM_MIN, MOTOR_PWM_MAX);
  analogWrite(R_MOTOR_PWM, _iR);
}

void stopMotors(void){ // (F) Motors stop
   digitalWrite(L_MOTOR_DIR, 0);
   analogWrite(L_MOTOR_PWM, 0);
   digitalWrite(R_MOTOR_DIR, 0);
   analogWrite(R_MOTOR_PWM, 0);
}

void turnLeft (int _iSpeed, int _iTime = 200){ // (F) Turn robot left with speed
  rightMotorMove(_iSpeed);
  delay(_iTime);
  stopMotors();
}

void turnRight (int _iSpeed, int _iTime = 200){ // (F) Turns robot right with speed
  leftMotorMove(_iSpeed); 
  delay(_iTime);
  stopMotors();
}


void forward (int _iSpeed, int _iTime = 200){ // (F) Move robot forward
  leftMotorMove(_iSpeed); 
  rightMotorMove(_iSpeed);
  delay(_iTime);
  stopMotors();
}

void backward (int _iSpeed, int _iTime = 200){ // (F) Move robot backward
  leftMotorMove(-_iSpeed);
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

void compassPing (void){ //(F) Ping for new compass scan
  compass.read();
  float _a_x = compass.a.x * 2.0f / 32678.0f;
  float _a_y = compass.a.y * 2.0f / 32678.0f;
  float _a_z = compass.a.z * 2.0f / 32678.0f;

  float heading = compass.heading();
}

void headNorth (void){
  int _iS, _iDiff;
  
  compass.read();
  int heading = compass.heading();
  
  while(heading > (ANGLE_NORTH + ANGLE_OFFSET) || heading < (ANGLE_NORTH - ANGLE_OFFSET)){
    if(heading > (ANGLE_NORTH + ANGLE_OFFSET)){
       _iDiff = heading - ANGLE_NORTH;
      _iS = map(_iDiff, 0, (360 - ANGLE_NORTH), 0, 100);
      turnLeft(_iS, 25);
    }
    else{
      if(heading < (ANGLE_NORTH - ANGLE_OFFSET)){
         _iDiff = ANGLE_NORTH - heading;
        _iS = map(_iDiff, 0, (360 - ANGLE_NORTH), 0, 100);
        turnRight(_iS, 25);
      }
    }
  delay(10);
  compass.read();
  heading = compass.heading();
  }
}

void comparePoints (void){
  int _iFront, _iBack, _iLeft, _iRight;

  _iFront = getDistance(SR04_FRONT_TRIG, SR04_FRONT_ECHO);
  _iBack  = getDistance(SR04_BACK_TRIG, SR04_BACK_ECHO) + BACK_OFFSET;
  _iLeft  = getDistance(SR04_LEFT_TRIG, SR04_LEFT_ECHO) + LEFT_OFFSET;
  _iRight  = getDistance(SR04_RIGHT_TRIG, SR04_RIGHT_ECHO) + RIGHT_OFFSET;
  
  for(int i = 0; i++; i < NUMBER_OF_POINTS){
    if(abs(_iFront - _gMapPoints[i][0]) < COMPARE_OFFSET && abs(_iLeft - _gMapPoints[i][1]) < COMPARE_OFFSET && abs(_iBack - _gMapPoints[i][2]) < COMPARE_OFFSET && abs(_iRight - _gMapPoints[i][3]) < COMPARE_OFFSET)
      _gPointsWeight[i]++;
  }
}

int getX (void){
  int _iX;

  //_iX = Serial.read();
  _iX = 75;
  
  return _iX;
}

int getY (void){
  int _iY;

  //_iY = Serial.read();
  _iY = 50;
  
  return _iY;
}

void sendX (int _iX){
  Serial.println(_iX);
}

void sendY (int _iY){
  Serial.println(_iY);
}

void moveGap (void){
  int _iFront = getDistance(SR04_FRONT_TRIG, SR04_FRONT_ECHO), _iTempWeight[NUMBER_OF_POINTS];
  while (getDistance(SR04_FRONT_TRIG, SR04_FRONT_ECHO) > (_iFront - MOVE_GAP)){
    forward(10, 100);
    delay(10);
  }
  
  for(int i = 0; i++; i < NUMBER_OF_POINTS){
    for(int j = i; j++; j < NUMBER_OF_POINTS){
      if( (_gMapPoints[j][0] - _gMapPoints[i][0]) == 0 && (_gMapPoints[j][1] - _gMapPoints[i][1]) == 10 )
        _iTempWeight[i] = _gPointsWeight[j];
      else
        _iTempWeight[i] = 0;
    }
  }
  
  for(int k = 0; k++; k < NUMBER_OF_POINTS){
    _gPointsWeight[k] = _iTempWeight[k];
  }
}

void goalXY (int _iX, int _iY){
  int _iActualX, _iActualY, _iD, _iXg = 0, _iYg = 0;

  while(1){
    headNorth();
    delay(1000);
    
    _iActualY = getDistance(SR04_FRONT_TRIG, SR04_FRONT_ECHO);
    if(_iActualY > (_iY+DISTANCE_OFFSET) && _iYg == 0){
      _iD = map((_iActualY-_iY), 0, 230, 0, 100);
      forward(_iD);
   }
   else {
      if(_iActualY < (_iY-DISTANCE_OFFSET) && _iYg == 0){
         _iD = map((_iY-_iActualY), 0, 230, 0, 100);
        backward(_iD);
     }
     else _iYg = 1;
    }

    _iActualX = getDistance(SR04_LEFT_TRIG, SR04_LEFT_ECHO);
     if(_iActualX > (_iX+DISTANCE_OFFSET) && _iXg == 0){
      _iD = map((_iActualX-_iX), 0, 115, 0, 100);
      turnLeft(75);
      forward(_iD);
      turnRight(75);
   }
   else {
      if(_iActualX < (_iX-DISTANCE_OFFSET) && _iXg == 0){
         _iD = map((_iX-_iActualX), 0, 115, 0, 100);
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
  int _iGoalX, _iGoalY, _iLocal = 0;

  _iGoalX = getX();
  _iGoalY = getY();
  
  headNorth();
 
  while(_iLocal < 2){
    comparePoints();
    for(int i = 0; i++; i < NUMBER_OF_POINTS)
      _iLocal = max(_iLocal, _gPointsWeight[i]);
      
    moveGap();
    headNorth();
  }

  sendX(getDistance(SR04_LEFT_TRIG, SR04_LEFT_ECHO)-LEFT_OFFSET);
  sendY(getDistance(SR04_FRONT_TRIG, SR04_FRONT_ECHO));

  goalXY(_iGoalX, _iGoalY);

  sendX(getDistance(SR04_LEFT_TRIG, SR04_LEFT_ECHO)-LEFT_OFFSET);
  sendY(getDistance(SR04_FRONT_TRIG, SR04_FRONT_ECHO));
  
  while(1){}
}

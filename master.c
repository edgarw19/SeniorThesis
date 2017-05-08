// The SFE_LSM9DS1 library requires both Wire and SPI be
// included BEFORE including the 9DS1 library.
#include <Wire.h>
#include <SPI.h>
#include <SparkFunLSM9DS1.h>
#include "HX711.h"
#include "TimerOne.h"

//////////////////////////
// LSM9DS1 Library Init //
//////////////////////////
// Use the LSM9DS1 class to create an object. [imu] can be
// named anything, we'll refer to that throught the sketch.
LSM9DS1 imu;

///////////////////////
// Example I2C Setup //
///////////////////////
// SDO_XM and SDO_G are both pulled high, so our addresses are:
#define LSM9DS1_M  0x1E // Would be 0x1C if SDO_M is LOW
#define LSM9DS1_AG  0x6B // Would be 0x6A if SDO_AG is LOW

/* SET UP THE WEIGHT SENSORS */
#define DOUT  3
#define CLK  2
#define DOUT2 4
#define CLK2 5

HX711 forwardScale(DOUT2, CLK2);
HX711 backwardScale(DOUT, CLK);


float calibration_factor = 9300; //-7050 worked for my 440lb max scale setup
int RIDERWEIGHT = 15;


/* Basic static variables for our algorithm */
char Dword;
int dutyRate = 82;
int rawDutyRate = 82;
int BASEDUTY = 82;
int MAXDUTY = 104;
int MINDUTY = 60;
int DUTYRANGE = 23;
int PERIOD = 18800;
int SPEEDPIN = 9;
int UNKNOWNPIN = 10;
int THRESHOLDWEIGHT = 2;
int increaseRate = 2;
int counter = 0;
int THRESHOLDPULSEWIDTH = 4;
int BACKWARDSRANGE = 8;
double SENSITIVITY = 0.000732;
double GYROZCALIBRATE = 5.95;

/* Dynamic global variables for our algorithm */
double gDutyAverage = 82;
double gNumForwardReadings = 20;
double gNumBackwardReadings =8;
double gNResetReadings = 15;
double gLastNReadingsReset = 82;
double gLastNAngleReadings = 0;
double gBrakeAverage = 82;
double gNumBrakeReadings = 2;
double gFractionForward = .85;
double gUpHillFWDMultiplier = 10;
double gDownHillFWDMultipler = 3;
double currentTime = 0;
double lastTime = 0;
double firstRead = true;
double complementaryFilterFraction = .98;
double gBoardAngle = 0;
boolean gRiderIsOn = false;
boolean isBraking = false;
unsigned long gStartTime = 0;
int gConstStart = 3;
double gBoardAngleAvg = 0;
double gNumBoardAngleReadings = 10;
double gAccelX = 0;
double gAccelY = 0;
double gAccelZ = 1.2;
double gGyroAverage = 0;
double gGyroValue = 0;

#define DECLINATION -12 // Declination (degrees) in Princeton

//Low pass filter which takes in the current reading, current 'avg', and sampling rate)
double lowPassFilter(double currentValue,double average,double fraction){
  double averagedValue = (1 - 1.0/fraction) * average + (1.0/fraction) * currentValue;
  return averagedValue;
}



//Generates averages using algorithm defined in our paper
double generateAverages(double average, double lastValue,double currentValue,double weighting,double differentialUP,double differentialDOWN,double upperBound,double errorMargin,double subtractNorm){

  if (abs(currentValue - lastValue) > differentialUP){
    if ((currentValue - lastValue) > 0){
        currentValue = average;
      }
  }
  else if (abs(currentValue - lastValue) > differentialDOWN){
      if ((currentValue - lastValue) < 0){
        currentValue = average;
      }
  }

  if (abs(currentValue-subtractNorm) > upperBound){
      if ((currentValue-subtractNorm) > 0){
        currentValue = upperBound;
      }
      else {
        currentValue = -upperBound;
      }
  }

  if (abs(currentValue - average) > errorMargin){ 
      if ((currentValue - average) > 0 ){
        currentValue  = average + errorMargin;
      }
      else{
        currentValue = average- errorMargin;
      }
  }
  double averagedValue = lowPassFilter(currentValue, average, weighting);
  return averagedValue;
}

//Slightly different from general averaging algorithm in that stray from the average is completely removed 
double generateAveragesX(double average, double lastValue,double currentValue,double weighting,double differentialUP,double differentialDOWN,double upperBound,double errorMargin,double subtractNorm){

  if (abs(currentValue - lastValue) > differentialUP){
    if ((currentValue - lastValue) > 0){
        currentValue = average;
      }
  }
  else if (abs(currentValue - lastValue) > differentialDOWN){
      if ((currentValue - lastValue) < 0){
        currentValue = average;
      }
  }

  if (abs(currentValue-subtractNorm) > upperBound){
      if ((currentValue-subtractNorm) > 0){
        currentValue = upperBound;
      }
      else {
        currentValue = -upperBound;
      }
  }

  if (abs(currentValue - average) > errorMargin){ 
      if ((currentValue - average) > 0 ){
        currentValue  = average;
      }
      else{
        currentValue = average;
      }
  }
  double averagedValue = lowPassFilter(currentValue, average, weighting);
  return averagedValue;
}

//Slightly different from general averaging algorithm in that large differentials are scaled and still affect the average
double generateAveragesZ(double average, double lastValue,double currentValue,double weighting,double differential,double differentialScaling,double upperBound,double errorMargin,double subtractNorm){

  if (abs(currentValue - lastValue) > differential){
    lastValue += (currentValue - lastValue) * differentialScaling;
  }

  if (abs(currentValue-subtractNorm) > upperBound){
      currentValue = upperBound;
  }

  if (abs(currentValue - average) > errorMargin){ 
      if ((currentValue - average) > 0 ){
        currentValue  = average + errorMargin;
      }
      else{
        currentValue = average- errorMargin;
      }
  }
  double averagedValue = lowPassFilter(currentValue, average, weighting);
  return averagedValue;
}



void setup() 
{
  
  Serial.begin(9600);
  
  // Initialize IMU
  imu.settings.device.commInterface = IMU_MODE_I2C;
  imu.settings.device.mAddress = LSM9DS1_M;
  imu.settings.device.agAddress = LSM9DS1_AG;
  imu.setGyroScale(500);
  imu.setAccelScale(16);

  // Initialize scale
  forwardScale.set_scale(calibration_factor);
  forwardScale.tare();
  backwardScale.set_scale(calibration_factor); 
  backwardScale.tare(); 
  Serial.println("WTF");
  Serial.println("Readings:");
  if (!imu.begin())
  {
    Serial.println("Failed to communicate with LSM9DS1.");
    Serial.println("Double-check wiring.");
    Serial.println("Default settings in this sketch will " \
                  "work for an out of the box LSM9DS1 " \
                  "Breakout, but may need to be modified " \
                  "if the board jumpers are.");
    while (1)
      ;
  }
  Timer1.initialize(500000); 
  Timer1.setPeriod(PERIOD); //initialize timer1, and set a 1/2 second period
  Timer1.pwm(SPEEDPIN, dutyRate); // setup pwm on pin 9, 50% duty cycle
  Timer1.pwm(UNKNOWNPIN, 81);
  gDutyAverage = BASEDUTY;
  Serial.println("FINISHED SET UP");
}

void updateAccel(){
    //Read from sensors
    double lastAccelX = imu.ax * SENSITIVITY;
    double lastAccelY = imu.ay* SENSITIVITY;
    double lastAccelZ = imu.az * SENSITIVITY;
    imu.readAccel();
    double curAccelX = imu.ax * SENSITIVITY;
    double curAccelY = imu.ay * SENSITIVITY;
    double curAccelZ = imu.az * SENSITIVITY;

    //Calculate with filter
    gAccelX = generateAveragesX(gAccelX, lastAccelX, curAccelX, 5, .1, .015, .5, .1, 0);
    gAccelZ = generateAveragesZ(gAccelZ, lastAccelZ, curAccelZ, 10, .5, .5, 3, .5, 1.2);
    gAccelY = generateAverages(gAccelY, lastAccelY, curAccelY, 10, 100, 100, 100, 100, 0);
}


void loop()
{

  boolean maxIsForward = true;
 
  //Read the current weight
  double backWeight = backwardScale.get_units() - 10;
  double forwardWeight = forwardScale.get_units() + 10;
  float lastAngle = gBoardAngle;
  updateAccel();
  
  //Calculate the current angle
  gBoardAngle = calculateAngle(gAccelX, gAccelY, gAccelZ, -imu.my, -imu.mx, imu.mz);
  float testAngle = calculateAngle(imu.ax, imu.ay, imu.az, -imu.my, -imu.mx, imu.mz);
  gBoardAngleAvg = generateAverages(gBoardAngleAvg, lastAngle, gBoardAngle, 5, 1.5, 1.5, 10, 1.5, 0);

  RIDERWEIGHT = forwardWeight + backWeight;
  

    //Formated Print Statement for data collection
      counter += 1;
      if (counter % 3 == 0){
      Serial.print("WEIGHTS, ");
      Serial.print(backWeight, 2);
      Serial.print(",");
      Serial.println(forwardWeight, 2); 
      Serial.print("DUTY, ");
      Serial.print(dutyRate);
      Serial.print(", ");
      Serial.print(gDutyAverage);
      Serial.print(", ");
      Serial.print(gLastNReadingsReset);
      Serial.print(", ");
      Serial.println(rawDutyRate);
      Serial.print("ANGLE, ");
      Serial.print((imu.ax));
      Serial.print(", ");
      Serial.print((imu.ay));
      Serial.print(", ");
      Serial.print((imu.az));
      Serial.print(", ");
      Serial.print((testAngle));
      Serial.print(", ");
      Serial.println(gBoardAngleAvg);
      Serial.println();
      }

  //If rider isn't on, don't run the motor
  if (RIDERWEIGHT < THRESHOLDWEIGHT){
    dutyRate = BASEDUTY;
    gDutyAverage = BASEDUTY;
    gRiderIsOn = false;
  }
  else if (RIDERWEIGHT >= THRESHOLDWEIGHT && !gRiderIsOn){
    gDutyAverage = BASEDUTY;
    gRiderIsOn = true;
    gStartTime = millis();
  }

  //If rider is on, calculate the right speed
  else {

    //Angle correction to riding experience
   if (gBoardAngleAvg > 1.5) {
      forwardWeight += gUpHillFWDMultiplier * gBoardAngleAvg;
   }
   else if (gBoardAngleAvg < -1.5){
      forwardWeight += gDownHillFWDMultipler * gBoardAngleAvg;
   }
  RIDERWEIGHT = forwardWeight + backWeight;
    
    double maxWeight = forwardWeight;

    //Calculate which direction rider is leaning
    if (backWeight > forwardWeight){
      maxWeight = backWeight;
      maxIsForward = false;
    }

    //Check if rider is in neutral position
    if (maxWeight < .55*RIDERWEIGHT && gDutyAverage < 85 && gDutyAverage > 80){
      dutyRate = BASEDUTY;
    }

    //If not, drive the motor
    else {
      //Calculate the current pulse width
      double speedFraction = 0;
      int extraPulseWidth = 0;

      //Calculate the speed fraction for forward and backwards
      if (maxIsForward){
        speedFraction = (maxWeight - .50*RIDERWEIGHT)/(.27*RIDERWEIGHT);
      }
      else {
        speedFraction = (maxWeight - .55*RIDERWEIGHT)/(.20*RIDERWEIGHT);
      }

      //Cap the speed fraction
      if (speedFraction > 1) speedFraction = 1.0;
      
      //Calculate the pulse width for forwards and backwards
      if (maxIsForward){
        extraPulseWidth = 1 * speedFraction * (DUTYRANGE-gConstStart) + gConstStart; //LINEAR IS HALF
      }
      else {
        extraPulseWidth = speedFraction * BACKWARDSRANGE;
      }

      //Determine which direction to drive the motor
      if (maxIsForward){
        dutyRate = BASEDUTY + extraPulseWidth;
      }
      else {
        dutyRate = 70 - extraPulseWidth;
      }

      //Set thresholding to prevent going too fast
      if (dutyRate > 100){
        dutyRate = 100;
      }
      else if (dutyRate < 62){ //TODO update this to variables
        dutyRate = 62;
      }


      // Allows you to check last N readings to trigger reset
      gLastNReadingsReset = (1-1/gNResetReadings) * gLastNReadingsReset + (1/gNResetReadings) * dutyRate;
      rawDutyRate = dutyRate;
      //if current num is TOO different from the last num, then set current NUm to working avg
      if (abs(gDutyAverage - dutyRate) > 10){
        dutyRate = gDutyAverage;
      }
      if (abs(gLastNReadingsReset - gDutyAverage) > 5){
        dutyRate = gLastNReadingsReset;
      }
      gBrakeAverage = (1-1/gNumBrakeReadings) * gBrakeAverage + (1/gNumBrakeReadings) * dutyRate;
      
    }
  }

  if (gRiderIsOn && (millis() - gStartTime) > 3000){
      if (dutyRate > BASEDUTY){
        gDutyAverage = (1-1/gNumForwardReadings) * gDutyAverage + (1/gNumForwardReadings) * dutyRate;
      }
      else {
        gDutyAverage = (1-1/gNumBackwardReadings) * gDutyAverage + (1/gNumBackwardReadings) * dutyRate;
      }
    
    //Drive the PWM
    int averageDutyRate = gDutyAverage;
    
    //Cap the duty rate
    if (averageDutyRate > 100){
      averageDutyRate = 100;
    }
    else if (averageDutyRate < 62){ //TODO update this to variables
      averageDutyRate = 62;
    }

    if (averageDutyRate < 75 && gLastNReadingsReset > 85){
      gDutyAverage = 85;
      Timer1.pwm(SPEEDPIN, 85);
    }
    else {

        Timer1.pwm(SPEEDPIN, averageDutyRate);
    }
  }
  else {
    Timer1.pwm(SPEEDPIN, BASEDUTY); //Fixes bug regarding jumping off while going fast and board leaves you
  }
}





// Calculate pitch, roll, and heading.
double calculateAngle(float ax, float ay, float az, float mx, float my, float mz)
{
  float roll = atan2(ay, az);
  float pitch = atan2(-ax, sqrt(ay * ay + (az) * az));
  
  pitch *= 180.0 / PI;
  roll  *= 180.0 / PI;


  return -(pitch+1);
} 

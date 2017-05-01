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


/* Set up the throttle connections */
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
double GYROZCALIBRATE = 5.95;
//change this number with gNumForwardReadings
double gDutyAverage = 82;
double gNumForwardReadings = 20;
double gNumBackwardReadings =8;
double gNResetReadings = 15;
double gLastNReadingsReset = 82;
double gLastNAngleReadings = 0;
double gBrakeAverage = 82;
double gNumBrakeReadings = 2;
double gFractionForward = .85;
double gUpHillFWDMultiplier = 2.2;
double gDownHillFWDMultipler = 5;
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
double gAccelZ = 0;
double gGyroAverage = 0;
double gGyroValue = 0;


////////////////////////////
// Sketch Output Settings //
////////////////////////////
#define PRINT_CALCULATED
//#define PRINT_RAW
#define PRINT_SPEED 1200 // 250 ms between prints

// Earth's magnetic field varies by location. Add or subtract 
// a declination to get a more accurate heading. Calculate 
// your's here:
// http://www.ngdc.noaa.gov/geomag-web/#declination
#define DECLINATION -12 // Declination (degrees) in Boulder, CO.


int findAverage(int readings[]){
  double average = 0;
  for (int i = 0; i < gNumForwardReadings; i++){
//    Serial.print(gDutyAverage[i]);
    average += readings[i];
  }
  return int(average/gNumForwardReadings);
}

double findDirection(int readings[]){
  double sum = 0;
  for (int i = 0; i < gNumForwardReadings; i++){
    sum += readings[i];
  }
  return sum/gNumForwardReadings;
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
  gDutyAverage = 82;
  Serial.println("FINISHED SET UP");
}

void updateAccel(){
    imu.readAccel();
    imu.readGyro();
    double curAccelX = imu.ax;
    double curAccelY = 0;
    double curAccelZ = 0;
    if (abs(gAccelX) < 25 && abs(imu.ax) > 240){
      curAccelX = gAccelX;
    }
    
    gAccelX = (1-1/gNumBoardAngleReadings) * gAccelX + (1/gNumBoardAngleReadings) * curAccelX;
    gAccelY = (1-1/gNumBoardAngleReadings) * gAccelY + (1/gNumBoardAngleReadings) * (imu.ay);
    gAccelZ = (1-1/gNumBoardAngleReadings) * gAccelZ + (1/gNumBoardAngleReadings) * (imu.az);
}


void loop()
{

          boolean maxIsForward = true;
 
  //Read the current weight
  double backWeight = backwardScale.get_units() - 10;
  double forwardWeight = forwardScale.get_units() + 10;
  float lastAngle = gBoardAngle;
  updateAccel();
  
  gBoardAngle = calculateAngle(gAccelX, gAccelY, gAccelZ, -imu.my, -imu.mx, imu.mz) + 147.7;
  double axCorrected = imu.ax;
  if (abs(imu.ax) > 240) axCorrected = gAccelX;
  float testAngle = calculateAngle(axCorrected, imu.ay, imu.az, -imu.my, -imu.mx, imu.mz) + 147.7;
  // angle fix
  if (abs(gBoardAngle) > 10){
    gBoardAngle = gBoardAngleAvg;
  }
  if (abs(gBoardAngle) < 2){
    gBoardAngle = 0;
  }
  if (abs(gBoardAngle - gBoardAngleAvg) > 3){
    gBoardAngle = gBoardAngleAvg;
  }
  if (abs(gBoardAngle - lastAngle) > 3){
    gBoardAngle = gBoardAngleAvg;
  }
  double gyroSwitch = gGyroValue;
  gGyroValue = imu.calcGyro(imu.gy);

  if (abs(gGyroValue) < 2){
    gGyroValue = 0;
  }
  if (abs(gGyroValue) > 8){
    gGyroValue = gGyroAverage;
  }
  if (abs(gGyroValue - gGyroAverage) > 4){
    gGyroValue = gGyroAverage;
  }

  
  gGyroAverage = (1.0/10.0)*gGyroValue + 9.0/10 * gGyroAverage;
  double trueAngle = gyroSwitch;

  

  gBoardAngleAvg = (1-1/gNumBoardAngleReadings) * gBoardAngleAvg + (1/gNumBoardAngleReadings) * gBoardAngle;




  RIDERWEIGHT = forwardWeight + backWeight;
  

                    //MASSIVE PRINT STATEMENT
                    counter += 1;
      if (counter % 2 == 0){
      Serial.print("WEIGHTS, ");
      Serial.print(backWeight, 2);
      Serial.print(",");
      Serial.println(forwardWeight, 2); //scale.get_units() returns a float
      Serial.print("DUTY, ");
      Serial.println(dutyRate);
//      Serial.print(", ");
//      Serial.print(gDutyAverage);
//      Serial.print(", ");
//      Serial.print(gLastNReadingsReset);
//      Serial.print(", ");
//      Serial.println(rawDutyRate);
      Serial.print("ANGLE, ");
      Serial.print((imu.ax));
      Serial.print(", ");
      Serial.print((imu.ay));
      Serial.print(", ");
      Serial.print((imu.az));
      Serial.print(", ");
      Serial.print((imu.gy));
      Serial.print(", ");
      Serial.print((imu.gz));
      Serial.print(", ");
      Serial.println((testAngle));
      Serial.println();
      }

  //If rider isn't on, don't run the motor
  if (RIDERWEIGHT < THRESHOLDWEIGHT){
    dutyRate = BASEDUTY;
    gDutyAverage = BASEDUTY;
    gRiderIsOn = false;
//    Serial.println("RIDER NOT ON");
//          Serial.print(backWeight, 2);
//      Serial.print(",");
//      Serial.println(forwardWeight, 2); //scale.get_units() returns a float
  }
  else if (RIDERWEIGHT >= THRESHOLDWEIGHT && !gRiderIsOn){
    gDutyAverage = BASEDUTY;
    gRiderIsOn = true;
    gStartTime = millis();
  }

  //If rider is on, calculate the right speed
  else {
//      if (firstRead){
//    lastTime = millis();
//    firstRead = false;
//  }
//  else {
//    currentTime = millis();
//    Serial.println((currentTime-lastTime));
//    lastTime = currentTime;
//  }
//angle add
  if (gBoardAngleAvg > 0) {
     forwardWeight -= gUpHillFWDMultiplier * gBoardAngleAvg;
  }
  else {
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

void printGyro()
{
  // To read from the gyroscope, you must first call the
  // readGyro() function. When this exits, it'll update the
  // gx, gy, and gz variables with the most current data.
  
  imu.readGyro();
  
  
  // Now we can use the gx, gy, and gz variables as we please.
  // Either print them as raw ADC values, or calculated in DPS.
  // If you want to print calculated values, you can use the
  // calcGyro helper function to convert a raw ADC value to
  // DPS. Give the function the value that you want to convert.
  double gyroAngle = imu.calcGyro(imu.gy);
  gGyroAverage = (1.0/10.0)*gyroAngle + 9.0/10 * gGyroAverage;
  Serial.print(imu.calcGyro(imu.gx), 2);
  Serial.print(", ");
  Serial.print(imu.calcGyro(imu.gy), 2);
  Serial.print(", ");
  Serial.println(imu.calcGyro(imu.gz), 2);
  Serial.println();
//  Serial.print(imu.gx);
//  Serial.print(", ");
//  Serial.print(imu.gy);
//  Serial.print(", ");
//  Serial.println(imu.gz);
}

void printAccel()
{
  // To read from the accelerometer, you must first call the
  // readAccel() function. When this exits, it'll update the
  // ax, ay, and az variables with the most current data.
  
  // Now we can use the ax, ay, and az variables as we please.
  // Either print them as raw ADC values, or calculated in g's.
  Serial.print("A: ");
  // If you want to print calculated values, you can use the
  // calcAccel helper function to convert a raw ADC value to
  // g's. Give the function the value that you want to convert.
//  Serial.print(imu.calcAccel(imu.ax), 5);
//  Serial.print(", ");
//  Serial.print(imu.calcAccel(imu.ay + 70), 5);
//  Serial.print(", ");
//  Serial.println(imu.calcAccel(imu.az-1360), 5);
  Serial.print(imu.ax);
  Serial.print(", ");
  Serial.print(imu.ay+70);
  Serial.print(", ");
  Serial.println(imu.az-1360);
    Serial.print(gAccelX);
  Serial.print(", ");
  Serial.print(gAccelY);
  Serial.print(", ");
  Serial.println(gAccelZ);
  

}


// Calculate pitch, roll, and heading.
// Pitch/roll calculations take from this app note:
// http://cache.freescale.com/files/sensors/doc/app_note/AN3461.pdf?fpsp=1
// Heading calculations taken from this app note:
// http://www51.honeywell.com/aero/common/documents/myaerospacecatalog-documents/Defense_Brochures-documents/Magnetic__Literature_Application_notes-documents/AN203_Compass_Heading_Using_Magnetometers.pdf
double calculateAngle(float ax, float ay, float az, float mx, float my, float mz)
{
  float roll = atan2(ay, az);
  float pitch = atan2(-ax, sqrt(ay * ay + (az) * az));
  
//  float heading;
//  if (my == 0)
//    heading = (mx < 0) ? 180.0 : 0;
//  else
//    heading = atan2(mx, my);
//    
//  heading -= DECLINATION * PI / 180;
//  
//  if (heading > PI) heading -= (2 * PI);
//  else if (heading < -PI) heading += (2 * PI);
//  else if (heading < 0) heading += 2 * PI;
//  
//  // Convert everything from radians to degrees:
//  heading *= 180.0 / PI;
  pitch *= 180.0 / PI;
  roll  *= 180.0 / PI;
//  
//   Serial.print("Pitch, Roll: ");
//   Serial.print(pitch, 2);
//   Serial.print(", ");
//   Serial.println(roll, 2);

  return -(pitch+148);
} 

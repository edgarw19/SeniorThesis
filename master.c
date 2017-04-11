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
int BASEDUTY = 82;
int MAXDUTY = 104;
int MINDUTY = 60;
int DUTYRANGE = 23;
int PERIOD = 18800;
int SPEEDPIN = 9;
int UNKNOWNPIN = 10;
int THRESHOLDWEIGHT = 50;
int increaseRate = 2;
int counter = 0;
int THRESHOLDPULSEWIDTH = 4;
int BACKWARDSRANGE = 8;
//change this number with gNumOfReadings
int gDutyReadings[30];
int gNumOfReadings = 30;
double gNReadings = 5;
double gNResetReadings = 5;
double gLastNReadingsAvg = 82;
double gLastNReadingsReset = 82;
double gBrakeAverage = 82;
double gNumBrakeReadings = 2;
double gFractionForward = .85;
boolean gRiderIsOn = false;
boolean isBraking = false;
unsigned long gStartTime = 0;
int gConstStart = 3;
double gBoardAngleAvg = 0;
double gNumBoardAngleReadings = 5;
double lastDutyRate = 82;


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

void resetDutyAverage(int resetNum){
  for (int i = 0; i < gNumOfReadings; i++){
    gDutyReadings[i] = resetNum;
  }
}
int findAverage(int readings[]){
  double average = 0;
  for (int i = 0; i < gNumOfReadings; i++){
//    Serial.print(gDutyReadings[i]);
    average += readings[i];
  }
  return int(average/gNumOfReadings);
}

double findDirection(int readings[]){
  double sum = 0;
  for (int i = 0; i < gNumOfReadings; i++){
    sum += readings[i];
  }
  return sum/gNumOfReadings;
}

void printWeightReadings(double forwardWeight, double backWeight){
   Serial.print(backWeight, 2);
   Serial.print(",");
   Serial.println(forwardWeight, 2); //scale.get_units() returns a float
   Serial.print(",");
   Serial.print(findAverage(gDutyReadings));
   Serial.print("DUTY RATE: ");
   Serial.println(dutyRate);

   // Print the heading and orientation for fun!
   // Call print attitude. The LSM9DS1's magnetometer x and y
   // axes are opposite to the accelerometer, so my and mx are
   // substituted for each other.
   printAttitude(imu.ax, imu.ay, imu.az, -imu.my, -imu.mx, imu.mz);
   Serial.println();
   printAccel(); // Print "A: ax, ay, az"
}

void setup() 
{
  
  Serial.begin(9600);
  
  // Initialize IMU
  imu.settings.device.commInterface = IMU_MODE_I2C;
  imu.settings.device.mAddress = LSM9DS1_M;
  imu.settings.device.agAddress = LSM9DS1_AG;

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
  for (int i = 0; i < gNumOfReadings; i++){
    gDutyReadings[i] = 82;
  }
  Serial.println("FINISHED SET UP");
}


void loop()
{
counter += 1;
      int pos = counter % gNumOfReadings;
          boolean maxIsForward = true;
 
  //Read the current weight
  double backWeight = backwardScale.get_units() - 10;
  double forwardWeight = forwardScale.get_units() + 10;
      float boardAngle = printAttitude(imu.ax, imu.ay, imu.az, -imu.my, -imu.mx, imu.mz);
  RIDERWEIGHT = forwardWeight + backWeight;
  gBoardAngleAvg = (1-1/gNumBoardAngleReadings) * gBoardAngleAvg + (1/gNumBoardAngleReadings) * boardAngle;



  //If rider isn't on, don't run the motor
  if (RIDERWEIGHT < THRESHOLDWEIGHT){
    dutyRate = BASEDUTY;
    resetDutyAverage(BASEDUTY);
    gRiderIsOn = false;
  }
  else if (RIDERWEIGHT >= THRESHOLDWEIGHT && !gRiderIsOn){
    resetDutyAverage(BASEDUTY);
    gRiderIsOn = true;
    gStartTime = millis();
    Serial.println("STARTED ENGINE");
  }

  //If rider is on, calculate the right speed
  else {
    double maxWeight = forwardWeight;
    lastDutyRate = dutyRate;

    //Calculate which direction rider is leaning
    if (backWeight > forwardWeight){
      maxWeight = backWeight;
      maxIsForward = false;
    }

    //Check if rider is in neutral position
    if (maxWeight < .55*RIDERWEIGHT){
      dutyRate = BASEDUTY;
//      Serial.println("RIDER IS NEUTRAL");
    }

    //If not, drive the motor
    else {
      //Calculate the current pulse width
      double speedFraction = 0;
      int extraPulseWidth = 0;

      //Calculate the speed fraction for forward and backwards
      if (maxIsForward){
        speedFraction = (maxWeight - .50*RIDERWEIGHT)/(.32*RIDERWEIGHT);
        
      }
      else {
        speedFraction = (maxWeight - .55*RIDERWEIGHT)/(.20*RIDERWEIGHT);
      }

      if (speedFraction > 1) speedFraction = 1.0;
      speedFraction = pow(speedFraction, 1);
      
      //Calculate the pulse width for forwards and backwards
      if (maxIsForward){
        extraPulseWidth = .8 * speedFraction * (DUTYRANGE-gConstStart) + gConstStart; //LINEAR IS HALF
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

  int testDutyNow = dutyRate;
  // Allows you to check last N readings to trigger reset
  gLastNReadingsReset = (1-1/gNResetReadings) * gLastNReadingsReset + (1/gNResetReadings) * dutyRate;
      //if current num is TOO different from the last num, then set current NUm to working avg
      if (abs(gLastNReadingsReset - dutyRate) > 15){
        dutyRate = gLastNReadingsReset;
      }
      //if current num is TOO different 
      if (abs(dutyRate - gLastNReadingsReset) > 15){
        dutyRate = gLastNReadingsReset;
      }
      
              //MASSIVE PRINT STATEMENT
  if (counter % 4 == 0){
  Serial.print(backWeight, 2);
  Serial.print(",");
  Serial.println(forwardWeight, 2); //scale.get_units() returns a float
  Serial.print("DUTY RATE: ");
  Serial.print(findAverage(gDutyReadings));
  Serial.print(", ");
  Serial.print(dutyRate);
    Serial.print(", ");
  Serial.println(testDutyNow);

//  // Print the heading and orientation for fun!
//  // Call print attitude. The LSM9DS1's magnetometer x and y
//  // axes are opposite to the accelerometer, so my and mx are
//  // substituted for each other.
//  Serial.println(gBoardAngleAvg);
//  Serial.println();
//  Serial.println(forwardWeight);
//  Serial.println(forwardWeight*1.0/(backWeight + forwardWeight));
//  printAccel(); // Print "A: ax, ay, az"
  }
      gLastNReadingsAvg = (1-1/gNReadings) * gLastNReadingsAvg + (1/gNReadings) * dutyRate;
      gBrakeAverage = (1-1/gNumBrakeReadings) * gBrakeAverage + (1/gNumBrakeReadings) * dutyRate;
      
    }
  }

  if (gRiderIsOn && (millis() - gStartTime) > 3000){
    
    gDutyReadings[pos] = dutyRate;
    //Drive the PWM
    int averageDutyRate = findAverage(gDutyReadings);
    
    //Cap the duty rate
    if (averageDutyRate > 100){
      averageDutyRate = 100;
    }
    else if (averageDutyRate < 62){ //TODO update this to variables
      averageDutyRate = 62;
    }

    //
    if (averageDutyRate > 87 && gLastNReadingsAvg < 80){
      Serial.println("FAST");
      resetDutyAverage(70);
      Timer1.pwm(SPEEDPIN, 70);
    }
    else if (averageDutyRate < 70 && gLastNReadingsAvg > 88){
      Serial.println("SLOW");
      resetDutyAverage(88);
      Timer1.pwm(SPEEDPIN, 88);
    }
    else {
      //Special brake case, if rider is actively trying to brake
      if (averageDutyRate > 86 && gBrakeAverage < 66 && gLastNReadingsAvg < 75){
        Timer1.pwm(SPEEDPIN, 64);
        resetDutyAverage(64);
        Serial.println("MAX BREAK TRIGGERED");
        delay(250);
      }
      else {
        Timer1.pwm(SPEEDPIN, averageDutyRate);
      }
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
  Serial.print("G: ");
#ifdef PRINT_CALCULATED
  // If you want to print calculated values, you can use the
  // calcGyro helper function to convert a raw ADC value to
  // DPS. Give the function the value that you want to convert.
  Serial.print(imu.calcGyro(imu.gx), 2);
  Serial.print(", ");
  Serial.println(imu.calcGyro(imu.gy), 2);
#elif defined PRINT_RAW
  Serial.print(imu.gx);
  Serial.print(", ");
  Serial.print(imu.gy);
  Serial.print(", ");
  Serial.println(imu.gz);
#endif
}

void printAccel()
{
  // To read from the accelerometer, you must first call the
  // readAccel() function. When this exits, it'll update the
  // ax, ay, and az variables with the most current data.
  imu.readAccel();
  
  // Now we can use the ax, ay, and az variables as we please.
  // Either print them as raw ADC values, or calculated in g's.
  Serial.print("A: ");
#ifdef PRINT_CALCULATED
  // If you want to print calculated values, you can use the
  // calcAccel helper function to convert a raw ADC value to
  // g's. Give the function the value that you want to convert.
  Serial.print(imu.calcAccel(imu.ax), 2);
  Serial.print(", ");
  Serial.println(imu.calcAccel(imu.ay), 2);
#elif defined PRINT_RAW 
  Serial.print(imu.ax);
  Serial.print(", ");
  Serial.print(imu.ay);
  Serial.print(", ");
  Serial.println(imu.az);
#endif

}

void printMag()
{
  // To read from the magnetometer, you must first call the
  // readMag() function. When this exits, it'll update the
  // mx, my, and mz variables with the most current data.
  imu.readMag();
  
  // Now we can use the mx, my, and mz variables as we please.
  // Either print them as raw ADC values, or calculated in Gauss.
  Serial.print("M: ");
#ifdef PRINT_CALCULATED
  // If you want to print calculated values, you can use the
  // calcMag helper function to convert a raw ADC value to
  // Gauss. Give the function the value that you want to convert.
  Serial.print(imu.calcMag(imu.mx), 2);
  Serial.print(", ");
  Serial.print(imu.calcMag(imu.my), 2);
  Serial.print(", ");
  Serial.print(imu.calcMag(imu.mz), 2);
  Serial.println(" gauss");
#elif defined PRINT_RAW
  Serial.print(imu.mx);
  Serial.print(", ");
  Serial.print(imu.my);
  Serial.print(", ");
  Serial.println(imu.mz);
#endif
}

// Calculate pitch, roll, and heading.
// Pitch/roll calculations take from this app note:
// http://cache.freescale.com/files/sensors/doc/app_note/AN3461.pdf?fpsp=1
// Heading calculations taken from this app note:
// http://www51.honeywell.com/aero/common/documents/myaerospacecatalog-documents/Defense_Brochures-documents/Magnetic__Literature_Application_notes-documents/AN203_Compass_Heading_Using_Magnetometers.pdf
double printAttitude(float ax, float ay, float az, float mx, float my, float mz)
{
  imu.readMag();
  imu.readAccel();
  float roll = atan2(ay, az);
  float pitch = atan2(-ax, sqrt(ay * ay + az * az));
  
  float heading;
  if (my == 0)
    heading = (mx < 0) ? 180.0 : 0;
  else
    heading = atan2(mx, my);
    
  heading -= DECLINATION * PI / 180;
  
  if (heading > PI) heading -= (2 * PI);
  else if (heading < -PI) heading += (2 * PI);
  else if (heading < 0) heading += 2 * PI;
  
  // Convert everything from radians to degrees:
  heading *= 180.0 / PI;
  pitch *= 180.0 / PI;
  roll  *= 180.0 / PI;
  
  // Serial.print("Pitch, Roll: ");
  // Serial.print(", ");
  // Serial.println(roll, 2);
  // Serial.print("Heading: "); Serial.println(heading, 2);
  return pitch;
} 

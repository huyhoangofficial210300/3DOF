#include "QuickPID.h"
#include <motor.h>

motor my_motor;

/********************************************************
   PID Basic Example
   Reading analog input 0 to control analog PWM output 3
 ********************************************************/

#include "QuickPID.h"

#define PIN_INPUT 0
#define PIN_OUTPUT 3

//Define Variables we'll be connecting to
float Setpoint, Input, Output;

//Specify the links and initial tuning parameters
float Kp = 15, Ki = 3, Kd = 0;

QuickPID myQuickPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, QuickPID::DIRECT);

void setup()
{
  //initialize the variables we're linked to
  Setpoint = 50;
  Serial.begin(115200);
  my_motor.init();
  myQuickPID.SetMode(QuickPID::AUTOMATIC);
  myQuickPID.SetSampleTimeUs(1);
  myQuickPID.SetOutputLimits(-200, 200);
}

void loop()
{
  Input = my_motor.get_encoder_value(1);
  myQuickPID.Compute();
  if(Output > 0) {
    my_motor.move(1, Output, true);
  } else {
    my_motor.move(1, abs(Output), false);
  }
  Serial.println(my_motor.get_encoder_value(1));
}

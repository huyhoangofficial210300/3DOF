#include <Arduino.h>
#include <MPU_sensor.h>
#include <motor.h>

motor MOTOR;
MPU_sensor MPU;
void setup() {
  // put your setup code here, to run once:
  MPU.init();
  MOTOR.init();
  Serial.begin(115200);

}

void loop() {
  // put your main code here, to run repeatedly:
  MPU.update();
  Serial.print("ROLL = ");  Serial.print(MPU.get_roll_angle());  Serial.print("    ");
  Serial.print("PITCH = "); Serial.print(MPU.get_pitch_angle()); Serial.println("    ");
}
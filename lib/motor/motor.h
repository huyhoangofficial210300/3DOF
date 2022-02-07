#include <Arduino.h>
#include <user_define.h>
#include <ESP32Encoder.h>


struct motor_info{
    uint8_t IN1;
    uint8_t IN2;
    uint8_t EN; 
    uint8_t A;
    uint8_t B;   
    uint8_t PWM_channel;
    ESP32Encoder encoder;

    //pid 
};


class motor
{
private:
    motor_info MOTOR1;
    motor_info MOTOR2;
    motor_info MOTOR3;
    const int freq = 31000;
    const int resolution = 8;
    int prev_setpoint[3] = {0, 0, 0};
    bool is_run[3] = {false, false, false};
    bool direction[3] = {false, false, false};
    /*motor brake time delay*/
    float mu[2] = {111.904761904762, 65.7737825899535};
    float p[6] = {1.65929699990314, -3.70875230124061, -1.09810681839327, 2.58795745971502, 2.00451442591242, 51.0510494741914};
public:
    motor(){
        //motor 1 assign GPIO pin
        this->MOTOR1.EN  = MOTOR_1_EN;
        this->MOTOR1.IN1 = MOTOR_1_IN1;
        this->MOTOR1.IN2 = MOTOR_1_IN2;
        //motor 2 assign GPIO pin
        this->MOTOR2.EN  = MOTOR_2_EN;
        this->MOTOR2.IN1 = MOTOR_2_IN1;
        this->MOTOR2.IN2 = MOTOR_2_IN2;
        //motor 3 assign GPIO pin
        this->MOTOR3.EN  = MOTOR_3_EN;
        this->MOTOR3.IN1 = MOTOR_3_IN1;
        this->MOTOR3.IN2 = MOTOR_3_IN2;
        //assign PWM channel
        this->MOTOR1.PWM_channel = 0;
        this->MOTOR2.PWM_channel = 1;
        this->MOTOR3.PWM_channel = 2;
        //encoder assign 
        this->MOTOR1.A = MOTOR_1_A;
        this->MOTOR1.B = MOTOR_1_B;
        this->MOTOR2.A = MOTOR_2_A;
        this->MOTOR2.B = MOTOR_2_B;
        this->MOTOR3.A = MOTOR_3_A;
        this->MOTOR3.B = MOTOR_3_B;
    }
    void init(){
        //set motor 1 GPIO output mode
        pinMode(this->MOTOR1.IN1, OUTPUT);
        pinMode(this->MOTOR1.IN2, OUTPUT);
        //set motor 2 GPIO output mode
        pinMode(this->MOTOR2.IN1, OUTPUT);
        pinMode(this->MOTOR2.IN2, OUTPUT);
        //set motor 3 GPIO output mode
        pinMode(this->MOTOR3.IN1, OUTPUT);
        pinMode(this->MOTOR3.IN2, OUTPUT);
        //setup PWM channal
        ledcSetup(this->MOTOR1.PWM_channel, this->freq, this->resolution);
        ledcSetup(this->MOTOR2.PWM_channel, this->freq, this->resolution);
        ledcSetup(this->MOTOR3.PWM_channel, this->freq, this->resolution);
        //assign EN pin to pwm channel
        ledcAttachPin(this->MOTOR1.EN, this->MOTOR1.PWM_channel);
        ledcAttachPin(this->MOTOR2.EN, this->MOTOR2.PWM_channel);
        ledcAttachPin(this->MOTOR3.EN, this->MOTOR3.PWM_channel);
        // Enable the weak pull up resistors
	    ESP32Encoder::useInternalWeakPullResistors=UP;
        this->MOTOR1.encoder.attachHalfQuad(this->MOTOR1.B, this->MOTOR1.A);
        this->MOTOR2.encoder.attachHalfQuad(this->MOTOR2.A, this->MOTOR2.B);
        this->MOTOR3.encoder.attachHalfQuad(this->MOTOR3.A, this->MOTOR3.B);
        //clear encoder count
        this->MOTOR1.encoder.clearCount();
        this->MOTOR2.encoder.clearCount();
        this->MOTOR3.encoder.clearCount();
    }
    void change_direction(motor_info motor, bool direction){
        if(direction == false){
            digitalWrite(motor.IN1, LOW);
            digitalWrite(motor.IN2, HIGH);
        } else {
            digitalWrite(motor.IN1, HIGH);
            digitalWrite(motor.IN2, LOW);
        }
    }
    
    void move(uint8_t motor_id, uint8_t speed, bool direction){
        switch (motor_id){
            case 1:{
                ledcWrite(this->MOTOR1.PWM_channel, speed);
                this->change_direction(this->MOTOR1, direction);
                break;
            }
            case 2:{
                ledcWrite(this->MOTOR2.PWM_channel, speed);
                this->change_direction(this->MOTOR2, direction);
                break;                
            }
            case 3:{
                ledcWrite(this->MOTOR3.PWM_channel, speed);
                this->change_direction(this->MOTOR3, direction);
                break;
            }
            default:
                break;
        }    
    }
    int get_encoder_value(uint8_t motor_id){
        switch (motor_id){
            case 1:{
                return this->MOTOR1.encoder.getCount();
                break;
            }
            case 2:{
                return this->MOTOR2.encoder.getCount();
                break;                
            }
            case 3:{
                return this->MOTOR3.encoder.getCount();
                break;
            }
            default:
                return 69;
                break;
        }
    }
    int brake_period(int motor_id, int setpoint) {
        float scaled_setpoint = (abs(setpoint - this->prev_setpoint[motor_id - 1]) - this->mu[0]) / this->mu[1];
        return  p[0]*pow(scaled_setpoint, 5) + p[1]*pow(scaled_setpoint, 4) 
                + p[2]*pow(scaled_setpoint, 3) + p[3]*pow(scaled_setpoint, 2)
                + p[4]*scaled_setpoint + p[5];
    }
    int move_to_target_setpoint(int motor_id, int setpoint) {      
        int error = abs(setpoint - this->get_encoder_value(motor_id));
        int error_margin = map(error, 10, 400, 5, 15);
        this->move(motor_id, 255, true);
        delay(10);
        while(error >= (error_margin)){    
            this->move(motor_id, 130, true);
            error = abs(setpoint - this->get_encoder_value(motor_id));
        }
        while (1)
        {
            this->move(motor_id, 255, false);
            //delay(0);
            this->move(motor_id, 255, true);   
            //Serial.println(this->get_encoder_value(motor_id));
            //delay(50);
        }
        return this->get_encoder_value(motor_id);
    } 
};

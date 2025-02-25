#include <Arduino.h>
class InverseKinematics
{
private:
    
    float phi0 = 25.84;
    float z_set = 105;
    float z0 = 32.0;
    
    //float pi = 3.141592653589793;
    float deg2rad = PI / 180.0;
    float rad2deg = 180.0 / PI;

    float k = 100.0/sin(60*deg2rad);
    float bRp0[3], bRp1[3], bRp2[3];
    float b1[3],b2[3],b3[3],p1[3],p2[3],p3[3]; 
    float l1[3], l2[3], l3[3];
    float L1_a, L2_a, L3_a;
    float T[3];
    //float angle;
public:
    InverseKinematics(float d, float e, float phi0, float z0)
    {
        this->d = d;
        this->e = e;
        this->phi0 = phi0;
		this->z0 = z0;

        b1[0] = k;                  // body origin to motor 1 shaft center X position
        b1[1] = 0.0;                // body origin to motor 1 shaft center Y position
        b1[2] = z0;                 // body origin to motor 1 shaft center Z position
        b2[0] = -k*sin(30*deg2rad); // same for motor 2
        b2[1] = k*cos(30*deg2rad);
        b2[2] = z0;
        b3[0] = -k*sin(30*deg2rad); // same for motor 3
        b3[1] = -k*cos(30*deg2rad);
        b3[2] = z0;
        p1[0] = k;                  // platform origin to effector pivot vector X position
        p1[1] = 0.0;                // platform origin to effector pivot vector Y position
        p1[2] = 0.0;                // platform origin to effector pivot vector Z position
        p2[0] = -k*sin(30*deg2rad); // same for second effector pivot point
        p2[1] = k*cos(30*deg2rad);
        p2[2] = 0.0;
        p3[0] = -k*sin(30*deg2rad); // same for third effector pivot point
        p3[1] = -k*cos(30*deg2rad);
        p3[2] = 0.0;
    }
    float dot_product(float v[], float u[])
    {
        float result = 0.0;
        for (int i = 0; i < 3; i++)
        {
            result += v[i]*u[i];
        }
        return result;
    }
    void BRP(float theta, float phi)
    {
        bRp0[0] = cos(theta*deg2rad);
        bRp0[1] = sin(theta*deg2rad)*sin(phi*deg2rad);
        bRp0[2] = cos(phi*deg2rad)*sin(theta*deg2rad);
        bRp1[0] = 0;
        bRp1[1] = cos(phi*deg2rad);
        bRp1[2] = -sin(phi*deg2rad);
        bRp2[0] = -sin(theta*deg2rad);
        bRp2[1] = cos(theta*deg2rad)*sin(phi*deg2rad);
        bRp2[2] = cos(phi*deg2rad)*cos(theta*deg2rad);
    }
    float step_transform(float length,float d,float e)
    {
        if(length > 145.0)
        {
            length = 145.0;
        }
        if(length < 32.0)
        {
            length = 32.0;
        }
        float intermed = ((length*length)+(d*d)-(e*e))/(2*d*length);
        if(intermed >= 1 )
        {
            intermed = 0.99;
        }
        float angle = -rad2deg*acos(intermed) + 90.0 + phi0;
        //angle = deg2step * angle;
        if(angle < 0)
        {
            angle = 0;
        }
        return angle;
    }
    void create_l_vectors(float theta, float phi)
    {
        
        this->BRP(theta, phi);
        T[0] = 0.0; T[1] = 0.0; T[2] = z_set;
        l1[0] = T[0] + (dot_product(bRp0,p1)) - b1[0];
        l1[1] = T[1] + (dot_product(bRp1,p1)) - b1[1];
        l1[2] = T[2] + (dot_product(bRp2,p1)) - b1[2];
        l2[0] = T[0] + (dot_product(bRp0,p2)) - b2[0];
        l2[1] = T[1] + (dot_product(bRp1,p2)) - b2[1];
        l2[2] = T[2] + (dot_product(bRp2,p2)) - b2[2];
        l3[0] = T[0] + (dot_product(bRp0,p3)) - b3[0];
        l3[1] = T[1] + (dot_product(bRp1,p3)) - b3[1];
        l3[2] = T[2] + (dot_product(bRp2,p3)) - b3[2];

    }
    void final_update(float theta, float phi,float d,float e)
	//goi ham nay 
    {
        create_l_vectors(theta, phi);
        this->L1_a = step_transform(sqrt((l1[0]*l1[0])+(l1[1]*l1[1])+(l1[2]*l1[2])),d,e);
        this->L2_a = step_transform(sqrt((l2[0]*l2[0])+(l2[1]*l2[1])+(l2[2]*l2[2])),d,e);
        this->L3_a = step_transform(sqrt((l3[0]*l3[0])+(l3[1]*l3[1])+(l3[2]*l3[2])),d,e);
    }
    float get_L_0()
    {
        return this->L1_a;
        }
        float get_L_1()
    {
        return this->L2_a;
    }
    float get_L_2()
    {
        return this->L3_a;
    }
};

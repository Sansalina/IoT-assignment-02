#ifndef __SERVO_MOTOR__
#define __SERVO_MOTOR__

class ServoMotor {

protected:
    virtual void setAngle(int angle) = 0;
};

#endif
#include "drive.h"


FourWheelDrive::FourWheelDrive(std::vector<pros::Motor> & right, std::vector<pros::Motor> & left)
{
    rightMotors = right;
    leftMotors = left;
    allMotors = left;
    allMotors.insert(allMotors.end(), rightMotors.end(), rightMotors.end());

    numMotors = allMotors.size();
    //gyro = (* inputGyro);
}
//take in a vector of motors, and set their speed to a value
void FourWheelDrive::setMotors(std::vector<pros::Motor> & motors, double speed)
{
  for(auto motor : motors)
  {
    motor = speed;
  }
}

//take in a vector of motors, and set their brake type to a given type
void FourWheelDrive::setBrakes(std::vector<pros::Motor> & motors,  pros::motor_brake_mode_e_t brakeType)
{
  for(auto motor: motors)
  {
    motor.set_brake_mode(brakeType);
  }
}

//tqke in a vector of motors, and call the move relative function for all of them with a given distance and speed
void FourWheelDrive::setMotorsRelative(std::vector<pros::Motor> & motors, double distance, double speed)
{
  for(auto motor : motors)
  {
    motor.move_relative(distance, speed);
  }
}

//function used in autononomous to drive for a given distance at a given speed
void FourWheelDrive::driveDist(double target, DIRECTION direction, int numCubes, double maxSpeed)
{
    double speed = maxSpeed;
    double endDistance = 0;
    double startDistance = 2 * ROTATION_MUL;
    double currDist = 0;
    double deccelDist = 2 * ROTATION_MUL;
    double averagePos = 0;
    double distPercent = 0;
    int stopCount = 0;

    auto previous_brake = leftMotors[0].get_brake_mode();
    setBrakes(allMotors,  pros::E_MOTOR_BRAKE_HOLD);
    //gyro.reset();


    setDirection(direction);
    target *= ROTATION_MUL;
    deccelDist = distReq(maxSpeed, numCubes, direction);
    startDistance = distReq(maxSpeed, numCubes, direction);
    endDistance = target - deccelDist;

    if ( startDistance + deccelDist > target)
    {
        distPercent = target / (startDistance + deccelDist);
        maxSpeed *= distPercent;
        startDistance *= distPercent;
        endDistance *= distPercent;
    }

    if (maxSpeed > 10)
    {
        for (int i = 0; i < allMotors.size(); i++) //sets the motors to 0
        {
            allMotors[i].set_zero_position(0);
        }

        while (averagePos < startDistance && stopCount < STOP_AMOUNT)
        {
            averagePos = (leftMotors.front().get_position() + rightMotors.front().get_position() +
                    leftMotors.back().get_position() + leftMotors.back().get_position()) / 4;
                    speed = maxSpeed * (averagePos / startDistance) + 14;
            if(speed < 20)
                speed = 20;
            if (speed > maxSpeed)
            {
                speed = maxSpeed;
            }

            correctDist(leftMotors, rightMotors, averagePos, speed, direction);
            pros::delay(20);
            if (leftMotors.front().get_actual_velocity() <= 5)
                stopCount++;
        }

        while (averagePos < endDistance && stopCount < STOP_AMOUNT)
        {
            averagePos = (leftMotors.front().get_position() + rightMotors.front().get_position() +
            leftMotors.back().get_position() + leftMotors.back().get_position()) / 4;

            correctDist(leftMotors, rightMotors, averagePos, speed, direction);

            pros::delay(20);
            if (leftMotors.front().get_actual_velocity() <= 5)
                stopCount++;
        }

        endDistance = target - averagePos;
        while (averagePos < target && stopCount < STOP_AMOUNT)
        {
            averagePos = (leftMotors.front().get_position() + rightMotors.front().get_position() +
            leftMotors.back().get_position() + leftMotors.back().get_position()) / 4;
            currDist = target - averagePos;
            speed = maxSpeed * (currDist / endDistance) + 14;
            if(speed < 20)
                speed = 20;
            if (speed > maxSpeed)
            {
               speed = maxSpeed;
            }

            correctDist(leftMotors, rightMotors, averagePos, speed, direction);
            pros::delay(20);
            if (leftMotors.front().get_actual_velocity() <= 5)
                stopCount++;
        }
    }
    else
    {
        speed = 10;

        while (averagePos < endDistance && stopCount < STOP_AMOUNT)
        {
            averagePos = (leftMotors.front().get_position() + rightMotors.front().get_position() +
            leftMotors.back().get_position() + leftMotors.back().get_position()) / 4;

            correctDist(leftMotors, rightMotors, averagePos, speed, direction);

            pros::delay(20);
            if (leftMotors.front().get_actual_velocity() <= 5)
                stopCount++;
        }
    }
    //displayPosition();

    setMotors(allMotors, 0);
    setDirection(FORWARD);
    pros::delay(150);
    setBrakes(allMotors,  previous_brake );
}

//a function that finds the best speed based on the distance of the wheels
void FourWheelDrive::correctDist (std::vector<pros::Motor> leftMotors, std::vector<pros::Motor> rightMotors,
    double target, double speed, DIRECTION direction)
{
    double leftValue = 0;
    double rightValue = 0;
    for(int i = 0; i < leftMotors.size(); i++)
    {
      leftValue += leftMotors[i].get_position();
      rightValue += rightMotors[i].get_position();
    }
    leftValue /= leftMotors.size();
    leftValue -= target;

    rightValue /= rightMotors.size();
    rightValue -= target;

    double leftSpeed = speed;
    double rightSpeed = speed;

    if ( rightValue > 2)
    {
        leftSpeed *= 0.96;
    }
    else if (rightValue < -2)
    {
        leftSpeed *= 1.04;
    }


    if ( rightValue > 2)
    {
        rightSpeed *= 0.96;
    }
    else if (rightValue < -2)
    {
        rightSpeed *= 1.04;
    }

    /*if(direction == FORWARD)
    {
        if(gyroVal < -10)
        {
            rightSpeed *= 0.92;
        }
        else if(gyroVal > 10)
        {
            leftSpeed *= 0.92;
        }
    }
    else
    {
        if(gyroVal < -10)
        {
            leftSpeed *= 1.15;
        }
        else if(gyroVal > 10)
        {
            rightSpeed *= 1.15;
        }
    }*/

    setMotors(leftMotors, leftSpeed);
    setMotors(rightMotors, rightSpeed);

}

double FourWheelDrive::distReq(double speed, int numCubes, DIRECTION direction)
{
    double result = 1.0 * ROTATION_MUL;

    if (numCubes == -1)
    {
        result = 0.5 * ROTATION_MUL;
    }
    if (numCubes == -2)
    {
        result = 0.05 * ROTATION_MUL;
    }
    if (direction == BACKWARD)
    {
        result = 1.8 * ROTATION_MUL;
        if(numCubes == 8)
            result = 2.2 * ROTATION_MUL;
    }

    return result;
}

void FourWheelDrive::setDirection(DIRECTION inputDirection)
{
    if (inputDirection != direction)
    {
        for(int i = 0; i < numMotors; i++)
        {
            allMotors[i].set_reversed(true);
        }
    }
}

//function used in autonomous to turn a given degree amount at a given speed
void FourWheelDrive::autoTurnRelative(std::vector<pros::Motor> & leftWheelMotorVector,
    std::vector<pros::Motor> & rightWheelMotorVector, double amount)
{
  double currSpeed;
  double speed = 80;

  pros::motor_brake_mode_e_t prevBrake = leftWheelMotorVector[0].get_brake_mode();
  setBrakes(leftWheelMotorVector,  pros::E_MOTOR_BRAKE_BRAKE );
  setBrakes(rightWheelMotorVector,  pros::E_MOTOR_BRAKE_BRAKE );

  amount *= 10;
  //gyro.reset();
  pros::delay(100);

  float lastRemainingTicks = 99999;
  float remainingTicks = amount;
  //white both not close to target and not overshooting
  while( fabs(remainingTicks) >= 5 && fabs(remainingTicks) - fabs(lastRemainingTicks) <= 1)
  {
    double multiplier = fmax(fmin(1, fabs(remainingTicks / 1250.0)), .25);
    currSpeed = speed;
    currSpeed *= multiplier;

    if (currSpeed < 30)
        currSpeed = 30;

    if(remainingTicks < 0)
    {
      setMotors(leftWheelMotorVector, -currSpeed);
      setMotors(rightWheelMotorVector, currSpeed);
    }
    else
    {
      setMotors(leftWheelMotorVector, currSpeed);
      setMotors(rightWheelMotorVector, -currSpeed);
    }

  	//if(master.get_digital(KILL_BUTTON))
    //{
    //  break;
    //}
    pros::delay(20);
    //lastRemainingTicks = remainingTicks;
    //remainingTicks = amount - gyro.get_value();
    pros::lcd::set_text(5, "gyro: " + std::to_string(remainingTicks));
    pros::lcd::set_text(6, "gyro: " + std::to_string(lastRemainingTicks) + " " + std::to_string(fabs(remainingTicks) <= fabs(lastRemainingTicks)));
  }

  setMotors(leftWheelMotorVector, 0);
  setMotors(rightWheelMotorVector, 0);
  pros::delay(50);

  setBrakes(leftWheelMotorVector, prevBrake );
  setBrakes(rightWheelMotorVector, prevBrake );

  return;
}

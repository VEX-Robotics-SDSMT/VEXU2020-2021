#ifndef _FOUR_WHEEL_DRIVE
#define _FOUR_WHEEL_DRIVE

#include "../../include/api.h"
#include "../logger/logger.h"
#include "../../include/helperfunctions.h"
#include <vector>
#include <sstream>
#include <fstream>
#include "math.h"

enum DIRECTION {FORWARD, BACKWARD};

class FourWheelDrive
{
    std::vector<pros::Motor> *rightMotors;
    std::vector<pros::Motor> *leftMotors;
    pros::Imu *inertialSensor;
    pros::Controller *master;

    Logger logger;

    //calibration values
    std::stringstream fileStream;
    //these values are observed from the routine in the past
    //they will work if the file input fails, but not as well
    double maxSpeed = 128;
    double midSpeed = 69;
    double minSpeed = 10;
    double speedBias = 1;

    double LRBiasHigh = 1;
    double LRBiasLow = 1;
    double LRBiasHighBack = 1;
    double LRBiasLowBack = 1;
    double maxAccelerationForward = 3;
    double maxAccelerationBackward = 3;

    const double ROTATION_MUL = 845;
    const int STOP_AMOUNT = 100;
    const int LOOP_DELAY = 20;

    int numMotors; //number of motors on ONE side

public:
    FourWheelDrive(std::vector<pros::Motor>&, std::vector<pros::Motor>&,
        pros::Imu & , pros::Controller & );

    void readCalibration();
    void writeCalibration();
    void calibrateAll();
    void calibrateMinSpeed();
    void calibrateMaxSpeed();
    void calibrateMaxAcceleration();
    void calibrateDrift();
    void calibrateDriftLoop(double testSpeed, double &bias);
    void waitForUser(std::string message);
    void showOff();

    void accelerate(double speed);
    void setMotorsRelative(std::vector<pros::Motor> *motors, double distance, double speed);
    void setMotorsRelative(double distance, double speed);
    void setBrakes(std::vector<pros::Motor> *motors,  pros::motor_brake_mode_e_t brakeType);
    void setBrakes(pros::motor_brake_mode_e_t brakeType);
    double rawGetAllSpeed(double bias);
    double getAllSpeed();
    double getAllPosition();
    double getPosition(std::vector<pros::Motor> * motors);

    void drive(std::vector<pros::Motor> *leftWheelMotorVector,
        std::vector<pros::Motor> *rightWheelMotorVector, int distance);
    void driveTilesPID(float numTiles, float desiredSpeed= 100);
    void turnDegreesPID(float numDegrees, float desiredSpeed= 100);

private:
    void rawSetMotors(double speed, double bias = 1);
    void setMotors(std::vector<pros::Motor> *motors, double speed);
    void setMotors(double speed);
    void setZeroPosition(std::vector<pros::Motor> * motors);
    void setZeroPosition();

    bool panic();



    void checkGyro();
    void correctGyroCalibration(float accel, float jerk);
    void addStream(std::stringstream &gyroStream, float speed);

};
#endif

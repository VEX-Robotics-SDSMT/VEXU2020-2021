#include "main.h"

/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */


void autonomous()
{
    pros::Task autonAlpha(twin::autonomousTaskAlpha, (void*)0,
                          TASK_PRIORITY_DEFAULT, TASK_STACK_DEPTH_DEFAULT, "Alpha auton");
    pros::Task autonBeta(twin::autonomousTaskBeta, (void*)1,
                         TASK_PRIORITY_DEFAULT, TASK_STACK_DEPTH_DEFAULT, "Beta auton");
    pros::lcd::set_text(7, "Finsished?");

}


/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize()
{
    pros::lcd::initialize();
    pros::lcd::set_text(2, "Calling initialize: " + std::to_string(pros::millis()));
    //visionSensor.clear_led();

    inertialSensorAlpha.reset();
    inertialSensorBeta.reset();

    // Delay to allow inertial sensors to reset properly
    pros::delay(1300 * 2);

    inserterAlpha.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD );
    inserterBeta.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD );
    pros::lcd::set_text(4, "Mode" + std::to_string(inserterAlpha.get_brake_mode()));
    pros::lcd::set_text(5, "Mode" + std::to_string(inserterAlpha.get_brake_mode()));

    driveBaseAlpha = new FourWheelDrive(rightWheelMotorVectorAlpha, leftWheelMotorVectorAlpha, inertialSensorAlpha, controllerAlpha);
    driveBaseBeta = new FourWheelDrive(rightWheelMotorVectorBeta, leftWheelMotorVectorBeta, inertialSensorBeta, controllerBeta);
    driveBasePair[0] = driveBaseAlpha;
    driveBasePair[1] = driveBaseBeta;
}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled()
{
    pros::lcd::set_text(3, "Calling disabled: " + std::to_string(pros::millis()));
    delete driveBaseAlpha;
    delete driveBaseBeta;
}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize() {
    pros::lcd::set_text(4, "Calling comp_init: " + std::to_string(pros::millis()));
    /*
    inserterAlpha.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD );
    inserterBeta.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD );
    pros::lcd::set_text(5, "Mode" + std::to_string(inserterAlpha.get_brake_mode()));
    pros::lcd::set_text(6, "Mode" + std::to_string(inserterAlpha.get_brake_mode()));
    */
}



/*
 * Runs the operator control code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the operator
 * control mode.
 *
 * If no competition control is connected, this function will run immediately
 * following initialize().
 *
 * If the robot is disabled or communications is lost, the
 * operator control task will be stopped. Re-enabling the robot will restart the
 * task, not resume it from where it left off.
 */

void opcontrol()
{
    pros::Task opControlAlpha(twin::opcontrolTask, (void*)0,
                              TASK_PRIORITY_DEFAULT, TASK_STACK_DEPTH_DEFAULT, "Alpha op control");
    pros::Task opControlBeta(twin::opcontrolTask, (void*)1,
                             TASK_PRIORITY_DEFAULT, TASK_STACK_DEPTH_DEFAULT, "Beta op control");
}

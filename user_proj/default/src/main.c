/** @file main.c
 *  @brief Encoder based wheel position control using a periodic PID task.
 */
#include <349_lib.h>
#include <349_peripheral.h>
#include <349_threads.h>
#include <stdlib.h>
#include <stdint.h>

/** @brief ticks for one revolution */
#define TICKS_PER_REV 1200

/** @brief motor speed bounds*/
#define MAX_MOTOR_SPEED 65
#define MIN_MOTOR_SPEED 20

/** @brief target position*/
#define TARGET_POSITION_TICKS TICKS_PER_REV

/** @brief pid thread period*/
#define CONTROL_PERIOD_TICKS 50 /* 1 kHz scheduler */

/** @brief position error bounds*/
#define POSITION_TOLERANCE 8

/** @brief PID gains to be tuned */
#define PID_KP_NUM 3
#define PID_KP_DEN 100
#define PID_KI_DEN 50000
#define PID_KD_DEN 100
#define PID_INTEGRAL_LIMIT 10000

/** @brief Set sign to -1 if forward motion decrements */
#define LEFT_ENCODER_FORWARD_SIGN 1
#define RIGHT_ENCODER_FORWARD_SIGN 1

#define NUM_ENCODERS 2

static volatile int32_t wheel_position[NUM_ENCODERS];
static volatile uint8_t previous_encoder_state[NUM_ENCODERS];
static volatile int32_t target_position = TARGET_POSITION_TICKS;

/** @brief Tick change based on transition, indexed by (previous << 2) | current. */
static const int8_t quadrature_delta[16] = {
     0,  1, -1,  0,
    -1,  0,  0,  1,
     1,  0,  0, -1,
     0, -1,  1,  0
};

static void encoder_update(uint32_t encoder, uint32_t a, uint32_t b)
{
    uint8_t state = (uint8_t)(((a & 1) << 1) | (b & 1));
    uint8_t previous = previous_encoder_state[encoder];
    //define transition as (prev, curr)
    uint8_t transition = (uint8_t)((previous << 2) | state);
    //depends if forward means decrement or increment
    int32_t sign = encoder == LEFT_ENCODER ? LEFT_ENCODER_FORWARD_SIGN
                                           : RIGHT_ENCODER_FORWARD_SIGN;
    //find change in wheel position based on transition, update wheel position
    wheel_position[encoder] += sign * quadrature_delta[transition];
    previous_encoder_state[encoder] = state;
}

static void left_encoder_callback(uint32_t a, uint32_t b)
{
    encoder_update(LEFT_ENCODER, a, b);
}

static void right_encoder_callback(uint32_t a, uint32_t b)
{
    encoder_update(RIGHT_ENCODER, a, b);
}

static uint32_t clamp_duty(int32_t output)
{
    //absolute duty for speed (sign means direction)
    int32_t duty = output < 0 ? -output : output;
    //keep within bounds
    if (duty > MAX_MOTOR_SPEED) duty = MAX_MOTOR_SPEED;
    if (duty < MIN_MOTOR_SPEED) duty = MIN_MOTOR_SPEED;
    return (uint32_t)duty;
}

static void drive_wheel(motor_t motor, int32_t position, int32_t target,
                        int32_t *integral, int32_t *previous_error)
{
    int32_t error = target - position;
    //error is out of bounds, stop
    if (error >= -POSITION_TOLERANCE && error <= POSITION_TOLERANCE) {
        *integral = 0;
        *previous_error = error;
        motor_set(motor, 0, MOTOR_STOP);
        return;
    }

    //update integral, keep it in bounds
    *integral += error;
    if (*integral > PID_INTEGRAL_LIMIT) *integral = PID_INTEGRAL_LIMIT;
    if (*integral < -PID_INTEGRAL_LIMIT) *integral = -PID_INTEGRAL_LIMIT;
    //calculate derivative
    int32_t derivative = error - *previous_error;
    //pid controlled duty cycle
    int32_t output = (error * PID_KP_NUM) / PID_KP_DEN +
                     *integral / PID_KI_DEN + derivative / PID_KD_DEN;
    *previous_error = error;

    direction_t direction = output >= 0 ? MOTOR_FORWARD : MOTOR_BACKWARD;
    motor_set(motor, clamp_duty(output), direction);
}

static void pid_thread(void *argument UNUSED)
{
    int32_t integral_left = 0;
    int32_t integral_right = 0;
    int32_t previous_error_left = 0;
    int32_t previous_error_right = 0;

    while (1) {
        // read wheel position
        int32_t left_position = wheel_position[LEFT_ENCODER];
        int32_t right_position = wheel_position[RIGHT_ENCODER];
        int32_t target = target_position;
        // call drive wheel
        drive_wheel(LEFT_MOTOR, left_position, target,
                    &integral_left, &previous_error_left);
        drive_wheel(RIGHT_MOTOR, right_position, target,
                    &integral_right, &previous_error_right);
        wait_until_next_period();
    }
}

int main(UNUSED int argc, UNUSED char const *argv[])
{
    ABORT_ON_ERROR(register_encoder_callback(LEFT_ENCODER,
                                               left_encoder_callback));
    ABORT_ON_ERROR(register_encoder_callback(RIGHT_ENCODER,
                                               right_encoder_callback));

    ABORT_ON_ERROR(thread_init(1, 256, NULL, 0));
    ABORT_ON_ERROR(thread_create(pid_thread, 0, 8, CONTROL_PERIOD_TICKS, NULL));
    ABORT_ON_ERROR(scheduler_start(1000));

    return RET_0349;
}

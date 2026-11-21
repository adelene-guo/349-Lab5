#include <encoder.h>
#include <gpio.h>
#include <motor_driver.h>
#include <pwm.h>
#include <stdint.h>
#include <unistd.h>

#define PWM_PERIOD 4000

#define MAX_DUTY_CYCLE 100

#define NUM_MOTORS UINT32_C(2)

#define GPIO_MOTOR_IN_ATTR \
    MODE_GP_OUTPUT, OUTPUT_PUSH_PULL, OUTPUT_SPEED_HIGH, PUPD_NONE, ALT0

static struct motor_attr motors[NUM_MOTORS];

void motor_init(enum motor_mapping motor, struct motor_attr *attr,
                struct encoder_pin_attr *enc_attr)
{
    motors[motor] = *attr;

    gpio_init(attr->motor_in1.port, attr->motor_in1.num, GPIO_MOTOR_IN_ATTR);
    gpio_init(attr->motor_in2.port, attr->motor_in2.num, GPIO_MOTOR_IN_ATTR);
    gpio_init(attr->motor_en.port, attr->motor_en.num, MODE_ALT,
              OUTPUT_PUSH_PULL, OUTPUT_SPEED_VERY_HIGH, PUPD_PULL_DOWN,
              attr->timer.gpio_alt);

    encoder_init((enum encoder_mapping)motor, enc_attr);

    IS_COMP = (attr->timer.is_comp) ? 1 : 0;
    timer_start_pwm(PWM_PERIOD, 0, attr->timer.timer, attr->timer.channel);
}

int sys_motor_set(enum motor_mapping motor, uint32_t duty_cycle,
                  direction_t direction)
{
    // NOTE: This function should return -1 if an invalid motor is given and 0
    // otherwise
    // TODO: Implement this function.
    // Hint: Take a look at the datasheet for the L298N motor driver

    (void)motor;
    (void)duty_cycle;
    (void)direction;

    return 0;
}

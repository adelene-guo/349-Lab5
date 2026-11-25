#ifndef _BOARD_PINS_H_
#define _BOARD_PINS_H_

#include <gpio.h>

/* Motor driver inputs and PWM enables. */
#define RIGHT_MOTOR_IN1_PORT GPIO_A /* D7 */
#define RIGHT_MOTOR_IN1_PIN  8
#define RIGHT_MOTOR_IN2_PORT GPIO_A /* D8 */
#define RIGHT_MOTOR_IN2_PIN  9
#define RIGHT_MOTOR_EN_PORT  GPIO_B /* D3 */
#define RIGHT_MOTOR_EN_PIN   3

#define LEFT_MOTOR_IN1_PORT GPIO_A /* D2 */
#define LEFT_MOTOR_IN1_PIN  10
#define LEFT_MOTOR_IN2_PORT GPIO_A /* D11 */
#define LEFT_MOTOR_IN2_PIN  7
#define LEFT_MOTOR_EN_PORT  GPIO_B /* D5 */
#define LEFT_MOTOR_EN_PIN   4

/* Wheel encoder inputs. */
#define LEFT_ENCODER_A_PORT GPIO_A /* A2 */
#define LEFT_ENCODER_A_PIN  4
#define LEFT_ENCODER_B_PORT GPIO_B /* D10 */
#define LEFT_ENCODER_B_PIN  6
#define RIGHT_ENCODER_A_PORT GPIO_B /* D4 */
#define RIGHT_ENCODER_A_PIN  5
#define RIGHT_ENCODER_B_PORT GPIO_B /* D6 */
#define RIGHT_ENCODER_B_PIN  10

/* On-board button and LEDs. */
#define BUTTON_PORT GPIO_A /* A1 */
#define BUTTON_PIN  1
#define LED_RED_PORT GPIO_A /* D12 */
#define LED_RED_PIN  6
#define LED_GREEN_PORT GPIO_A /* D13 */
#define LED_GREEN_PIN  5

/* Servo output used by the PWM test driver. */
#define SERVO_PWM_PORT GPIO_C /* D9 */
#define SERVO_PWM_PIN  7

/* EXTI IRQ number for each GPIO line (line number is the array index). */
static const int EXTI_IRQN[16] = {
    6, 7, 8, 9, 10, 23, 23, 23, 23, 23, 40, 40, 40, 40, 40, 40
};

/* Motor PWM timer configuration. */
#define RIGHT_MOTOR_PWM_TIMER    2
#define RIGHT_MOTOR_PWM_CHANNEL  2
#define RIGHT_MOTOR_PWM_GPIO_ALT ALT5
#define RIGHT_MOTOR_PWM_IS_COMP  1

#define LEFT_MOTOR_PWM_TIMER     3
#define LEFT_MOTOR_PWM_CHANNEL   1
#define LEFT_MOTOR_PWM_GPIO_ALT  ALT5
#define LEFT_MOTOR_PWM_IS_COMP   0

#endif /* _BOARD_PINS_H_ */

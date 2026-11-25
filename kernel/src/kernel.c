#include <stdlib.h>
#include <string.h>

#include <gpio.h>
#include <board_pins.h>

#include <systick.h>
#include <lcd_driver.h>

#include <timer.h>
#include <nvic.h>

#include "arm.h"
#include "kernel.h"
#include "timer.h"
#include "i2c.h"
#include "lcd_driver.h"
#include "printk.h"
#include "uart_polling.h"
#include "uart.h"
#include "stdint.h"
#include "syscall.h"
#include "stdio.h"
#include "unistd.h"
#include "string.h"
#include "arm.h"
#include "servok.h"
#include "pwm.h"
#include "exti.h"
#include "motor_driver.h"

/** @brief USARTDIV value */
#define USART_DIV 0x8B

/** @brief Documents kernel main. */
int kernel_main( void ) {
  init_349(); // DO NOT REMOVE THIS LINE
  uart_init(USART_DIV);
  systick_init(1000);
  
  //lcd_driver_init();
  //lcd_clear();
  //systick_delay(1000);

  //servo_init();
  //pwm_testleds();
  //exti_testbutton();
  enum motor_mapping motor_right = RIGHT_MOTOR;
  struct motor_attr attr_right;
  attr_right.motor_in1.port = RIGHT_MOTOR_IN1_PORT;
  attr_right.motor_in1.num = RIGHT_MOTOR_IN1_PIN;

  attr_right.motor_in2.port = RIGHT_MOTOR_IN2_PORT;
  attr_right.motor_in2.num = RIGHT_MOTOR_IN2_PIN;

  attr_right.motor_en.port = RIGHT_MOTOR_EN_PORT;
  attr_right.motor_en.num = RIGHT_MOTOR_EN_PIN;

  attr_right.timer.timer = RIGHT_MOTOR_PWM_TIMER;
  attr_right.timer.channel = RIGHT_MOTOR_PWM_CHANNEL;
  attr_right.timer.gpio_alt = RIGHT_MOTOR_PWM_GPIO_ALT;
  attr_right.timer.is_comp = RIGHT_MOTOR_PWM_IS_COMP;

  struct encoder_pin_attr enc_attr_right;
  enc_attr_right.encoder_pin_a.port = LEFT_ENCODER_A_PORT;
  enc_attr_right.encoder_pin_a.num = LEFT_ENCODER_A_PIN;
  enc_attr_right.encoder_pin_a.irq_num = EXTI_IRQN[LEFT_ENCODER_A_PIN];

  enc_attr_right.encoder_pin_b.port = LEFT_ENCODER_B_PORT;
  enc_attr_right.encoder_pin_b.num = LEFT_ENCODER_B_PIN;
  enc_attr_right.encoder_pin_b.irq_num = EXTI_IRQN[LEFT_ENCODER_B_PIN];

  motor_init(motor_right, &attr_right, &enc_attr_right);

  enum motor_mapping motor_left = LEFT_MOTOR;
  struct motor_attr attr_left;
  attr_left.motor_in1.port = LEFT_MOTOR_IN1_PORT;
  attr_left.motor_in1.num = LEFT_MOTOR_IN1_PIN;

  attr_left.motor_in2.port = LEFT_MOTOR_IN2_PORT;
  attr_left.motor_in2.num = LEFT_MOTOR_IN2_PIN;

  attr_left.motor_en.port = LEFT_MOTOR_EN_PORT;
  attr_left.motor_en.num = LEFT_MOTOR_EN_PIN;

  attr_left.timer.timer = LEFT_MOTOR_PWM_TIMER;
  attr_left.timer.channel = LEFT_MOTOR_PWM_CHANNEL;
  attr_left.timer.gpio_alt = LEFT_MOTOR_PWM_GPIO_ALT;
  attr_left.timer.is_comp = LEFT_MOTOR_PWM_IS_COMP;

  struct encoder_pin_attr enc_attr_left;
  enc_attr_left.encoder_pin_a.port = RIGHT_ENCODER_A_PORT;
  enc_attr_left.encoder_pin_a.num = RIGHT_ENCODER_A_PIN;
  enc_attr_left.encoder_pin_a.irq_num = EXTI_IRQN[RIGHT_ENCODER_A_PIN];


  enc_attr_left.encoder_pin_b.port = RIGHT_ENCODER_B_PORT;
  enc_attr_left.encoder_pin_b.num = RIGHT_ENCODER_B_PIN;
  enc_attr_left.encoder_pin_b.irq_num = EXTI_IRQN[RIGHT_ENCODER_B_PIN];

  motor_init(motor_left, &attr_left, &enc_attr_left);

  enter_user_mode();
  return 0;
}

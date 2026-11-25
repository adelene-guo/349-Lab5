#include <stdlib.h>
#include <string.h>

#include <gpio.h>

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
  static const int exti_irqn[] = {6, 7, 8, 9, 10, 23, 23, 23, 23, 23, 40, 40, 40, 40, 40, 40};

  enum motor_mapping motor_right = RIGHT_MOTOR;
  struct motor_attr attr_right;
  attr_right.motor_in1.port = GPIO_A; //D7
  attr_right.motor_in1.num = 8;

  attr_right.motor_in2.port = GPIO_A; //D8
  attr_right.motor_in2.num = 9;

  attr_right.motor_en.port = GPIO_B; //D3
  attr_right.motor_en.num = 3;

  attr_right.timer.timer = 2; //associated with D3
  attr_right.timer.channel = 2;
  attr_right.timer.gpio_alt = ALT5;
  attr_right.timer.is_comp = 1;

  struct encoder_pin_attr enc_attr_right;
  enc_attr_right.encoder_pin_a.port = GPIO_A; //A2
  enc_attr_right.encoder_pin_a.num = 4;
  enc_attr_right.encoder_pin_a.irq_num = exti_irqn[4];

  enc_attr_right.encoder_pin_b.port = GPIO_B; //D10
  enc_attr_right.encoder_pin_b.num = 6;
  enc_attr_right.encoder_pin_b.irq_num = exti_irqn[6];

  motor_init(motor_right, &attr_right, &enc_attr_right);

  enum motor_mapping motor_left = LEFT_MOTOR;
  struct motor_attr attr_left;
  attr_left.motor_in1.port = GPIO_A; //D2
  attr_left.motor_in1.num = 10;

  attr_left.motor_in2.port = GPIO_A;//D11
  attr_left.motor_in2.num = 7;

  attr_left.motor_en.port = GPIO_B; //D5
  attr_left.motor_en.num = 4;

  attr_left.timer.timer = 3; //associated with D5
  attr_left.timer.channel = 1;
  attr_left.timer.gpio_alt = ALT5;
  attr_left.timer.is_comp = 0;

  struct encoder_pin_attr enc_attr_left;
  enc_attr_left.encoder_pin_a.port = GPIO_B; //D4
  enc_attr_left.encoder_pin_a.num = 5;
  enc_attr_left.encoder_pin_a.irq_num = exti_irqn[5];


  enc_attr_left.encoder_pin_b.port = GPIO_B; //D6
  enc_attr_left.encoder_pin_b.num = 10;
  enc_attr_left.encoder_pin_b.irq_num = exti_irqn[10];

  motor_init(motor_left, &attr_left, &enc_attr_left);

  sys_motor_set(RIGHT_MOTOR, 50, FORWARD, attr_right);
  sys_motor_set(LEFT_MOTOR, 50, FORWARD, attr_left);
  
  while (1);
  enter_user_mode();
  return 0;
}

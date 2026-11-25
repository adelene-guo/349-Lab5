#include <printk.h>
#include <pwm.h>
#include <rcc.h>
#include <unistd.h>
#include <nvic.h>
#include <board_pins.h>

/** @brief enable bit for update generation */
#define TIM_EGR_UG (1 << 0)

/** @brief status register update interrupt flag */
#define TIM_SR_UIF (1 << 0)

/** @brief enable bit for update interrupt */
#define TIM_DIER_UIE (1 << 0)

/** @brief enable bit for counter */
#define TIM_CR1_CEN (1 << 0)

/** @brief OCxM=110 selects PWM mode 1; OCxPE=1 enables compare preload. */
#define TIM_CCMR_PWM_MODE1_PRELOAD 0x68

/** @brief TIM1 register map */
struct tim1 {
    volatile uint32_t cr1;     /**< 00 Control Register 1 */
    volatile uint32_t cr2;     /**< 04 Control Register 2 */
    volatile uint32_t smcr;    /**< 08 Slave Mode Control */
    volatile uint32_t dier;    /**<*< 0C DMA/Interrupt Enable */
    volatile uint32_t sr;      /**< 10 Status Register */
    volatile uint32_t egr;     /**< 14 Event Generation */
    volatile uint32_t ccmr[2]; /**< 18-1C Capture/Compare Mode */
    volatile uint32_t ccer;    /**< 20 Capture/Compare Enable */
    volatile uint32_t cnt;     /**< 24 Counter Register */
    volatile uint32_t psc;     /**< 28 Prescaler Register */
    volatile uint32_t arr;     /**< 2C Auto-Reload Register */
    volatile uint32_t rcr;     /**< 30 Repetition Counter Register */
    volatile uint32_t ccr[4];  /**< 34-40 Capture/Compare */
    volatile uint32_t bdtr;    /**< 44 Break and Dead-Time Register */
    volatile uint32_t dcr;     /**< 48 DMA Control Register */
    volatile uint32_t dmar;    /**< 4C DMA address for full transfer Register */
};

/** @brief TIM2-5 register map */
struct tim2_5 {
    volatile uint32_t cr1;        /**< 00 Control Register 1 */
    volatile uint32_t cr2;        /**< 04 Control Register 2 */
    volatile uint32_t smcr;       /**< 08 Slave Mode Control */
    volatile uint32_t dier;       /**< 0C DMA/Interrupt Enable */
    volatile uint32_t sr;         /**< 10 Status Register */
    volatile uint32_t egr;        /**< 14 Event Generation */
    volatile uint32_t ccmr[2];    /**< 18-1C Capture/Compare Mode */
    volatile uint32_t ccer;       /**< 20 Capture/Compare Enable */
    volatile uint32_t cnt;        /**< 24 Counter Register */
    volatile uint32_t psc;        /**< 28 Prescaler Register */
    volatile uint32_t arr;        /**< 2C Auto-Reload Register */
    volatile uint32_t reserved_1; /**< 30 */
    volatile uint32_t ccr[4];     /**< 34-40 Capture/Compare */
    volatile uint32_t reserved_2; /**< 44 */
    volatile uint32_t dcr;        /**< 48 DMA Control Register */
    volatile uint32_t dmar; /**< 4C DMA address for full transfer Register */
    volatile uint32_t or ;  /**< 50 Option Register */
};

uint8_t IS_COMP = 0;

/** @brief Documents timer irqn[]. */
static const int pwm_timer_irqn[] = {0, 0, 28, 29, 30, 50};

#define TIM1_BASE (struct tim1 *) 0x40010000

struct tim2_5* const timer_base_pwm[] = {(void *)0x0,
                                     (void *)0x0,
                                     (void *)0x40000000,
                                     (void *)0x40000400,
                                     (void *)0x40000800,
                                     (void *)0x40000C00};

void timer_start_pwm(uint32_t period, uint32_t duty_cycle, uint32_t timer, uint32_t channel) {
    if (timer < 2 || timer > 5 || channel < 1 || channel > 4) return;

    // Configure the pins that uses this timer's PWM signal
    // TIM2: AF1 and TIM3: AF2
    if (timer == 2 && channel == 1) {
        gpio_init(LED_GREEN_PORT, LED_GREEN_PIN, MODE_ALT, OUTPUT_PUSH_PULL,
                  OUTPUT_SPEED_VERY_HIGH, PUPD_NONE, ALT1);
    } else if (timer == 3) {
        if (channel == 1) {
            gpio_init(LED_RED_PORT, LED_RED_PIN, MODE_ALT, OUTPUT_PUSH_PULL,
                      OUTPUT_SPEED_VERY_HIGH, PUPD_NONE, ALT2);
            gpio_init(RIGHT_MOTOR_IN1_PORT, RIGHT_MOTOR_IN1_PIN, MODE_ALT, OUTPUT_PUSH_PULL,
                      OUTPUT_SPEED_VERY_HIGH, PUPD_NONE, ALT2);
        } else if (channel == 2) {
            gpio_init(LEFT_MOTOR_IN2_PORT, LEFT_MOTOR_IN2_PIN, MODE_ALT, OUTPUT_PUSH_PULL,
                      OUTPUT_SPEED_VERY_HIGH, PUPD_NONE, ALT2);
            gpio_init(RIGHT_MOTOR_IN2_PORT, RIGHT_MOTOR_IN2_PIN, MODE_ALT, OUTPUT_PUSH_PULL,
                      OUTPUT_SPEED_VERY_HIGH, PUPD_NONE, ALT2);
            gpio_init(SERVO_PWM_PORT, SERVO_PWM_PIN, MODE_ALT, OUTPUT_PUSH_PULL,
                      OUTPUT_SPEED_VERY_HIGH, PUPD_NONE, ALT2);
        } else if (channel == 3) {
            gpio_init(LEFT_MOTOR_IN1_PORT, LEFT_MOTOR_IN1_PIN, MODE_ALT, OUTPUT_PUSH_PULL,
                      OUTPUT_SPEED_VERY_HIGH, PUPD_NONE, ALT2);
        }
    }

    struct rcc_reg_map *rcc = RCC_BASE;
    rcc->apb1_enr |= 1 << (timer-2); // enable RCC for appropriate timer

    struct tim2_5 *tim = timer_base_pwm[timer];
    uint32_t channel_index = channel - 1;
    uint32_t ccmr_index = channel_index / 2;
    uint32_t ccmr_shift = (channel_index % 2) * 8;
    uint32_t ccmr_mask = 0xff << ccmr_shift;

    // Configure the selected channel for PWM mode 1 with preload enabled.
    tim->ccmr[ccmr_index] = (tim->ccmr[ccmr_index] & ~ccmr_mask) |
                            (TIM_CCMR_PWM_MODE1_PRELOAD << ccmr_shift);
    tim->ccer |= 1 << (channel_index * 4);
    tim->ccr[channel_index] = duty_cycle;

    tim->arr = period;
    tim->egr |= TIM_EGR_UG; // load the programmed auto-reload and compare values
    tim->sr &= ~TIM_SR_UIF; // clear update interrupt flag set by update event
    tim->dier |= TIM_DIER_UIE; // enable update interrupt
    nvic_irq(pwm_timer_irqn[timer], IRQ_ENABLE); // enable the interrupt
    tim->cr1 |= TIM_CR1_CEN; // enable counter
}

void timer_disable_pwm(uint32_t timer, uint32_t channel) {
    if (timer < 2 || timer > 5 || channel < 1 || channel > 4) return;

    struct tim2_5 *tim = timer_base_pwm[timer];
    uint32_t channel_index = channel - 1;
    tim->ccer &= ~(1u << (channel_index * 4)); // disable this channel's output
}

void timer_set_duty_cycle(uint32_t timer, uint32_t channel, uint32_t duty_cycle) {
    if (timer < 2 || timer > 5 || channel < 1 || channel > 4) return;

    struct tim2_5 *tim = timer_base_pwm[timer];
    uint32_t channel_index = channel - 1;
    tim->ccr[channel_index] = duty_cycle; //update duty cycle
}

void pwm_testleds() {
    const uint32_t period = 1000;
    timer_start_pwm(period, 0, 2, 1);
    timer_start_pwm(period, 0, 3, 1);

    while (1) {
        for (uint32_t duty = 0; duty <= period; duty += 10) {
            timer_set_duty_cycle(2, 1, duty);
            timer_set_duty_cycle(3, 1, duty);
            for (volatile uint32_t delay = 0; delay < 20000; ++delay) {
            }
        }
        for (uint32_t duty = period; duty > 0; duty -= 10) {
            timer_set_duty_cycle(2, 1, duty);
            timer_set_duty_cycle(3, 1, duty);
            for (volatile uint32_t delay = 0; delay < 20000; ++delay) {
            }
        }
    }
}

#include <exti.h>
#include <encoder.h>
#include <gpio.h>
#include <printk.h>
#include <rcc.h>
#include <nvic.h>
#include <board_pins.h>

/** @brief EXTI register map. */
struct exti
{
    volatile uint32_t imr;   /**< 00 Interrupt Mask Register */
    volatile uint32_t emr;   /**< 04 Event Mask Register */
    volatile uint32_t rtsr;  /**< 08 Rising trigger Selection */
    volatile uint32_t ftsr;  /**< 0C Falling trigger Selection */
    volatile uint32_t swier; /**< 10 Software Interrupt Event Register */
    volatile uint32_t pr;    /**< 14 Pending Register */
};

/** @brief System Config register map. */
struct syscfg
{
    volatile uint32_t memrmp;  /**< 00 Memory Remap */
    volatile uint32_t pmc;     /**< 04 Peripheral mode configuration */
    volatile uint32_t exti[4]; /**< 08-14 External interrupt configuration */
    volatile uint32_t cmpcr;   /**< 20 Compensation cell control */
};

#define EXTI_BASE ((volatile struct exti *)0x40013C00)
#define SYSCFG_BASE ((volatile struct syscfg *)0x40013800)

#define BITS_PER_EXTI 4

#define RCC_APB2_SYSCFG_EN (1 << 14)

void enable_exti(gpio_port port, uint32_t channel, uint32_t edge)
{
    uint32_t exti_bit = 1 << channel;
    uint32_t config_shift = (channel%4) * BITS_PER_EXTI;  //find the start of the 4-bit field
    uint32_t config_mask = 0xF << config_shift; //mask the 4 bits

    RCC_BASE->apb2_enr |= RCC_APB2_SYSCFG_EN; //system clock config
    SYSCFG_BASE->exti[channel/4] = //channels 0-3 use 0, 4-7 use 1, and so on
        (SYSCFG_BASE->exti[channel/4] & ~config_mask) |
        ((uint32_t)port << config_shift); //write port value

    EXTI_BASE->rtsr = (EXTI_BASE->rtsr & ~exti_bit) | //trigger on rising edge
                      ((edge & RISING_EDGE) ? exti_bit : 0);
    EXTI_BASE->ftsr = (EXTI_BASE->ftsr & ~exti_bit) | //trigger falling edge
                      ((edge & FALLING_EDGE) ? exti_bit : 0);
    EXTI_BASE->imr |= exti_bit; //generate interrupts

    nvic_irq(EXTI_IRQN[channel], IRQ_ENABLE);
}

void disable_exti(uint32_t channel)
{
    EXTI_BASE->imr &= ~(1 << channel);
}

void exti_clear_pending_bit(uint32_t channel){
    EXTI_BASE->pr = 1 << channel; //clear bit by programming it to one
}

/* EXTI line numbers match the GPIO pin numbers. */
void EXTI1_IRQHandler(void)
{
    static uint32_t led_on;
    if (EXTI_BASE->pr & (1 << BUTTON_PIN)) {
        exti_clear_pending_bit(BUTTON_PIN);
        led_on = !led_on;
        if (led_on)
            gpio_set(LED_RED_PORT, LED_RED_PIN);
        else
            gpio_clr(LED_RED_PORT, LED_RED_PIN);
    }
}

void EXTI4_IRQHandler(void)
{
    if (EXTI_BASE->pr & (1 << LEFT_ENCODER_A_PIN)) {
        exti_clear_pending_bit(LEFT_ENCODER_A_PIN);
        encoder_irq_handler_left();
    }
}

void EXTI9_5_IRQHandler(void)
{
    uint32_t pending = EXTI_BASE->pr;

    if (pending & (1 << RIGHT_ENCODER_A_PIN)) { //distinguish with pending bits
        exti_clear_pending_bit(RIGHT_ENCODER_A_PIN);
        encoder_irq_handler_right();
    }
    if (pending & (1 << LEFT_ENCODER_B_PIN)) {
        exti_clear_pending_bit(LEFT_ENCODER_B_PIN);
        encoder_irq_handler_left();
    }
}

void EXTI15_10_IRQHandler(void)
{
    if (EXTI_BASE->pr & (1 << RIGHT_ENCODER_B_PIN)) {
        exti_clear_pending_bit(RIGHT_ENCODER_B_PIN);
        encoder_irq_handler_right();
    }
}

void exti_testbutton(){
    gpio_init(BUTTON_PORT, BUTTON_PIN, MODE_INPUT, OUTPUT_PUSH_PULL,
              OUTPUT_SPEED_LOW, PUPD_PULL_UP, ALT0);
    gpio_init(LED_RED_PORT, LED_RED_PIN, MODE_GP_OUTPUT, OUTPUT_PUSH_PULL,
              OUTPUT_SPEED_LOW, PUPD_NONE, ALT0);
    gpio_clr(LED_RED_PORT, LED_RED_PIN);
    enable_exti(BUTTON_PORT, BUTTON_PIN, FALLING_EDGE);
}

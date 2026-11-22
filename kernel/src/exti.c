#include <exti.h>
#include <gpio.h>
#include <printk.h>
#include <rcc.h>

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

#define EXTI_BASE (struct exti *)0x40013C00
#define SYSCFG_BASE (struct syscfg *)0x40013800

#define BITS_PER_EXTI 4

#define RCC_APB2_SYSCFG_EN (1 << 14)

void enable_exti(gpio_port port, uint32_t channel, uint32_t edge)
{
    (void) port;
    (void) channel;
    (void) edge;
    return;
}

void disable_exti(uint32_t channel)
{
    (void) channel;
    return;
}

void exti_clear_pending_bit(uint32_t channel)
{
    (void) channel;
    return;
}

// Note: Add your IRQ handlers here

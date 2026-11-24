#include "stm32f4xx.h"

static uint8_t button_down = 0U;
static uint32_t consecutive_samples = 0U;
static uint32_t blink_ticks = 0U;

int main(void)
{
    RCC->AHB1ENR |= (1U << 0);     
    RCC->AHB1ENR |= (1U << 2);     
    RCC->APB1ENR |= (1U << 0);     

    GPIOA->MODER &= ~(3U << 0);
    GPIOA->PUPDR = (GPIOA->PUPDR & ~(3U << 0)) | (1U << 0);

    GPIOC->BSRR = 1U << 13;
    GPIOC->OTYPER &= ~(1U << 13);
    GPIOC->OSPEEDR &= ~(3U << 26);
    GPIOC->PUPDR &= ~(3U << 26);
    GPIOC->MODER = (GPIOC->MODER & ~(3U << 26)) | (1U << 26);

    TIM2->CR1 = TIM_CR1_URS;
    TIM2->PSC = 1599U;
    TIM2->ARR = 9U;
    TIM2->EGR = TIM_EGR_UG;
    TIM2->SR = 0U;
    TIM2->DIER = TIM_DIER_UIE;

    NVIC->ISER[0] = 1U << TIM2_IRQn;
    TIM2->CR1 |= TIM_CR1_CEN;

    while (1) {
    }
}

void TIM2_IRQHandler(void)
{
    if ((TIM2->SR & TIM_SR_UIF) == 0U) {
        return;
    }

    TIM2->SR = 0U;

    uint8_t is_pressed = (GPIOA->IDR & (1U << 0)) == 0U;
    if (is_pressed == button_down) {
        consecutive_samples = 0U;
    } else if (++consecutive_samples == 21U) {
        button_down = is_pressed;
        consecutive_samples = 0U;
        blink_ticks = 0U;
        GPIOC->BSRR = button_down ? (1U << 29) : (1U << 13);
        return;
    }

    if (button_down && ++blink_ticks == 250U) {
        blink_ticks = 0U;
        GPIOC->ODR ^= 1U << 13;
    }
}

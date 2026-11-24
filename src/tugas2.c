#include "stm32f4xx.h"

volatile uint8_t sem_debounce = 0U;
volatile uint8_t pressed = 0U;
volatile uint32_t press_count = 0U;
static uint8_t button_down = 0U;

int main(void)
{
    RCC->AHB1ENR |= (1U << 0);     
    RCC->AHB1ENR |= (1U << 2);     
    RCC->APB1ENR |= (1U << 1);     
    RCC->APB2ENR |= (1U << 14);    

    GPIOA->MODER &= ~(3U << 0);
    GPIOA->PUPDR = (GPIOA->PUPDR & ~(3U << 0)) | (1U << 0);

    GPIOC->BSRR = 1U << 13;
    GPIOC->OTYPER &= ~(1U << 13);
    GPIOC->OSPEEDR &= ~(3U << 26);
    GPIOC->PUPDR &= ~(3U << 26);
    GPIOC->MODER = (GPIOC->MODER & ~(3U << 26)) | (1U << 26);

    GPIOA->BSRR = 1U << 23;
    GPIOA->OTYPER &= ~(1U << 7);
    GPIOA->OSPEEDR &= ~(3U << 14);
    GPIOA->PUPDR &= ~(3U << 14);
    GPIOA->MODER = (GPIOA->MODER & ~(3U << 14)) | (1U << 14);

    TIM3->CR1 = TIM_CR1_OPM | TIM_CR1_URS;
    TIM3->PSC = 15999U;
    TIM3->ARR = 19U;
    TIM3->EGR = TIM_EGR_UG;
    TIM3->SR = 0U;
    TIM3->DIER = TIM_DIER_UIE;

    EXTI->IMR &= ~(1U << 0);
    SYSCFG->EXTICR[0] &= ~0xFU;
    EXTI->EMR &= ~(1U << 0);
    EXTI->FTSR |= 1U << 0;
    EXTI->RTSR |= 1U << 0;
    EXTI->PR = 1U << 0;

    NVIC->ISER[0] = (1U << EXTI0_IRQn) | (1U << TIM3_IRQn);
    EXTI->IMR |= 1U << 0;

    while (1) {
        if (pressed != 0U) {
            pressed = 0U;
            press_count++;
            GPIOC->ODR ^= 1U << 13;
        }
    }
}

void EXTI0_IRQHandler(void)
{
    if ((EXTI->PR & (1U << 0)) == 0U || sem_debounce != 0U) {
        return;
    }

    EXTI->IMR &= ~(1U << 0);
    EXTI->PR = 1U << 0;
    sem_debounce = 1U;
    GPIOA->BSRR = 1U << 7;

    TIM3->CNT = 0U;
    TIM3->CR1 |= TIM_CR1_CEN;
}

void TIM3_IRQHandler(void)
{
    if ((TIM3->SR & TIM_SR_UIF) == 0U) {
        return;
    }

    TIM3->SR = 0U;
    EXTI->PR = 1U << 0;

    uint8_t is_pressed = (GPIOA->IDR & (1U << 0)) == 0U;
    if (is_pressed && !button_down) {
        pressed = 1U;
    }
    button_down = is_pressed;

    GPIOA->BSRR = 1U << 23;
    sem_debounce = 0U;
    EXTI->IMR |= 1U << 0;
}

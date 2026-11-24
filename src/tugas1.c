#include "stm32f4xx.h"

int main(void)
{
    RCC->AHB1ENR |= (1U << 0);     /* Bit 0: GPIOAEN */
    RCC->AHB1ENR |= (1U << 2);     /* Bit 2: GPIOCEN */
    RCC->APB1ENR |= (1U << 0);     /* Bit 0: TIM2EN */
    RCC->APB1ENR |= (1U << 1);     /* Bit 1: TIM3EN */
    RCC->APB1ENR |= (1U << 2);     /* Bit 2: TIM4EN */

    GPIOC->BSRR = 1U << 13;
    GPIOC->OTYPER &= ~(1U << 13);
    GPIOC->OSPEEDR &= ~(3U << 26);
    GPIOC->PUPDR &= ~(3U << 26);
    GPIOC->MODER = (GPIOC->MODER & ~(3U << 26)) | (1U << 26);

    GPIOA->BSRR = (1U << 23) | (1U << 24);
    GPIOA->OTYPER &= ~((1U << 7) | (1U << 8));
    GPIOA->OSPEEDR &= ~((3U << 14) | (3U << 16));
    GPIOA->PUPDR &= ~((3U << 14) | (3U << 16));
    GPIOA->MODER = (GPIOA->MODER & ~((3U << 14) | (3U << 16)))
                 | (1U << 14) | (1U << 16);

    TIM2->CR1 = TIM_CR1_URS;
    TIM2->PSC = 15999U;
    TIM2->ARR = 499U;
    TIM2->EGR = TIM_EGR_UG;
    TIM2->SR = 0U;
    TIM2->DIER = TIM_DIER_UIE;

    TIM3->CR1 = TIM_CR1_URS;
    TIM3->PSC = 1599U;
    TIM3->ARR = 2499U;
    TIM3->EGR = TIM_EGR_UG;
    TIM3->SR = 0U;
    TIM3->DIER = TIM_DIER_UIE;

    TIM4->CR1 = TIM_CR1_URS;
    TIM4->PSC = 159U;
    TIM4->ARR = 9999U;
    TIM4->EGR = TIM_EGR_UG;
    TIM4->SR = 0U;
    TIM4->DIER = TIM_DIER_UIE;

    NVIC->ISER[0] = (1U << TIM2_IRQn) | (1U << TIM3_IRQn) | (1U << TIM4_IRQn);

    TIM2->CR1 |= TIM_CR1_CEN;
    TIM3->CR1 |= TIM_CR1_CEN;
    TIM4->CR1 |= TIM_CR1_CEN;

    while (1) {
    }
}

void TIM2_IRQHandler(void)
{
    if (TIM2->SR & TIM_SR_UIF) {
        TIM2->SR = 0U;
        GPIOC->ODR ^= 1U << 13;
    }
}

void TIM3_IRQHandler(void)
{
    if (TIM3->SR & TIM_SR_UIF) {
        TIM3->SR = 0U;
        GPIOA->ODR ^= 1U << 7;
    }
}

void TIM4_IRQHandler(void)
{
    if (TIM4->SR & TIM_SR_UIF) {
        TIM4->SR = 0U;
        GPIOA->ODR ^= 1U << 8;
    }
}

#include "stm32f4xx.h"
#include <stdint.h>
#include "delay.h"  // delay_ms() declared here

// === LED ===
void LED_Init(void) {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
    GPIOD->MODER |= (1 << (12 * 2)) | (1 << (14 * 2));  // PD12, PD14 = output

    GPIOD->BSRR = (GPIO_BSRR_BS_12 << 16); // Green OFF
    GPIOD->BSRR = (GPIO_BSRR_BS_14 << 16); // Red OFF
}

void SetRedLED(void) {
    GPIOD->BSRR = GPIO_BSRR_BS_14;
    GPIOD->BSRR = (GPIO_BSRR_BS_12 << 16);
}

void SetGreenLED(void) {
    GPIOD->BSRR = GPIO_BSRR_BS_12;
    GPIOD->BSRR = (GPIO_BSRR_BS_14 << 16);
}

// === Buzzer ===
void Buzzer_Init(void) {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    GPIOB->MODER |= (1 << (0 * 2)); // PB0 = output
}

void Buzzer_On(void) {
    GPIOB->BSRR = GPIO_BSRR_BS_0;
}

void Buzzer_Off(void) {
    GPIOB->BSRR = (GPIO_BSRR_BS_0 << 16);
}

// === USART2 Init (PA3 = RX) ===
void USART2_Init(void) {
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

    GPIOA->MODER |= (2 << (3 * 2));             // PA3 = AF
    GPIOA->AFR[0] |= (7 << (3 * 4));            // AF7 = USART2_RX

    USART2->BRR = 0x1117;  // 9600 @ 16MHz
    USART2->CR1 = USART_CR1_RE | USART_CR1_UE; // Enable RX
}

uint8_t USART2_ReadChar(void) {
    while (!(USART2->SR & USART_SR_RXNE));
    return USART2->DR;
}

// === RFID Read Buffer ===
#define RFID_BUFFER_SIZE 12
char rfid_data[RFID_BUFFER_SIZE];
uint8_t rfid_index = 0;

int main(void) {
    LED_Init();
    Buzzer_Init();
    USART2_Init();
    delay_ms(100);

    SetRedLED();
    Buzzer_Off();

    while (1) {
        if (USART2->SR & USART_SR_RXNE) {
            char c = USART2_ReadChar();

            if (c == '\r') {
                rfid_data[rfid_index] = '\0';  // Null-terminate
                rfid_index = 0;

                // Trigger LED + Buzzer
                SetGreenLED();
                Buzzer_On();
                delay_ms(1000); // Hold for 1 second
                SetRedLED();
                Buzzer_Off();
            } else {
                if (rfid_index < RFID_BUFFER_SIZE - 1) {
                    rfid_data[rfid_index++] = c;
                }
            }
        }
    }
}

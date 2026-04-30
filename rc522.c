#include "rc522.h"
#include "spi.h"
#include "delay.h"
#include "stm32f4xx.h"

// Use updated BSRR logic (CMSIS v2+)
#define RC522_CS_LOW()    GPIOA->BSRR = (GPIO_BSRR_BS_4 << 16)
#define RC522_CS_HIGH()   GPIOA->BSRR = GPIO_BSRR_BS_4

#define RC522_RST_LOW()   GPIOA->BSRR = (GPIO_BSRR_BS_2 << 16)
#define RC522_RST_HIGH()  GPIOA->BSRR = GPIO_BSRR_BS_2

void RC522_Write(uint8_t addr, uint8_t val) {
    RC522_CS_LOW();
    SPI1_Transfer((addr << 1) & 0x7E);
    SPI1_Transfer(val);
    RC522_CS_HIGH();
}

uint8_t RC522_Read(uint8_t addr) {
    uint8_t val;
    RC522_CS_LOW();
    SPI1_Transfer(((addr << 1) & 0x7E) | 0x80);
    val = SPI1_Transfer(0x00);
    RC522_CS_HIGH();
    return val;
}

void RC522_SetBitMask(uint8_t reg, uint8_t mask) {
    uint8_t tmp = RC522_Read(reg);
    RC522_Write(reg, tmp | mask);
}

void RC522_ClearBitMask(uint8_t reg, uint8_t mask) {
    uint8_t tmp = RC522_Read(reg);
    RC522_Write(reg, tmp & (~mask));
}

void RC522_AntennaOn(void) {
    uint8_t temp = RC522_Read(TxControlReg);
    if (!(temp & 0x03)) {
        RC522_SetBitMask(TxControlReg, 0x03);
    }
}

void RC522_Init(void) {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

    GPIOA->MODER |= (1 << (2 * 2)) | (1 << (4 * 2));     // PA2, PA4 as output
    GPIOA->OSPEEDR |= (3 << (2 * 2)) | (3 << (4 * 2));   // High speed

    RC522_RST_HIGH();
    RC522_CS_HIGH();
    delay_ms(50);

    RC522_Write(CommandReg, PCD_RESETPHASE);
    delay_ms(50);

    RC522_Write(TModeReg, 0x8D);
    RC522_Write(TPrescalerReg, 0x3E);
    RC522_Write(TReloadRegL, 30);
    RC522_Write(TReloadRegH, 0);
    RC522_Write(TxControlReg, 0x83);

    RC522_AntennaOn();
}

uint8_t RC522_Request(uint8_t reqMode, uint8_t *TagType) {
    uint8_t status = MI_ERR;
    uint8_t irqReg, i;

    RC522_Write(BitFramingReg, 0x07); // 7 bits = 1 byte
    RC522_ClearBitMask(Status2Reg, 0x08);
    RC522_Write(CommandReg, PCD_IDLE);
    RC522_Write(FIFOLevelReg, 0x80);
    RC522_Write(FIFODataReg, reqMode);
    RC522_Write(CommandReg, PCD_TRANSCEIVE);
    RC522_SetBitMask(BitFramingReg, 0x80); // StartSend=1

    // Wait for response
    i = 200;
    do {
        irqReg = RC522_Read(CommIrqReg);
        i--;
    } while (!(irqReg & 0x30) && i != 0);

    RC522_ClearBitMask(BitFramingReg, 0x80);

    if (i != 0) {
        if (!(RC522_Read(ErrorReg) & 0x1B)) {
            TagType[0] = RC522_Read(FIFODataReg);
            TagType[1] = RC522_Read(FIFODataReg + 1);
            status = MI_OK;
        }
    }

    return status;
}

uint8_t RC522_Anticoll(uint8_t *serNum) {
    uint8_t status = MI_ERR;
    uint8_t irqReg, i;

    RC522_Write(BitFramingReg, 0x00);
    RC522_Write(CommandReg, PCD_IDLE);
    RC522_Write(FIFOLevelReg, 0x80);
    RC522_Write(FIFODataReg, 0x93);
    RC522_Write(FIFODataReg, 0x20);
    RC522_Write(CommandReg, PCD_TRANSCEIVE);
    RC522_SetBitMask(BitFramingReg, 0x80);

    i = 200;
    do {
        irqReg = RC522_Read(CommIrqReg);
        i--;
    } while (!(irqReg & 0x30) && i != 0);

    RC522_ClearBitMask(BitFramingReg, 0x80);

    if (i != 0) {
        if (!(RC522_Read(ErrorReg) & 0x1B)) {
            for (uint8_t j = 0; j < 5; j++) {
                serNum[j] = RC522_Read(FIFODataReg + j);
            }
            status = MI_OK;
        }
    }

    return status;
}

void RC522_Halt(void) {
    RC522_Write(FIFOLevelReg, 0x80);
    RC522_Write(FIFODataReg, 0x50);
    RC522_Write(FIFODataReg, 0x00);
    RC522_Write(CommandReg, PCD_TRANSCEIVE);
}

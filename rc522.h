#ifndef __RC522_H
#define __RC522_H

#include "stm32f4xx.h"

// RC522 Commands
#define PCD_IDLE              0x00
#define PCD_AUTHENT           0x0E
#define PCD_RECEIVE           0x08
#define PCD_TRANSMIT          0x04
#define PCD_TRANSCEIVE        0x0C
#define PCD_RESETPHASE        0x0F
#define PCD_CALCCRC           0x03

// RC522 Registers
#define CommandReg            0x01
#define CommIEnReg            0x02
#define DivIEnReg             0x03
#define CommIrqReg            0x04
#define DivIrqReg             0x05
#define ErrorReg              0x06
#define Status1Reg            0x07
#define Status2Reg            0x08
#define FIFODataReg           0x09
#include <stdint.h>

#define FIFOLevelReg          0x0A
#define ControlReg            0x0C
#define BitFramingReg         0x0D
#define ModeReg               0x11
#define TxControlReg          0x14
#define TModeReg              0x2A
#define TPrescalerReg         0x2B
#define TReloadRegL           0x2D
#define TReloadRegH           0x2C

// Card Types
#define MI_OK                 0
#define MI_NOTAGERR           1
#define MI_ERR                2

void RC522_Init(void);
uint8_t RC522_Request(uint8_t reqMode, uint8_t *TagType);
uint8_t RC522_Anticoll(uint8_t *serNum);
void RC522_Halt(void);

#endif

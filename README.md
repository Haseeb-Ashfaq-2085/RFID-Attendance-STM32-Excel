# RFID Attendance System (STM32)

## Overview
An automated attendance solution built on the STM32 platform. The system reads unique RFID tags, displays user information on an I2C LCD, and transmits data to a PC for automated Excel logging.

## Key Features
* **RFID Authentication:** High-speed tag detection using the MFRC522 module via SPI.
* **I2C LCD Display:** Real-time feedback for "Access Granted" or "Unknown Tag" messages using an I2C expander.
* **Excel Integration:** Uses UART/Serial communication to push attendance logs directly to a CSV/Excel file on a host PC.
* **Embedded Logic:** Developed using HAL (Hardware Abstraction Layer) for efficient peripheral management.

## Hardware Components
* **Microcontroller:** STM32F103C8T6 (Blue Pill)
* **Reader:** MFRC522 RFID Module
* **Display:** 16x2 LCD with I2C PCF8574 Module
* **Communication:** USB-to-TTL (UART) for PC interface

## Tech Stack
* **Language:** C (STM32CubeIDE)
* **Communication Protocols:** SPI (RFID), I2C (LCD), UART (PC)
* **Scripts:** Python/VBA (for Excel data handling)

/*
 Enc28J60NetworkClass.h
 UIPEthernet network driver for Microchip ENC28J60 Ethernet Interface.

 Copyright (c) 2013 Norbert Truchsess <norbert.truchsess@t-online.de>
 All rights reserved.

 inspired by enc28j60.c file from the AVRlib library by Pascal Stang.
 For AVRlib See http://www.procyonengineering.com/

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef Enc28J60Network_H_
#define Enc28J60Network_H_

#include "mempool.h"

#if !defined(ENC28J60_CONTROL_CS)
   #if defined(__AVR__) || defined(ESP8266)
      // Arduino Uno (__AVR__) SS defined to pin 10
      // Arduino Mega(__AVR_ATmega2560__) SS defined to pin 53
      // ESP8266 (ESP8266) SS defined to pin 15
      #define ENC28J60_CONTROL_CS     SS
   #elif defined(ARDUINO_ARCH_SAM)
      // Arduino Due (ARDUINO_ARCH_SAM) BOARD_SPI_DEFAULT_SS (SS3) defined to pin 78
      #define ENC28J60_CONTROL_CS     BOARD_SPI_DEFAULT_SS
   #elif defined(STM32_MCU_SERIES) || defined(__STM32F1__) || defined(__STM32F3__) || defined(__STM32F4__)
      #ifdef ARDUINO_STM32F4_NETDUINO2PLUS
         #define ENC28J60_CONTROL_CS     PC8
      #else
         #define ENC28J60_CONTROL_CS     SPI.nssPin()
         //#define ENC28J60_CONTROL_CS     PA4
      #endif
   #endif
   #if defined(ENC28J60_CONTROL_CS)
      #pragma message "Not defined ENC28J60_CONTROL_CS. Use borad default SS pin setting. You can configure in 'utility/Enc28J60Network.h'."
   #endif
#endif
#if !defined(ENC28J60_CONTROL_CS)
   #error "Not defined ENC28J60_CONTROL_CS!"
#endif

#if !defined(SPI_MOSI)
   #if defined(__AVR__) || defined(ESP8266)
      #define SPI_MOSI MOSI
   #elif defined(ARDUINO_ARCH_SAM)
      #define SPI_MOSI PIN_SPI_MOSI
   #elif defined(__STM32F1__) || defined(__STM32F3__) || defined(__STM32F4__)
      #define SPI_MOSI SPI.mosiPin()
   #endif
#endif
#if !defined(SPI_MOSI)
   #error "Not defined SPI_MOSI!"
#endif

#if !defined(SPI_MISO)
   #if defined(__AVR__) || defined(ESP8266)
      #define SPI_MISO MISO
   #elif defined(ARDUINO_ARCH_SAM)
      #define SPI_MISO PIN_SPI_MISO
   #elif defined(__STM32F1__) || defined(__STM32F3__) || defined(__STM32F4__)
      #define SPI_MISO SPI.misoPin()
   #endif
#endif
#if !defined(SPI_MISO)
   #error "Not defined SPI_MISO!"
#endif
#if !defined(SPI_SCK)
   #if defined(__AVR__) || defined(ESP8266)
      #define SPI_SCK SCK
   #elif defined(ARDUINO_ARCH_SAM)
      #define SPI_SCK PIN_SPI_SCK
   #elif defined(__STM32F1__) || defined(__STM32F3__) || defined(__STM32F4__)
      #define SPI_SCK SPI.sckPin()
   #endif
#endif
#if !defined(SPI_SCK)
   #error "Not defined SPI_SCK!"
#endif

#if defined(ARDUINO_ARCH_SAM) || defined(__STM32F1__) || defined(__STM32F3__) || defined(__STM32F4__) || defined(ESP8266)
   #include <SPI.h>
   #define ENC28J60_USE_SPILIB 1
#endif

#define UIP_RECEIVEBUFFERHANDLE 0xff

/*
 * Empfangen von ip-header, arp etc...
 * wenn tcp/udp -> tcp/udp-callback -> assign new packet to connection
 */

class Enc28J60Network : public MemoryPool
{

private:
  static uint16_t nextPacketPtr;
  static uint8_t bank;
  static uint8_t erevid;

  static struct memblock receivePkt;

  static bool broadcast_enabled; //!< True if broadcasts enabled (used to allow temporary disable of broadcast for DHCP or other internal functions)

  static uint8_t readOp(uint8_t op, uint8_t address);
  static void writeOp(uint8_t op, uint8_t address, uint8_t data);
  static uint16_t setReadPtr(memhandle handle, memaddress position, uint16_t len);
  static void setERXRDPT();
  static void readBuffer(uint16_t len, uint8_t* data);
  static void writeBuffer(uint16_t len, uint8_t* data);
  static uint8_t readByte(uint16_t addr);
  static void writeByte(uint16_t addr, uint8_t data);
  static void setBank(uint8_t address);
  static uint8_t readReg(uint8_t address);
  static void writeReg(uint8_t address, uint8_t data);
  static void writeRegPair(uint8_t address, uint16_t data);
  static void phyWrite(uint8_t address, uint16_t data);
  static uint16_t phyRead(uint8_t address);
  static void clkout(uint8_t clk);

  static void enableBroadcast (bool temporary);
  static void disableBroadcast (bool temporary);
  static void enableMulticast ();
  static void disableMulticast ();

  static uint8_t readRegByte (uint8_t address);
  static void writeRegByte (uint8_t address, uint8_t data);

  friend void enc28J60_mempool_block_move_callback(memaddress,memaddress,memaddress);

public:

  void powerOn();
  void powerOff();
  static uint8_t geterevid(void);
  uint16_t PhyStatus(void);
  static bool linkStatus();

  static void init(uint8_t* macaddr);
  static memhandle receivePacket();
  static void freePacket();
  static memaddress blockSize(memhandle handle);
  static void sendPacket(memhandle handle);
  static uint16_t readPacket(memhandle handle, memaddress position, uint8_t* buffer, uint16_t len);
  static uint16_t writePacket(memhandle handle, memaddress position, uint8_t* buffer, uint16_t len);
  static void copyPacket(memhandle dest, memaddress dest_pos, memhandle src, memaddress src_pos, uint16_t len);
  static uint16_t chksum(uint16_t sum, memhandle handle, memaddress pos, uint16_t len);
};

extern Enc28J60Network Enc28J60;
#endif /* Enc28J60NetworkClass_H_ */

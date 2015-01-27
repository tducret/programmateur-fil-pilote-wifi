// **********************************************************************************
// ULPNode RF protocol payloads used for transmission
// **********************************************************************************
// Creative Commons Attrib Share-Alike License
// You are free to use/extend this library but please abide with the CC-BY-SA license:
// http://creativecommons.org/licenses/by-sa/4.0/
//
// For any explanation of ULPNode hardware at
// http://hallard.me/
//
// Written by Charles-Henri Hallard (http://hallard.me)
//
// History : V1.00 2014-07-14 - First release
//
// All text above must be included in any redistribution.
//
// **********************************************************************************
#ifndef ULPN_RF_PROTOCOL_H
#define ULPN_RF_PROTOCOL_H

#ifdef SPARK
#include <application.h>        // Spark Code main include file
#else
#include <Arduino.h>            //assumes Arduino IDE v1.0 or greater
#endif

// Payload command code
#define RF_PL_START      0
#define RF_PL_ALIVE      1
#define RF_PL_TEMP       2
#define RF_PL_TEMP_HUM   3
#define RF_PL_TEMP_LUX   4
#define RF_PL_TEMP_CO2   5
#define RF_PL_TH02       6
#define RF_PL_PING       7
#define RF_PL_PINGBACK   8
#define RF_PL_END        9

#ifdef SPARK
// This will force structure to 1 byte alignment
#pragma pack(push)  // push current alignment to stack
#pragma pack(1)     // set alignment to 1 byte boundary
#endif

// Ping Payload sent by RF
typedef struct
{
  uint8_t       command;    /* Command code         */
  uint16_t  vbat;           /* Battery voltage (in mV ex 1500 for 1.5V) */
  int8_t    rssi;     /* RSSI */
} RFPingPayload;

// Alive Payload sent by RF
typedef struct
{
  uint8_t   command;  /* Command code     */
  uint16_t  vbat;     /* Battery voltage (in mV ex 1500 for 1.5V) */
} RFAlivePayload;

// Payload sent by RF
typedef struct
{
  uint8_t       command;    /* Command code         */
  int16_t     sensor1;  /* Sensor 1 Value       */
  int16_t     sensor2;  /* Sensor 2 Value       */
  uint16_t  vbat;           /* Battery voltage (in mV ex 1500 for 1.5V) */
} RFPayload;

#ifdef SPARK
// restore original alignment from stack
#pragma pack(pop)
#endif

#ifdef SPARK
extern const char * rf_frame[];
#else
extern const char * const rf_frame[];
#endif

void decode_frame_type(uint8_t);
void decode_bat(uint16_t);
void decode_temp(int16_t);
void decode_hum(int16_t);
void decode_lux(int16_t);
void decode_co2(int16_t);
uint8_t decode_received_data(uint8_t len, uint8_t c, void * ppayload);


#endif

// **********************************************************************************
// ULPNode RF protocol payloads used for transmission
// **********************************************************************************
// Creative Commons Attrib Share-Alike License
// You are free to use/extend this library but please abide with the CC-BY-SA license:
// http://creativecommons.org/licenses/by-sa/4.0/
//
// Written by Charles-Henri Hallard (http://hallard.me)
//
// History : V1.00 2015-01-22 - First release
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
#include "ULPNode.h"
#endif


// Payload command code
#define RF_PL_SYSTEM_START  0x00 // first value for system payload type
#define RF_PL_ALIVE      		0x01
#define RF_PL_PING       		0x02
#define RF_PL_PINGBACK   		0x03
#define RF_PL_OTA_CONFIG 		0x04 // OTA Node Configuration
#define RF_PL_OTA_UPDATE 		0x05 // OTA Node Firmware update
#define RF_PL_OTA_AUTOID 		0x06 // OTA Node Auto ID request
#define RF_PL_SYSTEM_END 		0x07 // Lasted value for system payload

#define RF_PL_SENSOR_START	0x20	// first value for sensor payload type
#define RF_PL_TEMP       		0x21
#define RF_PL_TEMP_HUM   		0x22
#define RF_PL_TEMP_LUX   		0x23
#define RF_PL_TEMP_CO2   		0x24
#define RF_PL_TH02       		0x25
#define RF_PL_TEMP_TEMP  		0x26
#define RF_PL_SENSOR_END		0x27	// last value for sensor payload type

// Macro to determine payload type
#define isPayloadCmdSystem(c)	(c>RF_PL_SYSTEM_START && c<RF_PL_SYSTEM_END)
#define isPayloadCmdSensor(c)	(c>RF_PL_SENSOR_START && c<RF_PL_SENSOR_END)
#define isPayloadCmdValid(c)	(isPayloadCmdSystem(c) || isPayloadCmdSensor(c))

#define RF_ANSWER_TIMEOUT  100  // number of ms to receive ACK/Response
#define RF_ANSWER_ERROR    999  // no ack/answer received return value

#ifdef SPARK
// This will force structure to 1 byte alignment
#pragma pack(push)  // push current alignment to stack
#pragma pack(1)     // set alignment to 1 byte boundary
#define sprintf_P(s, f, ...) sprintf((s), (f), __VA_ARGS__)
#define strstr_P(a, b) strstr((a), (b))
#define PSTR(s) s
#define Debug(args...)    Serial.print(args)
#define Debugln(args...)  Serial.println(args)
#else
// Avoid GCC warning "only initialized variables can be placed into program memory area"
#ifdef PROGMEM
//#undef PROGMEM
//#define PROGMEM __attribute__((section(".progmem.data")))
#endif
#endif

// Ping Payload sent by RF
typedef struct
{
  uint8_t   command;  /* Command code         */
  uint16_t  vbat;     /* Battery voltage (in mV ex 1500 for 1.5V) */
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
  uint8_t     command;    /* Command code         */
  int16_t     sensor1;  /* Sensor 1 Value       */
  int16_t     sensor2;  /* Sensor 2 Value       */
  uint16_t  vbat;           /* Battery voltage (in mV ex 1500 for 1.5V) */
} RFPayload;

// Dummy payload, used to be able to handle any possible payload
// with pointer, and allocating the max payload size, take care to
// increase this payload size if you add some payload that can contains
// more than 16 bytes on above definition
typedef struct
{
  uint8_t command;   /* Command code is ALWAYS 1st byte */
  uint8_t dummy[15];
} RFVoidPayload;


#ifdef SPARK
// restore original alignment from stack
#pragma pack(pop)
#endif

#ifdef SPARK
extern const char * rf_frame[];
#else
extern const char * const rf_frame[];
#endif

char * decode_frame_type(uint8_t);
char * decode_bat(uint16_t);
char * decode_temp(int16_t);
char * decode_hum(int16_t);
char * decode_lux(int16_t);
char * decode_co2(int16_t);
char * decode_rssi(int8_t);
uint8_t decode_received_data(uint8_t len, uint8_t c, void * ppayload);

#endif

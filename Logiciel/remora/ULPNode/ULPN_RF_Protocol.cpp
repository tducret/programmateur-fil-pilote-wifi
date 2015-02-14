// **********************************************************************************
// ULPNode RF protocol paylaods used for transmission
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


#ifdef SPARK
#include <application.h>        // Spark Code main include file
#include "ULPN_RF_Protocol.h"
#else
#include <Arduino.h>            //assumes Arduino IDE v1.0 or greater
#include "ULPNode.h"
#include "ULPN_RF_Protocol.h"
#endif


#ifdef SPARK
const char * rf_frame[] = {
#else
const char * const rf_frame[] PROGMEM = {
#endif
    "UNKNOWN", "ALIVE", "PING", "PINGBACK", "OTA_CONFIG", "OTA_UPDATE", "OTA_AUTOID",
    "SYS_END", "","","","","","","","","","","","","","","","","","","","","","","",""
    "SENS_START", "TEMP", "TEMP_HUM", "TEMP_LUX", "TEMP_CO2", "TH02", "TEMP_TEMP", "SENS_END"
  };

// Payload command code
#define RF_PL_SYSTEM_START  0x00 // first value for system payload type
#define RF_PL_ALIVE         0x01
#define RF_PL_PING          0x02
#define RF_PL_PINGBACK      0x03
#define RF_PL_OTA_CONFIG    0x04 // OTA Node Configuration
#define RF_PL_OTA_UPDATE    0x05 // OTA Node Firmware update
#define RF_PL_OTA_AUTOID    0x06 // OTA Node Auto ID request
#define RF_PL_SYSTEM_END    0x07 // Lasted value for system payload

#define RF_PL_SENSOR_START  0x20  // first value for sensor payload type
#define RF_PL_TEMP          0x21
#define RF_PL_TEMP_HUM      0x22
#define RF_PL_TEMP_LUX      0x23
#define RF_PL_TEMP_CO2      0x24
#define RF_PL_TH02          0x25
#define RF_PL_TEMP_TEMP     0x26
#define RF_PL_SENSOR_END    0x27  // last value for sensor payload type

// Buffer used to decode data
char pbuf[24];

/* ======================================================================
Function: decode_frame_type
Purpose : print the frame type
Input   : type
Output  : -
Comments: -
====================================================================== */
char * decode_frame_type(uint8_t type)
{
  //DebugF(" DFT Type=");
  //Debug(type,DEC);
  //DebugF(" ");
  //DebugFlush();

  // check command type is known
  if (!isPayloadCmdValid(type))
    type =0;

#ifdef SPARK
  strcpy(pbuf, rf_frame[type]);
#else
  strcpy(pbuf, (char*)pgm_read_word(&(rf_frame[type])));
#endif

//Debug(pbuff);
//DebugFlush();

  return pbuf;
}

/* ======================================================================
Function: decode_batt
Purpose : print the battery value
Input   : battery (mV)
====================================================================== */
char * decode_bat(uint16_t bat)
{
  // convert bat to V format x.yyy
#ifdef SPARK
  sprintf(pbuf, " Bat:%1.3fV", bat/1000.0f);
#else
  char str_buff[6];
  dtostrf(bat/1000.0f, 4, 3, str_buff);
  sprintf_P(pbuf, PSTR(" Bat:%sV"), str_buff);
#endif

//  Debug(" Bat:");
//  Debug(bat/1000.0f);
//  Debug("V");
//  Serial.print(F(" Bat:"));
//  Debug(bat/1000.0f);
//  Serial.print(F("V"));

  return pbuf;
}

/* ======================================================================
Function: decode_temp
Purpose : print the temperature value
Input   : temp (*100)
====================================================================== */
char * decode_temp(int16_t temp)
{
// convert bat to V format xx.yy
#ifdef SPARK
  sprintf(pbuf, " Bat:%2.2fC", temp/100.0f );
#else
  char str_buff[6];
  dtostrf(temp/100.0f, 4, 2, str_buff);
  sprintf_P(pbuf, PSTR(" Temp:%sC"), str_buff);
#endif

  //Debug(" Temp:");
  //Debug(temp/100.0f);
  //Debug("C");

  return pbuf;

}

/* ======================================================================
Function: decode_hum
Purpose : print the humidity
Input   : humidity
====================================================================== */
char * decode_hum(int16_t hum)
{
  sprintf_P(pbuf, PSTR(" Hum:%d%%"), hum);
  //Debug(" Hum:");
  //Debug(hum);
  //Debug("%");
  return pbuf;
}

/* ======================================================================
Function: decode_lux
Purpose : print the lux value
Input   : lux (*10)
====================================================================== */
char * decode_lux(int16_t lux)
{
// convert bat to V format x.yyy
#ifdef SPARK
  sprintf(pbuf, " Lux:%d", lux/10 );
#else
  char str_buff[8];
  dtostrf(lux/10.0f, 6, 1, str_buff);
  sprintf_P(pbuf, PSTR(" Lux:%s"), str_buff);
#endif

//Debug(" Lux:");
//Debug(lux/10.0f);

  return pbuf;
}
/* ======================================================================
Function: decode_co2
Purpose : print the co2 value
Input   : co2
====================================================================== */
char * decode_co2(int16_t co2)
{
  sprintf_P(pbuf, PSTR(" CO2:%dppm"), co2);
  //Debug(" CO2:");
  //Debug(co2);
  //Debug("ppm");
  return pbuf;
}

/* ======================================================================
Function: decode_rssi
Purpose : print the rssi value
Input   : rssi
====================================================================== */
char * decode_rssi(int8_t rssi)
{
  sprintf_P(pbuf, PSTR(" RSSI:%ddB"), rssi);
  return pbuf;
}

/* ======================================================================
Function: decode_received_data
Purpose : send to serial received data in human format
Input   : size of data
          command of frame received
          pointer to the data
Output  : command code validated by payload size type reveived
Comments: if we had a command and payload does not match
          code as been set to 0 to avoid check in next
====================================================================== */
uint8_t decode_received_data(uint8_t len, uint8_t c, void * ppayload)
{
  // Show packet type name
  Debug("#   ");
  Debug(decode_frame_type(c));

  // Alive packet ?
  if ( c==RF_PL_ALIVE && len==sizeof(RFAlivePayload))
  {
    RFAlivePayload * p = (RFAlivePayload *) ppayload;
    Debug(decode_bat(p->vbat));
  }

  // ping/ping back packet ?
  else if ( (c==RF_PL_PING || c==RF_PL_PINGBACK) && len==sizeof(RFPingPayload))
  {
    RFPingPayload * p = (RFPingPayload *) ppayload;
    Debug(decode_bat(p->vbat));

    // RSSI from other side is sent only in pingback response
    if (c==RF_PL_PINGBACK)
      Debug(decode_rssi(p->rssi));
  }

  // Classic payload Packet for majority of sensors ?
  else if ( len==sizeof(RFPayload))
  {
    RFPayload * p = (RFPayload *) ppayload;

    decode_bat(p->vbat);

    if ( c==RF_PL_TEMP || c==RF_PL_TEMP_HUM || c==RF_PL_TEMP_LUX || c==RF_PL_TEMP_CO2)
    {
      decode_temp(p->sensor1);

      if (c==RF_PL_TEMP_HUM) decode_hum(p->sensor2);
      if (c==RF_PL_TEMP_LUX) decode_lux(p->sensor2);
      if (c==RF_PL_TEMP_CO2) decode_co2(p->sensor2);
      if (c==RF_PL_TH02)     decode_hum(p->sensor2);
    }
  }

  // not known, raw display packet
  else
  {
    uint8_t * p = (uint8_t *) ppayload;
    while (--len)
    {
      Debug(*p++,HEX);
      Debug(" ");
    }

    // here we did not validated known packet, so clear command
    // code for the rest of the operation
    c=0;
  }

  return (c);
}

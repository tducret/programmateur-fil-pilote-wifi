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
#include "ULPN_RF_Protocol.h"

#ifdef SPARK
const char * rf_frame[] = {
#else
const char * const rf_frame[] PROGMEM = {
#endif
    "UNKNOWN", "ALIVE", "TEMP", "TEMP_HUM",
    "TEMP_LUX", "TEMP_CO2", "TH02", "PING", "PINGBACK"
  };



/* ======================================================================
Function: decode_frame_type
Purpose : print the frame type
Input   : type
Output  : -
Comments:
====================================================================== */
void decode_frame_type(uint8_t type)
{
  Serial.print(type);
  Serial.print(' ');

  // check command type is known
  if (type<=RF_PL_START || type>=RF_PL_END)
    type =0;

#ifdef SPARK
  Serial.print(rf_frame[type]);
#else
  Serial.print((char*)pgm_read_word(&(rf_frame[type])));
#endif
}

/* ======================================================================
Function: decode_batt
Purpose : print the battery value
Input   : battery (mV)
====================================================================== */
void decode_bat(uint16_t bat)
{
  Serial.print(F(" Bat:"));
  Serial.print(bat/1000.0f);
  Serial.print(F("V"));
}

/* ======================================================================
Function: decode_temp
Purpose : print the temperature value
Input   : temp (*100)
====================================================================== */
void decode_temp(int16_t temp)
{
  Serial.print(F(" Temp:"));
  Serial.print(temp/100.0f);
  Serial.print(F("C"));
}

/* ======================================================================
Function: decode_hum
Purpose : print the humidity
Input   : humidity
====================================================================== */
void decode_hum(int16_t hum)
{
  Serial.print(F(" Hum:"));
  Serial.print(hum);
  Serial.print(F("%"));
}

/* ======================================================================
Function: decode_lux
Purpose : print the lux value
Input   : lux (*10)
====================================================================== */
void decode_lux(int16_t lux)
{
  Serial.print(F(" Lux:"));
  Serial.print(lux/10.0f);
}
/* ======================================================================
Function: decode_co2
Purpose : print the co2 value
Input   : co2
====================================================================== */
void decode_co2(int16_t co2)
{
  Serial.print(F(" CO2:"));
  Serial.print(co2);
  Serial.print(F("ppm"));
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

  Serial.print(F("# type "));
  decode_frame_type(c);

  // Alive packet ?
  if ( c==RF_PL_ALIVE && len==sizeof(RFAlivePayload))
  {
    RFAlivePayload * p = (RFAlivePayload *) ppayload;
    decode_bat(p->vbat/1000);
  }

  // Ping packet ?
  else if ( c==RF_PL_PING && len==sizeof(RFPingPayload))
  {
    RFPingPayload * p = (RFPingPayload *) ppayload;
    decode_bat(p->vbat/1000);
    Serial.print(F(" RSSI:"));
    Serial.print(p->rssi,DEC);
    Serial.print(F("dB"));
  }

  // Classic payload Packet for majority of sensors ?
  else if ( len==sizeof(RFPayload))
  {
    RFPayload * p = (RFPayload *) ppayload;

    decode_bat(p->vbat/1000);

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
      Serial.print(*p++,HEX);
      Serial.print(' ');
    }

    // here we did not validated known packet, so clear command
    // code for the rest of the operation
    c=0;
  }

  return (c);
}

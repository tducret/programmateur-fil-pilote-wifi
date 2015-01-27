// **********************************************************************************
// RFM69 module management headers file for remora project
// **********************************************************************************
// Creative Commons Attrib Share-Alike License
// You are free to use/extend but please abide with the CC-BY-SA license:
// http://creativecommons.org/licenses/by-sa/4.0/
//
// Written by Charles-Henri Hallard (http://hallard.me)
//
// History : V1.00 2015-01-22 - First release
//
// All text above must be included in any redistribution.
//
// **********************************************************************************

#ifndef RFM_h
#define RFM_h

#include "remora.h"

// Default RF settings
#define NETWORK_ID  69
#define NODE_ID     1
#define FREQUENCY   RF69_433MHZ
#define ENCRYPTKEY  null
#define AUTH_ACK    true  // we are uthorized to respond to ACK

//Match frequency to the hardware version of the radio
//#define FREQUENCY   RF69_868MHZ
//#define FREQUENCY   RF69_915MHZ
//#define ENCRYPTKEY  "sampleEncryptKey" //exactly the same 16 characters/bytes on all nodes!
//#define IS_RFM69HW  //uncomment only for RFM69HW! Leave out if you have RFM69W!

// radio module type we are using
// To DO port RFM12B driver
typedef enum
{
  rf12 = 12,
  rf69 = 69
} module_e;

// data received by RF module
// independent from module received (RF12 or RF69)
// used to display or send to serial
typedef struct
{
  module_e  type;     /* Module type, RF12 or RF69 */
  uint8_t   nodeid;   /* Node ID      */
  uint8_t   groupid;  /* Group ID     */
  uint8_t   size;     /* Data Size    */
  uint8_t * pdata;    /* Data pointer */
  int8_t    rssi;     /* RSSI         */
  bool      ack;      /* ACK          */
  uint8_t   checksum; /* Serial checksum*/
  uint8_t   data[RF69_MAX_DATA_LEN];
} RFData;

// Variables exported to other source file
// ========================================
// define RF var for whole project
extern RFM69      radio;
extern RFPayload  payload;
extern RFData     module;

// Function exported for other source file
// =======================================
bool rfm_setup(void);
void rfm_loop(void);


#endif

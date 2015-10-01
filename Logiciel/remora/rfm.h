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
#include "RH_RF69.h"

// You will need to initialize the radio by telling it what ID it has and what network it's on
// The NodeID takes values from 1-127, 0 is reserved for sending broadcast messages (send to all nodes)
// The Network ID takes values from 0-255
#define RFM69_NODEID    1     // Node ID used for this unit
#define RFM69_NETWORKID 69    // the network ID we are on
#define RFM69_FREQUENCY 43400 // RFM69 434.00MHz
#define RFM69_TXPWR     13    // +13dB (set to 14..20 for RFM69HW)
#define RFM69_MODEM_MAX 26    // Number of modem config values MAX strored in flash RH_RH69 driver
#define RFM69_MODEMCFG  RH_RF69::GFSK_Rb250Fd250  // RadioHead default modem config

// Mode of tranmission
#define ACK_TIME    50  // # of ms to wait for an ack if asked
#define RF_RETRIES  3   // # of retries before dropping response
#define RF_NO_ACK   0   // Just send
#define RF_ACK      1   // Just send and ask for ack
#define RF_RESP     2   // Just send and wait response

#define RF_LED_BLINK_MS  150 // Time of RGB LED blink


// data received by RF module
// independent from module received
// used to display or send to serial
typedef struct
{
  uint8_t  nodeid;   /* Node ID      */
  uint8_t  groupid;  /* Group ID     */
  uint8_t  size;     /* Data Size    */
  int8_t   rssi;     /* RSSI         */
  uint8_t  flags;    /* header flags */
  uint8_t  checksum; /* Serial checksum*/
  uint8_t  seqid;    /* Sequence ID  */
  uint8_t  ack;      /* do we ACKED  */
  uint8_t  buffer[RH_RF69_MAX_MESSAGE_LEN];
} RFData;

// Variables exported to other source file
// ========================================
// define RF var for whole project
extern unsigned long rf_rgb_led_timer;
extern RH_RF69 driver;

// Function exported for other source file
// =======================================
bool rfm_setup(void);
void rfm_loop(void);

#endif

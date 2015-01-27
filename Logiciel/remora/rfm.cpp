// **********************************************************************************
// RFM69 module management source file for remora project
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

#include "rfm.h"
#include "linkedlist.h"
#include "ULPN_RF_Protocol.h"

// Let's define a new class for our nodes
class Node {
  public:
    uint8_t nodeid;   // node ID
    uint8_t groupid;  // neetwork group ID
    uint8_t rssi;     // Rssi of reception
    long    lastseen; // time stamp of when we saw it
};

// Linked list of received node data
LinkedList<Node*> myNodeList = LinkedList<Node*>();
Node *nodes;

// instantiate RF var for whole project
RFM69      radio69;
RFPayload  payload;
RFData     module;
uint16_t   rf_crc;

/* ======================================================================
Function: _crc16_update
Purpose : update a 16 bit CRC
Input   : crc16
          crc value update with (byte)
Output  : new crc16 value
Comments: -
====================================================================== */
uint16_t _crc16_update(uint16_t crc16, uint8_t a)
{
  crc16 ^= a;
  for (uint8_t i = 0; i < 8; ++i)
  {
    if (crc16 & 1)
      crc16 = (crc16 >> 1) ^ 0xA001;
    else
      crc16 = (crc16 >> 1);
  }
  return crc16;
}

/* ======================================================================
Function: print_char_with_crc
Purpose : print a byte data but update CRC with the value printed
Input   : byte
Output  : -
Comments:
====================================================================== */
void print_char_with_crc(uint8_t _byte)
{
  // Update CRC
  rf_crc = _crc16_update(rf_crc, _byte);

  // display data
  Serial.print((char)_byte);
}

/* ======================================================================
Function: print_num_with_crc
Purpose : print a number but update CRC with the value printed
Input   : number to print
Output  : -
Comments:
====================================================================== */
void print_num_with_crc(int16_t n)
{
  uint8_t i,j,c;
  char  s[7];
  bool  negative = false;

  // Check sign
  if ( n< 0 )
  {
    // make n positive
    n = -n;
    negative = true;
  }

  i = 0;
  do
  {
    // generate digits in reverse order
    s[i++] = n % 10 + '0';   // get next digit
  }
  while ((n /= 10) > 0); // delete it

  // add sign
  if (negative)
    s[i++] = '-';

  // Now we have a string reversed
  s[i] = '\0';

  // Now we reverse the string
  for (i = 0, j = strlen(s)-1; i<j; i++, j--)
  {
    // Save char
    c = s[i];
    // put end char at the begining
    s[i] = s[j];
    // put beginning char at end
    s[j] = c;
  }

  // print and update crc
  for (i = 0; i< strlen(s); i++)
    print_char_with_crc(s[i]);
}


/* ======================================================================
Function: print_received_data
Purpose : send to serial received data frame with CRC
Input   : -
Output  : -
Comments:
====================================================================== */
void print_received_data(uint8_t nodeid, uint8_t groupid, byte * buffer, uint8_t len, int8_t rssi, bool acked)
{
  // clear the CRC
  rf_crc = ~0;

  // now print the group ID
  print_char_with_crc('g');
  print_num_with_crc((int16_t) groupid);
  print_char_with_crc(' ');

  // print the node ID
  print_char_with_crc('i');
  print_num_with_crc((int16_t) nodeid);

  // print the frame
  for (byte i = 0; i < len; ++i)
  {
    print_char_with_crc(' ');
    print_num_with_crc((int16_t) buffer[i]);
  }

  // Print ARSSI
  print_char_with_crc(' ');
  print_char_with_crc('(');
  print_num_with_crc((int16_t) rssi);
  print_char_with_crc(')');
  print_char_with_crc(' ');

  // Now print the CRC
  Serial.print(rf_crc);

  // Indicate if we acked the frame
  if ( acked)
    Serial.print(F(" -> ACK"));

  // end of  frame
  Serial.println();
}

/* ======================================================================
Function: receive_data_rfm69
Purpose : receive data payload from RFM69 and manage ACK
Input   : -
Output  : millis since this node has been seen
Comments: called from loop, this is a non blocking receiver
====================================================================== */
long receive_data_rfm69(void)
{
  bool  known_node = false;
  int   nb_node;
  int   current;
  long  last_seen;
  Node  *current_node;

  // default no ack
  module.ack = false;

  // Grab data information
  module.type     = rf69;
  module.nodeid   = radio69.SENDERID;
  module.groupid  = NETWORK_ID;
  module.size     = radio69.DATALEN;
  module.pdata    = module.data;
  module.rssi     = radio69.RSSI;

  // Copy Data buffer, this is mandatory cause
  // in case of ACK we loose the information
  for (uint8_t i=0 ; i< module.size; i++)
    module.data[i] = radio69.DATA[i];

  // Are we authorized to send ACK ?
  if ( AUTH_ACK )
  {
    // Ack as soon as possible if needed
    if (radio69.ACKRequested())
    {
      radio69.sendACK();
      module.ack = true;
    }
  }

  // Number of node we already seen
  nb_node = myNodeList.size();
  current = 1;

  // loop thru all existing nodes seen
  while ( nb_node && !known_node && current<=nb_node )
  {
    // Get current node from list
    current_node = myNodeList.get(current);

    // Do we already have received info from this node ?
    if( current_node->nodeid == module.nodeid )
      known_node = true;

    // next node
    current++;
  }

  // this node is already known ?
  if (known_node)
  {
    // update current entry
    last_seen = current_node->lastseen;
    current_node->lastseen = millis();
  }
  else
  {
    // Create new entry
    Node * new_node = new Node();

    // fill it
    new_node->nodeid  = module.nodeid;
    new_node->groupid = module.groupid;
    new_node->rssi    = module.rssi;
    new_node->lastseen= millis();
    last_seen         = new_node->lastseen;

    // Add to the linked list
    myNodeList.add(new_node);
  }

  // Return when we last saw this node
  return (last_seen);
}

/* ======================================================================
Function: rfm_setup
Purpose : prepare and init stuff, configuration, ..
Input   : -
Output  : true if RF module found, false otherwise
Comments: -
====================================================================== */
bool rfm_setup(void)
{
  Serial.print("Initializing RF Module RFM69");
  #ifdef IS_RFM69HW
  Serial.print("H");
  #endif
  Serial.print("W with IRQ:"); Serial.print(RF69_IRQ_PIN);
  Serial.print(F(" CS:")); Serial.print(RF69_SPI_CS);

  if ( !radio69.initialize(FREQUENCY,NODE_ID,NETWORK_ID) )
  {
    Serial.println(" Not found!");
    return(false);
  }
  else
  {
    uint8_t version = radio69.readReg(RF_VERSION);
    Serial.print(F(" Found V"));
    Serial.print((version & RF_VERSION_FULL_MASK) >> 4,HEX);
    Serial.print(F("."));
    Serial.println(version & RF_VERSION_METAL_MASK,HEX);

    Serial.print(F("Frequency  : "));
    Serial.print(FREQUENCY==RF69_433MHZ?"433":FREQUENCY==RF69_868MHZ?"868":"915");
    Serial.println(F("MHz"));
    Serial.print(F("Network Id : ")); Serial.println(NETWORK_ID);
    Serial.print(F("Node Id    : ")); Serial.println(NODE_ID);

    #ifdef IS_RFM69HW
      radio69.setHighPower(); //uncomment only for RFM69HW!
    #endif
    radio69.encrypt(ENCRYPTKEY);
  }

  return (true);
}

/* ======================================================================
Function: rfm_loop
Purpose : really need to tell ?
Input   : -
Output  : -
Comments: -
====================================================================== */
void rfm_loop(void)
{
  long  last_seen;

  // We received a RF frame ?
  if (radio69.receiveDone())
  {
    long started ;
    uint8_t c; // command code

    // receiveDone for RFM69 always true because checksum done in hardware
    last_seen = receive_data_rfm69();

    started = millis();

    // command code
    c = module.data[0];

    // Dump Raw packet
    Serial.print(F("# Packet Received node:"));
    Serial.print(module.nodeid);
    Serial.print(F(" size:"));
    Serial.print(module.size);
    Serial.print(F(" RSSI:"));
    Serial.print(module.rssi);
    Serial.print(F(" last seen "));
    Serial.print((millis()-last_seen)/1000);
    Serial.println(F(" s ago."));

    // print decoded format
    // return command code validated by payload type size reveived
    // so if we had a command and payload does not match
    // code as been set to 0 by decode_received_data
    c = decode_received_data(module.size, c, (void *) module.data);
    Serial.println();

    // Payload ok ?
    if ( c )
    {
      // special ping packet, we need to answer back to node
      if ( c==RF_PL_PING )
      {
        RFPingPayload * ppl = (RFPingPayload *) module.data;

        // prepare send back response
        ppl->command = RF_PL_PINGBACK;
        ppl->rssi = module.rssi;

        Serial.println(F("# Sending ping response "));

        // Send RSSI back (no ACK)
        if (module.type == rf69)
          radio69.send(module.nodeid, ppl, sizeof(RFPingPayload), false);
      }

      // check for known Payload
      if ( c > RF_PL_START && c < RF_PL_END )
      {
        // send frame to serial in jeenode style + checksum
        print_received_data(module.nodeid ,module.groupid, module.pdata, module.size, module.rssi, module.ack );
      }
    }
  }
}

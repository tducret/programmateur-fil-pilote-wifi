// **********************************************************************************
// Remora WEB Server, route web function
// **********************************************************************************
// Creative Commons Attrib Share-Alike License
// You are free to use/extend this library but please abide with the CC-BY-SA license:
// Attribution-NonCommercial-ShareAlike 4.0 International License
// http://creativecommons.org/licenses/by-nc-sa/4.0/
//
// This program works with the Remora board
// see schematic here https://github.com/thibdct/programmateur-fil-pilote-wifi
//
// Written by Charles-Henri Hallard (http://hallard.me)
//
// History : V1.00 2015-06-14 - First release
//
// All text above must be included in any redistribution.
//
// **********************************************************************************

// Include header
#include "route.h"

/* ======================================================================
Function: handleRoot
Purpose : handle main page /, display information
Input   : -
Output  : -
Comments: -
====================================================================== */
void handleRoot(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete)
{
  String response="";

  // Just to debug where we are
  Serial.print(F("Serving / page..."));

  // Send headers
  server.httpSuccess();

  // if we're handling a GET or POST, we can output our data here.
  // For a HEAD request, we just stop after outputting headers.
  if (type == WebServer::HEAD)
    return;

  server.printP(F("<html><head><title>Remora</title></head><body>\n"));
  switch (type)
  {
    case WebServer::GET:
      server.printP(F("<h1>GET from "));
    break;
    case WebServer::POST:
      server.printP(F("<h1>POST to "));
    break;
    default:
      server.printP(F("<h1>UNKNOWN request for "));
  }

  server.printP(F("unidentified URL requested.</h1><br>\n"));
  server.printP(tail_complete ? F("URL='") : F("INCOMPLETE URL='"));
  server.print(url_tail);
  server.printP(F("'<br>\n"));
  server.printP(F("</body></html>"));

  Serial.print(F("OK!"));
}

/* ======================================================================
Function: formatNumberJSON
Purpose : check if data value is full number and send correct JSON format
Input   : String where to add response
          char * value to check
Output  : -
Comments: 00150 => 150
          ADCO  => "ADCO"
          1     => 1
====================================================================== */
#ifdef ESP8266
void formatNumberJSON( String &response, char * value)
{
  // we have at least something ?
  if (value && strlen(value))
  {
    boolean isNumber = true;
    uint8_t c;
    char * p = value;

    // just to be sure
    if (strlen(p)<=16) {
      // check if value is number
      while (*p && isNumber) {
        if ( *p < '0' || *p > '9' )
          isNumber = false;
        p++;
      }

      // this will add "" on not number values
      if (!isNumber) {
        response += '\"' ;
        response += value ;
        response += F("\":") ;
      } else {
        // this will remove leading zero on numbers
        p = value;
        while (*p=='0' && *(p+1) )
          p++;
        response += p ;
      }
    } else {
      Serial.println(F("formatNumberJSON error!"));
    }
  }
}


/* ======================================================================
Function: tinfoJSONTable
Purpose : dump all teleinfo values in JSON table format for browser
Input   : linked list pointer on the concerned data
          true to dump all values, false for only modified ones
Output  : -
Comments: -
====================================================================== */
void tinfoJSONTable(void)
{
  ValueList * me = tinfo.getList();
  String response = "";

  // Just to debug where we are
  Serial.print(F("Serving /tinfoJSONTable page...\r\n"));

  // Got at least one ?
  if (me) {
    uint8_t index=0;

    boolean first_item = true;
    // Json start
    response += F("[\r\n");

    // Loop thru the node
    while (me->next) {

      // we're there
      #ifdef ESP8266
      ESP.wdtFeed();
      #endif

      // go to next node
      me = me->next;

      // First item do not add , separator
      if (first_item)
        first_item = false;
      else
        response += F(",\r\n");

      Serial.print(F("(")) ;
      Serial.print(++index) ;
      Serial.print(F(") ")) ;

      if (me->name) Serial.print(me->name) ;
      else Serial.print(F("NULL")) ;

      Serial.print(F("=")) ;

      if (me->value) Serial.print(me->value) ;
      else Serial.print(F("NULL")) ;

      Serial.print(F(" '")) ;
      Serial.print(me->checksum) ;
      Serial.print(F("' "));

      // Flags management
      if ( me->flags) {
        Serial.print(F("Flags:0x"));
        Serial.print(me->flags, HEX);
        Serial.print(F(" => "));
        Serial.print(me->flags);
        if ( me->flags & TINFO_FLAGS_EXIST)
          Serial.print(F("Exist ")) ;
        if ( me->flags & TINFO_FLAGS_UPDATED)
          Serial.print(F("Updated ")) ;
        if ( me->flags & TINFO_FLAGS_ADDED)
          Serial.print(F("New ")) ;
      }

      Serial.println() ;

      response += F("{\"na\":\"");
      response +=  me->name ;
      response += F("\", \"va\":\"") ;
      response += me->value;
      response += F("\", \"ck\":\"") ;
      if (me->checksum == '"' || me->checksum == '\\' || me->checksum == '/')
        response += '\\';
      response += (char) me->checksum;
      response += F("\", \"fl\":");
      response += me->flags ;
      response += '}' ;

    }
   // Json end
   response += F("\r\n]");

  } else {
    Serial.println(F("sending 404..."));
    server.send ( 404, "text/plain", "No data" );
  }
  Serial.print(F("sending..."));
  server.send ( 200, "text/json", response );
  Serial.print(F("OK!"));
}


/* ======================================================================
Function: sendJSON
Purpose : dump all values in JSON
Input   : linked list pointer on the concerned data
          true to dump all values, false for only modified ones
Output  : -
Comments: -
====================================================================== */
void sendJSON(void)
{
  ValueList * me = tinfo.getList();
  String response = "";

  // Got at least one ?
  if (me) {
    // Json start
    response += F("{\"_UPTIME\":");
    response += seconds;

    // Loop thru the node
    while (me->next) {

      // we're there
      ESP.wdtFeed();

      // go to next node
      me = me->next;

      response += F(",\"") ;
      response += me->name ;
      response += F("\":") ;
      formatNumberJSON(response, me->value);
    }
   // Json end
   response += F("}\r\n") ;

  } else {
    server.send ( 404, "text/plain", "No data" );
  }
  server.send ( 200, "text/json", response );
}

/* ======================================================================
Function: handleNotFound
Purpose : default WEB routing when URI is not found
Input   : linked list pointer on the concerned data
          true to dump all values, false for only modified ones
Output  : -
Comments: We search is we have a name that match to this URI, if one we
          return it's pair name/value in json
====================================================================== */
void handleNotFound(void)
{
  String response = "";

  // We check for an known label
  ValueList * me = tinfo.getList();
  const char * uri;
  boolean found = false;

  // Led on
  LedBluON();

  // convert uri to char * for compare
  uri = server.uri().c_str();

  Serial.printf("handleNotFound(%s)\r\n", uri);

  // Got at least one and consistent URI ?
  if (me && uri && *uri=='/' && *++uri ) {

    // Loop thru the linked list of values
    while (me->next && !found) {

      // we're there
      ESP.wdtFeed();

      // go to next node
      me = me->next;

      //Debugf("compare to '%s' ", me->name);
      // Do we have this one ?
      if (stricmp (me->name, uri) == 0 )
      {
        // no need to continue
        found = true;

        // Add to respone
        response += F("{\"") ;
        response += me->name ;
        response += F("\":") ;
        formatNumberJSON(response, me->value);
        response += F("}\r\n");
      }
    }
  }

  // Got it, send json
  if (found) {
    server.send ( 200, "text/json", response );
  } else {
    // send error message in plain text
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += server.uri();
    message += "\nMethod: ";
    message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
    message += "\nArguments: ";
    message += server.args();
    message += "\n";

    for ( uint8_t i = 0; i < server.args(); i++ ) {
      message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
    }

    server.send ( 404, "text/plain", message );
  }

  // Led off
  LedBluOFF();
}
#endif

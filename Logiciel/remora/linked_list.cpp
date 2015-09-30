// **********************************************************************************
// ULPNode RF Gateway Basic Linked List source file
// **********************************************************************************
// Creative Commons Attrib Share-Alike License
// You are free to use/extend this library but please abide with the CC-BY-SA license:
// http://creativecommons.org/licenses/by-sa/4.0/
//
// For any explanation of ULPNode RF Protocol see
// https://hallard.me/ulpnode-rf-protocol/
//
// For any explanation of ULPNode see
// https://hallard.me/category/ulpnode
//
// Written by Charles-Henri Hallard (http://hallard.me)
//
// History : V1.10 2015-09-05 - Creation
//
// All text above must be included in any redistribution.
//
// **********************************************************************************
#include "./linked_list.h"

/* ======================================================================
Function: ll_Delete
Purpose : Delete the ENTIRE Linked List, not a value
Input   : pointer to the top of the linked list
Output  : True if Ok False Otherwise
Comments: -
====================================================================== */
boolean ll_Delete(NodeList * me)
{
  // Got a pointer
  if (me) {
    NodeList *current;

    // For each linked list
    while ((current = me->next)) {
      // Get the next
      me->next =  current->next;

      // Free the current
      free(current);
    }

    // Free the top element
    me->next = NULL ;
    return (true);
  }

  return (false);
}

/* ======================================================================
Function: ll_Add
Purpose : Add element to the Linked List
Input   : pointer to the top of the linked list
          network ID
          node ID
          RSSI
          second ellapsed since start
Output  : pointer to the new node (or founded one)
Comments: last seen is filled with old value in return
====================================================================== */
NodeList * ll_Add(NodeList * me, uint8_t groupid, uint8_t nodeid, int8_t rssi, unsigned long * sec)
{
  // Create pointer on the new node
  NodeList *newNode = NULL;

  // Something to use
  if (me) {

    // Loop thru the node to see if we know it
    while (me->next) {
      // go to next node
      me = me->next;

      // Check if we already have this node id and group id
      if ( me->groupid == groupid && me->nodeid == nodeid) {

        // Save old value
        unsigned long old_sec = me->lastseen;

        // Update data
        me->rssi = rssi ;
        me->lastseen = *sec;

        // Return old value
        *sec = old_sec;

        // That's all
        return (me);
      }
    }

    // We did not find this node, it's new
    // Create new node with size to store data
    if ((newNode = (NodeList  *) malloc(sizeof(NodeList)) ) != NULL)  {
      // Setup our new node values
      newNode->next = NULL;
      newNode->groupid = groupid ;
      newNode->nodeid = nodeid;
      newNode->rssi = rssi ;
      newNode->lastseen = *sec;

      // add the new node on the list
      me->next = newNode;

      // return pointer on the new node
      return (newNode);
    }
  }
  // Error
  return ( (NodeList *) NULL );
}

/* ======================================================================
Function: ll_Dump
Purpose : dump linked list content
Input   : pointer on the linked list
          current seconds ellapsed
Output  : total number of values
Comments: -
====================================================================== */
uint8_t ll_Dump(NodeList * me, unsigned long sec)
{
  // Get our linked list
  uint8_t index = 0;

  // Got one ?
  if (me) {
    // Loop thru the node
    while (me->next) {
      // go to next node
      me = me->next;

      index++;
      Serial.print(index) ;        Serial.print(F(") ")) ;
      Serial.print(F("Group:"));   Serial.print(me->groupid, DEC) ;
      Serial.print(F("  Node:"));  Serial.print(me->nodeid, DEC) ;
      Serial.print(F("  RSSI:"));  Serial.print(me->rssi, DEC) ;
      Serial.print(F("  seen:"));  Serial.print(sec-me->lastseen) ;
      Serial.println(F("")) ;
    }
  }

  return index;
}

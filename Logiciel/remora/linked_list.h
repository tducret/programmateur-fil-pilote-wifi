// **********************************************************************************
// ULPNode RF Gateway Basic Linked List header file
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
#ifndef __LINKED_LIST_H__
#define __LINKED_LIST_H__

#include "remora.h"

// Linked list structure containing all nodes seen
typedef struct _NodeList NodeList;
struct _NodeList
{
  NodeList *next;         // next element
  uint8_t nodeid;         // Node ID
  uint8_t groupid;        // Network ID
  int8_t  rssi;           // RSSI
  unsigned long lastseen; // Last seen time (in second)
};

// Variables exported to other source file
// ========================================
// extern _NodeList nodes_list;

// Function exported to other source file
// =======================================
boolean    ll_Delete(NodeList * me);
NodeList * ll_Add(NodeList * me, uint8_t nodeid, uint8_t groupid, int8_t rssi, unsigned long * sec);
uint8_t    ll_Dump(NodeList * me, unsigned long sec);

#endif

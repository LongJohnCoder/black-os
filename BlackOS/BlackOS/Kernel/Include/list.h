// Copyright (c) 2020 Bjørn Brodtkorb
//
// This software is provided "as is" without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#ifndef LIST_H
#define LIST_H


//--------------------------------------------------------------------------------------------------//


#include "sam.h"


//--------------------------------------------------------------------------------------------------//


typedef struct List_item_s
{
	struct List_item_s* next;
	struct List_item_s* prev;
	
	
	// This is used for sorting the list. It adds 8 bytes of data to 
	// each element, but prevents dead clock cycles to access the 
	// object struct.
	uint64_t value;
	
	
	// Pointer to the list items object. Normally this would be a thread 
	// or a precess.
	void* object;
	
} list_item_s;



typedef struct List_s
{
	struct List_item_s* first;
	struct List_item_s* last;
	
	
	// Keep track of the size of the list
	uint32_t size;
	
} list_s;


//--------------------------------------------------------------------------------------------------//


void list_insert_first(list_item_s* list_item, list_s* list);

void list_insert_last(list_item_s* list_item, list_s* list);

void list_insert_sort(list_item_s* list_item, list_s* list);

uint8_t list_search(list_item_s* list_item, list_s* list);

list_item_s* list_remove_first(list_s* list);

list_item_s* list_remove_last(list_s* list);

list_item_s* list_remove_item(list_item_s* list_item, list_s* list);


//--------------------------------------------------------------------------------------------------//


#endif
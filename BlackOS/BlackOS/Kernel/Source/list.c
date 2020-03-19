// Copyright (c) 2020 Bjørn Brodtkorb
//
// This software is provided "as is" without warranty of any kind.
// Permission is granted, free of charge, to copy and modify this
// software, if this copyright notice is included in all copies of
// the software.

#include "list.h"
#include "check.h"


//--------------------------------------------------------------------------------------------------//


#include <stddef.h>


//--------------------------------------------------------------------------------------------------//


void list_insert_first(list_item_s* list_item, list_s* list)
{
	// Check if the size is zero
	if (list->size == 0)
	{
		// Update the global pointers
		list->first = list_item;
		list->last = list_item;
		
		// Update the list item pointer
		list_item->next = NULL;
		list_item->prev = NULL;
	}
	else
	{
		// Search the list to make sure it do not exist
		check(list_search(list_item, list) == 0);
		
		// Update the list item pointers
		list_item->next = list->first;
		list_item->prev = NULL;
		
		// Link backwards from first node
		check(list->first->prev == NULL);
		list->first->prev = list_item;
		
		// Update the first pointer
		list->first = list_item;
	}
	
	// Increment the size
	list->size++;
}


//--------------------------------------------------------------------------------------------------//


void list_insert_last(list_item_s* list_item, list_s* list)
{
	// Check if the size is zero
	if (list->size == 0)
	{
		// Update the global pointers
		list->first = list_item;
		list->last = list_item;
		
		// Update the list item pointer
		list_item->next = NULL;
		list_item->prev = NULL;
	}
	else
	{
		// Search the list to make sure it do not exist
		check(list_search(list_item, list) == 0);
		
		// Update the list item pointers
		list_item->next = NULL;
		list_item->prev = list->last;
		
		// Link backwards from first node
		check(list->last->next == NULL);
		list->last->next = list_item;
		
		// Update the first pointer
		list->last = list_item;
	}
	
	// Increment the size
	list->size++;
}


//--------------------------------------------------------------------------------------------------//


void list_insert_sort(list_item_s* list_item, list_s* list)
{
	// Check if the size if zero
	if (list->size == 0)
	{
		// Update the global pointers
		list->first = list_item;
		list->last = list_item;
		
		// Update the list item pointer
		list_item->next = NULL;
		list_item->prev = NULL;
		
		list->size = 1;
	}
	else
	{
		// Search the list to make sure it do not exist
		check(list_search(list_item, list) == 0);
		
		// Check the tick value
		uint32_t insert_value = list_item->value;
		
		if (insert_value <= list->first->value)
		{
			// Insert at the beginning
			list_insert_first(list_item, list);
			
			// Increment handled
		}
		else if (insert_value >= list->last->value)
		{
			// Insert at the end
			list_insert_last(list_item, list);
			
			// Increment handled
		}
		else
		{
			list_item_s* list_iterator = list->first->next;
			
			// Iterate though the list
			while (list_iterator != NULL)
			{
				// Check tick against the following item
				if (insert_value < list_iterator->value)
				{
					// Insert the item behind list_iterator
					list_item_s* prev_item = list_iterator->prev;
					
					list_item->next = list_iterator;
					list_iterator->prev = list_item;
					
					prev_item->next = list_item;
					list_item->prev = prev_item;
					
					list->size++;
					
					return;
				}
				
				list_iterator = list_iterator->next;
			}
			
			// Should not reach here
			check(0);
		}
	}
}


//--------------------------------------------------------------------------------------------------//


uint8_t list_search(list_item_s* list_item, list_s* list)
{
	if (list->first == NULL)
	{
		return 0;
	}
	
	list_item_s* list_iterator = list->first;
	
	while (list_iterator != NULL)
	{
		// If there is a match return
		if (list_item == list_iterator)
		{
			return 1;
		}
		
		list_iterator = list_iterator->next;
	}
	
	return 0;
}


//--------------------------------------------------------------------------------------------------//


list_item_s* list_remove_first(list_s* list)
{
	list_item_s* return_item = list->first;
	
	if (list->size == 0)
	{
		// Nothing to remove
		return NULL;
	}
	else if (list->size == 1)
	{		
		// Update the list first and last element
		list->first = NULL;
		list->last = NULL;
		
		list->size = 0;
	}
	else
	{
		// Update the first pointer
		list->first = list->first->next;
		
		// Remove the backward link from the new first item
		list->first->prev = NULL;
		
		list->size--;
	}
	
	
	// Remove the links from the item removed
	return_item->next = NULL;
	return_item->prev = NULL;
	
	return return_item;
}


//--------------------------------------------------------------------------------------------------//


list_item_s* list_remove_last(list_s* list)
{
	list_item_s* return_item = list->last;
	
	if (list->size == 0)
	{
		// Nothing to remove
		return NULL;
	}
	else if (list->size == 1)
	{		
		// Update the list first and last element
		list->first = NULL;
		list->last = NULL;
		
		list->size = 0;
	}
	else
	{
		// Remove the last element
		list->last = list->last->prev;
		
		// Remove the backward link from the new first item
		list->last->next = NULL;
		
		list->size--;
	}
	
	
	// Remove the links from the item removed
	return_item->next = NULL;
	return_item->prev = NULL;
	
	return return_item;
}


//--------------------------------------------------------------------------------------------------//


list_item_s* list_remove_item(list_item_s* list_item, list_s* list)
{
	if (list->size == 0)
	{
		return NULL;
	}
	else if (list_item == list->first)
	{
		list_remove_first(list);
	}
	else if (list_item == list->last)
	{
		list_remove_last(list);
	}
	else
	{
		// The element is not the first element and not the last element
		// Start at the second element
		list_item_s* list_iterator = list->first->next;
		
		while (list_iterator != list->last)
		{
			// Check for a match
			if (list_item == list_iterator)
			{
				// Remove the connections
				list_item->next->prev = list_item->prev;
				list_item->prev->next = list_item->next;
				
				// Update the list_item pointers
				list_item->next = NULL;
				list_item->prev = NULL;
				
				list->size--;
				
				return list_item;
			}
			
			list_iterator = list_iterator->next;
		}
		
		return NULL;
	}
	
	return list_item;
}


//--------------------------------------------------------------------------------------------------//
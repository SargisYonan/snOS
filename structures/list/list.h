// Generic Dynamic Doubly Linked-List Data Structure
// Sargis S Yonan
// April 2017

#ifndef _NOS_SYSTEM_LIST_H_
#define _NOS_SYSTEM_LIST_H_

#include <stdint.h>
#include <stdbool.h>

#include "snos_alloc.h"

#define LIST_SUCCESS (1)
#define LIST_OOM_ERROR (0)
#define LIST_ERROR (0)
#define LIST_NULL_ERROR (0)

#ifndef LIST_MALLOC
	#define LIST_MALLOC snos_alloc
#endif

#ifndef LIST_FREE
	#define LIST_FREE snos_free
#endif

struct _node{
	void *data_ptr;
	struct _node *prev_ptr;
	struct _node *next_ptr;
};
typedef struct _node node_t;

struct _list {
	node_t *head_ptr;
	node_t *tail_ptr;
	uint32_t list_length;
	node_t *cursor_ptr;
};
typedef struct _list list_t;


// basic list functions
list_t *list_create(void);
uint8_t list_append(list_t *list_ptr, void *data_ptr);
uint8_t list_prepend(list_t *list_ptr, void *data_ptr);
uint32_t list_length(list_t *list_ptr);

// cursor functions
uint8_t list_move_cursor_to_head(list_t *list_ptr);
uint8_t list_move_cursor_to_tail(list_t *list_ptr);
uint8_t list_move_cursor_right(list_t *list_ptr);
uint8_t list_move_cursor_left(list_t *list_ptr);
uint8_t list_append_from_cursor(list_t *list_ptr, void *data_ptr);
void *list_get_cursor_data(list_t *list_ptr);
void *list_get_tail_data(list_t *list_ptr);
bool list_updated(list_t *list_ptr);
uint8_t list_detete_current_item(list_t *list_ptr);

uint8_t list_destroy_list(list_t *list_ptr);

#endif // _NOS_SYSTEM_LIST_H_
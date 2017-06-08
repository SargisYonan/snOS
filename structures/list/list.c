#include "list.h"

// Example Usage:

/*
int main (void) {
	list_t *test_list_1 = NULL;
	uint8_t ret = 0;

	gps_t coor[3] = {{1.1, 1.1, 1}, {2.2, 2.2, 2}, {3.3, 3.3, 3}};
	gps_t *ptr;

	gps_t santa_cruz = {36.974134, -122.0258882, 1};
	gps_t paris = {48.8589507, 2.2775168, 2};

	test_list_1 = list_create();

	printf("haversine = %f\n", haversine(santa_cruz.latitude, santa_cruz.longitude, paris.latitude, paris.longitude));

	list_append(test_list_1, &(coor[0]));
	list_append(test_list_1, &(coor[1]));
	list_append(test_list_1, &(coor[2]));

	list_move_cursor_to_head(test_list_1);
	ptr = list_get_cursor_data(test_list_1);
	printf("\nitem 1: %f", ptr->longitude);
	ptr = list_get_cursor_data(test_list_1);
	printf("\nitem 2: %f", ptr->longitude);
	ptr = list_get_cursor_data(test_list_1);
	printf("\nitem 3: %f", ptr->longitude);

	list_append(test_list_1, &(coor[0]));
	if (list_updated(test_list_1)) {
		ptr = list_get_cursor_data(test_list_1);
		printf("\nitem 3: %f", ptr->longitude);
	}
}
*/

#define list_assert(list)     	\
	do {          				\
    	if (list == NULL) {return LIST_NULL_ERROR;} \
    }          					\
	while (list == NULL)


node_t *_list_create_node(void *data_ptr, node_t *prev_ptr, node_t *next_ptr) {
	node_t *new_node = NULL;
	new_node = LIST_MALLOC(sizeof(node_t));
	if (new_node) {
		new_node->data_ptr = data_ptr;
		new_node->prev_ptr = prev_ptr;
		new_node->next_ptr = next_ptr;
	} 		

	return new_node;
}

// basic list functions
list_t *list_create(void) {
	list_t *new_list_ptr = NULL;

	new_list_ptr = LIST_MALLOC(sizeof(list_t));
	if (new_list_ptr) {
		new_list_ptr->head_ptr = NULL;
		new_list_ptr->tail_ptr = NULL;
		new_list_ptr->list_length = 0;
		new_list_ptr->cursor_ptr = new_list_ptr->head_ptr;
	}

	return new_list_ptr; 
}

uint8_t list_append(list_t *list_ptr, void *data_ptr) {
	node_t *new_node;
	
	list_assert(list_ptr);

	new_node = _list_create_node(
		data_ptr, 
		list_ptr->tail_ptr, 
		NULL);

	if (new_node) {
		if (list_ptr->tail_ptr) {
			list_ptr->tail_ptr->next_ptr = new_node;
		}
		list_ptr->tail_ptr = new_node;

		if (list_ptr->list_length == 0) {
			list_ptr->head_ptr = new_node;
			list_ptr->cursor_ptr = new_node;
		}

		++list_ptr->list_length;
		return LIST_SUCCESS;
	} else {
		return LIST_OOM_ERROR;
	}
}

uint8_t list_prepend(list_t *list_ptr, void *data_ptr) {
	node_t *new_node;

	list_assert(list_ptr);

	new_node = _list_create_node(
		data_ptr, 
		NULL, 
		list_ptr->head_ptr);

	if (new_node) {
		if (list_ptr->head_ptr) {
			list_ptr->head_ptr->prev_ptr = new_node;
		}

		list_ptr->head_ptr = new_node;

		if (list_ptr->list_length == 0) {
			list_ptr->tail_ptr = new_node;
			list_ptr->cursor_ptr = new_node;
		}

		++list_ptr->list_length;
		return LIST_SUCCESS;
	} else {
		return LIST_OOM_ERROR;
	}
}

uint32_t list_length(list_t *list_ptr) {
	list_assert(list_ptr);
	return list_ptr->list_length;
}

// cursor functions
uint8_t list_move_cursor_to_head(list_t *list_ptr) {
	list_assert(list_ptr);

	list_ptr->cursor_ptr = list_ptr->head_ptr;
	return LIST_SUCCESS;
}

uint8_t list_move_cursor_to_tail(list_t *list_ptr) {
	list_assert(list_ptr);

	list_ptr->cursor_ptr = list_ptr->tail_ptr;
	return LIST_SUCCESS;
}

uint8_t list_move_cursor_right(list_t *list_ptr) {
	list_assert(list_ptr);
	list_assert(list_ptr->cursor_ptr);

	if (list_ptr->cursor_ptr->next_ptr) {
		list_ptr->cursor_ptr = list_ptr->cursor_ptr->next_ptr;
		return LIST_SUCCESS;
	} else {
		return LIST_ERROR;	
	}
}

uint8_t list_move_cursor_left(list_t *list_ptr) {
	list_assert(list_ptr);
	list_assert(list_ptr->cursor_ptr);

	if (list_ptr->cursor_ptr->next_ptr) {
		list_ptr->cursor_ptr = list_ptr->cursor_ptr->next_ptr;
		return LIST_SUCCESS;
	} else {
		return LIST_ERROR;
	}
}

uint8_t list_append_from_cursor(list_t *list_ptr, void *data_ptr) {
	node_t *new_node;

	list_assert(list_ptr);
	list_assert(list_ptr->cursor_ptr);

	new_node = _list_create_node(
		data_ptr, 
		NULL, 
		NULL);

	if (new_node) {
		new_node->prev_ptr = list_ptr->cursor_ptr;

		if (list_ptr->cursor_ptr->next_ptr) {
			new_node->next_ptr = list_ptr->cursor_ptr->next_ptr;
			list_ptr->cursor_ptr->next_ptr->prev_ptr = new_node;
			list_ptr->cursor_ptr->next_ptr = new_node;
		} else {
			list_ptr->cursor_ptr->next_ptr = new_node;
			list_ptr->tail_ptr = new_node;
		}

		return LIST_SUCCESS;
	} else {
		return LIST_OOM_ERROR;
	}
}

void *list_get_cursor_data(list_t *list_ptr) {
	void *ret = NULL;
	list_assert(list_ptr);
	
	if (list_ptr->cursor_ptr) {
		ret =  list_ptr->cursor_ptr->data_ptr;
//		list_move_cursor_right(list_ptr);
	} 

	return ret;

}

void *list_get_tail_data(list_t *list_ptr) {
	void *ret = NULL;
	list_assert(list_ptr);
	
	if (list_ptr->tail_ptr) {
		ret = list_ptr->tail_ptr->data_ptr;
	} 

	return ret;
}

bool list_updated(list_t *list_ptr) {
	list_assert(list_ptr);

	if (list_ptr->cursor_ptr) {
		if (list_ptr->cursor_ptr->next_ptr) {
			list_move_cursor_right(list_ptr);

			return true;
		}
	} 

	return false;
}

uint8_t list_detete_current_item(list_t *list_ptr) {
	list_assert(list_ptr);
	node_t *prev = NULL;
	node_t *next = NULL;
	node_t *this = NULL;

	if (list_ptr->cursor_ptr) {
		this = list_ptr->cursor_ptr;

		if (list_ptr->cursor_ptr->prev_ptr) {
			prev = list_ptr->cursor_ptr->prev_ptr;
		}
		if (list_ptr->cursor_ptr->next_ptr) {
			next = list_ptr->cursor_ptr->next_ptr;
		} 

		if (prev == NULL && next == NULL) {
			list_ptr->head_ptr = NULL;
			list_ptr->tail_ptr = NULL;
			list_ptr->cursor_ptr = NULL;
		}
		else if (prev == NULL && next != NULL) {
			list_ptr->head_ptr = next;
			next->prev_ptr = NULL;
			list_ptr->cursor_ptr = next;
		}
		else if (prev != NULL && next == NULL) {
			list_ptr->tail_ptr = prev;
			prev->next_ptr = NULL;
			list_ptr->cursor_ptr = prev;
		}
		else if (prev && next) {
			prev->next_ptr = next;
			next->prev_ptr = prev;
			list_ptr->cursor_ptr = next;
		}

		list_ptr->list_length--;
		LIST_FREE(this->data_ptr);
		LIST_FREE(this);

	}

	return LIST_SUCCESS;
}

uint8_t list_delete_cursor_item(list_t *list_ptr, node_t *item_ptr);

uint8_t list_destroy_list(list_t *list_ptr);
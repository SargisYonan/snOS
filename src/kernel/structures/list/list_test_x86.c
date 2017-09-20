// list_test_x86.c

#include <stdio.h>

int main (void) {
	list_t *test_list_1 = NULL;
	list_t *test_list_2 = NULL;
	uint8_t ret = 0;
	char *str;


	ret = list_create(test_list_1);
	printf("\nlist create 1 ret val = %d", ret);

	ret = list_create(test_list_2);
	printf("\nlist create 2 ret val = %d", ret);

	list_append(test_list_1, "1");
	list_append(test_list_1, "2");
	list_append(test_list_1, "3");

	list_move_cursor_to_head(test_list_1);

	list_get_cursor_data(test_list_1, str);
	printf("\nitem 1: %c", str);
	list_move_cursor_right(test_list_1);

	list_get_cursor_data(test_list_1, str);
	printf("\nitem 2: %c", str);
	list_move_cursor_right(test_list_1);
	
	list_get_cursor_data(test_list_1, str);
	printf("\nitem 3: %c", str);
	
}
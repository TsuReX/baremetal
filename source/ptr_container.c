/*
 * ptr_container.c
 *
 *  Created on: Jan 13, 2021
 *      Author: user
 */
#include <stdint.h>
#include <stddef.h>
#include "ptr_container.h"

#define PTR_LIST_MAX_SIZE 16
uint32_t pc_read_num = 0;
uint32_t pc_write_num = 0;
struct ptr_container pc_ptr_list[PTR_LIST_MAX_SIZE];
size_t pc_ptr_list_size = 0;

void pc_init(void **ptr_list, size_t ptr_list_size)
{
	pc_ptr_list_size = ptr_list_size;
	uint32_t i = 0;
	for (; i < ptr_list_size; ++i) {
		pc_ptr_list[i].data_ptr = ptr_list[i];
		pc_ptr_list[i].data_state = empty;
		pc_ptr_list[i].lock_state = unlocked;
	}
}

int32_t pc_read_get(void **ptr)
{
	/* unlock->lock */
	if (pc_ptr_list[pc_read_num].lock_state == locked)
		return -1;

	if (pc_ptr_list[pc_read_num].data_state == empty)
		return -1;

	pc_ptr_list[pc_read_num].lock_state = locked;

	*ptr = pc_ptr_list[pc_read_num].data_ptr;

	return 0;
}

int32_t pc_read_set(void)
{
	/* lock->unlock */
	if (pc_ptr_list[pc_read_num].lock_state == unlocked)
		return -1;

	if (pc_ptr_list[pc_read_num].data_state == full)
		return -1;

	pc_ptr_list[pc_read_num].lock_state = unlocked;
	pc_ptr_list[pc_read_num].data_state = empty;

	pc_read_num = (pc_read_num + 1) % pc_ptr_list_size;

	return 0;
}

int32_t pc_read_cancel(void)
{
	/* lock->unlock */
	/* full->full */
	if (pc_ptr_list[pc_read_num].lock_state == unlocked)
		return -1;

	pc_ptr_list[pc_read_num].lock_state = unlocked;
	pc_ptr_list[pc_read_num].data_state = full;

	return 0;
}

int32_t pc_write_get(void **ptr)
{
	/* unlock->lock */
	if (pc_ptr_list[pc_write_num].lock_state == locked)
		return -1;

	if (pc_ptr_list[pc_write_num].data_state == full)
		return -1;

	pc_ptr_list[pc_write_num].lock_state = locked;

	*ptr = pc_ptr_list[pc_write_num].data_ptr;

	return 0;
}

int32_t pc_write_set(void)
{
	/* lock->unlock */
	if (pc_ptr_list[pc_write_num].lock_state == unlocked)
		return -1;

	if (pc_ptr_list[pc_write_num].data_state == full)
		return -1;

	pc_ptr_list[pc_write_num].lock_state = unlocked;
	pc_ptr_list[pc_write_num].data_state = full;

	pc_write_num = (pc_write_num + 1) % pc_ptr_list_size;

	return 0;
}

int32_t pc_write_cancel(void)
{
	/* lock->unlock */
	/* empty->empty */
	if (pc_ptr_list[pc_write_num].lock_state == unlocked)
		return -1;

	if (pc_ptr_list[pc_write_num].data_state == full)
		return -1;

	pc_ptr_list[pc_write_num].lock_state = unlocked;
	pc_ptr_list[pc_write_num].data_state = empty;

	return 0;
}

uint32_t pc_write_size_get(void)
{
	return 0;
}

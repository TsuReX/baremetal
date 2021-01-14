/*
 * ptr_container.c
 *
 *  Created on: Jan 13, 2021
 *      Author: user
 */
#include <stdint.h>
#include <stddef.h>
#include "ptr_container.h"

#define SEQ_RING_PTR_LIST_MAX_SIZE 16
uint32_t srpl_read_pos = 0;
uint32_t srpl_write_pos = 0;
struct ptr_container srp_list[SEQ_RING_PTR_LIST_MAX_SIZE];
size_t srpl_size = 0;

void srpl_init(void **ptr_list, size_t ptr_list_size)
{
	srpl_size = ptr_list_size;
	uint32_t i = 0;
	for (; i < ptr_list_size; ++i) {
		srp_list[i].data_ptr = ptr_list[i];
		srp_list[i].data_state = empty;
		srp_list[i].lock_state = unlocked;
	}
	srpl_read_pos = 0;
	srpl_write_pos = 0;
}

int32_t srpl_read_get(void **ptr)
{
	/* unlock->lock */

	if (srp_list[srpl_read_pos].data_state == empty)
		return -SRPL_READ_EMPTY;

	if (srp_list[srpl_read_pos].lock_state == locked)
		return -SRPL_READ_LOCKED;

	srp_list[srpl_read_pos].lock_state = locked;

	*ptr = srp_list[srpl_read_pos].data_ptr;

	return SRPL_READ_OK;
}

int32_t srpl_read_set(void)
{
	/* lock->unlock */
	if (srp_list[srpl_read_pos].lock_state == unlocked)
		return -SRPL_READ_UNLOCKED;

//	if (pc_ptr_list[pc_read_pos].data_state == empty)
//		return -PC_READ_EMPTY;

	srp_list[srpl_read_pos].lock_state = unlocked;
	srp_list[srpl_read_pos].data_state = empty;

	if (srpl_read_pos != srpl_write_pos)
		srpl_read_pos = (srpl_read_pos + 1) % srpl_size;

	return SRPL_READ_OK;
}

int32_t srpl_read_cancel(void)
{
	/* lock->unlock */
	/* full->full */
	if (srp_list[srpl_read_pos].lock_state == unlocked)
		return -SRPL_READ_UNLOCKED;

	srp_list[srpl_read_pos].lock_state = unlocked;
//	pc_ptr_list[pc_read_pos].data_state = full;

	return SRPL_READ_OK;
}

size_t srpl_read_size_get(void)
{
	if (srpl_write_pos == srpl_read_pos &&
		(srp_list[srpl_read_pos].data_state == empty ||
		srp_list[srpl_read_pos].lock_state == locked)) {

		return 0;
	} else {
		return 1;
	}

	if (srp_list[srpl_read_pos].lock_state == locked)
		return (srpl_write_pos - srpl_read_pos) % srpl_size - 1;
	else
		return (srpl_write_pos - srpl_read_pos) % srpl_size;
}

int32_t srpl_write_get(void **ptr)
{
	/* unlock->lock */

	if (srp_list[srpl_write_pos].data_state == full)
		return -SRPL_WRITE_FULL;

	if (srp_list[srpl_write_pos].lock_state == locked)
		return -SRPL_WRITE_LOCKED;

	srp_list[srpl_write_pos].lock_state = locked;

	*ptr = srp_list[srpl_write_pos].data_ptr;

	return SRPL_WRITE_OK;
}

int32_t srpl_write_set(void)
{
	/* lock->unlock */
	if (srp_list[srpl_write_pos].lock_state == unlocked)
		return -SRPL_WRITE_UNLOCKED;

//	if (pc_ptr_list[pc_write_pos].data_state == full)
//		return -1;

	srp_list[srpl_write_pos].lock_state = unlocked;
	srp_list[srpl_write_pos].data_state = full;

	if (srpl_write_pos != srpl_read_pos)
		srpl_write_pos = (srpl_write_pos + 1) % srpl_size;

	return SRPL_WRITE_OK;
}

int32_t srpl_write_cancel(void)
{
	/* lock->unlock */
	/* empty->empty */
	if (srp_list[srpl_write_pos].lock_state == unlocked)
		return -SRPL_WRITE_UNLOCKED;

//	if (pc_ptr_list[pc_write_pos].data_state == full)
//		return -1;

	srp_list[srpl_write_pos].lock_state = unlocked;
//	srp_list[srpl_write_pos].data_state = empty;

	return SRPL_WRITE_OK;
}

size_t srpl_write_size_get(void)
{
	if (srpl_write_pos == srpl_read_pos &&
		(srp_list[srpl_write_pos].data_state == full ||
		srp_list[srpl_write_pos].lock_state == locked)) {

		return 0;
	} else {
		return 1;
	}

	if (srp_list[srpl_write_pos].lock_state == locked)
		return (srpl_size) - ((srpl_write_pos - srpl_read_pos) % srpl_size) - 1;
	else
		return (srpl_size) - ((srpl_write_pos - srpl_read_pos) % srpl_size);
}

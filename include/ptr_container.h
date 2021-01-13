/*
 * ptr_container.h
 *
 *  Created on: Jan 13, 2021
 *      Author: user
 */

#ifndef _PTR_CONTAINER_H_
#define _PTR_CONTAINER_H_

enum lock_state {
	unlocked = 0,
	locked
};

enum data_state {
	empty = 0,
	full
};

struct ptr_container {
	void	*data_ptr;
	enum	data_state data_state;
	enum	lock_state lock_state;
};

void pc_init(void **ptr_list, size_t ptr_list_size);

int32_t pc_read_get(void **ptr);
int32_t pc_read_set(void);
int32_t pc_read_cancel(void);
uint32_t pc_read_size_get(void);

int32_t pc_write_get(void **ptr);
int32_t pc_write_set(void);
int32_t pc_write_cancel(void);
uint32_t pc_write_size_get(void);

#endif /* _PTR_CONTAINER_H_ */

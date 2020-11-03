/*
 * communication.h
 *
 *  Created on: Sep 8, 2020
 *      Author: user
 */

#ifndef INCLUDE_COMMUNICATION_H_
#define INCLUDE_COMMUNICATION_H_

#include <stdint.h>
#include <stddef.h>

#include "drivers.h"

void comm_init(void* dst_buffer, size_t dst_buffer_size);
void comm_start(void);
void comm_stop(void);

#endif /* INCLUDE_COMMUNICATION_H_ */

#include <dbg.h>
#include <io.h>

void compare_and_stop (unsigned int value_1, unsigned int value_2, char error_code) {
    if (value_1 != value_2)
	while(1) {
	    outb(0xEE, 0x80);
	    outb(error_code, 0x80);
	}
}

void d_print(unsigned int data) {
    char* data_arr = (char*)&data;

    outb(0xB0, 0x80);
    outb(*(data_arr + 0), 0x80);

    outb(0xB1, 0x80);
    outb(*(data_arr + 1), 0x80);

    outb(0xB2, 0x80);
    outb(*(data_arr + 2), 0x80);

    outb(0xB3, 0x80);
    outb(*(data_arr + 3), 0x80);
}
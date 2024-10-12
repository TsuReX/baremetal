extern func_4
extern func_5
extern func_6
extern func_7
extern func_8
extern func_9
extern return_addr


section  .text.external

external_code:

call func_4

push eax
mov eax, 0x15
out 0x80, al
pop eax

jz short got_to_func_7
call func_5

push eax
mov eax, 0x25
out 0x80, al
pop eax

call func_6

push eax
mov eax, 0x35
out 0x80, al
pop eax

got_to_func_7:
call func_7

push eax
mov eax, 0x45
out 0x80, al
pop eax

call func_8

push eax
mov eax, 0x55
out 0x80, al
pop eax

call func_9

push eax
mov eax, 0x65
out 0x80, al
pop eax

jmp return_addr
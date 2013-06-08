ldr r0,=0x20200000
mov r1,#1
lsl r1,#18

str r1,[r0,#4]

mov r1,#1
lsl r1,#16

test:

mov r3,#0xFF0000

mov r4,#0xFF0000

str r1,[r0,#40]

loop1:
sub r3,r3,#1
cmp r3,#0
bne loop1

str r1,[r0,#28]

blah:
sub r4,r4,#1
cmp r4,#0
bne blah

b test
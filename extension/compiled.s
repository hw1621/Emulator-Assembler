
setPixel:
push { r4 }
push { r3 }
push { r2 }
push { r1 }
ldr r1, [sp, #28]
ldr r2, =0
cmp r1, r2
blt l9
ldr r1, [sp, #28]
ldr r2, [sp, #20]
cmp r1, r2
bge l8
ldr r1, [sp, #32]
ldr r2, =0
cmp r1, r2
blt l7
ldr r1, [sp, #32]
ldr r2, [sp, #24]
cmp r1, r2
bge l6
ldr r1, [sp, #36]
ldr r2, =0
cmp r1, r2
bge l0
add r1, sp, #36
ldr r2, =0
str r2, [r1]
l0:
ldr r1, [sp, #36]
ldr r2, =255
cmp r1, r2
ble l1
add r1, sp, #36
ldr r2, =255
str r2, [r1]
l1:
ldr r1, [sp, #40]
ldr r2, =0
cmp r1, r2
bge l2
add r1, sp, #40
ldr r2, =0
str r2, [r1]
l2:
ldr r1, [sp, #40]
ldr r2, =255
cmp r1, r2
ble l3
add r1, sp, #40
ldr r2, =255
str r2, [r1]
l3:
ldr r1, [sp, #44]
ldr r2, =0
cmp r1, r2
bge l4
add r1, sp, #44
ldr r2, =0
str r2, [r1]
l4:
ldr r1, [sp, #44]
ldr r2, =255
cmp r1, r2
ble l5
add r1, sp, #44
ldr r2, =255
str r2, [r1]
l5:
ldr r1, [sp, #16]
ldr r2, [sp, #32]
ldr r3, [sp, #20]
mul r2, r2, r3
ldr r3, [sp, #28]
add r2, r2, r3
add r1, r1, r2, lsl #2
ldr r2, [sp, #36]
ldr r3, [sp, #40]
ldr r4, =8
mov r3, r3, lsl r4
add r2, r2, r3
ldr r3, [sp, #44]
ldr r4, =16
mov r3, r3, lsl r4
add r2, r2, r3
str r2, [r1]
l6:
l7:
l8:
l9:
mov r0, r1
pop { r1 }
pop { r2 }
pop { r3 }
pop { r4 }
bx lr


trace:
push { lr }
push { r3 }
push { r2 }
push { r1 }
ldr r1, =1
push { r1 }
ldr r1, =0
push { r1 }
ldr r1, =0
push { r1 }
ldr r1, =0
push { r1 }
ldr r1, [sp, #48]
push { r1 }
bl normalise
mov r1, r0
add sp, sp, #4
l16:
ldr r1, [sp, #8]
ldr r2, [sp, #32]
cmp r1, r2
bge l17
sub sp, sp, #12
add r1, sp, #0
push { r1 }
ldr r1, [sp, #52]
ldr r2, [sp, #24]
ldr r3, =4
mul r2, r2, r3
add r1, r1, r2
push { r1 }
ldr r1, [sp, #64]
push { r1 }
bl vsub
mov r1, r0
add sp, sp, #12
ldr r1, =65536
push { r1 }
ldr r1, [sp, #64]
push { r1 }
add r1, sp, #8
push { r1 }
bl vdot
mov r1, r0
add sp, sp, #8
ldr r2, =1
mov r1, r1, lsl r2
push { r1 }
add r1, sp, #8
push { r1 }
add r1, sp, #12
push { r1 }
bl vdot
mov r1, r0
add sp, sp, #8
ldr r2, =65536
sub r1, r1, r2
push { r1 }
ldr r1, [sp, #4]
push { r1 }
ldr r1, [sp, #8]
push { r1 }
bl fmul
mov r1, r0
add sp, sp, #8
ldr r2, [sp, #0]
ldr r3, =2
mov r2, r2, lsl r3
sub r1, r1, r2
push { r1 }
ldr r1, [sp, #0]
ldr r2, =0
cmp r1, r2
blt l15
add r1, sp, #0
ldr r2, [sp, #0]
push { r2 }
bl fsqrt
mov r2, r0
add sp, sp, #4
str r2, [r1]
ldr r1, [sp, #8]
rsb r1, r1, #0
ldr r2, [sp, #0]
sub r1, r1, r2
ldr r2, =1
mov r1, r1, asr r2
push { r1 }
ldr r1, [sp, #12]
rsb r1, r1, #0
ldr r2, [sp, #4]
add r1, r1, r2
ldr r2, =1
mov r1, r1, asr r2
push { r1 }
ldr r1, [sp, #4]
push { r1 }
ldr r1, [sp, #0]
ldr r2, =64
cmp r1, r2
bgt l10
add r1, sp, #0
ldr r2, [sp, #4]
str r2, [r1]
l10:
ldr r1, [sp, #0]
ldr r2, =64
cmp r1, r2
ble l14
ldr r1, [sp, #52]
tst r1, r1
beq l12
add r1, sp, #52
ldr r2, =0
str r2, [r1]
add r1, sp, #44
ldr r2, [sp, #0]
str r2, [r1]
add r1, sp, #40
ldr r2, [sp, #48]
str r2, [r1]
b l13
l12:
ldr r1, [sp, #0]
ldr r2, [sp, #44]
cmp r1, r2
bge l11
add r1, sp, #44
ldr r2, [sp, #0]
str r2, [r1]
add r1, sp, #40
ldr r2, [sp, #48]
str r2, [r1]
l11:
l13:
l14:
add sp, sp, #12
l15:
add r1, sp, #36
ldr r2, [sp, #36]
ldr r3, =3
add r2, r2, r3
str r2, [r1]
add sp, sp, #28
b l16
l17:
ldr r1, [sp, #12]
ldr r2, =0
cmp r1, r2
bne l25
ldr r1, [sp, #52]
push { r1 }
ldr r1, [sp, #44]
ldr r2, [sp, #4]
ldr r3, =4
mul r2, r2, r3
add r1, r1, r2
push { r1 }
ldr r1, [sp, #60]
push { r1 }
bl vmul
mov r1, r0
add sp, sp, #12
ldr r1, [sp, #56]
ldr r2, =8
cmp r1, r2
bge l24
ldr r1, [sp, #44]
push { r1 }
ldr r1, [sp, #48]
push { r1 }
ldr r1, [sp, #56]
push { r1 }
ldr r1, [sp, #16]
push { r1 }
bl svmla
mov r1, r0
add sp, sp, #16
sub sp, sp, #12
add r1, sp, #0
push { r1 }
ldr r1, [sp, #52]
ldr r2, [sp, #16]
ldr r3, =4
mul r2, r2, r3
add r1, r1, r2
push { r1 }
ldr r1, [sp, #64]
push { r1 }
bl vsub
mov r1, r0
add sp, sp, #12
add r1, sp, #0
push { r1 }
bl normalise
mov r1, r0
add sp, sp, #4
add r1, sp, #0
push { r1 }
ldr r1, [sp, #64]
push { r1 }
bl vdot
mov r1, r0
add sp, sp, #8
push { r1 }
ldr r1, [sp, #16]
ldr r2, =1
and r1, r1, r2
tst r1, r1
beq l22
ldr r1, =65536
ldr r2, [sp, #0]
push { r2 }
ldr r2, [sp, #4]
push { r2 }
bl fmul
mov r2, r0
add sp, sp, #8
sub r1, r1, r2
push { r1 }
bl fsqrt
mov r1, r0
add sp, sp, #4
push { r1 }
ldr r1, [sp, #4]
ldr r2, =0
cmp r1, r2
bge l20
ldr r1, =147456
push { r1 }
bl finv
mov r1, r0
add sp, sp, #4
push { r1 }
ldr r1, [sp, #4]
push { r1 }
bl fmul
mov r1, r0
add sp, sp, #8
push { r1 }
ldr r1, =65536
ldr r2, [sp, #0]
push { r2 }
ldr r2, [sp, #4]
push { r2 }
bl fmul
mov r2, r0
add sp, sp, #8
sub r1, r1, r2
push { r1 }
bl fsqrt
mov r1, r0
add sp, sp, #4
push { r1 }
sub sp, sp, #12
add r1, sp, #0
push { r1 }
ldr r1, [sp, #92]
push { r1 }
add r1, sp, #36
push { r1 }
ldr r1, [sp, #36]
rsb r1, r1, #0
push { r1 }
bl svmla
mov r1, r0
add sp, sp, #16
add r1, sp, #0
push { r1 }
bl normalise
mov r1, r0
add sp, sp, #4
ldr r1, [sp, #88]
push { r1 }
add r1, sp, #4
push { r1 }
ldr r1, [sp, #24]
push { r1 }
bl svmul
mov r1, r0
add sp, sp, #12
ldr r1, [sp, #88]
push { r1 }
ldr r1, [sp, #92]
push { r1 }
add r1, sp, #36
push { r1 }
ldr r1, [sp, #24]
rsb r1, r1, #0
push { r1 }
bl svmla
mov r1, r0
add sp, sp, #36
b l21
l20:
ldr r1, =147456
push { r1 }
ldr r1, [sp, #4]
push { r1 }
bl fmul
mov r1, r0
add sp, sp, #8
push { r1 }
ldr r1, [sp, #0]
ldr r2, =65536
cmp r1, r2
bgt l18
ldr r1, =65536
ldr r2, [sp, #0]
push { r2 }
ldr r2, [sp, #4]
push { r2 }
bl fmul
mov r2, r0
add sp, sp, #8
sub r1, r1, r2
push { r1 }
bl fsqrt
mov r1, r0
add sp, sp, #4
push { r1 }
sub sp, sp, #12
add r1, sp, #0
push { r1 }
ldr r1, [sp, #92]
push { r1 }
add r1, sp, #36
push { r1 }
ldr r1, [sp, #36]
rsb r1, r1, #0
push { r1 }
bl svmla
mov r1, r0
add sp, sp, #16
add r1, sp, #0
push { r1 }
bl normalise
mov r1, r0
add sp, sp, #4
ldr r1, [sp, #88]
push { r1 }
add r1, sp, #4
push { r1 }
ldr r1, [sp, #24]
push { r1 }
bl svmul
mov r1, r0
add sp, sp, #12
ldr r1, [sp, #88]
push { r1 }
ldr r1, [sp, #92]
push { r1 }
add r1, sp, #36
push { r1 }
ldr r1, [sp, #24]
push { r1 }
bl svmla
mov r1, r0
add sp, sp, #32
b l19
l18:
add r1, sp, #12
push { r1 }
add r1, sp, #16
push { r1 }
ldr r1, [sp, #16]
rsb r1, r1, #0
ldr r2, =1
mov r1, r1, lsl r2
push { r1 }
bl svmul
mov r1, r0
add sp, sp, #12
ldr r1, [sp, #72]
push { r1 }
add r1, sp, #16
push { r1 }
ldr r1, [sp, #80]
push { r1 }
bl vadd
mov r1, r0
add sp, sp, #12
l19:
add sp, sp, #4
l21:
add sp, sp, #4
b l23
l22:
add r1, sp, #4
push { r1 }
add r1, sp, #8
push { r1 }
ldr r1, [sp, #8]
rsb r1, r1, #0
ldr r2, =1
mov r1, r1, lsl r2
push { r1 }
bl svmul
mov r1, r0
add sp, sp, #12
ldr r1, [sp, #64]
push { r1 }
add r1, sp, #8
push { r1 }
ldr r1, [sp, #72]
push { r1 }
bl vadd
mov r1, r0
add sp, sp, #12
l23:
ldr r1, [sp, #72]
ldr r2, =1
add r1, r1, r2
push { r1 }
ldr r1, [sp, #72]
push { r1 }
ldr r1, [sp, #72]
push { r1 }
ldr r1, [sp, #72]
push { r1 }
ldr r1, [sp, #72]
push { r1 }
ldr r1, [sp, #72]
push { r1 }
ldr r1, [sp, #72]
push { r1 }
bl trace
mov r1, r0
add sp, sp, #44
l24:
l25:
ldr r1, [sp, #12]
tst r1, r1
beq l34
ldr r1, [sp, #48]
ldr r2, =1
add r1, r1, r2, lsl #2
ldr r1, [r1]
ldr r2, =0
cmp r1, r2
bge l33
ldr r1, [sp, #48]
ldr r2, =1
add r1, r1, r2, lsl #2
ldr r1, [r1]
rsb r1, r1, #0
push { r1 }
bl finv
mov r1, r0
add sp, sp, #4
push { r1 }
ldr r1, [sp, #48]
ldr r2, =1
add r1, r1, r2, lsl #2
ldr r1, [r1]
push { r1 }
bl fmul
mov r1, r0
add sp, sp, #8
push { r1 }
ldr r1, [sp, #48]
push { r1 }
ldr r1, [sp, #52]
push { r1 }
ldr r1, [sp, #60]
push { r1 }
ldr r1, [sp, #12]
push { r1 }
bl svmla
mov r1, r0
add sp, sp, #16
ldr r1, [sp, #48]
ldr r2, =0
add r1, r1, r2, lsl #2
ldr r1, [r1]
ldr r2, =327680
rsb r2, r2, #0
cmp r1, r2
blt l32
ldr r1, [sp, #48]
ldr r2, =0
add r1, r1, r2, lsl #2
ldr r1, [r1]
ldr r2, =327680
cmp r1, r2
bgt l31
ldr r1, [sp, #48]
ldr r2, =2
add r1, r1, r2, lsl #2
ldr r1, [r1]
ldr r2, =327680
rsb r2, r2, #0
cmp r1, r2
blt l30
ldr r1, [sp, #48]
ldr r2, =2
add r1, r1, r2, lsl #2
ldr r1, [r1]
ldr r2, =327680
cmp r1, r2
bgt l29
add r1, sp, #16
ldr r2, =0
str r2, [r1]
ldr r1, [sp, #48]
ldr r2, =0
add r1, r1, r2, lsl #2
ldr r1, [r1]
ldr r2, =65536
and r1, r1, r2
ldr r2, [sp, #48]
ldr r3, =2
add r2, r2, r3, lsl #2
ldr r2, [r2]
ldr r3, =65536
and r2, r2, r3
eor r1, r1, r2
tst r1, r1
beq l26
ldr r1, [sp, #56]
push { r1 }
ldr r1, [sp, #60]
push { r1 }
ldr r1, =32768
push { r1 }
bl svmul
mov r1, r0
add sp, sp, #12
b l27
l26:
ldr r1, [sp, #56]
push { r1 }
ldr r1, [sp, #60]
push { r1 }
ldr r1, =16384
push { r1 }
bl svmul
mov r1, r0
add sp, sp, #12
l27:
ldr r1, [sp, #60]
ldr r2, =8
cmp r1, r2
bge l28
ldr r1, [sp, #52]
ldr r2, =1
add r1, r1, r2, lsl #2
ldr r2, [sp, #52]
ldr r3, =1
add r2, r2, r3, lsl #2
ldr r2, [r2]
rsb r2, r2, #0
str r2, [r1]
ldr r1, [sp, #60]
ldr r2, =1
add r1, r1, r2
push { r1 }
ldr r1, [sp, #60]
push { r1 }
ldr r1, [sp, #60]
push { r1 }
ldr r1, [sp, #60]
push { r1 }
ldr r1, [sp, #60]
push { r1 }
ldr r1, [sp, #60]
push { r1 }
ldr r1, [sp, #60]
push { r1 }
bl trace
mov r1, r0
add sp, sp, #28
l28:
l29:
l30:
l31:
l32:
add sp, sp, #4
l33:
l34:
ldr r1, [sp, #12]
tst r1, r1
beq l35
ldr r1, =20480
push { r1 }
ldr r1, [sp, #52]
ldr r2, =1
add r1, r1, r2, lsl #2
ldr r1, [r1]
ldr r2, =65536
add r1, r1, r2
push { r1 }
bl fmul
mov r1, r0
add sp, sp, #8
ldr r2, =12288
add r1, r1, r2
push { r1 }
ldr r1, [sp, #56]
push { r1 }
ldr r1, [sp, #60]
push { r1 }
ldr r1, [sp, #8]
push { r1 }
bl svmul
mov r1, r0
add sp, sp, #12
add r1, sp, #16
ldr r2, =0
str r2, [r1]
add sp, sp, #4
l35:
add sp, sp, #16
mov r0, r1
pop { r1 }
pop { r2 }
pop { r3 }
pop { lr }
bx lr


fsqrt:
push { lr }
push { r3 }
push { r2 }
push { r1 }
ldr r1, =0
push { r1 }
ldr r1, [sp, #20]
ldr r2, =0
cmp r1, r2
ble l40
ldr r1, =0
push { r1 }
ldr r1, =0
push { r1 }
l36:
ldr r1, [sp, #28]
ldr r2, =32768
cmp r1, r2
bge l37
add r1, sp, #28
ldr r2, [sp, #28]
ldr r3, =2
mov r2, r2, lsl r3
str r2, [r1]
add r1, sp, #4
ldr r2, [sp, #4]
ldr r3, =1
add r2, r2, r3
str r2, [r1]
b l36
l37:
l38:
ldr r1, [sp, #28]
ldr r2, =131072
cmp r1, r2
ble l39
add r1, sp, #28
ldr r2, [sp, #28]
ldr r3, =2
mov r2, r2, asr r3
str r2, [r1]
add r1, sp, #0
ldr r2, [sp, #0]
ldr r3, =1
add r2, r2, r3
str r2, [r1]
b l38
l39:
add r1, sp, #8
ldr r2, [sp, #28]
str r2, [r1]
add r1, sp, #8
ldr r2, [sp, #8]
ldr r3, [sp, #8]
push { r3 }
bl finv
mov r3, r0
add sp, sp, #4
push { r3 }
ldr r3, [sp, #32]
push { r3 }
bl fmul
mov r3, r0
add sp, sp, #8
add r2, r2, r3
ldr r3, =1
mov r2, r2, asr r3
str r2, [r1]
add r1, sp, #8
ldr r2, [sp, #8]
ldr r3, [sp, #8]
push { r3 }
bl finv
mov r3, r0
add sp, sp, #4
push { r3 }
ldr r3, [sp, #32]
push { r3 }
bl fmul
mov r3, r0
add sp, sp, #8
add r2, r2, r3
ldr r3, =1
mov r2, r2, asr r3
str r2, [r1]
add r1, sp, #8
ldr r2, [sp, #8]
ldr r3, [sp, #8]
push { r3 }
bl finv
mov r3, r0
add sp, sp, #4
push { r3 }
ldr r3, [sp, #32]
push { r3 }
bl fmul
mov r3, r0
add sp, sp, #8
add r2, r2, r3
ldr r3, =1
mov r2, r2, asr r3
str r2, [r1]
add r1, sp, #8
ldr r2, [sp, #8]
ldr r3, [sp, #0]
mov r2, r2, lsl r3
ldr r3, [sp, #4]
mov r2, r2, asr r3
str r2, [r1]
add sp, sp, #8
l40:
add r1, sp, #0
add sp, sp, #4
ldr r1, [r1]
mov r0, r1
pop { r1 }
pop { r2 }
pop { r3 }
pop { lr }
bx lr


normalise:
push { lr }
push { r1 }
ldr r1, [sp, #8]
push { r1 }
ldr r1, [sp, #12]
push { r1 }
ldr r1, [sp, #16]
push { r1 }
ldr r1, [sp, #20]
push { r1 }
bl vdot
mov r1, r0
add sp, sp, #8
push { r1 }
bl fsqrt
mov r1, r0
add sp, sp, #4
push { r1 }
bl finv
mov r1, r0
add sp, sp, #4
push { r1 }
bl svmul
mov r1, r0
add sp, sp, #12
mov r0, r1
pop { r1 }
pop { lr }
bx lr


fsin:
push { lr }
push { r3 }
push { r2 }
push { r1 }
ldr r1, =0
push { r1 }
add r1, sp, #20
ldr r2, =20861
push { r2 }
ldr r2, [sp, #24]
push { r2 }
bl fmul
mov r2, r0
add sp, sp, #8
ldr r3, =32768
add r2, r2, r3
str r2, [r1]
ldr r1, [sp, #20]
ldr r2, =65536
and r1, r1, r2
tst r1, r1
beq l41
add r1, sp, #0
ldr r2, =1
str r2, [r1]
l41:
add r1, sp, #20
ldr r2, [sp, #20]
ldr r3, =65535
and r2, r2, r3
str r2, [r1]
add r1, sp, #20
ldr r2, =205887
push { r2 }
ldr r2, [sp, #24]
ldr r3, =32768
sub r2, r2, r3
push { r2 }
bl fmul
mov r2, r0
add sp, sp, #8
str r2, [r1]
ldr r1, [sp, #20]
push { r1 }
ldr r1, [sp, #24]
push { r1 }
ldr r1, [sp, #28]
push { r1 }
bl fmul
mov r1, r0
add sp, sp, #8
push { r1 }
ldr r1, [sp, #0]
push { r1 }
ldr r1, [sp, #32]
push { r1 }
bl fmul
mov r1, r0
add sp, sp, #8
push { r1 }
add r1, sp, #8
ldr r2, [sp, #8]
ldr r3, =10923
push { r3 }
ldr r3, [sp, #4]
push { r3 }
bl fmul
mov r3, r0
add sp, sp, #8
sub r2, r2, r3
str r2, [r1]
add r1, sp, #0
ldr r2, [sp, #4]
push { r2 }
ldr r2, [sp, #4]
push { r2 }
bl fmul
mov r2, r0
add sp, sp, #8
str r2, [r1]
add r1, sp, #8
ldr r2, [sp, #8]
ldr r3, =546
push { r3 }
ldr r3, [sp, #4]
push { r3 }
bl fmul
mov r3, r0
add sp, sp, #8
add r2, r2, r3
str r2, [r1]
ldr r1, [sp, #12]
tst r1, r1
beq l42
add r1, sp, #8
ldr r2, [sp, #8]
rsb r2, r2, #0
str r2, [r1]
l42:
add r1, sp, #8
add sp, sp, #16
ldr r1, [r1]
mov r0, r1
pop { r1 }
pop { r2 }
pop { r3 }
pop { lr }
bx lr


fcos:
push { lr }
push { r3 }
push { r2 }
push { r1 }
ldr r1, =0
push { r1 }
add r1, sp, #20
ldr r2, =20861
push { r2 }
ldr r2, [sp, #24]
push { r2 }
bl fmul
mov r2, r0
add sp, sp, #8
ldr r3, =32768
add r2, r2, r3
str r2, [r1]
ldr r1, [sp, #20]
ldr r2, =65536
and r1, r1, r2
tst r1, r1
beq l43
add r1, sp, #0
ldr r2, =1
str r2, [r1]
l43:
add r1, sp, #20
ldr r2, [sp, #20]
ldr r3, =65535
and r2, r2, r3
str r2, [r1]
add r1, sp, #20
ldr r2, =205887
push { r2 }
ldr r2, [sp, #24]
ldr r3, =32768
sub r2, r2, r3
push { r2 }
bl fmul
mov r2, r0
add sp, sp, #8
str r2, [r1]
ldr r1, =65536
push { r1 }
ldr r1, [sp, #24]
push { r1 }
ldr r1, [sp, #28]
push { r1 }
bl fmul
mov r1, r0
add sp, sp, #8
push { r1 }
ldr r1, [sp, #0]
push { r1 }
add r1, sp, #8
ldr r2, [sp, #8]
ldr r3, =32768
push { r3 }
ldr r3, [sp, #4]
push { r3 }
bl fmul
mov r3, r0
add sp, sp, #8
sub r2, r2, r3
str r2, [r1]
add r1, sp, #0
ldr r2, [sp, #4]
push { r2 }
ldr r2, [sp, #4]
push { r2 }
bl fmul
mov r2, r0
add sp, sp, #8
str r2, [r1]
add r1, sp, #8
ldr r2, [sp, #8]
ldr r3, =2731
push { r3 }
ldr r3, [sp, #4]
push { r3 }
bl fmul
mov r3, r0
add sp, sp, #8
add r2, r2, r3
str r2, [r1]
ldr r1, [sp, #12]
tst r1, r1
beq l44
add r1, sp, #8
ldr r2, [sp, #8]
rsb r2, r2, #0
str r2, [r1]
l44:
add r1, sp, #8
add sp, sp, #16
ldr r1, [r1]
mov r0, r1
pop { r1 }
pop { r2 }
pop { r3 }
pop { lr }
bx lr


testPlot:
push { lr }
push { r3 }
push { r2 }
push { r1 }
ldr r1, [sp, #20]
ldr r2, =1
mov r1, r1, asr r2
push { r1 }
ldr r1, [sp, #28]
ldr r2, =1
mov r1, r1, asr r2
push { r1 }
ldr r1, =0
push { r1 }
l51:
ldr r1, [sp, #0]
ldr r2, [sp, #32]
cmp r1, r2
bge l52
ldr r1, =0
push { r1 }
l49:
ldr r1, [sp, #0]
ldr r2, [sp, #40]
cmp r1, r2
bge l50
ldr r1, [sp, #8]
ldr r2, [sp, #0]
sub r1, r1, r2
ldr r2, =31
and r1, r1, r2
ldr r2, =0
cmp r1, r2
bne l45
ldr r1, =128
push { r1 }
ldr r1, =128
push { r1 }
ldr r1, =128
push { r1 }
ldr r1, [sp, #12]
push { r1 }
ldr r1, [sp, #20]
push { r1 }
ldr r1, [sp, #60]
push { r1 }
ldr r1, [sp, #60]
push { r1 }
ldr r1, [sp, #60]
push { r1 }
bl setPixel
mov r1, r0
add sp, sp, #32
l45:
ldr r1, [sp, #4]
ldr r2, [sp, #12]
sub r1, r1, r2
ldr r2, =31
and r1, r1, r2
ldr r2, =0
cmp r1, r2
bne l46
ldr r1, =128
push { r1 }
ldr r1, =128
push { r1 }
ldr r1, =128
push { r1 }
ldr r1, [sp, #12]
push { r1 }
ldr r1, [sp, #20]
push { r1 }
ldr r1, [sp, #60]
push { r1 }
ldr r1, [sp, #60]
push { r1 }
ldr r1, [sp, #60]
push { r1 }
bl setPixel
mov r1, r0
add sp, sp, #32
l46:
ldr r1, [sp, #0]
ldr r2, [sp, #8]
cmp r1, r2
bne l47
ldr r1, =255
push { r1 }
ldr r1, =255
push { r1 }
ldr r1, =255
push { r1 }
ldr r1, [sp, #12]
push { r1 }
ldr r1, [sp, #20]
push { r1 }
ldr r1, [sp, #60]
push { r1 }
ldr r1, [sp, #60]
push { r1 }
ldr r1, [sp, #60]
push { r1 }
bl setPixel
mov r1, r0
add sp, sp, #32
l47:
ldr r1, [sp, #4]
ldr r2, [sp, #12]
cmp r1, r2
bne l48
ldr r1, =255
push { r1 }
ldr r1, =255
push { r1 }
ldr r1, =255
push { r1 }
ldr r1, [sp, #12]
push { r1 }
ldr r1, [sp, #20]
push { r1 }
ldr r1, [sp, #60]
push { r1 }
ldr r1, [sp, #60]
push { r1 }
ldr r1, [sp, #60]
push { r1 }
bl setPixel
mov r1, r0
add sp, sp, #32
l48:
add r1, sp, #0
ldr r2, [sp, #0]
ldr r3, =1
add r2, r2, r3
str r2, [r1]
b l49
l50:
add r1, sp, #4
ldr r2, [sp, #4]
ldr r3, =1
add r2, r2, r3
str r2, [r1]
add sp, sp, #4
b l51
l52:
add r1, sp, #0
ldr r2, =0
str r2, [r1]
l53:
ldr r1, [sp, #0]
ldr r2, [sp, #32]
cmp r1, r2
bge l54
ldr r1, =0
push { r1 }
ldr r1, =255
push { r1 }
ldr r1, =0
push { r1 }
ldr r1, [sp, #16]
ldr r2, [sp, #12]
ldr r3, [sp, #20]
sub r2, r2, r3
ldr r3, =11
mov r2, r2, lsl r3
push { r2 }
bl finv
mov r2, r0
add sp, sp, #4
ldr r3, =11
mov r2, r2, asr r3
sub r1, r1, r2
push { r1 }
ldr r1, [sp, #16]
push { r1 }
ldr r1, [sp, #56]
push { r1 }
ldr r1, [sp, #56]
push { r1 }
ldr r1, [sp, #56]
push { r1 }
bl setPixel
mov r1, r0
add sp, sp, #32
ldr r1, =0
push { r1 }
ldr r1, =0
push { r1 }
ldr r1, =255
push { r1 }
ldr r1, [sp, #16]
ldr r2, [sp, #12]
ldr r3, [sp, #20]
sub r2, r2, r3
ldr r3, =11
mov r2, r2, lsl r3
push { r2 }
bl fsin
mov r2, r0
add sp, sp, #4
ldr r3, =11
mov r2, r2, asr r3
sub r1, r1, r2
push { r1 }
ldr r1, [sp, #16]
push { r1 }
ldr r1, [sp, #56]
push { r1 }
ldr r1, [sp, #56]
push { r1 }
ldr r1, [sp, #56]
push { r1 }
bl setPixel
mov r1, r0
add sp, sp, #32
ldr r1, =255
push { r1 }
ldr r1, =0
push { r1 }
ldr r1, =0
push { r1 }
ldr r1, [sp, #16]
ldr r2, [sp, #12]
ldr r3, [sp, #20]
sub r2, r2, r3
ldr r3, =11
mov r2, r2, lsl r3
push { r2 }
bl fcos
mov r2, r0
add sp, sp, #4
ldr r3, =11
mov r2, r2, asr r3
sub r1, r1, r2
push { r1 }
ldr r1, [sp, #16]
push { r1 }
ldr r1, [sp, #56]
push { r1 }
ldr r1, [sp, #56]
push { r1 }
ldr r1, [sp, #56]
push { r1 }
bl setPixel
mov r1, r0
add sp, sp, #32
ldr r1, =255
push { r1 }
ldr r1, =0
push { r1 }
ldr r1, =255
push { r1 }
ldr r1, [sp, #16]
ldr r2, [sp, #12]
ldr r3, [sp, #20]
sub r2, r2, r3
ldr r3, =11
mov r2, r2, lsl r3
push { r2 }
bl fsqrt
mov r2, r0
add sp, sp, #4
ldr r3, =11
mov r2, r2, asr r3
sub r1, r1, r2
push { r1 }
ldr r1, [sp, #16]
push { r1 }
ldr r1, [sp, #56]
push { r1 }
ldr r1, [sp, #56]
push { r1 }
ldr r1, [sp, #56]
push { r1 }
bl setPixel
mov r1, r0
add sp, sp, #32
ldr r1, =255
push { r1 }
ldr r1, =0
push { r1 }
ldr r1, =255
push { r1 }
ldr r1, [sp, #16]
ldr r2, [sp, #12]
ldr r3, [sp, #20]
sub r2, r2, r3
ldr r3, =11
mov r2, r2, lsl r3
push { r2 }
bl fsqrt
mov r2, r0
add sp, sp, #4
push { r2 }
bl finv
mov r2, r0
add sp, sp, #4
ldr r3, =11
mov r2, r2, asr r3
sub r1, r1, r2
push { r1 }
ldr r1, [sp, #16]
push { r1 }
ldr r1, [sp, #56]
push { r1 }
ldr r1, [sp, #56]
push { r1 }
ldr r1, [sp, #56]
push { r1 }
bl setPixel
mov r1, r0
add sp, sp, #32
add r1, sp, #0
ldr r2, [sp, #0]
ldr r3, =1
add r2, r2, r3
str r2, [r1]
b l53
l54:
add sp, sp, #12
mov r0, r1
pop { r1 }
pop { r2 }
pop { r3 }
pop { lr }
bx lr


main:
push { lr }
push { r4 }
push { r3 }
push { r2 }
push { r1 }
ldr r1, [sp, #28]
ldr r2, =16
mov r1, r1, lsl r2
ldr r2, =1
mov r1, r1, asr r2
push { r1 }
bl finv
mov r1, r0
add sp, sp, #4
push { r1 }
sub sp, sp, #36
sub sp, sp, #36
add r1, sp, #36
ldr r2, =0
add r1, r1, r2, lsl #2
ldr r2, =98304
rsb r2, r2, #0
str r2, [r1]
add r1, sp, #36
ldr r2, =1
add r1, r1, r2, lsl #2
ldr r2, =71680
str r2, [r1]
add r1, sp, #36
ldr r2, =2
add r1, r1, r2, lsl #2
ldr r2, =98304
rsb r2, r2, #0
str r2, [r1]
add r1, sp, #36
ldr r2, =3
add r1, r1, r2, lsl #2
ldr r2, =98304
str r2, [r1]
add r1, sp, #36
ldr r2, =4
add r1, r1, r2, lsl #2
ldr r2, =71680
str r2, [r1]
add r1, sp, #36
ldr r2, =5
add r1, r1, r2, lsl #2
ldr r2, =32768
rsb r2, r2, #0
str r2, [r1]
add r1, sp, #36
ldr r2, =6
add r1, r1, r2, lsl #2
ldr r2, =65536
str r2, [r1]
add r1, sp, #36
ldr r2, =7
add r1, r1, r2, lsl #2
ldr r2, =71680
str r2, [r1]
add r1, sp, #36
ldr r2, =8
add r1, r1, r2, lsl #2
ldr r2, =196608
rsb r2, r2, #0
str r2, [r1]
add r1, sp, #0
ldr r2, =0
add r1, r1, r2, lsl #2
ldr r2, =32768
str r2, [r1]
add r1, sp, #0
ldr r2, =1
add r1, r1, r2, lsl #2
ldr r2, =45875
str r2, [r1]
add r1, sp, #0
ldr r2, =2
add r1, r1, r2, lsl #2
ldr r2, =65536
str r2, [r1]
add r1, sp, #0
ldr r2, =3
add r1, r1, r2, lsl #2
ldr r2, =65536
str r2, [r1]
add r1, sp, #0
ldr r2, =4
add r1, r1, r2, lsl #2
ldr r2, =45875
str r2, [r1]
add r1, sp, #0
ldr r2, =5
add r1, r1, r2, lsl #2
ldr r2, =32768
str r2, [r1]
add r1, sp, #0
ldr r2, =6
add r1, r1, r2, lsl #2
ldr r2, =45875
str r2, [r1]
add r1, sp, #0
ldr r2, =7
add r1, r1, r2, lsl #2
ldr r2, =65536
str r2, [r1]
add r1, sp, #0
ldr r2, =8
add r1, r1, r2, lsl #2
ldr r2, =32768
str r2, [r1]
sub sp, sp, #12
sub sp, sp, #12
sub sp, sp, #12
add r1, sp, #24
ldr r2, =0
add r1, r1, r2, lsl #2
ldr r2, =65536
str r2, [r1]
add r1, sp, #24
ldr r2, =1
add r1, r1, r2, lsl #2
ldr r2, =0
str r2, [r1]
add r1, sp, #24
ldr r2, =2
add r1, r1, r2, lsl #2
ldr r2, =0
str r2, [r1]
add r1, sp, #12
ldr r2, =0
add r1, r1, r2, lsl #2
ldr r2, =0
str r2, [r1]
add r1, sp, #12
ldr r2, =1
add r1, r1, r2, lsl #2
ldr r2, =16384
rsb r2, r2, #0
push { r2 }
bl fcos
mov r2, r0
add sp, sp, #4
str r2, [r1]
add r1, sp, #12
ldr r2, =2
add r1, r1, r2, lsl #2
ldr r2, =16384
rsb r2, r2, #0
push { r2 }
bl fsin
mov r2, r0
add sp, sp, #4
str r2, [r1]
add r1, sp, #0
ldr r2, =0
add r1, r1, r2, lsl #2
ldr r2, =0
str r2, [r1]
add r1, sp, #0
ldr r2, =1
add r1, r1, r2, lsl #2
ldr r2, =16384
rsb r2, r2, #0
push { r2 }
bl fsin
mov r2, r0
add sp, sp, #4
rsb r2, r2, #0
str r2, [r1]
add r1, sp, #0
ldr r2, =2
add r1, r1, r2, lsl #2
ldr r2, =16384
rsb r2, r2, #0
push { r2 }
bl fcos
mov r2, r0
add sp, sp, #4
str r2, [r1]
ldr r1, =0
push { r1 }
l57:
ldr r1, [sp, #0]
ldr r2, [sp, #140]
cmp r1, r2
bge l58
ldr r1, =0
push { r1 }
l55:
ldr r1, [sp, #0]
ldr r2, [sp, #148]
cmp r1, r2
bge l56
sub sp, sp, #12
add r1, sp, #0
ldr r2, =0
add r1, r1, r2, lsl #2
ldr r2, =0
str r2, [r1]
add r1, sp, #0
ldr r2, =1
add r1, r1, r2, lsl #2
ldr r2, =163840
str r2, [r1]
add r1, sp, #0
ldr r2, =2
add r1, r1, r2, lsl #2
ldr r2, =196608
str r2, [r1]
sub sp, sp, #12
add r1, sp, #0
ldr r2, =0
add r1, r1, r2, lsl #2
add r2, sp, #0
ldr r3, =1
add r2, r2, r3, lsl #2
add r3, sp, #0
ldr r4, =2
add r3, r3, r4, lsl #2
ldr r4, =0
str r4, [r3]
ldr r3, [r3]
str r3, [r2]
ldr r2, [r2]
str r2, [r1]
add r1, sp, #0
push { r1 }
add r1, sp, #4
push { r1 }
add r1, sp, #64
push { r1 }
ldr r1, [sp, #152]
push { r1 }
ldr r1, [sp, #44]
ldr r2, [sp, #184]
ldr r3, =1
mov r2, r2, asr r3
sub r1, r1, r2
ldr r2, =16
mov r1, r1, lsl r2
push { r1 }
bl fmul
mov r1, r0
add sp, sp, #8
push { r1 }
bl svmla
mov r1, r0
add sp, sp, #16
add r1, sp, #0
push { r1 }
add r1, sp, #4
push { r1 }
add r1, sp, #52
push { r1 }
ldr r1, [sp, #152]
push { r1 }
ldr r1, [sp, #188]
ldr r2, =1
mov r1, r1, asr r2
ldr r2, [sp, #40]
sub r1, r1, r2
ldr r2, =16
mov r1, r1, lsl r2
push { r1 }
bl fmul
mov r1, r0
add sp, sp, #8
push { r1 }
bl svmla
mov r1, r0
add sp, sp, #16
add r1, sp, #0
push { r1 }
add r1, sp, #4
push { r1 }
add r1, sp, #40
push { r1 }
ldr r1, =98304
rsb r1, r1, #0
push { r1 }
bl svmla
mov r1, r0
add sp, sp, #16
sub sp, sp, #12
add r1, sp, #0
ldr r2, =0
add r1, r1, r2, lsl #2
add r2, sp, #0
ldr r3, =1
add r2, r2, r3, lsl #2
add r3, sp, #0
ldr r4, =2
add r3, r3, r4, lsl #2
ldr r4, =98304
str r4, [r3]
ldr r3, [r3]
str r3, [r2]
ldr r2, [r2]
str r2, [r1]
ldr r1, =0
push { r1 }
add r1, sp, #4
push { r1 }
add r1, sp, #20
push { r1 }
add r1, sp, #36
push { r1 }
add r1, sp, #96
push { r1 }
add r1, sp, #136
push { r1 }
ldr r1, =9
push { r1 }
bl trace
mov r1, r0
add sp, sp, #28
add r1, sp, #0
ldr r2, =2
add r1, r1, r2, lsl #2
ldr r1, [r1]
ldr r2, =8
mov r1, r1, asr r2
push { r1 }
add r1, sp, #4
ldr r2, =1
add r1, r1, r2, lsl #2
ldr r1, [r1]
ldr r2, =8
mov r1, r1, asr r2
push { r1 }
add r1, sp, #8
ldr r2, =0
add r1, r1, r2, lsl #2
ldr r1, [r1]
ldr r2, =8
mov r1, r1, asr r2
push { r1 }
ldr r1, [sp, #48]
push { r1 }
ldr r1, [sp, #56]
push { r1 }
ldr r1, [sp, #204]
push { r1 }
ldr r1, [sp, #204]
push { r1 }
ldr r1, [sp, #204]
push { r1 }
bl setPixel
mov r1, r0
add sp, sp, #32
add r1, sp, #36
ldr r2, [sp, #36]
ldr r3, =1
add r2, r2, r3
str r2, [r1]
add sp, sp, #36
b l55
l56:
add r1, sp, #4
ldr r2, [sp, #4]
ldr r3, =1
add r2, r2, r3
str r2, [r1]
add sp, sp, #4
b l57
l58:
ldr r1, =0
add sp, sp, #116
mov r0, r1
pop { r1 }
pop { r2 }
pop { r3 }
pop { r4 }
pop { lr }
bx lr



_start:
    [00 a4 c1 00 00 00 04 00]
    mov r4,$0x40000
    [00 a5 03 04]
    mov r5,r4
    [7c 14 00 00] // relocated
    fcall _MachineSpecificInit
    [00 a4 03 00]
    mov r1,r0
    [00 a0 80 01]
    mov r0,$1
    [80 71]
    out.w
    [00 90 03 01 40 b1]
    lea r1,(r1+1)
    [00 a0 90 EC]
    mov r0,$0x10000
    out.w
    hlt

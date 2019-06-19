void _startOthersCPU()
{
  asm("eor     r0, r0 \t\n\
        ldr     r1, =__bss_start__\t\n\
        ldr     r2, =__bss_end__\t\n\
.L1:    str     r0, [r1]\t\n\
        add     r1, #4\t\n\
        cmp     r1, r2\t\n\
        blt     .L1");
}
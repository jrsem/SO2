// EPOS Cortex-A SETUP
        .file "cortex_a_setup.S" 
        // Interrupt Vector Table
        .section .init
        .type _vector_table, object
        .global _reset, _vector_table, copy_vectors
        .type   _reset,  function

_vector_table:
        ldr pc, _reset_addr
        ldr pc, _undefined_instruction_addr
        ldr pc, _swi_addr
        ldr pc, _prefetch_abort_addr
        ldr pc, _data_abort_addr
        ldr pc, _reserved_addr
        ldr pc, _irq_addr
        ldr pc, _fiq_addr

_reset_addr:
        .word _reset
_undefined_instruction_addr:
        .word _undefined_instruction
_swi_addr:
        .word _software_interrupt
_prefetch_abort_addr:
        .word _prefetch_abort
_data_abort_addr:
        .word _data_abort
_reserved_addr:
        .word _reserved
_irq_addr:
        .word _int_entry
_fiq_addr:
        .word _fiq
svc_stack:
.word 0x0FEFFFFC
irq_stack:
.word 0xFF00FFC //8kb
 _reset: 
  // get CPU ID and keep it in R11
 MRC p15, 0, r11, c0, c0, 5 // read CPU ID register into R11
 AND r11, r11, #0x03 // mask in only CPUID
 // set SVC stack
 LDR r0, =svc_stack // r0->svc_stack (16KB area in t.ld)
 mov r1, r11 // r1 = cpuid
 add r1, r1, #1 // cpuid++
 lsl r2, r1, #12 // (cpuid+1)* 4096
 add r0, r0, r2
 mov sp, r0 // SVC sp=svc_stack[cpuid] high end
 // go in IRQ mode with interrupts OFF
 MSR cpsr, #0x92
 // set IRQ stack
 LDR r0, =irq_stack // r0->irq_stack (16KB)
 mov r1, r11
 add r1, r1, #1
 lsl r2, r1, #12 // (cpuid+1) * 4096
 add r0, r0, r2
 mov sp, r0 // IRQ sp=irq_stack[cpuid] high end
 // go back to SVC mode with IRQ ON
 MSR cpsr, #0x13
 cmp r11, #0
 bne APs // only CPU0 copy vectors, call main()
 bl _main


UNLOCKED = 0
LOCKED = 1
int spin = 0; // spinlock, 0=UNLOCKED, 1=LOCKED
slock: // slock(int *spin): acquire spinlock
ldrex r1, [r0] // read spinlock value
cmp r1, #0x0 // compare with 0
WFENE // not 0 means already locked: do WFE
bne slock // try again after woken up by event
mov r1, #1 // set r1=1
strex r2, r1, [r0] // try to store 1 to [r0]; r2=return value
cmp r2, #0x0 // check return value in r2
bne slock // not 0 means failed; retry to lock again
DMB // memory barrier BEFORE accessing CR
bx lr // return only if has acquired the spinlock
sunlock: // sunlock(int *spin)
mov r1, #0x0 // set r1=0
DMB // memory barrier BEFORE releasing CR
str r1, [r0] // store 0 to [r0]
DSB // ensure update has completed before SEV
SEV // signal event to wakeup CPUs in WFE mode
bx lr // return

APs: // each AP call APstart() in C
adr r0, APaddr
ldr pc, [r0]
APaddr: .word APstart

@  APs:
@  wfi
@  bl APs


 

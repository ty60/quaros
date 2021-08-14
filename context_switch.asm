bits 32
section .text

extern curr_task
global context_switch

; By `call`ing this function from `switch_to()`
; eip just after the `call` function will be pushed on to the stack,
; which will be used when `ret`urning from another context
; back to this context.

; context_switch(struct task_struct *next_context)
context_switch:
    ; Save pointer to next stack in ecx.
    ; eax can be overwritten according to the x86 calling convension
    ; the caller of this function should have saved it.
    mov eax, [esp + 4]
    ; Save context (general purpose regs)
    pushad
    ; Save pointer to context on stack
    mov [curr_task], esp ; curr_task->context = esp;
    ; Switch stack to next context
    mov esp, [eax]
    ; pop context regs
    popa
    ; return to next context
    ret

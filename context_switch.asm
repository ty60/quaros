bits 32
section .text

extern curr_task
global context_switch

; By `call`ing this function from `switch_to()`
; eip just after the `call` function will be pushed on to the stack,
; which will be used when `ret`urning from another context
; back to this context.

; context_switch(struct task_struct *next_task)
context_switch:
    ; Caller saved regs can be used freely in this function.
    ; e.g. eax, ecx, edx
    ; The caller of this function should have saved these regs (caller saved).
    ; offsetof(struct task_struct, context) == 0.
    mov edx, [esp + 4] ; edx = next_task;
    mov eax, [edx] ; eax = next_task->context; // ok since offset is 0
    ; Save context (general purpose regs)
    pushad
    ; Save pointer to context on stack
    mov ecx, [curr_task] ; ecx = &curr_task->context
    mov [ecx], esp ; curr_task->context = esp;
    ; Switch current_task
    ; The below cast is ok, since offsetof(struct task_struct, context) == 0
    mov [curr_task], edx ; curr_task = next_task;
    ; Switch stack to next context
    mov esp, eax
    ; pop context regs
    popa
    ; return to next context
    ret

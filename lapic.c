#include "int.h"
#include "trap.h"
#include "lapic.h"


#define LAPIC 0xfee00000


volatile uint32_t *lapic = (uint32_t *)LAPIC;


#define LA_ID (0x20 / 4)
#define LA_SIV (0xf0 / 4)
#define LA_LVT_LINT0 (0x350 / 4)
#define LA_LVT_LINT1 (0x360 / 4)
#define LA_LVT_PERF_MON_CTR (0x340 / 4)
#define LA_LVT_ERR (0x370 / 4)
#define LA_ERR_STAT (0x280 / 4)
#define LA_EOI (0xb0 / 4)
#define LA_TASK_PRI (0x80 / 4)
#define LA_LVT_TIMER (0x320 / 4)
#define LA_DCR (0x3e0 / 4)
#define LA_INIT_CNT (0x380 / 4)

#define LA_ENABLE (0x100)
#define LA_MASK (0x10000)

#define LA_TIMER_PERIODIC (1 << 17)
#define LA_DIVIDE_BY 0xb // 1
#define LA_TIMER_INIT 10000000 // same as xv6


void write_lapic(uint32_t reg, uint32_t value) {
    lapic[reg] = value;
    lapic[LA_ID];
}


void eoi(void) {
    write_lapic(LA_EOI, 0);
}


void initlapic(void) {
    write_lapic(LA_ID, 0);


    write_lapic(LA_SIV, (T_IRQ_BASE + IRQ_SPURIOUS) | LA_ENABLE); // enable
    write_lapic(LA_LVT_LINT0, LA_MASK);
    write_lapic(LA_LVT_LINT1, LA_MASK);
    write_lapic(LA_LVT_PERF_MON_CTR, LA_MASK);

    write_lapic(LA_LVT_ERR, T_IRQ_BASE + IRQ_ERR);

    write_lapic(LA_ERR_STAT, 0);
    write_lapic(LA_ERR_STAT, 0);

    eoi();

    write_lapic(LA_TASK_PRI, 0);

    // Initialize timer
    // Divide by 1
    write_lapic(LA_DCR, LA_DIVIDE_BY);
    // Set IRQ num and periodic mode
    write_lapic(LA_LVT_TIMER, LA_TIMER_PERIODIC | (T_IRQ_BASE + IRQ_TIMER));
    // Set initial counter
    write_lapic(LA_INIT_CNT, LA_TIMER_INIT);
}


void disablepic(void) {
    __asm__ volatile (
            "mov al, 0xff\n\t"
            "out 0xa1, al\n\t"
            "out 0x21, al\n\t"
            :
            :
            : "al");
}

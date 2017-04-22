/**************************************************
 SysTick (from pqlabs) for delay and caculate time
**************************************************/
static unsigned int __tickStartValue;
#define NS2SYSTICK(nscount)      ((nscount) * 168 / 1000)      // the count of systick of n ns
#define US2SYSTICK(uscount)    ((uscount) * 168) 

#define MARK_SYSTICK()           __tickStartValue = SysTick->VAL;
#define WAIT_SYSTICK(tickCount)  while(((__tickStartValue - SysTick->VAL) & 0x00FFFFFF) < tickCount)

#define START_SYSTICK()          SysTick->LOAD = 0x00FFFFFF; SysTick->CTRL = 0x5;

#define MARK_SYSTICK2()         (SysTick->VAL)
#define WAIT_SYSTICK2(beginTick, tickCount)     while(((beginTick - SysTick->VAL) & 0x00FFFFFF) < tickCount)


/*
            start = SysTick->VAL;
            测量脉冲数量（时间）
            start = start - SysTick->VAL;
*/

























/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include  <app_cfg.h>
#include  <includes.h>


/* ----------------- APPLICATION GLOBALS ------------------ */
OS_TCB   StartUPTCB;
CPU_STK  StartUP_Stk[APP_CFG_TASK_START_STK_SIZE];


/*
*********************************************************************************************************
*                                                main()
*
* Description : This is the standard entry point for C code.  It is assumed that your code will call
*               main() once you have performed all necessary initialization.
*
* Arguments   : none
*
* Returns     : none
*********************************************************************************************************
*/

int main(void)
{
    OS_ERR  err;

    BSP_IntDisAll();                                            /* Disable all interrupts.                              */
    OSInit(&err);                                               /* Init uC/OS-III.                                      */

	/***************1**************/
    OSTaskCreate((OS_TCB       *)&StartUPTCB,              /* Create the start task                                */
	
                 (CPU_CHAR     *)"Task Start",
                 (OS_TASK_PTR   )Task_Start, 
                 (void         *)0,
									 
                 (OS_PRIO       )STARTUP_TASK_PRIO,
                 (CPU_STK      *)&StartUP_Stk[0],
                 (CPU_STK_SIZE  )STARTUP_TASK_STK_SIZE / 10,
								 
                 (CPU_STK_SIZE  )STARTUP_TASK_STK_SIZE,
                 (OS_MSG_QTY    )0,
								 
                 (OS_TICK       )0,
                 (void         *)0,
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
								 
                 (OS_ERR       *)&err);

		OSStart(&err);                                              /* Start multitasking (i.e. give control to uC/OS-III). */
        
    return (0);
}




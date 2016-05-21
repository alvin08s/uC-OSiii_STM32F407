/*
*********************************************************************************************************
*
*                                            EXAMPLE CODE
*
*                                     ST Microelectronics STM32
*                                              on the
*
*                                           STM32F407ZET
*                                         Evaluation Board
*
* Filename      : app.c
* Version       : V1.00
* Programmer(s) : EHS
*                 DC
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include  "app_cfg.h"
#include  <includes.h>
#include "led.h"
#include "sys.h"
#include "usart.h"
#include "mpu6050.h"
#include "pwm.h"
/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/

                                                                /* ----------------- APPLICATION GLOBALS ------------------ */
extern  OS_TCB   StartUPTCB;

static OS_TCB AppTaskStartTCB;
static  CPU_STK  AppTaskStartStk[APP_CFG_TASK_START_STK_SIZE];

static  OS_TCB   LED1_TCB;
static  CPU_STK  LED1StartStk[TASK_LED1_STK_SIZE];

static  OS_TCB   MPU6050_TCB;
static  CPU_STK  MPU6050StartStk[MPU6050_STK_SIZE];


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

void AppTaskStart(void *arg);
void  AppTaskCreate(void);
void  AppObjCreate(void);
void Task_LED1 (void *p_arg);
void Task_MPU6050(void *p_arg);


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

void Task_Start(void *p_arg)
{
  OS_ERR  err;
	
	/***************1**************/
    OSTaskCreate((OS_TCB       *)&AppTaskStartTCB,              /* Create the start task                                */
                 (CPU_CHAR     *)"App Task Start",
                 (OS_TASK_PTR   )AppTaskStart, 
                 (void         *)0,
                 (OS_PRIO       )APP_CFG_TASK_START_PRIO,
                 (CPU_STK      *)&AppTaskStartStk[0],
                 (CPU_STK_SIZE  )APP_CFG_TASK_START_STK_SIZE / 10,
                 (CPU_STK_SIZE  )APP_CFG_TASK_START_STK_SIZE,
                 (OS_MSG_QTY    )0,
                 (OS_TICK       )0,
                 (void         *)0,
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR       *)&err);

   
		/***********LED1 TEST *****************/						 
		OSTaskCreate((OS_TCB       *)&LED1_TCB,              /* Create the start task                                */
                 (CPU_CHAR     *)"LED1",
                 (OS_TASK_PTR   )Task_LED1, 
                 (void         *)0,
                 (OS_PRIO       )LED_CFG_TASK_START_PRIO,
                 (CPU_STK      *)&LED1StartStk[0],
                 (CPU_STK_SIZE  )TASK_LED1_STK_SIZE / 10,
                 (CPU_STK_SIZE  )TASK_LED1_STK_SIZE,
                 (OS_MSG_QTY    )0,
                 (OS_TICK       )0,
                 (void         *)0,
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR       *)&err);
								 					 
				
		/***********MPU6050 *****************/						 
		OSTaskCreate((OS_TCB       *)&MPU6050_TCB,              // Create the start task
                 (CPU_CHAR     *)"MPU6050",
                 (OS_TASK_PTR   )Task_MPU6050, 
                 (void         *)0,
                 (OS_PRIO       )MPU6050_TASK_PRIO,
                 (CPU_STK      *)&MPU6050StartStk[0],
                 (CPU_STK_SIZE  )MPU6050_STK_SIZE / 10,
                 (CPU_STK_SIZE  )MPU6050_STK_SIZE,
                 (OS_MSG_QTY    )0,
                 (OS_TICK       )0,
                 (void         *)0,
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR       *)&err);    
								 
  
		OSTaskDel(&StartUPTCB,&err);

}



/*
*********************************************************************************************************
*                                          STARTUP TASK
*
* Description : This is an example of a startup task.  As mentioned in the book's text, you MUST
*               initialize the ticker only once multitasking has started.
*
* Arguments   : p_arg   is the argument passed to 'AppTaskStart()' by 'OSTaskCreate()'.
*
* Returns     : none
*
* Notes       : 1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                  used.  The compiler should not generate any code for this statement.
*********************************************************************************************************
*/
void  AppTaskStart (void *p_arg)
{
    OS_ERR      err;


   (void)p_arg;

    BSP_Init();                                                 /* Initialize BSP functions                             */
    CPU_Init();                                                 /* Initialize the uC/CPU services                       */
    
    BSP_Tick_Init();                                            /* Start Tick Initialization                            */
    
    Mem_Init();                                                 /* Initialize Memory Management Module                  */
    Math_Init();                                                /* Initialize Mathematical Module                       */


#if OS_CFG_STAT_TASK_EN > 0u
    OSStatTaskCPUUsageInit(&err);                               /* Compute CPU capacity with no task running            */
#endif

#ifdef CPU_CFG_INT_DIS_MEAS_EN
    CPU_IntDisMeasMaxCurReset();
#endif

    BSP_LED_Off(0);
    
#if (APP_CFG_SERIAL_EN == DEF_ENABLED)    
    App_SerialInit();                                           /* Initialize Serial Communication                      */
#endif

    APP_TRACE_DBG(("Creating Application kernel objects\n\r"));
    AppObjCreate();                                             /* Create Applicaiton kernel objects                    */
    
    APP_TRACE_DBG(("Creating Application Tasks\n\r"));
    AppTaskCreate();                                            /* Create Application tasks                             */

    while (DEF_TRUE) {                                          /* Task body, always written as an infinite loop.       */
        BSP_LED_Toggle(0);
        OSTimeDlyHMSM(0, 0, 0, 50, OS_OPT_TIME_HMSM_STRICT, &err);
    }
}

void Task_LED1(void *p_arg)
{
	OS_ERR err;

	uint16_t led0pwmval=0;    
	uint8_t dir=1;
	
	(void)p_arg;

	LED_Init();
	TIM14_PWM_Init(500-1,84-1);

	while(1){
		OSTimeDlyHMSM(0, 0, 0, 80, OS_OPT_TIME_HMSM_STRICT, &err);

		if(dir)
			led0pwmval++;//dir==1 led0pwmval递增
		else 
			led0pwmval--;	//dir==0 led0pwmval递减 
 		if(led0pwmval>300)
			dir=0;//led0pwmval到达300后，方向为递减
		if(led0pwmval==0)
			dir=1;	//led0pwmval递减到0后，方向改为递增
		
		TIM_SetCompare1(TIM14,led0pwmval);	//修改比较值，修改占空比
	}
}


void Task_MPU6050(void *p_arg)
{
	uint8_t t;
	uint8_t len;	
	uint16_t ACCEL_DATA[3];
	uint16_t GYSO_DATA[3];
	OS_ERR err;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	uart_init(115200);
	mpu6050_init();
	
	while(1)
	{
		OSTimeDlyHMSM(0, 0, 0, 60, OS_OPT_TIME_HMSM_STRICT, &err);

		if(USART_RX_STA&0x8000)
		{					   
			len=USART_RX_STA&0x3fff;
			printf("\r\n Msg from :\r\n");
			for(t=0;t<len;t++)
			{
				USART_SendData(USART1, USART_RX_BUF[t]);
				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);
			}
			printf("\r\n\r\n");
			USART_RX_STA=0;
		}else
		{
			printf("start Read_MPU6050_Data...\r\n");
			Read_MPU6050_Data(ACCEL_XOUT_H,ACCEL_DATA,sizeof(ACCEL_DATA));
			printf("ACCEL:x = %d ,y = %d ,z = %d \r\n",ACCEL_DATA[0],ACCEL_DATA[1],ACCEL_DATA[2]);
						
			Read_MPU6050_Data(GYRO_XOUT_H,GYSO_DATA,sizeof(GYSO_DATA));
			printf("GYSO:x = %d ,y = %d ,z = %d \r\n",GYSO_DATA[0],GYSO_DATA[1],GYSO_DATA[2]);
			
		}
	}
}



/*
*********************************************************************************************************
*                                          AppTaskCreate()
*
* Description : Create application tasks.
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   : AppTaskStart()
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  AppTaskCreate (void)
{

}


/*
*********************************************************************************************************
*                                          AppObjCreate()
*
* Description : Create application kernel objects tasks.
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   : AppTaskStart()
*
* Note(s)     : none.
*********************************************************************************************************
*/
void  AppObjCreate (void)
{

}


/**============================================================================
                    ##### How to use this driver #####
 ===============================================================================
    [..]
      (#) Enable peripheral clock using RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2Cx, ENABLE)
          function for I2C1, I2C2 or I2C1.
  
      (#) Enable SDA, SCL  and SMBA (when used) GPIO clocks using 
          RCC_AHBPeriphClockCmd() function. 
  
      (#) Peripherals alternate function: 
        (++) Connect the pin to the desired peripherals' Alternate 
             Function (AF) using GPIO_PinAFConfig() function
        (++) Configure the desired pin in alternate function by:
             GPIO_InitStruct->GPIO_Mode = GPIO_Mode_AF
        (++) Select the type, pull-up/pull-down and output speed via 
             GPIO_PuPd, GPIO_OType and GPIO_Speed members
        (++) Call GPIO_Init() function
             Recommended configuration is Push-Pull, Pull-up, Open-Drain.
             Add an external pull up if necessary (typically 4.7 KOhm).      
          
      (#) Program the Mode, duty cycle , Own address, Ack, Speed and Acknowledged
          Address using the I2C_Init() function.
  
      (#) Optionally you can enable/configure the following parameters without
          re-initialization (i.e there is no need to call again I2C_Init() function):
        (++) Enable the acknowledge feature using I2C_AcknowledgeConfig() function
        (++) Enable the dual addressing mode using I2C_DualAddressCmd() function
        (++) Enable the general call using the I2C_GeneralCallCmd() function
        (++) Enable the clock stretching using I2C_StretchClockCmd() function
        (++) Enable the fast mode duty cycle using the I2C_FastModeDutyCycleConfig()
             function.
        (++) Configure the NACK position for Master Receiver mode in case of 
             2 bytes reception using the function I2C_NACKPositionConfig().  
        (++) Enable the PEC Calculation using I2C_CalculatePEC() function
        (++) For SMBus Mode: 
          (+++) Enable the Address Resolution Protocol (ARP) using I2C_ARPCmd() function
          (+++) Configure the SMBusAlert pin using I2C_SMBusAlertConfig() function
  
      (#) Enable the NVIC and the corresponding interrupt using the function 
          I2C_ITConfig() if you need to use interrupt mode. 
  
      (#) When using the DMA mode 
        (++) Configure the DMA using DMA_Init() function
        (++) Active the needed channel Request using I2C_DMACmd() or
             I2C_DMALastTransferCmd() function.
        -@@- When using DMA mode, I2C interrupts may be used at the same time to
             control the communication flow (Start/Stop/Ack... events and errors).
   
      (#) Enable the I2C using the I2C_Cmd() function.
   
      (#) Enable the DMA using the DMA_Cmd() function when using DMA mode in the 
          transfers.
  ******************************************************************************  
  */ 
	
#include "stm32f4xx_i2c.h"
#include "sys.h"
#include "mpu6050.h"

void mpu6050_init(void)
{
	
	I2C_InitTypeDef I2C_InitStruct;  //define struct of I2C
	GPIO_InitTypeDef GPIO_InitStruct; //define struct of GPIO
	
	/**
	Enable peripheral clock using RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2Cx, ENABLE)function for I2C1, I2C2 or I2C1.
	**/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	
	/*Connect the pin to the desired peripherals' Alternate Function (AF) using GPIO_PinAFConfig() function*/
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource6,GPIO_AF_I2C1);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource7,GPIO_AF_I2C1);
	
	/***Configure the desired pin in alternate function by:GPIO_InitStruct->GPIO_Mode = GPIO_Mode_AF***/
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP; 
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL; 
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	I2C_DeInit(I2C1);
//	I2C_StructInit(&I2C_InitStruct); //initial	
	I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  /* Initialize the I2C_OwnAddress1 member */
  I2C_InitStruct.I2C_OwnAddress1 = I2C1_ADDR;
  /* Initialize the I2C_Ack member */
  I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStruct.I2C_ClockSpeed = 100000;
	I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;

	I2C_Cmd(I2C1,ENABLE);
	I2C_Init(I2C1,&I2C_InitStruct);

	I2C_AcknowledgeConfig(I2C1,ENABLE);  //Enables or disables the specified I2C acknowledge feature.
	
}


uint8_t I2C_Read(I2C_TypeDef *I2Cx,uint8_t slave_addr,uint8_t reg_addr,uint8_t *buf,uint16_t length)
{  
  if(length==0)
		return 1;
	
//	printf("I2C check busy 1....\r\n");
	while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY)); //检测总线是否空闲
//	printf("after 1....\r\n");

	I2C_AcknowledgeConfig(I2Cx, ENABLE);
   
  I2C_GenerateSTART(I2Cx, ENABLE);
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT));
//	printf("after 2....\r\n");

  I2C_Send7bitAddress(I2Cx,  slave_addr, I2C_Direction_Transmitter);
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
//	printf("after 3....\r\n");
//  I2C_Cmd(I2C1, ENABLE);
	
	I2C_SendData(I2Cx, reg_addr);
	while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
//	printf("after 4....\r\n");
  
	I2C_GenerateSTART(I2Cx, ENABLE);
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT));
//	printf("after 5....\r\n");
	
	I2C_Send7bitAddress(I2C1, slave_addr, I2C_Direction_Receiver);
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
//	printf("after 6....\r\n");

  while (length)
  {
		/* Decrement the read bytes counter */
		if(length==1)
		{
			I2C_AcknowledgeConfig(I2Cx, DISABLE);
			I2C_GenerateSTOP(I2Cx, ENABLE);
			break;
		}
//		printf("after 7....\r\n");
		while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED));  /* EV7 */
//		printf("after 8....\r\n");
		
		*buf = I2C_ReceiveData(I2Cx);
		buf++;
		length--;
  }
//	I2C_AcknowledgeConfig(I2Cx, ENABLE);
//	printf("Read data complete....\r\n");

	return 0;
}

uint8_t I2C_Write(I2C_TypeDef *I2Cx,uint8_t slave_addr,uint8_t reg_addr,uint8_t value)
{

	printf("here  ???....\r\n");
	while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY))
	printf("after 11....\r\n");

  I2C_GenerateSTART(I2Cx, ENABLE);
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT));
//	printf("after 22....\r\n");

  I2C_Send7bitAddress(I2Cx, slave_addr, I2C_Direction_Transmitter);
  while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
//	printf("after 33....\r\n");

  I2C_SendData(I2Cx, reg_addr);
  while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
//	printf("after 44....\r\n");

  I2C_SendData(I2Cx, value);
  while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	printf("after 55....\r\n");

  I2C_GenerateSTOP(I2Cx, ENABLE);
//  I2C_AcknowledgeConfig(I2Cx, DISABLE);
 
	return 0;
}


uint8_t Read_MPU6050_Data(uint8_t reg_addr,uint16_t *data,u8 len)
{
	uint8_t tmp[6];
	I2C_Read(I2C1,MPU6050_ADDR,reg_addr,tmp,len);
	switch(reg_addr){
		case ACCEL_XOUT_H:
		{
			data[0] = (((u16)tmp[0]) << 8) | tmp[1];
			data[1] = (((u16)tmp[2]) << 8) | tmp[3];
			data[2] = (((u16)tmp[4]) << 8) | tmp[5];
			break;
		}
		case GYRO_XOUT_H:
		{
			data[0] = (((u16)tmp[0]) << 8) | tmp[1];
			data[1] = (((u16)tmp[2]) << 8) | tmp[3];
			data[2] = (((u16)tmp[4]) << 8) | tmp[5];
			break;
		}
		case TEMP_OUT_H:
		{
			*data = (((u16)tmp[0]) << 8) | tmp[1];
			
			break;
		}
		default:
			break;
	}
			
	
	return 0;
}


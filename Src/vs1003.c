/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               VS1003.c
** Descriptions:            The VS1003 application function
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2011-2-27
** Version:                 v1.0
** Descriptions:            The original version
**
**--------------------------------------------------------------------------------------------------------
** Modified by:             
** Modified date:           
** Version:                 
** Descriptions:            
**
*********************************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "vs1003.h"

/* Private variables ---------------------------------------------------------*/

/* VS1003设置参数 */ 
/* 0 , henh.1 , hfreq.2 , lenh.3 , lfreq 5 ,主音量 */
uint8_t vs1003ram[5] = { 0 , 0 , 0 , 0 , 250 };
		
SPI_HandleTypeDef hspi2;

/*******************************************************************************
* Function Name  : VS1003_SPI_Init
* Description    : VS1003 SPI Init
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void VS1003_SPI_Init(void)
{
	  __HAL_RCC_SPI2_CLK_ENABLE();

	  hspi2.Instance = SPI2;
	  hspi2.Init.Mode = SPI_MODE_MASTER;
	  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
	  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
	  hspi2.Init.CLKPolarity = SPI_POLARITY_HIGH;
	  hspi2.Init.CLKPhase = SPI_PHASE_2EDGE;
	  hspi2.Init.NSS = SPI_NSS_SOFT;
	  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
	  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
	  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
	  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	  hspi2.Init.CRCPolynomial = 0;
	  if (HAL_SPI_Init(&hspi2) != HAL_OK)
	  {
		printf("aa\r\n");
		Error_Handler(__FILE__,__LINE__);
	  }
}

/*******************************************************************************
* Function Name  : VS1003_SPI_SetSpeed
* Description    : None
* Input          : - SpeedSet: SPI_SPEED_HIGH 1  SPI_SPEED_LOW 0
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static void VS1003_SPI_SetSpeed( uint8_t SpeedSet)
{
	  hspi2.Instance = SPI2;
	  hspi2.Init.Mode = SPI_MODE_MASTER;
	  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
	  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
	  hspi2.Init.CLKPolarity = SPI_POLARITY_HIGH;
	  hspi2.Init.CLKPhase = SPI_PHASE_2EDGE;
	  hspi2.Init.NSS = SPI_NSS_SOFT;
	  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
	  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
	  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	  hspi2.Init.CRCPolynomial = 0;

  if( SpeedSet == SPI_SPEED_LOW )
  {
      hspi2.Init.BaudRatePrescaler =  SPI_BAUDRATEPRESCALER_128;
  }
  else 
  {
	  hspi2.Init.BaudRatePrescaler =  SPI_BAUDRATEPRESCALER_8;
  }
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
	  printf("bb\r\n");
    Error_Handler(__FILE__,__LINE__);
  }
}

/*******************************************************************************
* Function Name  : VS1003_SPI_ReadWriteByte
* Description    : None
* Input          : - TxData: Write Byte
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
static uint16_t VS1003_SPI_ReadWriteByte( uint16_t TxData )
{
  /* Loop while DR register in not emplty */
  uint8_t RxData;
  HAL_SPI_TransmitReceive(&hspi2,(uint8_t*)&TxData,&RxData,1,10);
  return RxData;
}

/*******************************************************************************
* Function Name  : VS1003_Init
* Description    : VS1003端口初始化
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/	 
void VS1003_Init(void)
{		  
	GPIO_InitTypeDef GPIO_InitStruct;


//	  __HAL_RCC_GPIOA_CLK_ENABLE();
//	  __HAL_RCC_GPIOB_CLK_ENABLE();
//	 // __HAL_RCC_GPIOC_CLK_ENABLE();
//
//
//	  /*Configure GPIO pins : PA4 PA6 */
//	  GPIO_InitStruct.Pin = GPIO_PIN_9;
//	  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
//	  GPIO_InitStruct.Pull = GPIO_NOPULL;
//	  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
//	  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
//
//	  /*Configure GPIO pin : PC7 */
//	  GPIO_InitStruct.Pin = GPIO_PIN_4;
//	  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
//
//	  GPIO_InitStruct.Pin = GPIO_PIN_6;
//	  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
//
//	  /*Configure GPIO pin : PB9 */
//	  GPIO_InitStruct.Pin = GPIO_PIN_7;
//	  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
//	  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
//
//	  GPIO_InitStruct.Pin = GPIO_PIN_10 | GPIO_PIN_14 | GPIO_PIN_15;
//	  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
//	  GPIO_InitStruct.Pull = GPIO_NOPULL;
//	  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
//	  GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
//	  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	  MP3_RESET(0);
	  HAL_Delay(1);
	  MP3_RESET(1);

	  MP3_DCS(1);
	  MP3_CCS(1);

	  VS1003_SPI_Init();

}

/*******************************************************************************
* Function Name  : delay_ms
* Description    : delay time
* Input          : - ms: time
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
//void delay_ms(uint32_t ms)
//{
//    uint32_t len;
//    for (;ms > 0; ms --)
//        for (len = 0; len < 100; len++ );
//}

/*******************************************************************************
* Function Name  : VS1003_WriteReg
* Description    : VS1003写寄存器
* Input          : - reg: 命令地址
*				   - value: 命令数据
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void VS1003_WriteReg( uint8_t reg,uint16_t value )
{  
   while(MP3_DREQ ==0);           /* 等待空闲 */
   
   VS1003_SPI_SetSpeed( SPI_SPEED_LOW );	 
   MP3_DCS(1); 
   MP3_CCS(0); 
   VS1003_SPI_ReadWriteByte(VS1003_WRITE_COMMAND); /* 发送VS1003的写命令 */
   VS1003_SPI_ReadWriteByte(reg);             
   VS1003_SPI_ReadWriteByte(value>>8);        
   VS1003_SPI_ReadWriteByte(value);	          
   MP3_CCS(1);         
   VS1003_SPI_SetSpeed( SPI_SPEED_HIGH );
} 

/*******************************************************************************
* Function Name  : VS1003_ReadReg
* Description    : VS1003读寄存器
* Input          : - reg: 命令地址
* Output         : None
* Return         : - value: 数据
* Attention		 : None
*******************************************************************************/
uint16_t VS1003_ReadReg( uint8_t reg)
{ 
   uint16_t value;
    
   while(  MP3_DREQ ==0 );           /* 等待空闲 */
   VS1003_SPI_SetSpeed( SPI_SPEED_LOW );
   MP3_DCS(1);     
   MP3_CCS(0);     
   VS1003_SPI_ReadWriteByte(VS1003_READ_COMMAND);/* 发送VS1003的读命令 */
   VS1003_SPI_ReadWriteByte( reg );   
   value = VS1003_SPI_ReadWriteByte(0xff);		
   value = value << 8;
   value |= VS1003_SPI_ReadWriteByte(0xff); 
   MP3_CCS(1);   
   VS1003_SPI_SetSpeed( SPI_SPEED_HIGH );
   return value; 
} 

/*******************************************************************************
* Function Name  : VS1003_ResetDecodeTime
* Description    : None
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/                       
void VS1003_ResetDecodeTime(void)
{
   VS1003_WriteReg(SPI_DECODE_TIME,0x0000);
   VS1003_WriteReg(SPI_DECODE_TIME,0x0000); /* 操作两次 */
}

/*******************************************************************************
* Function Name  : VS1003_GetDecodeTime
* Description    : None
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/    
uint16_t VS1003_GetDecodeTime(void)
{ 
   return VS1003_ReadReg(SPI_DECODE_TIME);   
} 

/*******************************************************************************
* Function Name  : VS1003_SoftReset
* Description    : VS1003软复位
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void VS1003_SoftReset(void)
{
   uint8_t retry; 	
   			   
   while(  MP3_DREQ ==0 );
   VS1003_SPI_ReadWriteByte(0xff);
   retry = 0;
   while( VS1003_ReadReg(SPI_MODE) != 0x0804 )
   {

	   VS1003_WriteReg(SPI_MODE,0x0804);
	   HAL_Delay(2);
	   if( retry++ > 100 )
	   { 
	      break;
	   }
   }	 	

   while(  MP3_DREQ ==0 );

   retry = 0;

   while( VS1003_ReadReg(SPI_CLOCKF) != 0x9800 )
   {
	   VS1003_WriteReg(SPI_CLOCKF,0x9800);
	   if( retry++ > 100 )
	   { 
	      break; 
	   }
   }		   
   retry = 0;

   while( VS1003_ReadReg(SPI_AUDATA) != 0xBB81 )
   {
	   VS1003_WriteReg(SPI_AUDATA,0xBB81);
	   if( retry++ > 100 )
	   { 
	      break; 
	   }   
   }

   VS1003_WriteReg(SPI_VOL,0x0000);	 /* 音量 */
   VS1003_ResetDecodeTime();     /* 复位解码时间 */	     
   /* 向vs1003发送4个字节无效数据，用以启动SPI发送 */	
   MP3_DCS(0);  /* 选中数据传输 */
   VS1003_SPI_ReadWriteByte(0xFF);
   VS1003_SPI_ReadWriteByte(0xFF);
   VS1003_SPI_ReadWriteByte(0xFF);
   VS1003_SPI_ReadWriteByte(0xFF);
   MP3_DCS(1);  /* 取消数据传输 */
   HAL_Delay(20);
} 

/*******************************************************************************
* Function Name  : VS1003_Reset
* Description    : VS1003复位
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void VS1003_Reset(void)
{
   MP3_RESET(0);
   HAL_Delay(20);
   MP3_RESET(1);
   HAL_Delay(20);
   VS1003_SPI_ReadWriteByte(0xFF);
   MP3_DCS(1);              /* 取消数据传输 */
   MP3_CCS(1);              /* 取消数据传输 */
   while(  MP3_DREQ ==0 );  /* 等待DREQ为高 */
   HAL_Delay(20);
}

/*******************************************************************************
* Function Name  : VS1003_SineTest
* Description    : VS1003正弦测试
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void VS1003_SineTest(void)
{	
   VS1003_WriteReg(SPI_VOL,0x2020);	  /* 设置音量 */
   VS1003_WriteReg(SPI_MODE,0x0820); /* 进入vs1003的测试模式 */	    
   while(  MP3_DREQ ==0 );  /* 等待DREQ为高 */
 	
   /* 向vs1003发送正弦测试命令：0x53 0xef 0x6e n 0x00 0x00 0x00 0x00 */
   MP3_DCS(0);/* 选中数据传输 */
   VS1003_SPI_ReadWriteByte(0x53);
   VS1003_SPI_ReadWriteByte(0xef);
   VS1003_SPI_ReadWriteByte(0x6e);
   VS1003_SPI_ReadWriteByte(0x24);
   VS1003_SPI_ReadWriteByte(0x00);
   VS1003_SPI_ReadWriteByte(0x00);
   VS1003_SPI_ReadWriteByte(0x00);
   VS1003_SPI_ReadWriteByte(0x00);
   HAL_Delay(100);
   MP3_DCS(1); 
   /* 退出正弦测试 */
   MP3_DCS(0);/* 选中数据传输 */
   VS1003_SPI_ReadWriteByte(0x45);
   VS1003_SPI_ReadWriteByte(0x78);
   VS1003_SPI_ReadWriteByte(0x69);
   VS1003_SPI_ReadWriteByte(0x74);
   VS1003_SPI_ReadWriteByte(0x00);
   VS1003_SPI_ReadWriteByte(0x00);
   VS1003_SPI_ReadWriteByte(0x00);
   VS1003_SPI_ReadWriteByte(0x00);
   HAL_Delay(100);
   MP3_DCS(1);		 

   /* 再次进入正弦测试并设置n值为0x44，即将正弦波的频率设置为另外的值 */
   MP3_DCS(0);/* 选中数据传输 */      
   VS1003_SPI_ReadWriteByte(0x53);
   VS1003_SPI_ReadWriteByte(0xef);
   VS1003_SPI_ReadWriteByte(0x6e);
   VS1003_SPI_ReadWriteByte(0x44);
   VS1003_SPI_ReadWriteByte(0x00);
   VS1003_SPI_ReadWriteByte(0x00);
   VS1003_SPI_ReadWriteByte(0x00);
   VS1003_SPI_ReadWriteByte(0x00);
   HAL_Delay(100);
   MP3_DCS(1);
   /* 退出正弦测试 */
   MP3_DCS(0);/* 选中数据传输 */      
   VS1003_SPI_ReadWriteByte(0x45);
   VS1003_SPI_ReadWriteByte(0x78);
   VS1003_SPI_ReadWriteByte(0x69);
   VS1003_SPI_ReadWriteByte(0x74);
   VS1003_SPI_ReadWriteByte(0x00);
   VS1003_SPI_ReadWriteByte(0x00);
   VS1003_SPI_ReadWriteByte(0x00);
   VS1003_SPI_ReadWriteByte(0x00);
   HAL_Delay(100);
   MP3_DCS(1);	 
}	 

/*******************************************************************************
* Function Name  : VS1003_RamTest
* Description    : VS1003 RAM测试
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/																			 
void VS1003_RamTest(void)
{
   volatile uint16_t value;
     
   VS1003_Reset();     
   VS1003_WriteReg(SPI_MODE,0x0820);/* 进入vs1003的测试模式 */
   while(  MP3_DREQ ==0 );           /* 等待DREQ为高 */
   MP3_DCS(0);	       			     /* xDCS = 1，选择vs1003的数据接口 */
   VS1003_SPI_ReadWriteByte(0x4d);
   VS1003_SPI_ReadWriteByte(0xea);
   VS1003_SPI_ReadWriteByte(0x6d);
   VS1003_SPI_ReadWriteByte(0x54);
   VS1003_SPI_ReadWriteByte(0x00);
   VS1003_SPI_ReadWriteByte(0x00);
   VS1003_SPI_ReadWriteByte(0x00);
   VS1003_SPI_ReadWriteByte(0x00);
   HAL_Delay(50);
   MP3_DCS(1);
   value = VS1003_ReadReg(SPI_HDAT0); /* 如果得到的值为0x807F，则表明完好 */
}     
		 				
/*******************************************************************************
* Function Name  : VS1003_SetVol
* Description    : 设定vs1003播放的音量和高低音
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/   
void VS1003_SetVol(void)
{
   uint8_t i;
   uint16_t bass=0;  /* 暂存音调寄存器值 */
   uint16_t vol=0;  /* 暂存音量值 */
   uint8_t  vset=0;  /* 暂存音量值 */ 
		 
   vset = 255 - vs1003ram[4]; /* 取反得到最大值 */ 
   vol =vset;
   vol <<= 8;
   vol += vset;/* 得到音量设置后大小 */
   /* 0,henh.1,hfreq.2,lenh.3,lfreq */      
   for( i = 0; i < 4; i++ )
   {
       bass <<= 4;
       bass += vs1003ram[i]; 
   }     
   VS1003_WriteReg(SPI_BASS,bass);/* BASS */   
   VS1003_WriteReg(SPI_VOL,vol); /* 设音量 */
}    

/*******************************************************************************
* Function Name  : VS1003_WriteData
* Description    : VS1003写数据
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/  
void VS1003_WriteData( uint8_t * buf)
{		
   uint8_t  count = 32;

   MP3_DCS(0);   
   while( count-- )
   {
	  VS1003_SPI_ReadWriteByte( *buf++ );
   }
   MP3_DCS(1);  
   MP3_CCS(1);  
} 

/*******************************************************************************
* Function Name  : VS1003_Record_Init
* Description    : VS1003录音初始化
* Input          : None
* Output         : None
* Return         : None
* Attention		 : VS1003录音初始化成功之后可以听到耳机有回放
*                  此时不能用喇叭容易产生自激
*******************************************************************************/ 
void VS1003_Record_Init(void)
{
  uint8_t retry; 	

  /* 配置时钟 */
  while( VS1003_ReadReg(SPI_CLOCKF) != 0x9800 )   
  {
	  VS1003_WriteReg(SPI_CLOCKF,0x9800);   
	  HAL_Delay(2);                        /* 等待至少1.35ms */
	  if( retry++ > 100 )
	  { 
	      break; 
	  }
  }	

  while( VS1003_ReadReg(SPI_BASS) != 0x0000 )   
  {
	  VS1003_WriteReg(SPI_CLOCKF,0x0000);   
	  HAL_Delay(2);                        /* 等待至少1.35ms */
	  if( retry++ > 100 )
	  { 
	      break; 
	  }
  }	

  /* Set sample rate divider=12 */ 
  while( VS1003_ReadReg(SPI_AICTRL0) != 0x0012 )   
  {
	  VS1003_WriteReg(SPI_AICTRL0,0x0012);   
	  HAL_Delay(2);                        /* 等待至少1.35ms */
	  if( retry++ > 100 )
	  { 
	      break; 
	  }
  }	

  /* AutoGain OFF, reclevel 0x1000 */ 
  while( VS1003_ReadReg(SPI_AICTRL1) != 0x1000 )   
  {
	  VS1003_WriteReg(SPI_AICTRL1,0x1000);   
	  HAL_Delay(2);                        /* 等待至少1.35ms */
	  if( retry++ > 100 )
	  { 
	      break; 
	  }
  }	

  /* RECORD,NEWMODE,RESET */ 
  while( VS1003_ReadReg(SPI_MODE) != 0x1804 )   
  {
	  VS1003_WriteReg(SPI_MODE,0x1804);   
	  HAL_Delay(2);                        /* 等待至少1.35ms */
	  if( retry++ > 100 )
	  { 
	      break; 
	  }
  }	

  while( VS1003_ReadReg(SPI_CLOCKF) != 0x9800 )   
  {
	  VS1003_WriteReg(SPI_CLOCKF,0x9800);   
	  HAL_Delay(2);                        /* 等待至少1.35ms */
	  if( retry++ > 100 )
	  { 
	      break; 
	  }
  }	
}

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/


#include "delay.h"


static uint8_t  fac_us=0;							//us延时倍乘数			   
static uint16_t fac_ms=0;							//ms延时倍乘数,在ucos下,代表每个节拍的ms数

#if SYSTEM_SUPPORT_OS								//如果SYSTEM_SUPPORT_OS定义了,说明要支持FreeRTOS了.

 /**
  * @brief  This function handles SysTick Handler for FreeRTOS.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{	
	if(xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
	{
		xPortSysTickHandler();
	}
}

/**
 * @brief: Init delay function
 * @param: None
 * @retval: None
 */
void delay_init(void)
{
	uint32_t reload;

	SysTick->CTRL = 0;
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);		//选择外部时钟  HCLK
	fac_us=SystemCoreClock/1000000;							//为系统时钟72  72次1us

	reload=SystemCoreClock/1000000;							//每us需要计数72下 
	reload*=1000000/configTICK_RATE_HZ;						//根据configTICK_RATE_HZ设定溢出时间, configTICK_RATE_HZ us溢出一次
															//reload为24位寄存器,最大值:16777216,在72M下,约合1.86s左右	
	fac_ms=1000/configTICK_RATE_HZ;							//代表OS可以延时的最少单位	   
	SysTick->CTRL|=SysTick_CTRL_TICKINT_Msk;   				//开启SYSTICK中断
	SysTick->LOAD=reload; 									//每1/delay_ostickspersec秒中断一次	
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;   				//开启SYSTICK    

}

//延时nus
//nus为要延时的us数.		    								   
void delay_us(uint32_t nus)
{		
	uint32_t ticks;
	uint32_t told, tnow, tcnt = 0;
	uint32_t reload = SysTick->LOAD;					//LOAD的值
	ticks = nus*fac_us;   	 
	told = SysTick->VAL;		 
	while(1)
	{
		tnow = SysTick->VAL;	
		if(tnow!=told)
		{
			if(tnow < told) tcnt += told-tnow;			//这里注意一下SYSTICK是一个递减的计数器就可以了.
			else tcnt += reload-tnow+told;	    
			told = tnow;
			if(tcnt >= ticks) break;					//时间超过/等于要延迟的时间,则退出.
		}  
	};									    

}

//延时nms
//nms:要延时的ms数
void delay_ms(uint16_t nms)
{	
	if(xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)	//如果OS已经在跑了,并且不是在中断里面(中断里面不能任务调度)	    
	{		 
		if(nms >= fac_ms)										//延时的时间大于OS的最少时间周期 
		{ 
   			vTaskDelay(nms/fac_ms);								//OS延时
		}
		nms %= fac_ms;											//OS已经无法提供这么小的延时了,采用普通方式延时    
	}
	delay_us((uint32_t)(nms*1000));								//普通方式延时  
}


#else
/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
}

/**
 * @brief: Init delay function
 * @param: None
 * @retval: None
 */
void delay_init(void)
{
	SysTick->CTRL = 0;
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);		//选择外部时钟  HCLK
	fac_us=SystemCoreClock/8000000;								//为系统时钟72  72次1us
	fac_ms=(uint16_t)fac_us*1000;								//非OS下,代表每个ms需要的systick时钟数   
}

void delay_us(uint32_t nus)
{		
	uint32_t temp;	    	 
	SysTick->LOAD=nus*fac_us; 					//时间加载	  		 
	SysTick->VAL=0x00;        					//清空计数器
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;	//开始倒数	  
	do
	{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));		//等待时间到达   
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;	//关闭计数器
	SysTick->VAL =0X00;      					 //清空计数器	 
}

//延时nms
//注意nms的范围
//SysTick->LOAD为24位寄存器,所以,最大延时为:
//nms<=0xffffff*8*1000/SYSCLK
//SYSCLK单位为Hz,nms单位为ms
//对72M条件下,nms<=1864 
void delay_ms(uint16_t nms)
{	 		  	  
	uint32_t temp;		   
	SysTick->LOAD=(uint32_t)nms*fac_ms;				//时间加载(SysTick->LOAD为24bit)
	SysTick->VAL =0x00;							//清空计数器
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;	//开始倒数  
	do
	{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));		//等待时间到达   
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;	//关闭计数器
	SysTick->VAL =0X00;       					//清空计数器	  	    
} 
#endif


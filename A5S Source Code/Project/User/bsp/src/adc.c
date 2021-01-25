 #include "adc.h"
 #include "pins.h"
		   
//初始化ADC
//这里我们仅以规则通道为例
//我们默认将开启通道0~3																	   
void  Adc_Init(void)
{ 	
	ADC_InitTypeDef ADC_InitStructure; 
	GPIO_InitTypeDef GPIO_InitStructure;

	ADC_TEMP_RCC_APBX_PERIPH_CLK_CMD;

	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //设置ADC分频因子6 72M/6=12,ADC最大时间不能超过14M
                     
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;	
	
  	#if defined(TEMP_0_PIN) 
		GPIO_InitStructure.GPIO_Pin = ADC_EXT0_PIN;
		GPIO_Init(ADC_EXT0_PORT, &GPIO_InitStructure);	
  	#endif
	
	 #if defined(TEMP_1_PIN) 
  	#endif
	
  	#if defined(TEMP_BED_PIN)
		GPIO_InitStructure.GPIO_Pin = ADC_BED_PIN;
		GPIO_Init(ADC_BED_PORT, &GPIO_InitStructure);	
  	#endif
	
	ADC_DeInit(ADC_NUMx);  //复位ADC1,将外设 ADC1 的全部寄存器重设为缺省值

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC工作模式:ADC1和ADC2工作在独立模式
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	//模数转换工作在单通道模式
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	//模数转换工作在单次转换模式
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//转换由软件而不是外部触发启动
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = 1;	//顺序进行规则转换的ADC通道的数目
	ADC_Init(ADC_NUMx, &ADC_InitStructure);	//根据ADC_InitStruct中指定的参数初始化外设ADCx的寄存器   

  
	ADC_Cmd(ADC_NUMx, ENABLE);	//使能指定的ADC1
	
	ADC_ResetCalibration(ADC_NUMx);	//使能复位校准  
	 
	while(ADC_GetResetCalibrationStatus(ADC_NUMx));	//等待复位校准结束
	
	ADC_StartCalibration(ADC_NUMx);	 //开启AD校准
 
	while(ADC_GetCalibrationStatus(ADC_NUMx));	 //等待校准结束
 
//	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//使能指定的ADC1的软件转换启动功能

}				  
//获得ADC值
//ch:通道值 0~3
u16 Get_Adc(u8 ch)   
{
  	//设置指定ADC的规则组通道，一个序列，采样时间
	ADC_RegularChannelConfig(ADC_NUMx, ch, 1, ADC_SampleTime_239Cycles5 );	//ADC3,ADC通道,采样时间为239.5周期	  			    
  
	ADC_SoftwareStartConvCmd(ADC_NUMx, ENABLE);		//使能指定的ADC3的软件转换启动功能	
	 
	while(!ADC_GetFlagStatus(ADC_NUMx, ADC_FLAG_EOC ));//等待转换结束

	return ADC_GetConversionValue(ADC_NUMx);	//返回最近一次ADC1规则组的转换结果
}

u16 Get_Adc_Average(u8 ch,u8 times)
{
	u32 temp_val=0;
	u8 t;
	for(t=0;t<times;t++)
	{
		temp_val+=Get_Adc(ch);
		bsp_DelayMS(5);
	}
	return temp_val/times;
} 	 




























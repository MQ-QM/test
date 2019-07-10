                    //STM32 ʹ��DMA+DAC+TIMER������Ҳ�//

/* 

          ��ô����ʹ��DMA+DAC+TIMER�������Ҳ���ԭ�����̣���������һ���򵥵���⣺
      �Ƚ�һ�������������Ҳ������ݱ����ھ�̬�ڴ��У�Ȼ����DAC�Լ�����ڴ��м�ʹ
      ��DMA����һ��ͨ�����������ϲ���֮��DACģ��Ϳ���ͨ��DMAͨ����ȡ��̬�ڴ���
      �����������Ҳ������ݣ���ȡ���ݣ�Ȼ�󾭹���ģ׼���������Ž�������Ϳ��Եõ���
      �Ҳ��ˡ���ô��Ȼ������ٶ��Ƿǳ���ģ����û��һ������ʱ����ô�õ��Ĺ��ƾ���
      һ���仯�ܿ��ģ�������������ʱ�����Ҫʹ�ö�ʱ��TIMER�ˡ�DAC�ڳ�ʼ����ʱ��
      �������ó�ʹ�ö�ʱ�������������ζ�ţ�����ʱ��������ʱ�򣬾ͻᴥ��DAC������
      ����һ�����Ϳ���ͨ���ı䶨ʱ���Ķ�ʱʱ�����ı����Ҳ��������ˡ�

                          ��ѹ��С����ʾ��DAC������
*/


//#include "dac.h"
#include "sys.h"
#include "math.h"
#include "waveform.h"
#include "key.h"
#include "delay.h"
#define PI  3.14159
#define DAC_DHR12R1_ADDRESS     0x40007408

u16 sinTable[tableSize];//256;
//u16 sawtoothTable[tableSize];//256;
//u16 triangleTable[tableSize];//256;
//u16 rectangleTable[tableSize];//256;

u8 KEY_Scan(u8 mode);

//�����Ҳ����ݱ����ھ�̬�ڴ���
void sin_Generation(void)
{
	u16 n;
	u16 temp=1023;
	//temp=KEY_Scan(0);
	if(KEY_Scan(0) == 4)
	{
		temp=temp+200;
	}
	for(n=0;n<tableSize;n++)
	{
		sinTable[n] = (sin(2*PI*n/tableSize)+1)*(temp);	
	}	

}

//��ݲ�
void sawtooth_Generation(void)
{
	u16 n;
	for(n=0;n<tableSize;n++)
	{
		sinTable[n] = ((2*n*1000)/tableSize);	
	}
}	


//���ǲ�
void triangle_Generation(void)
{
	u16 n;
	for(n=0;n<tableSize/2;n++)
	{
		sinTable[n] = ((2*n*1000)/tableSize);	
	}
	for(;n<tableSize;n++)
	{
		sinTable[n] = (2*1000-(2*n*1000)/tableSize);	
	}
}

//���β�
void rectangle_Generation(void)
{
	u16 n;
		for(n=0;n<tableSize/2;n++)
	{
		sinTable[n] = 0;		
	}
	for(;n<(tableSize);n++)
	{
		sinTable[n] = 1000;	 
	}
}

//����ʱ��������ʱ�򣬾ͻᴥ��DAC����������һ�����Ϳ���ͨ���ı䶨ʱ���Ķ�ʱʱ�����ı����Ҳ���������
void TIM6_Configuration(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);	

 	TIM_PrescalerConfig(TIM6, 83, TIM_PSCReloadMode_Update);
  TIM_SetAutoreload(TIM6, 4);
 	TIM_SelectOutputTrigger(TIM6, TIM_TRGOSource_Update);
	TIM_Cmd(TIM6, ENABLE);
}

void DacGPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;//�ṹ��

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//ʹ��GPIOAʱ��

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;//ѡ��4������,4,5������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;//ģ������
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//��������������
	GPIO_Init(GPIOA, &GPIO_InitStructure);//ȷ��ΪGPIOA�ĳ�ʼ��
}

//����DAC�Լ�����ڴ��м�ʹ��DMA����һ��ͨ��
void DAC_DMA_Configuration(void)
{
	DAC_InitTypeDef  DAC_InitStructure;	//DAC�ṹ��

	DMA_InitTypeDef DMA_InitStructure;//DMA�ṹ��

	//ʹ��DMA1ʱ�ӣ�������Ҫ��DMA1������ӳ�����������������
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);//ʹ��DACʱ��

	DAC_InitStructure.DAC_Trigger = DAC_Trigger_T6_TRGO;//ʹ�ô�������
	DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;//����STM32�Դ��Ĳ���
	DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Disable;//һ�����ò�ʹ���������
	//STM32��DAC������ͨ������������ʹ�õ���DAC��ͨ��1�����Զ�DAC���г�ʼ��
	DAC_Init(DAC_Channel_1, &DAC_InitStructure);

	//DMA1����ӳ�䣬DMA��������5��ͨ��7ΪDAC1����
	DMA_DeInit(DMA1_Stream5);//����������5
	DMA_InitStructure.DMA_Channel = DMA_Channel_7;//����ͨ��7  
	//�����Ҳ������ݱ����ھ�̬�ڴ���
	DMA_InitStructure.DMA_PeripheralBaseAddr = DAC_DHR12R1_ADDRESS;
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&sinTable;//����DMA�������ַ
	DMA_InitStructure.DMA_BufferSize = tableSize;//����DMA�����С
	//�������ݿ��
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	//����Ϊѭ������ģʽ
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA1_Stream5, &DMA_InitStructure);    

	DMA_Cmd(DMA1_Stream5, ENABLE);//ʹ��DMA1������5

	DAC_Cmd(DAC_Channel_1, ENABLE);//ʹ��DAC��ͨ��1

	//��DACͨ����DMAͨ��������
	DAC_DMACmd(DAC_Channel_1, ENABLE);//ʹ��DAC_DMAͨ��1
} 

//void opt(void)
//{
//	u8 t ;
//	t=KEY_Scan(0);
//	if(t==2)
//	{
//		while(1)
//		{
//			sin_Generation();
//			if(KEY_Scan(0)!=1)
//				break;
//		}
//	}
//}

void MYDAC_Init(void)
{
// opt();
	//u8 key;
	//sawtooth_Generation();
	DacGPIO_Configuration();
	TIM6_Configuration();
	DAC_DMA_Configuration();  
//	while(1)
//	{
//		key=KEY_Scan(0);		//�õ���ֵ
//	   	if(key)
//		{						   
//			switch(key)
//			{				 
//				case WKUP_PRES:	//���Ʒ�����
//					sin_Generation();
//					break;
////				case KEY0_PRES:	//����LED0��ת
////					
////					break;
//				case KEY1_PRES:	//����LED1��ת
//					triangle_Generation();
//					break;
//				case KEY2_PRES:	//ͬʱ����LED0,LED1��ת
//           sawtooth_Generation();
//					break;
//			}
//		}
//		else delay_ms(10); 
//	}
}





////	u8 opt=0;
////	opt = KEY_Scan(0);
////	if(opt==1)
////	{
////		while(1)
////		{
//	sin_Generation();
////		}
////	}
//	//else if(opt==2)
//	triangle_Generation();	
//	
//	//else if(opt==3)
//	sawtooth_Generation();
//	
//	//else if(opt==4)
//	rectangle_Generation();
//	

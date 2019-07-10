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

#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "lcd.h"
//#include "dac.h"
#include "adc.h"
#include "waveform.h"
#include "timer.h"
#include "stm32f4xx_it.h"
#include "key.h"
#include "stm32f4xx.h"                  // Device header
//void touch_task(void);  //�������ܣ�����ӣ�����ֻ��TFTLCD���Բ���
void clear_point(u16 num);//������ʾ����ǰ��	
void Set_BackGround(void);//���ñ���
void Lcd_DrawNetwork(void);//������
float get_vpp(u16 *buf);//��ȡ���ֵ
void DrawOscillogram(u16* buf);//������ͼ

void sin_Generation(void);
void sawtooth_Generation(void);
void triangle_Generation(void);
void rectangle_Generation(void);

   u32 max_data; 
	 u16 position=140;//����ͼ������
   u8	num=0;
	 u8 runstop=1;
	 

int main(void)
{
	u16   buff[800];
	float Adresult = 0;
	u8    Vpp_buff[20] = {0};
  u8 key=0;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
	delay_init(168);            //��ʼ����ʱ����
	uart_init(115200);		    //��ʼ�����ڲ�����Ϊ115200
	LED_Init();					//��ʼ��LED
 	LCD_Init();                 //��ʼ��LCD FSMC�ӿ�
  Adc_Init();                 //��ʼ��ADC
	Set_BackGround();	        //��ʾ����
	Lcd_DrawNetwork();
	KEY_Init();
	LED0 = 0;
  MYDAC_Init();
	
	
  while(1) 
		
	 {		
		 //void touch_task(void);
		 DrawOscillogram(buff);//������
		 Adresult = get_vpp(buff);//���ֵmv		
  	 sprintf((char*)Vpp_buff,"Vpp = %0.3fV",Adresult);
		 POINT_COLOR = WHITE;
		 BACK_COLOR = BLACK;
		 LCD_ShowString(156,201,66,12,12,Vpp_buff);
		//runstop=KEY_Scan(0);
//		if(runstop==1){
	   POINT_COLOR = WHITE;
	   LCD_ShowString(258,0,288,29,12,"STOP");
	   POINT_COLOR = YELLOW;
	   LCD_ShowString(289,0,319,29,12,"RUN");
		 LED0 = !LED0;//}
//else if(runstop==0)
//{
//	POINT_COLOR = YELLOW;
//	LCD_ShowString(258,0,288,29,12,"STOP");
//	LCD_ShowString(289,0,319,29,12,"RUN");
//}	
		 
		 key=KEY_Scan(0);		//�õ���ֵ
	   	if(key)
		{						   
			switch(key)
			{				 
				case WKUP_PRES:	//���Ʒ�����
					sin_Generation();
					break;
//				case KEY0_PRES:	//����LED0��ת
//					runstop=0;
//					break;
				case KEY1_PRES:	//����LED1��ת
					triangle_Generation();
					break;
				case KEY2_PRES:	//ͬʱ����LED0,LED1��ת
           sawtooth_Generation();
					break;
			}
		}
		else if(key == 0)
		{
			do
			{
			//runstop=0;
			//void DrawOscillogram(u16* buf);
			POINT_COLOR = YELLOW;
	  	LCD_ShowString(258,0,288,29,12,"STOP");
     	POINT_COLOR = WHITE;
			LCD_ShowString(289,0,319,29,12,"RUN");
				key=KEY_Scan(0);				
				if(key==0||key==2||key==3||key==4)break;
			}while(1);
			
		}
	 } 
}

void clear_point(u16 num)//������ʾ����ǰ��
{
	u16 lie = 0; 
	POINT_COLOR = BLACK ;
	for(lie = 1;lie < 199;lie++)
	{		
		LCD_DrawPoint(num,lie );
	}
	if(!(num%50))//�ж�hang�Ƿ�Ϊ50�ı��� ���е�
	{
		for(lie = 10;lie < 199;lie += 10)
		{		
			LCD_Fast_DrawPoint(num ,lie,WHITE );
		}
	}
	if(!(num%10))//�ж�hang�Ƿ�Ϊ10�ı��� ���е�
	{
		for(lie = 50;lie <199;lie += 50)
		{		
			LCD_Fast_DrawPoint(num ,lie,WHITE );
		}
	}	
	POINT_COLOR = YELLOW;	
}

void DrawOscillogram(u16 *buff)//������ͼ
{
	runstop = KEY_Scan(0);
	
	if(runstop==1|runstop==2|runstop==3|runstop==4)//�����Ҳ�
		{
			static u16 Ypos1 = 0,Ypos2 = 0;
			u16 i = 0;
			for(i = 1;i < 255;i++)//�洢AD��ֵ
				{
					buff[i]=Get_Adc(ADC_Channel_5);
//					delay_us(1);
				}
				for(i = 1;i < 255;i++)
				{
					clear_point(i);	
					Ypos2 = position - ( buff[i] * 165 / 4096);//ת������//4096
					if(Ypos2 >255)
						Ypos2 =255; //������Χ����ʾ
					LCD_DrawLine (i ,Ypos1 , i+1 ,Ypos2);//��������
				Ypos1 = Ypos2 ;
				}
    Ypos1 = 0;	
		}
		
//		else if(runstop==0)//��ͣ��̬��ʾ
//		{
//			do
//			{
////				u8 a=0;
//				runstop = KEY_Scan(0);
//				POINT_COLOR = YELLOW;
//				LCD_ShowString(258,0,288,29,12,"STOP");
//				POINT_COLOR = WHITE;
//				LCD_ShowString(289,0,319,29,12,"RUN");
//				//runstop=1;
//				if( runstop == 2|runstop==3|runstop==4 ) break;
//			}
//			while(1);
//		}
		
		else if(runstop==2)
		{
//			static u16 Ypos1 = 0,Ypos2 = 0;
//			u16 i = 0;
//			for(i = 1;i < 255;i++)//�洢AD��ֵ
//				{
//					buff[i]=Get_Adc(ADC_Channel_5);
////					delay_us(1);
//				}
//				for(i = 1;i < 255;i++)
//				{
//					clear_point(i);	
//					Ypos2 = position - ( buff[i] * 165 / 4096);//ת������//4096
//					if(Ypos2 >255)
//						Ypos2 =255; //������Χ����ʾ
//					LCD_DrawLine (i ,Ypos1 , i+1 ,Ypos2);//��������
//				Ypos1 = Ypos2 ;
//				}
//    Ypos1 = 0;	
		}
		
		else if(runstop==3)
		{
			
		}
		
		else if(runstop==4)
		{
			
		}

}
void Set_BackGround(void)
{
 LCD_Clear(BLACK);
	POINT_COLOR = WHITE;
	LCD_DrawRectangle(0,0,257,200);
//	POINT_COLOR = WHITE;
//	LCD_ShowString(258,0,288,29,12,"STOP");
//	LCD_ShowString(289,0,319,29,12,"RUN");
}

void Lcd_DrawNetwork(void)
{
	u16 index_y = 0;
	u16 index_x = 0;	
	POINT_COLOR = WHITE;
	LCD_DrawRectangle(258,0,288,29);
	POINT_COLOR = WHITE;
	LCD_DrawRectangle(289,0,319,29);
    //���е�	
	for(index_x = 50;index_x < 255;index_x += 50)
	{
		for(index_y = 10;index_y < 199;index_y += 10)
		{
			LCD_Fast_DrawPoint(index_x,index_y,WHITE);	
		}
	}
	//���е�
	for(index_y = 50;index_y < 199;index_y += 50)
	{
		for(index_x = 10;index_x < 255;index_x += 10)
		{
			LCD_Fast_DrawPoint(index_x,index_y,WHITE);	
		}
	}
}

float get_vpp(u16 *buf)	   //��ȡ���ֵ
{
	
  u32 max_data=buf[0];
	u32 min_data=buf[0];
	u32 n=0;
	float Vpp=0;
	for(n = 1;n<256;n++)
	{
		if(buf[n] > max_data)
		{
			max_data = buf[n];
		}	
		else if(buf[n] < min_data)
			min_data = buf[n];
	} 	
	Vpp = (float)(max_data - min_data);
	Vpp = Vpp*(3.3/4096);
	max_data = max_data*(3.3/4096);
	min_data = min_data*(3.3/4096);
	return Vpp;	
}


			
		
	

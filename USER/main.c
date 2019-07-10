#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "lcd.h"
#include "dac.h"
#include "adc.h"
#include "waveform.h"
#include "timer.h"
#include "stm32f4xx_it.h"
//ALIENTEK ̽����STM32F407������ ʵ��13
//LCD��ʾʵ��-�⺯���汾
//����֧�֣�www.openedv.com
//�Ա����̣�http://eboard.taobao.com  
//������������ӿƼ����޹�˾  
//���ߣ�����ԭ�� @ALIENTEK


void clear_point(u16 num);//������ʾ����ǰ��	
void Set_BackGround(void);//���ñ���
void Lcd_DrawNetwork(void);//������
float get_vpp(u16 *buf);//��ȡ���ֵ
void DrawOscillogram(u16* buf);//������ͼ	

int main(void)
{
	u16   buff[800];
	float Adresult = 0;
	u8    Vpp_buff[20] = {0};
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
	delay_init(168);            //��ʼ����ʱ����
	uart_init(115200);		    //��ʼ�����ڲ�����Ϊ115200
	LED_Init();					//��ʼ��LED
 	LCD_Init();                 //��ʼ��LCD FSMC�ӿ�
    Adc_Init();                 //��ʼ��ADC
	Set_BackGround();	        //��ʾ����
	Lcd_DrawNetwork();
    MYDAC_Init();               //�������Ҳ�
	LED0 = 0;
	
  	while(1) 
	{		
		DrawOscillogram(buff);//������
		Adresult = get_vpp(buff);//���ֵmv		
		sprintf((char*)Vpp_buff,"Vpp = %0.3fV",Adresult);
		POINT_COLOR = WHITE;
		BACK_COLOR = DARKBLUE;
		LCD_ShowString(330,425,210,24,24,Vpp_buff);
		LED0 = !LED0;					
	} 
}

void clear_point(u16 num)//������ʾ����ǰ��
{
	u16 index_clear_lie = 0; 
	POINT_COLOR = DARKBLUE ;
	for(index_clear_lie = 1;index_clear_lie < 400;index_clear_lie++)
	{		
		LCD_DrawPoint(num,index_clear_lie );
	}
	if(!(num%50))//�ж�hang�Ƿ�Ϊ50�ı��� ���е�
	{
		for(index_clear_lie = 10;index_clear_lie < 400;index_clear_lie += 10)
		{		
			LCD_Fast_DrawPoint(num ,index_clear_lie,WHITE );
		}
	}
	if(!(num%10))//�ж�hang�Ƿ�Ϊ10�ı��� ���е�
	{
		for(index_clear_lie = 50;index_clear_lie <400;index_clear_lie += 50)
		{		
			LCD_Fast_DrawPoint(num ,index_clear_lie,WHITE );
		}
	}	
	POINT_COLOR = YELLOW;	
}

void DrawOscillogram(u16 *buff)//������ͼ
{
	static u16 Ypos1 = 0,Ypos2 = 0;
	u16 Yinit=100;
	u16 i = 0;
	POINT_COLOR = YELLOW;
	for(i = 1;i < 700;i++)//�洢AD��ֵ
	{
		buff[i] = Get_Adc(ADC_Channel_5);
	}
	for(i = 1;i < 700;i++)
	{
		clear_point(i );	
		Ypos2 = 400 - (Yinit + buff[i] * 165 / 4096);//ת������
		//Ypos2 = Ypos2 * bei;
		if(Ypos2 >700)
			Ypos2 =700; //������Χ����ʾ
		LCD_DrawLine (i ,Ypos1 , i+1 ,Ypos2);
		Ypos1 = Ypos2 ;
	}
    Ypos1 = 0;	
}

void Set_BackGround(void)
{
	POINT_COLOR = YELLOW;
    LCD_Clear(DARKBLUE);
	LCD_DrawRectangle(0,0,700,400);//����
	//LCD_DrawLine(0,220,700,220);//����
	//LCD_DrawLine(350,20,350,420);//����
	//POINT_COLOR = WHITE;
	//BACK_COLOR = DARKBLUE;
	//LCD_ShowString(330,425,210,24,24,(u8*)"vpp=");	
}

void Lcd_DrawNetwork(void)
{
	u16 index_y = 0;
	u16 index_x = 0;	
	
    //���е�	
	for(index_x = 50;index_x < 700;index_x += 50)
	{
		for(index_y = 10;index_y < 400;index_y += 10)
		{
			LCD_Fast_DrawPoint(index_x,index_y,WHITE);	
		}
	}
	//���е�
	for(index_y = 50;index_y < 400;index_y += 50)
	{
		for(index_x = 10;index_x < 700;index_x += 10)
		{
			LCD_Fast_DrawPoint(index_x,index_y,WHITE);	
		}
	}
}

float get_vpp(u16 *buf)	   //��ȡ���ֵ
{
	
	u32 max_data=buf[1];
	u32 min_data=0;//buf[1];
	u32 n=0;
	float Vpp=0;
	for(n = 1;n<700;n++)
	{
		if(buf[n] > max_data)
		{
			max_data = buf[n];
		}
//		if(buf[n] < min_data)
//		{
//			min_data = buf[n];
//		}			
	} 	
	Vpp = (float)(max_data - min_data);
	Vpp = Vpp*(3.3/4096);
	return Vpp;	
}


/*
 * finger.c
 *
 *  Created on: 2024年5月17日
 *      Author: lenovo
 */

#include "xil_io.h"
#include "stdio.h"
#include "xgpio_l.h"
#include "xintc_l.h"
#include "xuartlite_l.h"

volatile unsigned char FPM10A_RECEICE_BUFFER[32];
unsigned int recv_cnt = 0;
//volatile unsigned char recv_tmp[32];
unsigned char ucLength;
unsigned int finger_id = 0;
unsigned char finger_data_received = 0;

//FINGERPRINT通信协议定义

unsigned char FPM10A_Get_Device[10] ={0x01,0x00,0x07,0x13,0x00,0x00,0x00,0x00,0x00,0x1b};//口令验证
unsigned char FPM10A_Pack_Head[6] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF};  //协议包头
unsigned char FPM10A_Get_Img[6] = {0x01,0x00,0x03,0x01,0x00,0x05};    //获得指纹图像
unsigned char FPM10A_Get_Templete_Count[6] ={0x01,0x00,0x03,0x1D,0x00,0x21 }; //获得模版总数
unsigned char FPM10A_Search[11]={0x01,0x00,0x08,0x04,0x01,0x00,0x00,0x03,0xE7,0x00,0xF8}; //搜索指纹搜索范围0 - 999,使用BUFFER1中的特征码搜索
unsigned char FPM10A_Search_0_9[11]={0x01,0x00,0x08,0x04,0x01,0x00,0x00,0x00,0x13,0x00,0x21}; //搜索0-9号指纹
unsigned char FPM10A_Img_To_Buffer1[7]={0x01,0x00,0x04,0x02,0x01,0x00,0x08}; //将图像放入到BUFFER1
unsigned char FPM10A_Img_To_Buffer2[7]={0x01,0x00,0x04,0x02,0x02,0x00,0x09}; //将图像放入到BUFFER2
unsigned char FPM10A_Reg_Model[6]={0x01,0x00,0x03,0x05,0x00,0x09}; //将BUFFER1跟BUFFER2合成特征模版
unsigned char FPM10A_Delete_All_Model[6]={0x01,0x00,0x03,0x0d,0x00,0x11};//删除指纹模块里所有的模版
volatile unsigned char  FPM10A_Save_Finger[9]={0x01,0x00,0x06,0x06,0x01,0x00,0x0B,0x00,0x19};//将BUFFER1中的特征码存放到指定的位置
//volatile:系统总是重新从它所在的内存读取数据，即使它前面的指令刚刚从该处读取过数据
/*------------------ FINGERPRINT命令字 --------------------------*/
 //发送包头
void FPM10A_Cmd_Send_Pack_Head(void)
{
	int i;
	for(i=0;i<6;i++) //包头
   {
     Xil_Out8(XPAR_AXI_UARTLITE_1_BASEADDR+XUL_TX_FIFO_OFFSET, FPM10A_Pack_Head[i]);
    }
}
//发送指令
void FPM10A_Cmd_Check(void)
{
	int i=0;
	FPM10A_Cmd_Send_Pack_Head(); //发送通信协议包头
	for(i=0;i<10;i++)
	 {
		Xil_Out8(XPAR_AXI_UARTLITE_1_BASEADDR+XUL_TX_FIFO_OFFSET, FPM10A_Get_Device[i]);
	  }
}
////接收反馈数据缓冲
//void FPM10A_Receive_Data(unsigned char ucLength)
//{
//  unsigned char i;
//
//  for (i=0;i<ucLength;i++)
//     FPM10A_RECEICE_BUFFER[i] = Uart_Receive_Byte();
//
//}

//FINGERPRINT_获得指纹图像命令
void FPM10A_Cmd_Get_Img(void)
{
    unsigned char i;
    FPM10A_Cmd_Send_Pack_Head(); //发送通信协议包头
    for(i=0;i<6;i++) //发送命令 0x1d
	{
    	Xil_Out8(XPAR_AXI_UARTLITE_1_BASEADDR+XUL_TX_FIFO_OFFSET, FPM10A_Get_Img[i]);
	}
}
//讲图像转换成特征码存放在Buffer1中
void FINGERPRINT_Cmd_Img_To_Buffer1(void)
{
 	unsigned char i;
	FPM10A_Cmd_Send_Pack_Head(); //发送通信协议包头
   	for(i=0;i<7;i++)   //发送命令 将图像转换成 特征码 存放在 CHAR_buffer1
     {
   		Xil_Out8(XPAR_AXI_UARTLITE_1_BASEADDR+XUL_TX_FIFO_OFFSET, FPM10A_Img_To_Buffer1[i]);
   	  }
}
//将图像转换成特征码存放在Buffer2中
void FINGERPRINT_Cmd_Img_To_Buffer2(void)
{
     unsigned char i;
     for(i=0;i<6;i++)    //发送包头
	 {
    	 Xil_Out8(XPAR_AXI_UARTLITE_1_BASEADDR+XUL_TX_FIFO_OFFSET, FPM10A_Pack_Head[i]);
   	 }
     for(i=0;i<7;i++)   //发送命令 将图像转换成 特征码 存放在 CHAR_buffer1
      {
    	 Xil_Out8(XPAR_AXI_UARTLITE_1_BASEADDR+XUL_TX_FIFO_OFFSET, FPM10A_Img_To_Buffer2[i]);
   	  }
}
//搜索全部用户999枚
void FPM10A_Cmd_Search_Finger(void)
{
       unsigned char i;
			 FPM10A_Cmd_Send_Pack_Head(); //发送通信协议包头
       for(i=0;i<11;i++)
           {
    	   Xil_Out8(XPAR_AXI_UARTLITE_1_BASEADDR+XUL_TX_FIFO_OFFSET, FPM10A_Search[i]);
   		   }
}

void FPM10A_Cmd_Reg_Model(void)
{
       unsigned char i;

			 FPM10A_Cmd_Send_Pack_Head(); //发送通信协议包头

       for(i=0;i<6;i++)
           {
    	   Xil_Out8(XPAR_AXI_UARTLITE_1_BASEADDR+XUL_TX_FIFO_OFFSET, FPM10A_Reg_Model[i]);
   		   }


}
//删除指纹模块里的所有指纹模版
void FINGERPRINT_Cmd_Delete_All_Model(void)
{
     unsigned char i;
    for(i=0;i<6;i++) //包头
    	Xil_Out8(XPAR_AXI_UARTLITE_1_BASEADDR+XUL_TX_FIFO_OFFSET, FPM10A_Pack_Head[i]);
    for(i=0;i<6;i++) //命令合并指纹模版
	   {
    	Xil_Out8(XPAR_AXI_UARTLITE_1_BASEADDR+XUL_TX_FIFO_OFFSET, FPM10A_Delete_All_Model[i]);
		 }
}
//保存指纹
void FPM10A_Cmd_Save_Finger( unsigned int storeID )
{
       unsigned long temp = 0;
		   unsigned char i;
       FPM10A_Save_Finger[5] =(storeID&0xFF00)>>8;
       FPM10A_Save_Finger[6] = (storeID&0x00FF);
		   for(i=0;i<7;i++)   //计算校验和
		   	   temp = temp + FPM10A_Save_Finger[i];
		   FPM10A_Save_Finger[7]=(temp & 0x00FF00) >> 8; //存放校验数据
		   FPM10A_Save_Finger[8]= temp & 0x0000FF;
       FPM10A_Cmd_Send_Pack_Head(); //发送通信协议包头
       for(i=0;i<9;i++)
    	   Xil_Out8(XPAR_AXI_UARTLITE_1_BASEADDR+XUL_TX_FIFO_OFFSET, FPM10A_Save_Finger[i]);      //发送命令
}


//添加指纹
void FPM10A_Add_Fingerprint()
{
	// while(1){
		finger_data_received = 0;
		FPM10A_Cmd_Get_Img(); //获得指纹图像
		ucLength = 12;
//		while(finger_data_received == 0);
		//判断接收到的确认码,等于0指纹获取成功
		xil_printf("%d %d\n",finger_data_received, FPM10A_RECEICE_BUFFER[9]);
		if(finger_data_received == 1 && FPM10A_RECEICE_BUFFER[9]==0)
		{
//				for(int i=0;i<100000;i++);
//				Delay_Ms(100);
			finger_data_received = 0;
			FINGERPRINT_Cmd_Img_To_Buffer1();
//			while(finger_data_received == 0);
//					for(int i=0;i<1000000;i++);
//					Buzz_Times(1);
//					Delay_Ms(1000);
			// while(1)
			// {
				finger_data_received = 0;
				FPM10A_Cmd_Get_Img(); //获得指纹图像
				//判断接收到的确认码,等于0指纹获取成功
				if(finger_data_received ==1 && FPM10A_RECEICE_BUFFER[9]==0)
				{
					finger_data_received = 0;
					FINGERPRINT_Cmd_Img_To_Buffer2();
//					while(finger_data_received == 0);

					finger_data_received = 0;
					FPM10A_Cmd_Reg_Model();//转换成特征码
//					while(finger_data_received == 0);

					finger_data_received = 0;
					FPM10A_Cmd_Save_Finger(finger_id);
//					while(finger_data_received == 0);
//						Buzz_Times(1);
//						Delay_Ms(1000);
					finger_id=finger_id+1;
					xil_printf("success,id: %d\n", finger_id-1);
					for(int j=0;j<15;j++)
					{
						xil_printf("%x ", FPM10A_RECEICE_BUFFER[j]);
					}
				    // break;
				}
			// }

	        // break;
		}
	// }
		xil_printf("end\n");
}

////搜索指纹
//void FPM10A_Find_Fingerprint()
//{
//	unsigned int find_fingerid = 0;
//	unsigned char id_show[]={0,0,0};
//	do
//	{
//		LCD1602_Display(0x80," Please  finger ",0,16);
//		LCD1602_Display(0xc0,"                ",0,16);
//		FPM10A_Cmd_Get_Img(); //获得指纹图像
//		FPM10A_Receive_Data(12);
//		//判断接收到的确认码,等于0指纹获取成功
//		if(FPM10A_RECEICE_BUFFER[9]==0)
//		{
//			Delay_Ms(100);
//			FINGERPRINT_Cmd_Img_To_Buffer1();
//			FPM10A_Receive_Data(12);
//			FPM10A_Cmd_Search_Finger();
//			FPM10A_Receive_Data(16);
//			if(FPM10A_RECEICE_BUFFER[9] == 0) //搜索到
//			{
//				 LCD1602_Display(0x80," Search success ",0,16);
//				 LCD1602_Display(0xc0,"    ID is       ",0,16);
//				 Buzz_Times(1);
//				//拼接指纹ID数
//				 find_fingerid = FPM10A_RECEICE_BUFFER[10]*256 + FPM10A_RECEICE_BUFFER[11];
//				 //指纹iD值显示处理
//				 LCD1602_WriteCMD(0xc0+10);
//				 LCD1602_WriteDAT(find_fingerid/100+48);
//				 LCD1602_WriteDAT(find_fingerid%100/10+48);
//				 LCD1602_WriteDAT(find_fingerid%100%10+48);
//				 SRD = 0;
//				 Delay_Ms(3000);
//				 SRD = 1;
//			  }
//				else //没有找到
//				{
//					LCD1602_Display(0x80," Search  failed ",0,16);
//					LCD1602_Display(0xc0,"                ",0,16);
//				 	Buzz_Times(3);
//				}
//			}
//		}while(KEY_CANCEL == 1);
//}
////删除所有存贮的指纹库
//void FPM10A_Delete_All_Fingerprint()
//{
//		unsigned char i=0;
//				LCD1602_Display(0x80,"   empty all    ",0,16);
//				LCD1602_Display(0xc0,"   Yes or no ?  ",0,16);
//		do
//		 {
//			if(KEY_OK==0)
//			{
//				while(KEY_OK==0);
//				LCD1602_Display(0x80,"   emptying     ",0,16);
//				LCD1602_Display(0xc0,"                ",0,16);
//				Delay_Ms(300);
//				LCD1602_WriteCMD(0xc0);
//				for(i=0;i<16;i++)
//				 {
//					LCD1602_WriteDAT(42);
//					Delay_Ms(100);
//				 }
//				FINGERPRINT_Cmd_Delete_All_Model();
//			  FPM10A_Receive_Data(12);
//				LCD1602_Display(0x80,"   All empty    ",0,16);
//				LCD1602_Display(0xc0,"                ",0,16);
//				Buzz_Times(3);
//				break;
//			}
//		 }while(KEY_CANCEL==1);
//}
//
//
//
//
//
//

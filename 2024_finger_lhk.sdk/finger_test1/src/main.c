/*
 * finger_test1.c
 *
 *  Created on: 2024年5月17日
 *      Author: lenovo
 */
/*
 * SerialIO_task0.c
 *
 *  Created on: 2023年4月26日
 *      Author: lenovo
 */
/*fast interrupt*/
#include "xil_io.h"
#include "stdio.h"
#include "xgpio_l.h"
#include "xintc_l.h"
#include "xuartlite_l.h"
#include "xtmrctr.h"
#include "finger.h"

#define RESET_VALUE 100000-2 //0.001s

void UART1_RECV()__attribute__((fast_interrupt));
//void UART2_RECV()__attribute__((fast_interrupt));
void BtnHandler()__attribute__((fast_interrupt));
void SwtHandler()__attribute__((fast_interrupt));
void timer_handle()__attribute__((fast_interrupt));


int main()
{
    /*GPIO*/
    /*GPIO_0*/
    Xil_Out16(XPAR_AXI_GPIO_0_BASEADDR + XGPIO_TRI_OFFSET, 0xffff);//switch
    Xil_Out16(XPAR_AXI_GPIO_0_BASEADDR + XGPIO_TRI2_OFFSET, 0x0);//LED
    Xil_Out32(XPAR_AXI_GPIO_0_BASEADDR+XGPIO_IER_OFFSET,XGPIO_IR_CH1_MASK);//IER 通道1允许中断(sw)
    Xil_Out32(XPAR_AXI_GPIO_0_BASEADDR+XGPIO_GIE_OFFSET,XGPIO_GIE_GINTR_ENABLE_MASK);//GIE 允许GPIO_0中断输出

    /*GPIO_1*/
    Xil_Out8(XPAR_AXI_GPIO_1_BASEADDR + XGPIO_TRI_OFFSET, 0x0);//设定位码为输出
    Xil_Out8(XPAR_AXI_GPIO_1_BASEADDR + XGPIO_TRI2_OFFSET, 0x0);//设定段码为输出
    Xil_Out8(XPAR_AXI_GPIO_1_BASEADDR + XGPIO_DATA_OFFSET, 0xfe);//segment初值，DATA位选

    /*GPIO_2*/
    Xil_Out8(XPAR_AXI_GPIO_2_BASEADDR + XGPIO_TRI_OFFSET, 0x1f);//设定5个按键为输入
    Xil_Out32(XPAR_AXI_GPIO_2_BASEADDR+XGPIO_IER_OFFSET,XGPIO_IR_CH1_MASK); //CH1允许 中断，CH1对应的是按键
    Xil_Out32(XPAR_AXI_GPIO_2_BASEADDR+XGPIO_GIE_OFFSET,XGPIO_GIE_GINTR_ENABLE_MASK);//允许 按键和LD所对应的GPIO IP核 GPIO_2 中断输出

    /*UART*/
    Xil_Out32(XPAR_AXI_UARTLITE_1_BASEADDR+XUL_CONTROL_REG_OFFSET,
            XUL_CR_ENABLE_INTR|XUL_CR_FIFO_RX_RESET|XUL_CR_FIFO_TX_RESET); //启用中断、复位了接收和发送FIFO
//    Xil_Out32(XPAR_AXI_UARTLITE_2_BASEADDR+XUL_CONTROL_REG_OFFSET,
//            XUL_CR_ENABLE_INTR|XUL_CR_FIFO_RX_RESET|XUL_CR_FIFO_TX_RESET);

    //Timer T0
    Xil_Out32(XPAR_AXI_TIMER_0_BASEADDR+XTC_TCSR_OFFSET,
    		Xil_In32((XPAR_AXI_TIMER_0_BASEADDR+XTC_TCSR_OFFSET)&~XTC_CSR_ENABLE_TMR_MASK));//写 TCSR，停止定时器
    Xil_Out32(XPAR_AXI_TIMER_0_BASEADDR+XTC_TLR_OFFSET,RESET_VALUE);//写TLR 预置定时器初值
    Xil_Out32(XPAR_AXI_TIMER_0_BASEADDR+XTC_TCSR_OFFSET,
    		Xil_In32((XPAR_AXI_TIMER_0_BASEADDR+XTC_TCSR_OFFSET)|XTC_CSR_LOAD_MASK));//装载计数器初值，写 TCSR 使得 LOADx=1
    Xil_Out32(XPAR_AXI_TIMER_0_BASEADDR+XTC_TCSR_OFFSET,
    		Xil_In32((XPAR_AXI_TIMER_0_BASEADDR+XTC_TCSR_OFFSET)&XTC_CSR_LOAD_MASK)\
			|XTC_CSR_ENABLE_TMR_MASK|XTC_CSR_AUTO_RELOAD_MASK|XTC_CSR_ENABLE_INT_MASK|XTC_CSR_DOWN_COUNT_MASK);
    //LOADx=0（不装载），ENTx=1（中断使能），ARHT=1（自动重复装载），
    //设置 T0 工作模式并启动计数器，其余位不变
    //开始计时运行，自动获取，允许中断，减计数

    /*INTC*/
    Xil_Out32(XPAR_INTC_0_BASEADDR+XIN_IAR_OFFSET,
            Xil_In32(XPAR_INTC_0_BASEADDR+XIN_ISR_OFFSET));
    Xil_Out32(XPAR_INTC_0_BASEADDR+XIN_IER_OFFSET,XPAR_AXI_GPIO_0_IP2INTC_IRPT_MASK|XPAR_AXI_GPIO_2_IP2INTC_IRPT_MASK|
            XPAR_AXI_UARTLITE_1_INTERRUPT_MASK);//使能
    Xil_Out32(XPAR_INTC_0_BASEADDR+XIN_IMR_OFFSET,XPAR_AXI_GPIO_2_IP2INTC_IRPT_MASK|XPAR_AXI_GPIO_0_IP2INTC_IRPT_MASK|
            XPAR_AXI_UARTLITE_1_INTERRUPT_MASK);//工作模式
    Xil_Out32(XPAR_INTC_0_BASEADDR+XIN_MER_OFFSET,XIN_INT_MASTER_ENABLE_MASK|XIN_INT_HARDWARE_ENABLE_MASK);//允许中断输出
    Xil_Out32(XPAR_INTC_0_BASEADDR+XIN_IVAR_OFFSET+4*XPAR_INTC_0_GPIO_2_VEC_ID,(u32)BtnHandler);
    Xil_Out32(XPAR_INTC_0_BASEADDR+XIN_IVAR_OFFSET+4*XPAR_INTC_0_GPIO_0_VEC_ID,(u32)SwtHandler);
    Xil_Out32(XPAR_INTC_0_BASEADDR+XIN_IVAR_OFFSET+4*XPAR_INTC_0_UARTLITE_1_VEC_ID,(u32)UART1_RECV);
//    Xil_Out32(XPAR_INTC_0_BASEADDR+XIN_IVAR_OFFSET+4*XPAR_INTC_0_UARTLITE_2_VEC_ID,(u32)UART2_RECV);
    Xil_Out32(XPAR_INTC_0_BASEADDR+XIN_MER_OFFSET,XIN_INT_MASTER_ENABLE_MASK|XIN_INT_HARDWARE_ENABLE_MASK);
    Xil_Out32(XPAR_INTC_0_BASEADDR + XIN_IVAR_OFFSET +
    		4*XPAR_INTC_0_TMRCTR_0_VEC_ID, (int)timer_handle); //IVAR填中断向量表
    //填写中断控制器维护的中断向量表 写IVAR寄存器

    /*MicroBlaze*/
    microblaze_enable_interrupts();
    //microblaze中断开放
    return 0;
}

unsigned char pkg[12] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF, 0x01, 0x00, 0x03, 0x10, 0x00, 0x14};  //协议包头

void BtnHandler()
{
    u8 btncode=Xil_In8(XPAR_AXI_GPIO_2_BASEADDR+XGPIO_DATA_OFFSET);

    if(btncode)
    {
    	xil_printf("\n button %d \n", btncode);
    	Xil_Out16(XPAR_AXI_GPIO_0_BASEADDR + XGPIO_DATA2_OFFSET, btncode); //测试按键
    	switch (btncode)
    	{
    		case 0x1:
//    			for(int i=0; i<12; i++)
//				{
//    				Xil_Out8(XPAR_AXI_UARTLITE_1_BASEADDR+XUL_TX_FIFO_OFFSET, pkg[i]);
//				}
    			FPM10A_Add_Fingerprint();
    			break;

    	}

    }
//    if(btncode)
//    {
//    //将按键值写入UART2的发送FIFO到UART1
//    Xil_Out32(XPAR_AXI_UARTLITE_2_BASEADDR+XUL_TX_FIFO_OFFSET,btncode);
//    }
//	int finger_code;
//	scanf("%x", &finger_code);
    Xil_Out32(XPAR_AXI_GPIO_2_BASEADDR+XGPIO_ISR_OFFSET,
            Xil_In32(XPAR_AXI_GPIO_2_BASEADDR+XGPIO_ISR_OFFSET));//清除中断
}

void SwtHandler(){
    //将开关写入UART1的发送FIFO到UART2
    Xil_Out32(XPAR_AXI_UARTLITE_1_BASEADDR+XUL_TX_FIFO_OFFSET,Xil_In32(XPAR_AXI_GPIO_0_BASEADDR+XGPIO_DATA_OFFSET));
    Xil_Out32(XPAR_AXI_GPIO_0_BASEADDR+XGPIO_ISR_OFFSET,Xil_In32(XPAR_AXI_GPIO_0_BASEADDR+XGPIO_ISR_OFFSET));   //清除中断
}

//void UART2_RECV()
//{
//    if((Xil_In32(XPAR_AXI_UARTLITE_2_BASEADDR+XUL_STATUS_REG_OFFSET)
//            &XUL_SR_RX_FIFO_VALID_DATA)==XUL_SR_RX_FIFO_VALID_DATA)
//        Xil_Out32(XPAR_AXI_GPIO_0_BASEADDR+XGPIO_DATA2_OFFSET,
//                Xil_In32(XPAR_AXI_UARTLITE_2_BASEADDR+XUL_RX_FIFO_OFFSET));
//}


unsigned char recv_tmp[32];

void UART1_RECV()
{
    if((Xil_In32(XPAR_AXI_UARTLITE_1_BASEADDR+XUL_STATUS_REG_OFFSET)
            &XUL_SR_RX_FIFO_VALID_DATA)==XUL_SR_RX_FIFO_VALID_DATA)
    {
//        u8 btncode = Xil_In8(XPAR_AXI_UARTLITE_1_BASEADDR+XUL_RX_FIFO_OFFSET);
//        for(int i=0;i<5;i++)
//        {
//            if(btncode==scancode[i][0])
//            {
//                btncode=scancode[i][1];
//                Xil_Out32(XPAR_AXI_GPIO_1_BASEADDR+XGPIO_DATA2_OFFSET,btncode);
//                break;
//            }
//        }
    	u8 finger_recv = Xil_In8(XPAR_AXI_UARTLITE_1_BASEADDR+XUL_RX_FIFO_OFFSET);
    	xil_printf("0x%x ",finger_recv);
    	recv_tmp[recv_cnt] = finger_recv;
    	recv_cnt ++;
    	xil_printf(" %d\n", recv_cnt);
    	if(recv_cnt >= ucLength)
    	{
    		recv_cnt = 0;
    		finger_data_received = 1;
    		for(int i=0; i<ucLength; i++)
    		{
    			FPM10A_RECEICE_BUFFER[i] = recv_tmp[i];
    			recv_tmp[i] = 0;
    		}
//    		if(recv_finger[9] == 0x0) xil_printf("Success!\n");
//    		else xil_printf("No finger\n");
    	}
    }
}

void timer_handle()
{

}

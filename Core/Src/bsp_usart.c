#include "bsp_usart.h"

//DebugUartHandle管理串口所有配置
UART_HandleTypeDef DebugUartHandle;
//WifiUart
UART_HandleTypeDef WifiUartHandle;

//配置与硬件底层无关内容：如串口协议，其中包括波特率，奇偶校验，停止位
void DEBUG_USART_Config()
{
    DebugUartHandle.Instance = DEBUG_USART;

    //波特率，8位字长，1停止位，无奇偶校验，无硬件控制，收发模式
    DebugUartHandle.Init.BaudRate = DEBUG_USART_BAUDRATE;
    DebugUartHandle.Init.WordLength = UART_WORDLENGTH_8B;
    DebugUartHandle.Init.StopBits = UART_STOPBITS_1;//stm32f1xx_hal_uart.h
    DebugUartHandle.Init.Parity = UART_PARITY_NONE;
    DebugUartHandle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    DebugUartHandle.Init.Mode = UART_MODE_TX_RX;

    HAL_UART_Init(&DebugUartHandle);

    //使能串口接收
    __HAL_UART_ENABLE_IT(&DebugUartHandle,UART_IT_RXNE);
}

//配置WiFi的通信的uart3
void WIFI_USART_Config()
{
    WifiUartHandle.Instance = WIFI_USART;

    WifiUartHandle.Init.BaudRate = DEBUG_USART_BAUDRATE;
    WifiUartHandle.Init.WordLength = UART_WORDLENGTH_8B;
    WifiUartHandle.Init.StopBits = UART_STOPBITS_1;//stm32f1xx_hal_uart.h
    WifiUartHandle.Init.Parity = UART_PARITY_NONE;
    WifiUartHandle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    WifiUartHandle.Init.Mode = UART_MODE_TX_RX;

    HAL_UART_Init(&WifiUartHandle);

    //使能串口接收
    __HAL_UART_ENABLE_IT(&WifiUartHandle,UART_IT_RXNE);
}

// mcu 底层硬件相关的配置如引脚、时钟、DMA、中断
//实际被HAL_UART_Init（stm32f1xx_hal_uart.c）该函数调用
/*
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    if(huart->Instance == USART1)
    {
        GPIO_InitTypeDef GPIO_Init1;

        //串口1时钟 GPIO时钟
        DEBUG_USART_CLK_ENABLE();
        DEBUG_USART_RX_GPIO_CLK_ENABLE();
        DEBUG_USART_TX_GPIO_CLK_ENABLE();

        //配置引脚复用功能 TX
        GPIO_Init1.Pin = DEBUG_USART_TX_PIN;
        GPIO_Init1.Mode = GPIO_MODE_AF_PP;
        GPIO_Init1.Pull = GPIO_PULLUP;
        GPIO_Init1.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(DEBUG_USART_TX_GPIO_PORT, &GPIO_Init1);

        //配置Rx
        GPIO_Init1.Pin = DEBUG_USART_RX_PIN;
        GPIO_Init1.Mode = GPIO_MODE_AF_INPUT;//复用输入模式
        HAL_GPIO_Init(DEBUG_USART_RX_GPIO_PORT, &GPIO_Init1);

        HAL_NVIC_SetPriority(DEBUG_USART_IRQ ,0,1);	//抢占优先级0，子优先级1
        HAL_NVIC_EnableIRQ(DEBUG_USART_IRQ );		    //使能USART1中断通道
    }

    if(huart->Instance == USART3)
    {
        //串口3的配置
        GPIO_InitTypeDef GPIO_Init3;

        WIFI_USART_CLK_ENABLE();
        WIFI_USART_RX_GPIO_CLK_ENABLE();
        WIFI_USART_TX_GPIO_CLK_ENABLE();
        //配置Tx引脚复用功能
        GPIO_Init3.Pin = WIFI_USART_TX_GPIO_PIN;
        GPIO_Init3.Mode = GPIO_MODE_AF_PP;
        GPIO_Init3.Pull = GPIO_PULLUP;
        GPIO_Init3.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(WIFI_USART_TX_GPIO_PORT, &GPIO_Init3);

        //配置Rx
        GPIO_Init3.Pin = WIFI_USART_RX_GPIO_PIN;
        GPIO_Init3.Mode = GPIO_MODE_AF_INPUT;
        HAL_GPIO_Init(WIFI_USART_RX_GPIO_PORT, &GPIO_Init3);

        HAL_NVIC_SetPriority(WIFI_UASRT_IRQ ,0,1);	//抢占优先级0，子优先级1
        HAL_NVIC_EnableIRQ(WIFI_UASRT_IRQ );		    //使能USART3中断通道
    }
}*/





/*****************  发送字符串 **********************/
/*  HAL_UART_Transmit 函数（这是一个阻塞的发送函数，无需重复判断串口是否发送完成）
    发送每个字符，直到遇到
    空字符才停止发送。*/
void Usart_SendString(UART_HandleTypeDef *usarthandle,uint8_t *str)
{
	unsigned int k=0;
  do
  {
      HAL_UART_Transmit(usarthandle,(uint8_t *)(str + k) ,1,1000);
      k++;
  } while(*(str + k)!='\0');

}
//重定向c库函数printf到串口DEBUG_USART，重定向后可使用printf函数
int fputc(int ch, FILE *f)
{
	/* 发送一个字节数据到串口DEBUG_USART */
	HAL_UART_Transmit(&DebugUartHandle, (uint8_t *)&ch, 1, 1000);

	return (ch);
}

//重定向c库函数scanf到串口DEBUG_USART，重写向后可使用scanf、getchar等函数
int fgetc(FILE *f)
{
	int ch;
	HAL_UART_Receive(&DebugUartHandle, (uint8_t *)&ch, 1, 1000);
	return (ch);
}

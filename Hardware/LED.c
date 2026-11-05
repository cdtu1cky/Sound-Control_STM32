#include "stm32f10x.h"

// LED初始化（PA0，推挽输出，高电平点亮）
void LED_Init(void)
{
    // 使能GPIOA时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP; // 推挽输出
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    // 初始设为低电平 → LED熄灭
    GPIO_ResetBits(GPIOA, GPIO_Pin_0);
}

// LED点亮：PA0输出高电平
void LED_ON(void)
{
    GPIO_SetBits(GPIOA, GPIO_Pin_0);
}

// LED熄灭：PA0输出低电平
void LED_OFF(void)
{
    GPIO_ResetBits(GPIOA, GPIO_Pin_0);
}

// LED翻转
void LED_Turn(void)
{
    // 读取当前PA0电平，反向输出
    if(GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_0) == 1)
    {
        GPIO_ResetBits(GPIOA, GPIO_Pin_0); // 高→低，灭
    }
    else
    {
        GPIO_SetBits(GPIOA, GPIO_Pin_0); // 低→高，亮
    }
}
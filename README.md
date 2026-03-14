# Sound-Control_STM32
基于STM32的声控LED交互系统项目
# 基于STM32的声控交互系统

## 项目简介
我独立完成了一个基于STM32单片机的声控系统。通过麦克风采集声音，经过算法处理后，可以可靠地控制LED灯。这个项目让我深入学习了嵌入式开发的全流程。

## 硬件清单
- 主控芯片：STM32F103C8T6最小系统板
- 声音传感器：模拟输出驻极体麦克风模块
- 输出设备：LED灯、电阻
- 辅助工具：USB转TTL串口模块（用于调试）、杜邦线、面包板

## 功能展示
1.  **基础功能**：声音触发LED开关。
2.  **算法升级**：采用软件滤波，提高了在嘈杂环境下的识别准确性。
3.  **效果演示**：[在这里粘贴你后期拍摄的视频链接]

## 核心代码与算法
这里我重点解决了**环境噪声干扰**的问题。
### 移动平均滤波算法
我编写了一个滤波函数来让信号更稳定。
```c
// 示例：移动平均滤波函数核心部分
#define SAMPLE_SIZE 10
int filter(int newValue) {
    static int history[SAMPLE_SIZE] = {0};
    static int index = 0;
    int sum = 0;
    
    history[index] = newValue;
    index = (index + 1) % SAMPLE_SIZE;
    
    for(int i=0; i<SAMPLE_SIZE; i++) {
        sum += history[i];
    }
    return sum / SAMPLE_SIZE;
}
```
开发日志（学习记录）
这是我的项目成长日记，记录了所有关键问题和解决方法。

2026年3月8日：项目启动
目标：搭建开发环境，点亮第一颗LED。
问题：程序下载后LED没反应。
解决：检查发现GPIO口初始化函数中，时钟使能语句写错了位置。纠正后成功点亮。
重温了以前关于学习使用stm32库函数的知识，再次学习了怎么调动库函数
这是我总结了在点亮led灯过程中以及后续会常用的关于gpio的库函数：
void RCC_AHBPeriphClockCmd(uint32_t RCC_AHBPeriph, FunctionalState NewState);
void RCC_APB2PeriphClockCmd(uint32_t RCC_APB2Periph, FunctionalState NewState);
void RCC_APB1PeriphClockCmd(uint32_t RCC_APB1Periph, FunctionalState NewState);

void GPIO_DeInit(GPIO_TypeDef* GPIOx);   复位
void GPIO_AFIODeInit(void);    复位
void GPIO_Init(GPIO_TypeDef* GPIOx, GPIO_InitTypeDef* GPIO_InitStruct);
用结构体的参数来初始化gpio口
void GPIO_StructInit(GPIO_InitTypeDef* GPIO_InitStruct);
把结构体变量赋予一个默认值
uint8_t GPIO_ReadInputDataBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
 读取输入数据寄存器某一个端口的输入值
uint16_t GPIO_ReadInputData(GPIO_TypeDef* GPIOx);
读取整个输入数据寄存器
uint8_t GPIO_ReadOutputDataBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
读取输出数据寄存器的 某一个位
uint16_t GPIO_ReadOutputData(GPIO_TypeDef* GPIOx);
读取整个输出数据寄存器
（4个读取函数）
void GPIO_SetBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);把指定端口 设置为高电平
void GPIO_ResetBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);把指定端口 设置为低电平
void GPIO_WriteBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, BitAction BitVal);
void GPIO_Write(GPIO_TypeDef* GPIOx, uint16_t PortVal);
（4个写入函数）

关于GPIO_Mode有：
{ GPIO_Mode_AIN = 0x0,  模拟输入 
  GPIO_Mode_IN_FLOATING = 0x04,  浮空输入
  GPIO_Mode_IPD = 0x28,  下拉
  GPIO_Mode_IPU = 0x48,  上拉
  GPIO_Mode_Out_OD = 0x14, 开漏输出    低电平具有驱动能力
  GPIO_Mode_Out_PP = 0x10,推挽输出      点灯使用    高低电平均有驱动能力 
  GPIO_Mode_AF_OD = 0x1C,复用开漏
  GPIO_Mode_AF_PP = 0x18，复用推挽
}GPIOMode_TypeDef;

常用led初始化 代码：
void LED_Init(void)
{
RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
GPIO_InitTypeDef GPIO_InitStructure;
GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
GPIO_InitStructure.GPIO_Pin=GPIO_Pin_1 | GPIO_Pin_2;
GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
 GPIO_Init(GPIOA,&GPIO_InitStructure);
GPIO_SetBits(GPIOA, GPIO_Pin_1 | GPIO_Pin_2);//设置PA1和PA2引脚为高电平
}

2026年3月9日：学习按键控制的知识
目标：搭建开发环境，通过使用按键控制灯的亮灭，学习如何封装函数
困难：运行代码时总是报错，但是在仔细检查我写的每一个代码都无误
解决：新手小白犯的这种低级错误，就是每在一个xxx.c文件写一个函数就忘记去xxx.h文件里声明可以使用
同样，我总结出了常会用到的库函数：

按键初始化
void Key_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);		//开启GPIOB的时钟
	/GPIO初始化/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);		//将PB1和PB11引脚初始化为上拉输入
}
uint8_t Key_GetNum(void)        //用于返回按键键码
{
	uint8_t KeyNum = 0;		//定义变量，默认键码值为0
	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 0)  
        //读PB1输入寄存器的状态，如果为0，则代表按键1按下
	{
		Delay_ms(20);                       //延时消抖
		while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 0);	
               //等待按键松手
		Delay_ms(20);			//延时消抖
		KeyNum = 1;				//置键码为1
	}
                return KeyNum;			
               //返回键码值，如果没有按键按下，所有if都不成立，则键码为默认值0
}
	
led.c设置：
void LED1_ON(void)
{
	GPIO_ResetBits(GPIOA, GPIO_Pin_1);
}
void LED1_OFF(void)
{
	GPIO_SetBits(GPIOA, GPIO_Pin_1);
}
void LED1_Turn(void)
{
if(GPIO_ReadOutputDataBit(GPIOA,GPIO_Pin_1)==0)
     {
	GPIO_SetBits(GPIOA,GPIO_Pin_1);
      }
else
     {
	GPIO_ResetBits(GPIOA,GPIO_Pin_1);
      }
}
main.c 运用
int main(void)
{
	uint8_t KeyNum;
	LED_Init();//直接调用完成LED初始化
	Key_Init();
while (1)
 {
  KeyNum= Key_GetNum();
		if(KeyNum==1)
		{
			LED1_Turn();
	}
		if(KeyNum==2)
		{
			LED1_Turn();
	}
   }
}

后续计划
加入FreeRTOS，让系统能够同时处理更多任务。

尝试用Wi-Fi模块将设备连接到网络。

如何运行我的项目
硬件连接：按照原理图连接STM32、麦克风模块和LED。

软件准备：使用Keil MDK打开工程文件。

编译下载：连接USB线，编译代码并下载到单片机。

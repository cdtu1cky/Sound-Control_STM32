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
重温了以前关于学习使用stm32库函数的知识，再次学习了怎么调动库函数。
硬件连接：![这里是图片描述](点灯.jpg)
这是我总结了在点亮led灯过程中以及后续会常用的关于gpio的库函数：
```c
void RCC_AHBPeriphClockCmd(uint32_t RCC_AHBPeriph, FunctionalState NewState);
void RCC_APB2PeriphClockCmd(uint32_t RCC_APB2Periph, FunctionalState NewState);
void RCC_APB1PeriphClockCmd(uint32_t RCC_APB1Periph, FunctionalState NewState);

void GPIO_DeInit(GPIO_TypeDef* GPIOx);  // 复位
void GPIO_AFIODeInit(void);  //  复位
void GPIO_Init(GPIO_TypeDef* GPIOx, GPIO_InitTypeDef* GPIO_InitStruct);
//用结构体的参数来初始化gpio口
void GPIO_StructInit(GPIO_InitTypeDef* GPIO_InitStruct);
//把结构体变量赋予一个默认值
uint8_t GPIO_ReadInputDataBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
 //读取输入数据寄存器某一个端口的输入值
uint16_t GPIO_ReadInputData(GPIO_TypeDef* GPIOx);
//读取整个输入数据寄存器
uint8_t GPIO_ReadOutputDataBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
//读取输出数据寄存器的 某一个位
uint16_t GPIO_ReadOutputData(GPIO_TypeDef* GPIOx);
//读取整个输出数据寄存器
//（4个读取函数）
void GPIO_SetBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);//把指定端口 设置为高电平
void GPIO_ResetBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);//把指定端口 设置为低电平
void GPIO_WriteBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, BitAction BitVal);
void GPIO_Write(GPIO_TypeDef* GPIOx, uint16_t PortVal);
//（4个写入函数）

//关于GPIO_Mode有：
{ GPIO_Mode_AIN = 0x0,  //模拟输入 
  GPIO_Mode_IN_FLOATING = 0x04,  //浮空输入
  GPIO_Mode_IPD = 0x28, // 下拉
  GPIO_Mode_IPU = 0x48,  //上拉
  GPIO_Mode_Out_OD = 0x14, //开漏输出    低电平具有驱动能力
  GPIO_Mode_Out_PP = 0x10,//推挽输出      点灯使用    高低电平均有驱动能力 
  GPIO_Mode_AF_OD = 0x1C,//复用开漏
  GPIO_Mode_AF_PP = 0x18，//复用推挽
}GPIOMode_TypeDef;

//常用led初始化 代码：
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
```
2026年3月9日：重温学习按键控制的知识
目标：搭建开发环境，通过使用按键控制灯的亮灭，学习如何封装函数
困难：运行代码时总是报错，但是在仔细检查我写的每一个代码都无误
解决：新手小白犯的这种低级错误，就是每在一个xxx.c文件写一个函数就忘记去xxx.h文件里声明可以使用
硬件连接：![这里是图片描述](按键控制灯.jpg)
同样，我总结出了常会用到的库函数：
```c
//按键初始化
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
```
2026年3月10日至3月14日：重温adc的知识
在课余时间，通过江协科技 STM32 教程，大致了解了adc原理与配置，并重新学习了ADC 单次转换模式，理解了采样时间、数据对齐、软件触发等关键配置。
以下是我根据视频总结出adc.c会常用的库函数
```c
//adc初始化函数：
rcc库函数 void RCC_ADCCLKConfig(uint32_t RCC_PCLK2); //用于配置adcclk分频器

void ADC_DeInit(ADC_TypeDef* ADCx);    //恢复缺省配置
void ADC_Init(ADC_TypeDef* ADCx, ADC_InitTypeDef* ADC_InitStruct);//init初始化
void ADC_StructInit(ADC_InitTypeDef* ADC_InitStruct);//结构体初始化

对adc的一些基本功能和规则组的配置
{
void ADC_Cmd(ADC_TypeDef* ADCx, FunctionalState NewState);//给adc上电
void ADC_DMACmd(ADC_TypeDef* ADCx, FunctionalState NewState);
//开启dma输出信号
void ADC_ITConfig(ADC_TypeDef* ADCx, uint16_t ADC_IT, FunctionalState NewState);//中断输出控制
void ADC_ResetCalibration(ADC_TypeDef* ADCx);//复位校准
FlagStatus ADC_GetResetCalibrationStatus(ADC_TypeDef* ADCx);//获取复位校准 状态
void ADC_StartCalibration(ADC_TypeDef* ADCx);//开始校准
FlagStatus ADC_GetCalibrationStatus(ADC_TypeDef* ADCx);//获取开始校准状态
void ADC_SoftwareStartConvCmd(ADC_TypeDef* ADCx, FunctionalState NewState);//软件触发的函数
FlagStatus ADC_GetSoftwareStartConvStatus(ADC_TypeDef* ADCx);//获取软件开始转换状态

  //配置间断模式的两个函数
  {
void ADC_DiscModeChannelCountConfig(ADC_TypeDef* ADCx, uint8_t Number);//每隔几个通道间断一次
void ADC_DiscModeCmd(ADC_TypeDef* ADCx, FunctionalState NewState);
//是不是启用间断模式
   }
void ADC_RegularChannelConfig(ADC_TypeDef* ADCx, uint8_t ADC_Channel, uint8_t Rank, uint8_t ADC_SampleTime);//adc——规则组通道配置    （重要）
第一个参数 ADCx，第二个ADC_Channel指定通道，第三个Rank序列几的位置，第四个SampleTIime指定通道的采样时间
void ADC_ExternalTrigConvCmd(ADC_TypeDef* ADCx, FunctionalState NewState);//adc_外部触发转换控制（是否允许外部触发转换）
uint16_t ADC_GetConversionValue(ADC_TypeDef* ADCx);//adc_获取转换值
（重要）获取ad转换的数据寄存器，读取转换结果 需要用到这个函数
uint32_t ADC_GetDualModeConversionValue(void);//adc_获取双模式转换值（双adc模式读取转换结果的函数）


}
//对adc注入组进行配置

{
void ADC_AutoInjectedConvCmd(ADC_TypeDef* ADCx, FunctionalState NewState);
void ADC_InjectedDiscModeCmd(ADC_TypeDef* ADCx, FunctionalState NewState);
void ADC_ExternalTrigInjectedConvConfig(ADC_TypeDef* ADCx, uint32_t ADC_ExternalTrigInjecConv);
void ADC_ExternalTrigInjectedConvCmd(ADC_TypeDef* ADCx, FunctionalState NewState);
void ADC_SoftwareStartInjectedConvCmd(ADC_TypeDef* ADCx, FunctionalState NewState);
FlagStatus ADC_GetSoftwareStartInjectedConvCmdStatus(ADC_TypeDef* ADCx);
void ADC_InjectedChannelConfig(ADC_TypeDef* ADCx, uint8_t ADC_Channel, uint8_t Rank, uint8_t ADC_SampleTime);
void ADC_InjectedSequencerLengthConfig(ADC_TypeDef* ADCx, uint8_t Length);
void ADC_SetInjectedOffset(ADC_TypeDef* ADCx, uint8_t ADC_InjectedChannel, uint16_t Offset);
uint16_t ADC_GetInjectedConversionValue(ADC_TypeDef* ADCx, uint8_t ADC_InjectedChannel);


}

//对模拟看门狗进行配置

{
void ADC_AnalogWatchdogCmd(ADC_TypeDef* ADCx, uint32_t ADC_AnalogWatchdog);//是否启动模拟看门狗
void ADC_AnalogWatchdogThresholdsConfig(ADC_TypeDef* ADCx, uint16_t HighThreshold, uint16_t LowThreshold);//配置高低阈值
void ADC_AnalogWatchdogSingleChannelConfig(ADC_TypeDef* ADCx, uint8_t ADC_Channel);//配置看门的通道

}
void ADC_TempSensorVrefintCmd(FunctionalState NewState);//adc_温度传感器,内部参考电压控制
FlagStatus ADC_GetFlagStatus(ADC_TypeDef* ADCx, uint8_t ADC_FLAG);//获取标志位状态
void ADC_ClearFlag(ADC_TypeDef* ADCx, uint8_t ADC_FLAG);//清除标志位
ITStatus ADC_GetITStatus(ADC_TypeDef* ADCx, uint16_t ADC_IT);//获取中断状态
void ADC_ClearITPendingBit(ADC_TypeDef* ADCx, uint16_t ADC_IT);//清除中断挂起位


```
2026年3月15日至3月18日:声控基础实现
一.在 STM32F103 上配置 ADC1 通道 1（PA1），成功读取麦克风模块的模拟输出电压,通过调试器观察adc_value的变化，掌握了用硬件调试定位问题的方法  
二.将 ADC 值与设定的阈值比较，实现了基础声控逻辑：  
1.声音超过阈值时 LED 点亮  
2.安静时 LED 熄灭

硬件连接：
![这里是图片描述](声控灯.jpg)

说话时亮灯：
![这里是图片描述](说话时点亮.jpg)
记录了不同环境下的数值范围，为后续算法优化提供依据  
三.遇到的主要问题:  
1.刚开始我使用的低电平点亮led灯，结果出现初始时常亮，对着麦克风说话是闪灭闪灭的状态，即使我反复确认了我的led.c的代码初始时设置的状态确实是使PA0口设置为高电平，在询问ai后才知道这是我的硬件接线逻辑认知偏差，主观认为 LED 低电平点亮，但实际接线为STM32 PA0 → LED长脚（正极） → LED短脚（负极） → GND，该接线天然是高电平点亮（PA0 输出 3.3V 时 LED 形成电压差点亮，0V 时熄灭），与代码中 “低电平点亮” 的逻辑完全反向，直接导致 LED 初始常亮。
2.刚开始 ADC 读数一直为 0 ,检查后发现是 GPIO 模式未配置为模拟输入  
3.环境噪音导致误触发,决定下一步引入软件滤波  
四.当前进度：  
声控基础功能已跑通  
下一步：学习移动平均滤波，提升系统稳定性






后续计划
加入FreeRTOS，让系统能够同时处理更多任务。

尝试用Wi-Fi模块将设备连接到网络。

如何运行我的项目
硬件连接：按照原理图连接STM32、麦克风模块和LED。

软件准备：使用Keil MDK打开工程文件。

编译下载：连接USB线，编译代码并下载到单片机。

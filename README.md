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

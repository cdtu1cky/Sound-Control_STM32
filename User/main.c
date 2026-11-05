#include "stm32f10x.h"
#include "Delay.h"
#include "AD.h"
#include "LED.h"
#include "Serial.h"

#define DEBUG_ENABLE  0   // 调试时改为 1

int main(void)
{
    LED_Init();
    AD_Init();

#if DEBUG_ENABLE
    Serial_Init();
    Serial_SendString("Start\r\n");
#endif

    uint16_t raw, filt;
    uint16_t threshold_on = 200;   // 亮灯阈值（原始值）
    uint16_t threshold_off = 100;  // 灭灯阈值（滤波值）
    uint8_t led_state = 0;
    uint8_t stable_cnt = 0;        // 计数防抖

    // 自适应阈值初始化（测环境底噪）
    uint16_t base_noise = 0;
    for (int i = 0; i < 20; i++) {
        base_noise += AD_GetFiltered();
        Delay_ms(10);
    }
    base_noise /= 20;
    threshold_on = base_noise * 2 + 50;
    threshold_off = base_noise + 20;

    while(1)
    {
        raw = AD_GetValue();
        filt = AD_GetFiltered();

        //计数防抖逻辑
        if (raw > threshold_on) {
            stable_cnt++;
            if (stable_cnt >= 3) {
                led_state = 1;
                stable_cnt = 3;
            }
        }
        else if (filt < threshold_off) {
            if (stable_cnt > 0) stable_cnt--;
            if (stable_cnt == 0) {
                led_state = 0;
            }
        }

        // 更新 LED
        if (led_state) {
            LED_ON();
        } else {
            LED_OFF();
        }

        // 每隔 30 秒重新校准环境底噪
        static uint32_t adapt_timer = 0;
        if (++adapt_timer >= 600) {
            base_noise = 0;
            for (int i = 0; i < 20; i++) {
                base_noise += AD_GetFiltered();
                Delay_ms(10);
            }
            base_noise /= 20;
            threshold_on = base_noise * 2 + 50;
            threshold_off = base_noise + 20;
            adapt_timer = 0;
        }

#if DEBUG_ENABLE
        static uint32_t cnt = 0;
        if (++cnt >= 40) {
            char buf[60];
            sprintf(buf, "Raw:%4d Filt:%4d Cnt:%d State:%d\r\n",
                    raw, filt, stable_cnt, led_state);
            Serial_SendString(buf);
            cnt = 0;
        }
#endif

        Delay_ms(50);
    }
}
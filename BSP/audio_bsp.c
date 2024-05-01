/**
 * audio_bsp.c
 * @tingzhong666
 * 2024-05-01 13:53:07
 */
#include <audio_bsp.h>

typedef struct
{
    uint8_t* pAudio;
    uint16_t index;
    uint32_t length;
} audio;

audio audioCurrent;

void init_audio_bsp(uint16_t samplingRate)
{
    timer_parameter_struct timer_cfg;
    // DAC IO
    // -时钟
    rcu_periph_clock_enable(RCU_AUDIO_IO);
    // -模拟模式
    gpio_mode_set(IO_AUDIO, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, PIN_AUDIO);
    // DAC
    // -时钟
    rcu_periph_clock_enable(RCU_AUDIO_DAC);
    // -触发源
    dac_trigger_source_config(DAC_AUDIO, DAC_TRIGGER_SOFTWARE);
    // -wave关闭
    dac_wave_mode_config(DAC_AUDIO, DAC_WAVE_DISABLE);
    // -输出缓冲开启
    dac_output_buffer_enable(DAC_AUDIO);
    // -软件触发源开启
    dac_software_trigger_enable(DAC_AUDIO);
    // -开启
    dac_enable(DAC_AUDIO);
    // TIMER
    // -时钟
    rcu_periph_clock_enable(RCU_AUDIO_TIMER);
    // -倍频
    rcu_timer_clock_prescaler_config(RCU_TIMER_PSC_MUL2); // 120*2=240M
    // -结构体初始化
    timer_struct_para_init(&timer_cfg);
    // --分频
    // timer_cfg.clockdivision = TIMER_CKDIV_DIV1;
    // --预分频
    timer_cfg.prescaler = 4000 - 1; // 240M/4000=60000
    // --对齐方式
    // --计数方向
    // --周期
    timer_cfg.period = 60000 / samplingRate - 1;
    timer_init(TIMER_AUDIO, &timer_cfg);
    // --重复计数器
    // -开启
    // timer_enable(TIMER_AUDIO);
    // -中断
    // --优先级
    nvic_irq_enable(IRQn_TIMER_AUDIO, 2, 2);
    // --开启中断
    timer_interrupt_enable(TIMER_AUDIO, TIMER_INT_UP);
    // timer cb
    // -修改dac模拟值
    // -audio数组索引++
}
void TIMER0_UP_TIMER9_IRQHandler()
{
    if (timer_interrupt_flag_get(TIMER_AUDIO, TIMER_INT_FLAG_UP) == SET) {
        dac_data_set(DAC_AUDIO, DAC_ALIGN_8B_R, audioCurrent.pAudio[audioCurrent.index++]);
        // dac_software_trigger_enable(DAC_AUDIO); // 软件触发使能 输出数据
        if (audioCurrent.index >= audioCurrent.length) {
            audioCurrent.index = 0;
        }
        timer_interrupt_flag_clear(TIMER_AUDIO, TIMER_INT_FLAG_UP);
    }
}

void stop_audio()
{
    timer_disable(TIMER_AUDIO);
}

void start_audio(uint8_t* p, uint32_t length)
{
    audioCurrent.pAudio = p;
    audioCurrent.index = 0;
    audioCurrent.length = length;
    timer_enable(TIMER_AUDIO);
}

void continue_audio()
{
    timer_enable(TIMER_AUDIO);
}

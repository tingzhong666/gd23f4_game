/**
 * audio_bsp.h
 * @tingzhong666
 * 2024-05-01 13:53:22
 */

#ifndef audio_bsp_H
#define audio_bsp_H

#ifdef __cplusplus
extern "C" {
#endif

#include "gd32f4xx_libopt.h"

#define RCU_AUDIO_IO RCU_GPIOA
#define IO_AUDIO GPIOA
#define PIN_AUDIO GPIO_PIN_5
#define RCU_AUDIO_DAC RCU_DAC
#define DAC_AUDIO DAC1
#define RCU_AUDIO_TIMER RCU_TIMER9
#define TIMER_AUDIO TIMER9
#define IRQn_TIMER_AUDIO TIMER0_UP_TIMER9_IRQn
#define IRQHandler_TIMER_AUDIO TIMER0_UP_TIMER9_IRQHandler

void init_audio_bsp(uint16_t samplingRate);
void stop_audio(void);
void start_audio(uint8_t* p, uint32_t length);
void continue_audio(void);

#ifdef __cplusplus
}
#endif

#endif

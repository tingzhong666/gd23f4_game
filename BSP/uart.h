/**
 * uart.h
 * @tingzhong666
 * 2024-04-30 08:41:24
 */
#ifndef UART_H
#define UART_H

#ifdef __cplusplus
extern "C" {
#endif

#include "gd32f4xx_gpio.h"
#include "gd32f4xx_rcu.h"
#include "gd32f4xx_usart.h"
#include "gd32f4xx_dma.h"
#include "string.h"
#include <stdio.h>

void bsp_uart_init(void);

#ifdef __cplusplus
}
#endif

#endif

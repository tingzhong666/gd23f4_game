/**
 * bsp_lcd.h
 * @tingzhong666
 * 2024-04-30 09:01:28
 */

#ifndef BSP_LCD_H
#define BSP_LCD_H

#ifdef __cplusplus
extern "C" {
#endif

#include "gd32f4xx.h"
#include "systick.h"

#define BSP_LCD_SPI_RCU RCU_SPI4
#define BSP_LCD_SPI SPI4
#define BSP_LCD_SPI_CLOCK_POLARITY_PHASE SPI_CK_PL_HIGH_PH_2EDGE

#define BSP_LCD_SPI_CS_GPIO_RCU RCU_GPIOA
#define BSP_LCD_SPI_CS_GPIO GPIOA
#define BSP_LCD_SPI_CS_GPIO_PIN GPIO_PIN_4

#define BSP_LCD_SPI_CLK_GPIO_RCU RCU_GPIOF
#define BSP_LCD_SPI_CLK_GPIO GPIOF
#define BSP_LCD_SPI_CLK_GPIO_PIN GPIO_PIN_7

#define BSP_LCD_SPI_MOSI_GPIO_RCU RCU_GPIOF
#define BSP_LCD_SPI_MOSI_GPIO GPIOF
#define BSP_LCD_SPI_MOSI_GPIO_PIN GPIO_PIN_9

#define BSP_LCD_SPI_DC_GPIO_RCU RCU_GPIOA
#define BSP_LCD_SPI_DC_GPIO GPIOA
#define BSP_LCD_SPI_DC_GPIO_PIN GPIO_PIN_6

#define BSP_LCD_SPI_RESET_GPIO_RCU RCU_GPIOF
#define BSP_LCD_SPI_RESET_GPIO GPIOF
#define BSP_LCD_SPI_RESET_GPIO_PIN GPIO_PIN_10

#define BSP_LCD_SPI_BLK_GPIO_RCU RCU_GPIOA
#define BSP_LCD_SPI_BLK_GPIO GPIOA
#define BSP_LCD_SPI_BLK_GPIO_PIN GPIO_PIN_7

void Lcd_Init(void);
void LCD_Fill(uint16_t xsta, uint16_t ysta, uint16_t xend, uint16_t yend, uint16_t color);
void LCD_ShowPicture(uint16_t x,uint16_t y,uint16_t length,uint16_t width,const uint8_t pic[]);

#ifdef __cplusplus
}
#endif

#endif

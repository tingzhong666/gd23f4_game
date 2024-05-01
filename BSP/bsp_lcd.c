/**
 * bsp_spi.c
 * @tingzhong666
 * 2024-04-30 08:59:12
 */

#include <bsp_lcd.h>

void bsp_lcd_init()
{
    spi_parameter_struct spi_struct;
    // io复用
    rcu_periph_clock_enable(BSP_LCD_SPI_CS_GPIO_RCU);
    rcu_periph_clock_enable(BSP_LCD_SPI_CLK_GPIO_RCU);
    rcu_periph_clock_enable(BSP_LCD_SPI_MOSI_GPIO_RCU);
    rcu_periph_clock_enable(BSP_LCD_SPI_DC_GPIO_RCU);
    rcu_periph_clock_enable(BSP_LCD_SPI_RESET_GPIO_RCU);
    rcu_periph_clock_enable(BSP_LCD_SPI_BLK_GPIO_RCU);

    gpio_mode_set(BSP_LCD_SPI_CS_GPIO, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, BSP_LCD_SPI_CS_GPIO_PIN);
    gpio_output_options_set(BSP_LCD_SPI_CS_GPIO, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, BSP_LCD_SPI_CS_GPIO_PIN);
    // gpio_bit_set(BSP_LCD_SPI_CS_GPIO, BSP_LCD_SPI_CS_GPIO_PIN);

    gpio_mode_set(BSP_LCD_SPI_DC_GPIO, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, BSP_LCD_SPI_DC_GPIO_PIN);
    gpio_output_options_set(BSP_LCD_SPI_DC_GPIO, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, BSP_LCD_SPI_DC_GPIO_PIN);
    // gpio_bit_set(BSP_LCD_SPI_DC_GPIO, BSP_LCD_SPI_DC_GPIO_PIN);

    gpio_mode_set(BSP_LCD_SPI_RESET_GPIO, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, BSP_LCD_SPI_RESET_GPIO_PIN);
    gpio_output_options_set(BSP_LCD_SPI_RESET_GPIO, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, BSP_LCD_SPI_RESET_GPIO_PIN);
    // gpio_bit_set(BSP_LCD_SPI_RESET_GPIO, BSP_LCD_SPI_RESET_GPIO_PIN);

    gpio_mode_set(BSP_LCD_SPI_BLK_GPIO, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, BSP_LCD_SPI_BLK_GPIO_PIN);
    gpio_output_options_set(BSP_LCD_SPI_BLK_GPIO, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, BSP_LCD_SPI_BLK_GPIO_PIN);
    // gpio_bit_set(BSP_LCD_SPI_BLK_GPIO, BSP_LCD_SPI_BLK_GPIO_PIN);

    gpio_mode_set(BSP_LCD_SPI_CLK_GPIO, GPIO_MODE_AF, GPIO_PUPD_NONE, BSP_LCD_SPI_CLK_GPIO_PIN);
    gpio_output_options_set(BSP_LCD_SPI_CLK_GPIO, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, BSP_LCD_SPI_CLK_GPIO_PIN);
    gpio_af_set(BSP_LCD_SPI_CLK_GPIO, GPIO_AF_5, BSP_LCD_SPI_CLK_GPIO_PIN);
    gpio_mode_set(BSP_LCD_SPI_MOSI_GPIO, GPIO_MODE_AF, GPIO_PUPD_NONE, BSP_LCD_SPI_MOSI_GPIO_PIN);
    gpio_output_options_set(BSP_LCD_SPI_MOSI_GPIO, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, BSP_LCD_SPI_MOSI_GPIO_PIN);
    gpio_af_set(BSP_LCD_SPI_MOSI_GPIO, GPIO_AF_5, BSP_LCD_SPI_MOSI_GPIO_PIN);
    gpio_bit_set(GPIOF, GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9); //==
    // spi
    rcu_periph_clock_enable(BSP_LCD_SPI_RCU);

    spi_struct_para_init(&spi_struct);
    spi_struct.device_mode = SPI_MASTER; // 主机模式
    spi_struct.trans_mode = SPI_TRANSMODE_FULLDUPLEX; // 全双工
    spi_struct.frame_size = SPI_FRAMESIZE_8BIT; // 1帧数据大小 8位
    spi_struct.nss = SPI_NSS_SOFT; // 软件片选
    spi_struct.clock_polarity_phase = SPI_CK_PL_HIGH_PH_2EDGE; // SPI模式3 (1,1) 高极性 下降沿
    spi_struct.prescale = SPI_PSC_2; // 预分频
    spi_struct.endian = SPI_ENDIAN_MSB; // 高位在前 大端模式
    spi_init(BSP_LCD_SPI, &spi_struct);
    spi_enable(BSP_LCD_SPI);
}

// SPI发送数据
void BSP_LCD_spi_send_8bit(uint8_t data)
{
    spi_i2s_data_transmit(BSP_LCD_SPI, (uint16_t)data);
    while (spi_i2s_flag_get(BSP_LCD_SPI, SPI_FLAG_TBE) == RESET)
        ;
    while (spi_i2s_flag_get(BSP_LCD_SPI, SPI_FLAG_RBNE) == RESET)
        ;
    spi_i2s_data_receive(BSP_LCD_SPI);
}

// SPI接收数据
uint8_t bsp_lcd_spi_rec_8bit()
{
    spi_i2s_data_transmit(BSP_LCD_SPI, 0xff);
    while (spi_i2s_flag_get(BSP_LCD_SPI, SPI_FLAG_TBE) == RESET)
        ;
    while (spi_i2s_flag_get(BSP_LCD_SPI, SPI_FLAG_RBNE) == RESET)
        ;
    return (uint8_t)spi_i2s_data_receive(BSP_LCD_SPI);
}

// 开启lcd
void bsp_lcd_open()
{
    gpio_bit_reset(BSP_LCD_SPI_CS_GPIO, BSP_LCD_SPI_CS_GPIO_PIN);
}

// 关闭lcd
void bsp_lcd_close()
{
    gpio_bit_set(BSP_LCD_SPI_CS_GPIO, BSP_LCD_SPI_CS_GPIO_PIN);
}

// LCD通信
#define LCD_CS_L bsp_lcd_open()
#define LCD_CS_H bsp_lcd_close()
#define Lcd_SpiRead(dat) bsp_lcd_spi_rec_8bit(dat)
#define Lcd_SpiWrite(dat) BSP_LCD_spi_send_8bit(dat)
#define LCD_DC_L gpio_bit_reset(BSP_LCD_SPI_DC_GPIO, BSP_LCD_SPI_DC_GPIO_PIN)
#define LCD_DC_H gpio_bit_set(BSP_LCD_SPI_DC_GPIO, BSP_LCD_SPI_DC_GPIO_PIN)
void delay(int t)
{
	while(t--);
}
void LCD_Writ_Bus(uint8_t dat)
{
    LCD_CS_L;
    Lcd_SpiWrite(dat);
    // delay(4);
    LCD_CS_H;
}
void Lcd_WriteCom(uint8_t com)
{
    LCD_DC_L;
    LCD_Writ_Bus(com);
    LCD_DC_H;
}
void Lcd_WriteData(uint8_t data)
{
    LCD_Writ_Bus(data);
}
void Lcd_WriteData16(uint16_t data)
{
    LCD_Writ_Bus(data >> 8);
    LCD_Writ_Bus(data);
}

typedef enum {
    lcdOrientation_Portrait = 0x00, // 竖屏
    lcdOrientation_APortrait = 0xC0, // 竖屏，手机倒过来
    lcdOrientation_Left = 0x70, // 横屏，屏幕在home键左边（比较常用）
    lcdOrientation_Right = 0xA0 // 横屏，屏幕在home键右边
} LcdOrientation;
LcdOrientation lcd_Orientation = lcdOrientation_Portrait;
/**
 * @brief  Lcd_SetOrientation: 设置LCD屏幕的视角方向
 * @param [in] lcdOrientation: 视角方向
 * @return void
 */
void Lcd_SetOrientation(LcdOrientation lcdOrientation)
{
    lcd_Orientation = lcdOrientation;
    Lcd_WriteCom(0x36);
    Lcd_WriteData(lcd_Orientation);
}
// LCD初始化
#define LCD_RST_L gpio_bit_reset(BSP_LCD_SPI_RESET_GPIO, BSP_LCD_SPI_RESET_GPIO_PIN)
#define LCD_RST_H gpio_bit_set(BSP_LCD_SPI_RESET_GPIO, BSP_LCD_SPI_RESET_GPIO_PIN)
#define LCD_BLK_H gpio_bit_set(BSP_LCD_SPI_BLK_GPIO, BSP_LCD_SPI_BLK_GPIO_PIN)
void Lcd_Init()
{
    bsp_lcd_init();

    LCD_RST_L; // 复位
    delay_1ms(100);
    LCD_RST_H;
    delay_1ms(100);
    LCD_BLK_H; // 打开背光

    Lcd_WriteCom(0x11); // Sleep out
    delay_1ms(120); // Delay 120ms

    Lcd_SetOrientation(lcdOrientation_Portrait);

    Lcd_WriteCom(0x3A);
    Lcd_WriteData(0x05);

    Lcd_WriteCom(0xB2);
    Lcd_WriteData(0x0C);
    Lcd_WriteData(0x0C);
    Lcd_WriteData(0x00);
    Lcd_WriteData(0x33);
    Lcd_WriteData(0x33);

    Lcd_WriteCom(0xB7);
    Lcd_WriteData(0x35);

    Lcd_WriteCom(0xBB);
    Lcd_WriteData(0x32); // Vcom=1.35V

    Lcd_WriteCom(0xC2);
    Lcd_WriteData(0x01);

    Lcd_WriteCom(0xC3);
    Lcd_WriteData(0x15); // GVDD=4.8V  颜色深度

    Lcd_WriteCom(0xC4);
    Lcd_WriteData(0x20); // VDV, 0x20:0v

    Lcd_WriteCom(0xC6);
    Lcd_WriteData(0x0F); // 0x0F:60Hz

    Lcd_WriteCom(0xD0);
    Lcd_WriteData(0xA4);
    Lcd_WriteData(0xA1);

    Lcd_WriteCom(0xE0);
    Lcd_WriteData(0xD0);
    Lcd_WriteData(0x08);
    Lcd_WriteData(0x0E);
    Lcd_WriteData(0x09);
    Lcd_WriteData(0x09);
    Lcd_WriteData(0x05);
    Lcd_WriteData(0x31);
    Lcd_WriteData(0x33);
    Lcd_WriteData(0x48);
    Lcd_WriteData(0x17);
    Lcd_WriteData(0x14);
    Lcd_WriteData(0x15);
    Lcd_WriteData(0x31);
    Lcd_WriteData(0x34);

    Lcd_WriteCom(0xE1);
    Lcd_WriteData(0xD0);
    Lcd_WriteData(0x08);
    Lcd_WriteData(0x0E);
    Lcd_WriteData(0x09);
    Lcd_WriteData(0x09);
    Lcd_WriteData(0x15);
    Lcd_WriteData(0x31);
    Lcd_WriteData(0x33);
    Lcd_WriteData(0x48);
    Lcd_WriteData(0x17);
    Lcd_WriteData(0x14);
    Lcd_WriteData(0x15);
    Lcd_WriteData(0x31);
    Lcd_WriteData(0x34);
    Lcd_WriteCom(0x21);

    Lcd_WriteCom(0x29);

    delay_1ms(120); // Delay 120ms
}
/**
 * @brief Lcd_PushStart: 开始写入屏幕数据
 * @param        [in] x: 写入屏幕数据的起始x坐标
 * @param        [in] y: 起始y坐标
 * @param        [in] w: 宽度
 * @param        [in] h: 高度
 * @return void
 */
void Lcd_PushStart(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    if (lcd_Orientation == lcdOrientation_Portrait || lcd_Orientation == lcdOrientation_APortrait) {
        y1 += 20;
        y2 += 20;
    } else if (lcd_Orientation == lcdOrientation_Left || lcd_Orientation == lcdOrientation_Right) {
        x1 += 20;
        x2 += 20;
    }

    Lcd_WriteCom(0x2a); // 列地址设置
    Lcd_WriteData16(x1);
    Lcd_WriteData16(x2);
    Lcd_WriteCom(0x2b); // 行地址设置
    Lcd_WriteData16(y1);
    Lcd_WriteData16(y2);
    Lcd_WriteCom(0x2c); // 储存器写
}

/******************************************************************************
      函数说明：在指定区域填充颜色
      入口数据：xsta,ysta   起始坐标
                xend,yend   终止坐标
                color       要填充的颜色
      返回值：  无
******************************************************************************/
void LCD_Fill(uint16_t xsta, uint16_t ysta, uint16_t xend, uint16_t yend, uint16_t color)
{
    uint16_t i, j;
    Lcd_PushStart(xsta, ysta, xend - 1, yend - 1); // 设置显示范围
    for (i = ysta; i < yend; i++) {
        for (j = xsta; j < xend; j++) {
            Lcd_WriteData16(color);
        }
    }
}

/******************************************************************************
      函数说明：显示图片
      入口数据：x,y起点坐标
                length 图片长度
                width  图片宽度
                pic[]  图片数组
      返回值：  无
******************************************************************************/
void LCD_ShowPicture(uint16_t x, uint16_t y, uint16_t length, uint16_t width, const uint8_t pic[])
{
    uint16_t i, j;
    uint32_t k = 0;
    Lcd_PushStart(x, y, x + length - 1, y + width - 1);
    for (i = 0; i < length; i++) {
        for (j = 0; j < width; j++) {
            Lcd_WriteData(pic[k * 2]);
            Lcd_WriteData(pic[k * 2 + 1]);
            k++;
        }
    }
}

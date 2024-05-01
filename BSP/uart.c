/**
 * uart.c
 * @tingzhong666
 * 2024-04-30 08:41:15
 */

#include "uart.h"

#define BSP_UART_BUFFER_SIZE 128
#define BSP_UsART0_RXBUFFER_SIZE BSP_UART_BUFFER_SIZE
char bsp_usart0_RXBuffer[BSP_UsART0_RXBUFFER_SIZE] = { 0 };

typedef struct
{
    // io
    uint32_t alt_func_num;
    rcu_periph_enum rx_gpio_rcu;
    uint32_t rx_gpio;
    uint32_t rx_pin;
    rcu_periph_enum tx_gpio_rcu;
    uint32_t tx_gpio;
    uint32_t tx_pin;

    // uart
    rcu_periph_enum uart_rcu;
    uint32_t usart_periph;
    uint32_t baudval;
    uint8_t nvic_irq;

    // dma
    rcu_periph_enum dma_rcu;
    uint32_t memory0_addr;
    uint32_t number;
    uint32_t dma_periph;
    dma_channel_enum channelx;
    dma_subperipheral_enum sub_periph;
} Bsp_uart_init;

// 初始化
void bsp_uart_init_(Bsp_uart_init* cfg)
{
    dma_single_data_parameter_struct dma_single_data_parameter;

    // IO
    rcu_periph_clock_enable(cfg->rx_gpio_rcu);
    rcu_periph_clock_enable(cfg->tx_gpio_rcu);

    gpio_mode_set(cfg->rx_gpio, GPIO_MODE_AF, GPIO_PUPD_NONE, cfg->rx_pin);
    gpio_output_options_set(cfg->rx_gpio, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, cfg->rx_pin);
    gpio_af_set(cfg->rx_gpio, cfg->alt_func_num, cfg->rx_pin);
    gpio_mode_set(cfg->tx_gpio, GPIO_MODE_AF, GPIO_PUPD_NONE, cfg->tx_pin);
    gpio_output_options_set(cfg->tx_gpio, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, cfg->tx_pin);
    gpio_af_set(cfg->tx_gpio, cfg->alt_func_num, cfg->tx_pin);

    // 串口
    rcu_periph_clock_enable(cfg->uart_rcu);

    usart_baudrate_set(cfg->usart_periph, 9600);
    usart_parity_config(cfg->usart_periph, USART_PM_NONE);
    usart_word_length_set(cfg->usart_periph, USART_WL_8BIT);
    usart_stop_bit_set(cfg->usart_periph, USART_STB_1BIT);
    usart_enable(cfg->usart_periph);
    usart_transmit_config(cfg->usart_periph, USART_TRANSMIT_ENABLE);
    usart_receive_config(cfg->usart_periph, USART_RECEIVE_ENABLE);

    usart_interrupt_enable(cfg->usart_periph, USART_INT_IDLE);
    nvic_irq_enable(cfg->nvic_irq, 2, 2);
    // DMA 外设到内存
    rcu_periph_clock_enable(cfg->dma_rcu);

    dma_single_data_para_struct_init(&dma_single_data_parameter);
    dma_single_data_parameter.periph_addr = (uint32_t)&USART_DATA(cfg->usart_periph);
    dma_single_data_parameter.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_single_data_parameter.memory0_addr = (uint32_t)cfg->memory0_addr;
    dma_single_data_parameter.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_single_data_parameter.periph_memory_width = DMA_PERIPH_WIDTH_8BIT;
    dma_single_data_parameter.circular_mode = DMA_CIRCULAR_MODE_DISABLE;
    dma_single_data_parameter.direction = DMA_PERIPH_TO_MEMORY;
    dma_single_data_parameter.number = cfg->number;
    dma_single_data_parameter.priority = DMA_PRIORITY_ULTRA_HIGH;
    dma_single_data_mode_init(cfg->dma_periph, cfg->channelx, &dma_single_data_parameter);
    dma_channel_subperipheral_select(cfg->dma_periph, cfg->channelx, cfg->sub_periph);
    dma_channel_enable(cfg->dma_periph, cfg->channelx);
    usart_dma_receive_config(cfg->usart_periph, USART_RECEIVE_DMA_ENABLE);
}

Bsp_uart_init bsp_uart_init_cfg;
void bsp_uart_init()
{
    // ===== USART0 上位机调试
    // io
    bsp_uart_init_cfg.alt_func_num = GPIO_AF_7;
    bsp_uart_init_cfg.rx_gpio_rcu = RCU_GPIOA;
    bsp_uart_init_cfg.rx_gpio = GPIOA;
    bsp_uart_init_cfg.rx_pin = GPIO_PIN_10;
    bsp_uart_init_cfg.tx_gpio_rcu = RCU_GPIOA;
    bsp_uart_init_cfg.tx_gpio = GPIOA;
    bsp_uart_init_cfg.tx_pin = GPIO_PIN_9;

    // uart
    bsp_uart_init_cfg.uart_rcu = RCU_USART0;
    bsp_uart_init_cfg.usart_periph = USART0;
    bsp_uart_init_cfg.baudval = 9600;
    bsp_uart_init_cfg.nvic_irq = USART0_IRQn;

    // dma
    bsp_uart_init_cfg.dma_rcu = RCU_DMA1;
    bsp_uart_init_cfg.memory0_addr = (uint32_t)bsp_usart0_RXBuffer;
    bsp_uart_init_cfg.number = BSP_UART_BUFFER_SIZE;
    bsp_uart_init_cfg.dma_periph = DMA1;
    bsp_uart_init_cfg.channelx = DMA_CH2;
    bsp_uart_init_cfg.sub_periph = DMA_SUBPERI4;
    bsp_uart_init_(&bsp_uart_init_cfg);
}

int fputc(int ch, FILE* fp)
{
    while (!usart_flag_get(bsp_uart_init_cfg.usart_periph, USART_FLAG_TBE))
        ;
    usart_data_transmit(bsp_uart_init_cfg.usart_periph, ch);
    usart_flag_clear(bsp_uart_init_cfg.usart_periph, USART_FLAG_TBE);
    return 1;
}

typedef struct
{
    uint32_t usart_periph;
    uint32_t memory0_addr;
    uint32_t number;
    uint32_t dma_periph;
    dma_channel_enum channelx;
    Bsp_uart_init* bsp_uart_init;

    char* name;
} Bsp_uart_dmaRX;
// 接收处理
void bsp_uart_dmaRX(Bsp_uart_dmaRX* cfg)
{

    if (usart_interrupt_flag_get(cfg->usart_periph, USART_INT_FLAG_IDLE) == SET) {
        // 串口0 上位机调试用
        usart_data_receive(cfg->usart_periph);

        printf("debug %s USART_INT_FLAG_IDLE\n", cfg->name);

        ((char*)cfg->memory0_addr)[cfg->number - 1] = 0;
        printf("debug %s RX %s \n", cfg->name, (char*)cfg->memory0_addr);
        memset((char*)cfg->memory0_addr, 0, sizeof((char*)cfg->memory0_addr));

        printf("debug %s RX dma_transfer_number_get = %d \n", cfg->name, dma_transfer_number_get(cfg->dma_periph, cfg->channelx));
        dma_channel_disable(cfg->dma_periph, cfg->channelx);
        dma_deinit(cfg->dma_periph, cfg->channelx);
        bsp_uart_init_(cfg->bsp_uart_init);
        dma_channel_enable(cfg->dma_periph, cfg->channelx);

        usart_interrupt_flag_clear(cfg->usart_periph, USART_INT_FLAG_IDLE);
    }
}

void USART0_IRQHandler()
{
    Bsp_uart_dmaRX cfg;
    // usart0 上位机调试
    cfg.usart_periph = USART0;
    cfg.memory0_addr = (uint32_t)bsp_usart0_RXBuffer;
    cfg.number = BSP_UsART0_RXBUFFER_SIZE;
    cfg.dma_periph = DMA1;
    cfg.channelx = DMA_CH2;
    cfg.bsp_uart_init = &bsp_uart_init_cfg;
    cfg.name = "USART0";
    bsp_uart_dmaRX(&cfg);
    // 蓝牙
}

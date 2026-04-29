#include "bsp_debug.h"
#include "hc32_ddl.h"

#define DEBUG_USART_CH                     (M4_USART3)
#define DEBUG_USART_PWC_FCG                (PWC_FCG3_USART3)
#define DEBUG_USART_BAUDRATE               (115200UL)
/* UART RX/TX Port/Pin definition */
#define DEBUG_USART_RX_PORT                   (GPIO_PORT_F) 
#define DEBUG_USART_RX_PIN                    (GPIO_PIN_12)
#define DEBUG_USART_RX_GPIO_FUNC              (GPIO_FUNC_37_USART3_RX)

#define DEBUG_USART_TX_PORT                   (GPIO_PORT_F)   
#define DEBUG_USART_TX_PIN                    (GPIO_PIN_11)
#define DEBUG_USART_TX_GPIO_FUNC              (GPIO_FUNC_36_USART3_TX)

static void rx_pin_init(void)
{
	stc_gpio_init_t stcGpioInit;
	(void)GPIO_StructInit(&stcGpioInit);
	stcGpioInit.u16PinDir = PIN_DIR_OUT;  //端口方向配置->输入
	stcGpioInit.u16PullUp = PIN_PU_ON; 
	
	(void)GPIO_Init(DEBUG_USART_RX_PORT, DEBUG_USART_RX_PIN, &stcGpioInit);
}

static void Usart_Port_Init(void)
{
    const stc_usart_uart_init_t stcUartInit = {
        .u32Baudrate = DEBUG_USART_BAUDRATE,
        .u32BitDirection = USART_LSB,
        .u32StopBit = USART_STOPBIT_1BIT,
        .u32Parity = USART_PARITY_NONE,
        .u32DataWidth = USART_DATA_LENGTH_8BIT,
        .u32ClkMode = USART_INTERNCLK_OUTPUT,
        .u32PclkDiv = USART_PCLK_DIV64,
        .u32OversamplingBits = USART_OVERSAMPLING_8BIT,
        .u32NoiseFilterState = USART_NOISE_FILTER_DISABLE,
        .u32SbDetectPolarity = USART_SB_DETECT_FALLING,
    };
	 
	GPIO_SetFunc(DEBUG_USART_RX_PORT, DEBUG_USART_RX_PIN, DEBUG_USART_RX_GPIO_FUNC, Disable);
    GPIO_SetFunc(DEBUG_USART_TX_PORT, DEBUG_USART_TX_PIN, DEBUG_USART_TX_GPIO_FUNC, Disable);
 
    /* Initialize UART */
    USART_UartInit(DEBUG_USART_CH, &stcUartInit);
}

void Debug_Usart_Init(void)
{
	DDL_PrintfInit(DEBUG_USART_CH, DEBUG_USART_BAUDRATE, Usart_Port_Init);
	rx_pin_init();
}

void text_pin_ctr(uint8_t state)
{
	 if(state)
		 GPIO_SetPins(DEBUG_USART_RX_PORT,DEBUG_USART_RX_PIN);
	 else
		 GPIO_ResetPins(DEBUG_USART_RX_PORT,DEBUG_USART_RX_PIN);
}



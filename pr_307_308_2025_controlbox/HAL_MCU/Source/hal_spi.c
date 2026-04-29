#include "hal_spi.h"
#include "hc32_ddl.h"

enum hal_spi_irq
{
	HAL_SPI_IRQ_TX,
	HAL_SPI_IRQ_RX,

};

struct hc4a0_spi_cfg
{
    M4_SPI_TypeDef* spix;
    uint32_t  fcg1_periph;
    struct hc4a0_gpio_cfg
    {
        uint8_t  port;
        uint16_t pin;
        uint16_t func;
    } pin_cs, pin_sck, pin_mosi, pin_miso;
    struct hc4a0_irq_cfg
    {
        stc_irq_signin_config_t stc_irq_signin_cfg;
        uint32_t priority;
          
    }rx_irq,tx_irq;

};

struct spi_recv
{
    uint8_t rec_flg;
    uint8_t rec_data;
};

static const struct hc4a0_spi_cfg spi_cfg[HAL_SPI_NUM] = 
{
    [HAL_SPI_CRMX] = 
    {
        .spix = M4_SPI1,
        .fcg1_periph = PWC_FCG1_SPI1,
        .pin_cs =   {   GPIO_PORT_A, GPIO_PIN_15, GPIO_FUNC_47_SPI1_NSS0 },
        .pin_sck =  {   GPIO_PORT_C, GPIO_PIN_10, GPIO_FUNC_40_SPI1_SCK },
        .pin_mosi = {   GPIO_PORT_C, GPIO_PIN_12, GPIO_FUNC_41_SPI1_MOSI },
        .pin_miso = {   GPIO_PORT_C, GPIO_PIN_11, GPIO_FUNC_42_SPI1_MISO },
        //.rx_irq = {.stc_irq_signin_cfg = { INT_SPI1_SPRI, Int002_IRQn, crmx_spi_rx_irq_cb},DDL_IRQ_PRIORITY_05},
    }, 
};

void hal_spi_pin_reset(enum hal_spi_type unit)
{
	stc_gpio_init_t stcGpioCfg;
	(void)GPIO_StructInit(&stcGpioCfg);
	
    stcGpioCfg.u16PinDir = PIN_DIR_OUT;
    stcGpioCfg.u16PinDrv = PIN_DRV_HIGH;
	(void)GPIO_Init(spi_cfg[unit].pin_sck.port,  spi_cfg[unit].pin_sck.pin, &stcGpioCfg);
    (void)GPIO_Init(spi_cfg[unit].pin_mosi.port, spi_cfg[unit].pin_mosi.pin, &stcGpioCfg);
	(void)GPIO_Init(spi_cfg[unit].pin_cs.port, spi_cfg[unit].pin_cs.pin, &stcGpioCfg);
	(void)GPIO_Init(spi_cfg[unit].pin_miso.port, spi_cfg[unit].pin_miso.pin, &stcGpioCfg);    
	
	GPIO_ResetPins(spi_cfg[unit].pin_cs.port, spi_cfg[unit].pin_cs.pin);
	GPIO_ResetPins(spi_cfg[unit].pin_mosi.port, spi_cfg[unit].pin_mosi.pin);
	GPIO_ResetPins(spi_cfg[unit].pin_sck.port, spi_cfg[unit].pin_sck.pin);
	GPIO_ResetPins(spi_cfg[unit].pin_miso.port, spi_cfg[unit].pin_miso.pin);
	
	GPIO_SetFunc(spi_cfg[unit].pin_sck.port,  spi_cfg[unit].pin_sck.pin,  0, PIN_SUBFUNC_DISABLE);
    GPIO_SetFunc(spi_cfg[unit].pin_mosi.port, spi_cfg[unit].pin_mosi.pin, 0, PIN_SUBFUNC_DISABLE);
    GPIO_SetFunc(spi_cfg[unit].pin_miso.port, spi_cfg[unit].pin_miso.pin, 0, PIN_SUBFUNC_DISABLE);
}

void hal_spi_pin_enable(enum hal_spi_type unit)
{
	stc_gpio_init_t stcGpioCfg;
	GPIO_SetDebugPort(GPIO_PIN_TDI, Disable);
    (void)GPIO_StructInit(&stcGpioCfg);
    stcGpioCfg.u16PinDir = PIN_DIR_OUT;
    stcGpioCfg.u16PinDrv = PIN_DRV_HIGH;
    (void)GPIO_Init(spi_cfg[unit].pin_cs.port, spi_cfg[unit].pin_cs.pin, &stcGpioCfg);
    GPIO_SetPins(spi_cfg[unit].pin_cs.port, spi_cfg[unit].pin_cs.pin);
    
    (void)GPIO_StructInit(&stcGpioCfg);
    stcGpioCfg.u16PinDrv = PIN_DRV_HIGH;
    //(void)GPIO_Init(spi_cfg[unit].pin_cs.port,  spi_cfg[unit].pin_cs.pin, &stcGpioCfg);
    (void)GPIO_Init(spi_cfg[unit].pin_sck.port,  spi_cfg[unit].pin_sck.pin, &stcGpioCfg);
    (void)GPIO_Init(spi_cfg[unit].pin_mosi.port, spi_cfg[unit].pin_mosi.pin, &stcGpioCfg);

    /* CMOS input for input pin */
    stcGpioCfg.u16PinIType = PIN_ITYPE_CMOS;
    (void)GPIO_Init(spi_cfg[unit].pin_miso.port, spi_cfg[unit].pin_miso.pin, &stcGpioCfg);    

    //GPIO_SetFunc(spi_cfg[unit].pin_cs.port,  spi_cfg[unit].pin_cs.pin,  spi_cfg[unit].pin_cs.func, PIN_SUBFUNC_DISABLE);
    GPIO_SetFunc(spi_cfg[unit].pin_sck.port,  spi_cfg[unit].pin_sck.pin,  spi_cfg[unit].pin_sck.func, PIN_SUBFUNC_DISABLE);
    GPIO_SetFunc(spi_cfg[unit].pin_mosi.port, spi_cfg[unit].pin_mosi.pin, spi_cfg[unit].pin_mosi.func, PIN_SUBFUNC_DISABLE);
    GPIO_SetFunc(spi_cfg[unit].pin_miso.port, spi_cfg[unit].pin_miso.pin, spi_cfg[unit].pin_miso.func, PIN_SUBFUNC_DISABLE);
}

void hal_spi_init(enum hal_spi_type unit)
{
    stc_gpio_init_t stcGpioCfg;
    stc_spi_init_t stcSpiInit;
    stc_spi_delay_t stcSpiDelay;
    
    if(unit >= HAL_SPI_NUM)
        return;
    PWC_Fcg1PeriphClockCmd(spi_cfg[unit].fcg1_periph, Enable);
    
    
    GPIO_SetDebugPort(GPIO_PIN_TDI, Disable);
    (void)GPIO_StructInit(&stcGpioCfg);
    stcGpioCfg.u16PinDir = PIN_DIR_OUT;
    stcGpioCfg.u16PinDrv = PIN_DRV_HIGH;
    (void)GPIO_Init(spi_cfg[unit].pin_cs.port, spi_cfg[unit].pin_cs.pin, &stcGpioCfg);
    GPIO_SetPins(spi_cfg[unit].pin_cs.port, spi_cfg[unit].pin_cs.pin);
    
    (void)GPIO_StructInit(&stcGpioCfg);
    stcGpioCfg.u16PinDrv = PIN_DRV_HIGH;
    //(void)GPIO_Init(spi_cfg[unit].pin_cs.port,  spi_cfg[unit].pin_cs.pin, &stcGpioCfg);
    (void)GPIO_Init(spi_cfg[unit].pin_sck.port,  spi_cfg[unit].pin_sck.pin, &stcGpioCfg);
    (void)GPIO_Init(spi_cfg[unit].pin_mosi.port, spi_cfg[unit].pin_mosi.pin, &stcGpioCfg);

    /* CMOS input for input pin */
    stcGpioCfg.u16PinIType = PIN_ITYPE_CMOS;
    (void)GPIO_Init(spi_cfg[unit].pin_miso.port, spi_cfg[unit].pin_miso.pin, &stcGpioCfg);    

    //GPIO_SetFunc(spi_cfg[unit].pin_cs.port,  spi_cfg[unit].pin_cs.pin,  spi_cfg[unit].pin_cs.func, PIN_SUBFUNC_DISABLE);
    GPIO_SetFunc(spi_cfg[unit].pin_sck.port,  spi_cfg[unit].pin_sck.pin,  spi_cfg[unit].pin_sck.func, PIN_SUBFUNC_DISABLE);
    GPIO_SetFunc(spi_cfg[unit].pin_mosi.port, spi_cfg[unit].pin_mosi.pin, spi_cfg[unit].pin_mosi.func, PIN_SUBFUNC_DISABLE);
    GPIO_SetFunc(spi_cfg[unit].pin_miso.port, spi_cfg[unit].pin_miso.pin, spi_cfg[unit].pin_miso.func, PIN_SUBFUNC_DISABLE);

    SPI_DeInit(spi_cfg[unit].spix);
    (void)SPI_StructInit(&stcSpiInit);   /* Clear initialize structure */
    stcSpiInit.u32WireMode          = SPI_WIRE_3;
    stcSpiInit.u32TransMode         = SPI_FULL_DUPLEX;
    stcSpiInit.u32MasterSlave       = SPI_MASTER;
    stcSpiInit.u32SuspMode          = SPI_COM_SUSP_FUNC_OFF;
    stcSpiInit.u32Modfe             = SPI_MODFE_DISABLE;
    stcSpiInit.u32Parity            = SPI_PARITY_INVALID;
    stcSpiInit.u32SpiMode           = SPI_MODE_0;
    stcSpiInit.u32BaudRatePrescaler = SPI_BR_PCLK1_DIV256;
    stcSpiInit.u32DataBits          = SPI_DATA_SIZE_8BIT;
    stcSpiInit.u32FirstBit          = SPI_FIRST_MSB;
    (void)SPI_Init(spi_cfg[unit].spix, &stcSpiInit);
    
    SPI_DelayStructInit(&stcSpiDelay);
    stcSpiDelay.u32IntervalDelay = SPI_INTERVAL_TIME_8SCK_2PCLK1;
    stcSpiDelay.u32ReleaseDelay = SPI_RELEASE_TIME_1SCK;
    stcSpiDelay.u32SetupDelay = SPI_SETUP_TIME_2SCK;
    SPI_DelayTimeCfg(spi_cfg[unit].spix, &stcSpiDelay);
    
    SPI_FunctionCmd(spi_cfg[unit].spix, Enable);

}

void hal_spi_irq_register(enum hal_spi_type unit,enum hal_spi_irq irq_type, void (*irq_cb)(void))
{
    stc_irq_signin_config_t stcIrqRegiConf;

    switch(irq_type)
    {
        case HAL_SPI_IRQ_RX:
            stcIrqRegiConf.enIRQn = spi_cfg[unit].rx_irq.stc_irq_signin_cfg.enIRQn;                    /* Register INT_SPI1_SPRI Int to Vect.No.003 */
            stcIrqRegiConf.enIntSrc = spi_cfg[unit].rx_irq.stc_irq_signin_cfg.enIntSrc;               /* Select Event interrupt of SPI1 */
            stcIrqRegiConf.pfnCallback = spi_cfg[unit].rx_irq.stc_irq_signin_cfg.pfnCallback;  /* Callback function */
            (void)INTC_IrqSignIn(&stcIrqRegiConf);                  /* Registration IRQ */

            NVIC_ClearPendingIRQ(stcIrqRegiConf.enIRQn);            /* Clear Pending */
            NVIC_SetPriority(stcIrqRegiConf.enIRQn, spi_cfg[unit].rx_irq.priority);/* Set priority */
            NVIC_EnableIRQ(stcIrqRegiConf.enIRQn);                  /* Enable NVIC */   
            SPI_IntCmd(spi_cfg[unit].spix, SPI_INT_RX_BUFFER_FULL, Enable);
        break;
        default:
            break;
    }

}

void set_spi_cs_pin(enum hal_spi_type unit, uint8_t state)
{
    if(state)
    {
        GPIO_SetPins(spi_cfg[unit].pin_cs.port, spi_cfg[unit].pin_cs.pin);
    }
    else
    {
        GPIO_ResetPins(spi_cfg[unit].pin_cs.port, spi_cfg[unit].pin_cs.pin);
    }
}

uint8_t hal_spi_send_recv_byte(enum hal_spi_type unit,uint8_t data)
{
    uint8_t read_data = 0;
     
    while(Reset == SPI_GetStatus(spi_cfg[unit].spix, SPI_FLAG_TX_BUFFER_EMPTY))
    {
        ;
    }
    SPI_WriteDataReg(spi_cfg[unit].spix, (uint32_t)data);
    while(Reset == SPI_GetStatus(spi_cfg[unit].spix, SPI_FLAG_RX_BUFFER_FULL))
    {
        ;
    }
    read_data = (uint8_t)SPI_ReadDataReg(spi_cfg[unit].spix);
    return read_data;
}


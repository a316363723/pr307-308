/**
 * @file board.c
 * @author Darwin (darwin.xiao@aputure.com)
 * @brief 
 * @version 0.1
 * @date 2022-09-09
 * 
 * @copyright Copyright (c) 2022 爱图仕
 * 
 * @par 修改日志:
 * <table>
 * <tr><th> Date          <th>Version <th>Author  <th>Description
 * <tr><td>2022-09-09     <td>1.0     <td>Darwin.Xiao   <td>初始创建
 * </table>
 */
#include "board.h"
#include "hc32_ddl.h"


static void system_clock_config(void);

void board_init(void)
{
    system_clock_config();
}


static void system_clock_config(void)
{
    stc_clk_pllh_init_t stcPLLHInit;
    
    /* unlock GPIO register: PSPCR, PCCR, PINAER, PCRxy, PFSRxy */
    GPIO_Unlock();
    /* unlock PWC register: FCG0 */
    PWC_FCG0_Unlock();
    /* unlock PWC, CLK, PVD registers, @ref PWC_REG_Write_Unlock_Code for details */
    PWC_Unlock(PWC_UNLOCK_CODE_0| PWC_UNLOCK_CODE_1| PWC_UNLOCK_CODE_2);
	/* unlock PWR, CMU_PERICKSEL, RMU_RSTF0,PWR_VBATRSTR,PWR_BATCR0,PWR_WKTC0,PWR_WKTC1,PWR_WKTC2, RMU_PRSTCR0 */
	
    /* unlock SRAM register: WTCR */
    SRAM_WTCR_Unlock();
    /* Lock SRAM register: CKCR */
    SRAM_CKCR_Unlock();
  
//    /* Lock EFM register: FWMC */
//    EFM_FWMC_Unlock();
    /* Lock all EFM registers */
    EFM_Unlock();
    /* Lock all MPU registers */
    // MPU_Lock();

    /* PCLK0, HCLK  Max 240MHz */
    /* PCLK1, PCLK4 Max 120MHz */
    /* PCLK2, PCLK3 Max 60MHz  */
    /* EX BUS Max 120MHz */
    CLK_ClkDiv(CLK_CATE_ALL, (CLK_PCLK0_DIV1 | CLK_PCLK1_DIV2 | CLK_PCLK2_DIV16 | CLK_PCLK3_DIV4 | CLK_PCLK4_DIV2 | CLK_EXCLK_DIV2 | CLK_HCLK_DIV1));

    (void)CLK_PLLHStrucInit(&stcPLLHInit);
    /* VCO = (8/1)*120 = 960MHz*/
    stcPLLHInit.u8PLLState = CLK_PLLH_ON;
    stcPLLHInit.PLLCFGR = 0UL;
    stcPLLHInit.PLLCFGR_f.PLLM = 1UL - 1UL;
    stcPLLHInit.PLLCFGR_f.PLLN = 60UL - 1UL;
    stcPLLHInit.PLLCFGR_f.PLLP = 4UL - 1UL;
    stcPLLHInit.PLLCFGR_f.PLLQ = 4UL - 1UL;
    stcPLLHInit.PLLCFGR_f.PLLR = 4UL - 1UL;
    stcPLLHInit.PLLCFGR_f.PLLSRC = CLK_PLLSRC_XTAL;
    (void)CLK_PLLHInit(&stcPLLHInit);

    /* Highspeed SRAM set to 1 Read/Write wait cycle */
    SRAM_SetWaitCycle(SRAM_SRAMH, SRAM_WAIT_CYCLE_1, SRAM_WAIT_CYCLE_1);

    /* SRAM1_2_3_4_backup set to 2 Read/Write wait cycle */
    SRAM_SetWaitCycle((SRAM_SRAM123 | SRAM_SRAM4 | SRAM_SRAMB), SRAM_WAIT_CYCLE_2, SRAM_WAIT_CYCLE_2);

    /* 0-wait @ 40MHz */
    EFM_SetWaitCycle(EFM_WAIT_CYCLE_5);

    /* 4 cycles for 200 ~ 250MHz */
    GPIO_SetReadWaitCycle(GPIO_READ_WAIT_4);

    /* 2. Specify the clock source of ADC. Only 'CLK_PERI_CLK_PCLK' can be used in this example. */
    CLK_PERI_ClkConfig(CLK_PERI_CLK_PCLK);

    CLK_SetSysClkSrc(CLK_SYSCLKSOURCE_PLLH);
    /* EXBUS CLK 60MHz */
    CLK_ClkDiv(CLK_CATE_EXCLK, CLK_EXCLK_DIV2);

    PWC_Fcg1PeriphClockCmd(PWC_FCG1_ETHER, Enable); /*��̫��ʱ��*/
    EFM_FWMC_Unlock();
    EFM_OTP_WP_Unlock();
    EFM_Cmd(EFM_FLASH0_ACT_FLASH1_ACT);    
    
    NVIC_SetPriorityGrouping(0x03);
    
    GPIO_SetDebugPort(GPIO_PIN_TRST,Disable) ;
	GPIO_SetDebugPort(GPIO_PIN_SWO,Disable) ;
}

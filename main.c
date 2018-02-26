
#include "inc/loader.h"

#include "m2sxxx.h"
#include "mss_gpio.h"
#include <stdint.h>

#if (MSS_SYS_MDDR_CONFIG_BY_CORTEX == 1)
#error "Please turn off DDR initialization! See the comment in this file above."
#endif

/* See loader.h for details on these constants. */
uint32_t * const esram0_base = (uint32_t *)ESRAM0_BASE_ADDR;
uint32_t * const esram0_stack_ptr = (uint32_t *)ESRAM0_STACK_PTR;
uint32_t * const esram0_program_ptr = (uint32_t *)ESRAM0_PROGRAM_PTR;
uint32_t * const envm_isp_store_address = (uint32_t *)ENVM_ISP_STORE_ADDRESS;

const uint32_t boot_loader_ram_allocation = (1024 * 4);  /* Needs to match the linker file! */
const uint32_t esram_length = (1024 * 64);

/* function to copy code to eSRAM*/
void copy_image_to_esram() {
  uint32_t i = 0;
  uint32_t *exeDestAddr;
  uint32_t *exeSrcAddr;

  exeDestAddr = esram0_base;
  exeSrcAddr = envm_isp_store_address;

  uint32_t words = (esram_length - boot_loader_ram_allocation) / sizeof(uint32_t);
  /* Copy from internal flash to internal sram 32-bits at a time. */
  for (i = 0; i < words; ++i) {
    *exeDestAddr++ = *exeSrcAddr++;
  }
}

/* Function to launch ISP application from eSRAM. */
void launch_from_eSRAM_0(void) {
  /* pointer to reset handler of application. */
  program program_entry = (program)*(esram0_program_ptr);
  /* Set the stack pointer to that of the application's definition. */
  __set_MSP(*esram0_stack_ptr);
  /* Go! */
  program_entry();
  /* TODO I see loops like this where return is never intended. Maybe 'RTS'
     * gets omitted? */
  while(1){ };
}

/* Initialize DDR SRAM as per Emcraft SOM specs. */
void init_mddr_lpddr(void) {
  MDDR->core.ddrc.DYN_SOFT_RESET_CR                   = 0x0000;
  MDDR->core.ddrc.DYN_REFRESH_1_CR                    = 0x27de;
  MDDR->core.ddrc.DYN_REFRESH_2_CR                    = 0x030f;
  MDDR->core.ddrc.DYN_POWERDOWN_CR                    = 0x0002;
  MDDR->core.ddrc.DYN_DEBUG_CR                        = 0x0000;
  MDDR->core.ddrc.MODE_CR                             = 0x00C1;
  MDDR->core.ddrc.ADDR_MAP_BANK_CR                    = 0x099f;
  MDDR->core.ddrc.ECC_DATA_MASK_CR                    = 0x0000;
  MDDR->core.ddrc.ADDR_MAP_COL_1_CR                   = 0x3333;
  MDDR->core.ddrc.ADDR_MAP_COL_2_CR                   = 0xffff;
  MDDR->core.ddrc.ADDR_MAP_ROW_1_CR                   = 0x7777;
  MDDR->core.ddrc.ADDR_MAP_ROW_2_CR                   = 0x0fff;
  MDDR->core.ddrc.INIT_1_CR                           = 0x0001;
  MDDR->core.ddrc.CKE_RSTN_CYCLES_CR[0]               = 0x4242;
  MDDR->core.ddrc.CKE_RSTN_CYCLES_CR[1]               = 0x0008;
  MDDR->core.ddrc.INIT_MR_CR                          = 0x0033;
  MDDR->core.ddrc.INIT_EMR_CR                         = 0x0020;
  MDDR->core.ddrc.INIT_EMR2_CR                        = 0x0000;
  MDDR->core.ddrc.INIT_EMR3_CR                        = 0x0000;
  MDDR->core.ddrc.DRAM_BANK_TIMING_PARAM_CR           = 0x00c0;
  MDDR->core.ddrc.DRAM_RD_WR_LATENCY_CR               = 0x0023;
  MDDR->core.ddrc.DRAM_RD_WR_PRE_CR                   = 0x0235;
  MDDR->core.ddrc.DRAM_MR_TIMING_PARAM_CR             = 0x0064;
  MDDR->core.ddrc.DRAM_RAS_TIMING_CR                  = 0x0108;
  MDDR->core.ddrc.DRAM_RD_WR_TRNARND_TIME_CR          = 0x0178;
  MDDR->core.ddrc.DRAM_T_PD_CR                        = 0x0033;
  MDDR->core.ddrc.DRAM_BANK_ACT_TIMING_CR             = 0x1947;
  MDDR->core.ddrc.ODT_PARAM_1_CR                      = 0x0010;
  MDDR->core.ddrc.ODT_PARAM_2_CR                      = 0x0000;
  MDDR->core.ddrc.ADDR_MAP_COL_3_CR                   = 0x3300;
  MDDR->core.ddrc.MODE_REG_RD_WR_CR                   = 0x0000;
  MDDR->core.ddrc.MODE_REG_DATA_CR                    = 0x0000;
  MDDR->core.ddrc.PWR_SAVE_1_CR                       = 0x0514;
  MDDR->core.ddrc.PWR_SAVE_2_CR                       = 0x0000;
  MDDR->core.ddrc.ZQ_LONG_TIME_CR                     = 0x0200;
  MDDR->core.ddrc.ZQ_SHORT_TIME_CR                    = 0x0040;
  MDDR->core.ddrc.ZQ_SHORT_INT_REFRESH_MARGIN_CR[0]   = 0x0012;
  MDDR->core.ddrc.ZQ_SHORT_INT_REFRESH_MARGIN_CR[1]   = 0x0002;
  MDDR->core.ddrc.PERF_PARAM_1_CR                     = 0x4000;
  MDDR->core.ddrc.HPR_QUEUE_PARAM_CR[0]               = 0x80f8;
  MDDR->core.ddrc.HPR_QUEUE_PARAM_CR[1]               = 0x0007;
  MDDR->core.ddrc.LPR_QUEUE_PARAM_CR[0]               = 0x80f8;
  MDDR->core.ddrc.LPR_QUEUE_PARAM_CR[1]               = 0x0007;
  MDDR->core.ddrc.WR_QUEUE_PARAM_CR                   = 0x0200;
  MDDR->core.ddrc.PERF_PARAM_2_CR                     = 0x0001;
  MDDR->core.ddrc.PERF_PARAM_3_CR                     = 0x0000;
  MDDR->core.ddrc.DFI_RDDATA_EN_CR                    = 0x0003;
  MDDR->core.ddrc.DFI_MIN_CTRLUPD_TIMING_CR           = 0x0003;
  MDDR->core.ddrc.DFI_MAX_CTRLUPD_TIMING_CR           = 0x0040;
  MDDR->core.ddrc.DFI_WR_LVL_CONTROL_CR[0]            = 0x0000;
  MDDR->core.ddrc.DFI_WR_LVL_CONTROL_CR[1]            = 0x0000;
  MDDR->core.ddrc.DFI_RD_LVL_CONTROL_CR[0]            = 0x0000;
  MDDR->core.ddrc.DFI_RD_LVL_CONTROL_CR[1]            = 0x0000;
  MDDR->core.ddrc.DFI_CTRLUPD_TIME_INTERVAL_CR        = 0x0309;
  MDDR->core.ddrc.AXI_FABRIC_PRI_ID_CR                = 0x0000;
  MDDR->core.ddrc.ECC_INT_CLR_REG                     = 0x0000;

  MDDR->core.phy.LOOPBACK_TEST_CR                     = 0x0000;
  MDDR->core.phy.CTRL_SLAVE_RATIO_CR                  = 0x0080;
  MDDR->core.phy.DATA_SLICE_IN_USE_CR                 = 0x0003;
  MDDR->core.phy.DQ_OFFSET_CR[0]                      = 0x00000000;
  MDDR->core.phy.DQ_OFFSET_CR[2]                      = 0x0000;
  MDDR->core.phy.DLL_LOCK_DIFF_CR                     = 0x000B;
  MDDR->core.phy.FIFO_WE_SLAVE_RATIO_CR[0]            = 0x0040;
  MDDR->core.phy.FIFO_WE_SLAVE_RATIO_CR[1]            = 0x0401;
  MDDR->core.phy.FIFO_WE_SLAVE_RATIO_CR[2]            = 0x4010;
  MDDR->core.phy.FIFO_WE_SLAVE_RATIO_CR[3]            = 0x0000;
  MDDR->core.phy.LOCAL_ODT_CR                         = 0x0001;
  MDDR->core.phy.RD_DQS_SLAVE_RATIO_CR[0]             = 0x0040;
  MDDR->core.phy.RD_DQS_SLAVE_RATIO_CR[1]             = 0x0401;
  MDDR->core.phy.RD_DQS_SLAVE_RATIO_CR[2]             = 0x4010;
  MDDR->core.phy.WR_DATA_SLAVE_RATIO_CR[0]            = 0x0040;
  MDDR->core.phy.WR_DATA_SLAVE_RATIO_CR[1]            = 0x0401;
  MDDR->core.phy.WR_DATA_SLAVE_RATIO_CR[2]            = 0x4010;
  MDDR->core.phy.WR_RD_RL_CR                          = 0x0021;
  MDDR->core.phy.RDC_WE_TO_RE_DELAY_CR                = 0x0003;
  MDDR->core.phy.USE_FIXED_RE_CR                      = 0x0001;
  MDDR->core.phy.USE_RANK0_DELAYS_CR                  = 0x0001;
  MDDR->core.phy.CONFIG_CR                            = 0x0009;
  MDDR->core.phy.DYN_RESET_CR                         = 0x01;
  MDDR->core.ddrc.DYN_SOFT_RESET_CR                   = 0x01;

  /* Wait for config */
  while((MDDR->core.ddrc.DDRC_SR) == 0x0000) {
  }
}

void execute_isp(void) {
  /* Initialize MSS GPIOs. */
  MSS_GPIO_init();

  /* Configure MSS GPIOs. */
  MSS_GPIO_config(DS3_LED_GPIO, MSS_GPIO_OUTPUT_MODE);
  MSS_GPIO_config(DS4_LED_GPIO, MSS_GPIO_OUTPUT_MODE);

  /* Set LEDs to initial state. */
  MSS_GPIO_set_outputs(MSS_GPIO_get_outputs() | DS3_LED_MASK | DS4_LED_MASK);
}

int main() {
  /* Initialize external DDR RAM */
  init_mddr_lpddr();

  /* Initialize MSS GPIOs. */
  MSS_GPIO_init();

  /* Configure MSS GPIOs. */
  MSS_GPIO_config(DS3_LED_GPIO, MSS_GPIO_OUTPUT_MODE);
  MSS_GPIO_config(DS4_LED_GPIO, MSS_GPIO_OUTPUT_MODE);

  /* Set LEDs to initial state. */
  MSS_GPIO_set_outputs(MSS_GPIO_get_outputs() | DS3_LED_MASK | DS4_LED_MASK);

  /* Initialization all necessary hardware components */
  copy_image_to_esram();
  launch_from_eSRAM_0();
  while (1) {
    // execution never comes here
  }

  return 0;
}

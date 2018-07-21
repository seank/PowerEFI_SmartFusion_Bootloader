/* Copyright 2018 Sean Keys

   Licensed under the Apache License, Version 2.0 (the "License")
   You may not use this file except in compliance with the License.
   You may obtain a copy of the License at
      https://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#include "inc/loader.h"

#include "m2sxxx.h"
#include "mss_gpio.h"

#if (MSS_SYS_MDDR_CONFIG_BY_CORTEX == 1)
#error "Please turn off DDR initialization! See the comment in this file above."
#endif

typedef void(*program)(void);

typedef enum RAMmapping {
  DEFAULT_REMAP = 0,
  LPDDR_REMAP   = 1,
  ESRAM_REMAP   = 2,
}RAMmapping_t;

/* Prototypes */
void LoadAndLaunchFromRAM(uint32_t * const image_addr,
                    uint32_t * const ram_addr,
                    uint32_t image_length, RAMmapping_t map)
                    __attribute__((noreturn)) __attribute__((optimize("O0")));

/* See loader.h for details on these constants. */
uint32_t * const esram_base = &__esram_base_address;
uint32_t * const lpddr_base = &__lpddr_base_addresses;
const uint32_t boot_loader_ram_allocation = (uint32_t)&__bootloader_ram_size;
const uint32_t esram_length = (uint32_t)&__esram_length;
uint32_t * const envm_isp_store = (uint32_t *)ENVM_ISP_STORE_ADDRESS;
uint32_t * const envm_app_store = (uint32_t *)ENVM_APP_STORE_ADDRESS;

/* Image offsets (in 32-bit words). */
/* TODO (skeys) We need a header section to store a CRC or a signature. */
const uint32_t reset_vector_ptr_offset = 1;
const uint32_t stack_ptr_offset = 0;

/* function to copy code to eSRAM*/
void CopyImgToRAM(uint32_t * const image_addr, uint32_t * const ram_addr,
    uint32_t image_length) {
  uint32_t i = 0;
  uint32_t *exeDestAddr;
  uint32_t *exeSrcAddr;

  exeDestAddr = ram_addr;
  exeSrcAddr = image_addr;

  uint32_t words = image_length / sizeof(uint32_t);
  /* Copy from internal flash to internal sram 32-bits at a time. */
  for (i = 0; i < words; ++i) {
    *exeDestAddr++ = *exeSrcAddr++;
  }
}

void LoadAndLaunchFromRAM(uint32_t * const image_addr, uint32_t * const ram_addr,
    uint32_t image_length, RAMmapping_t map) {

  CopyImgToRAM(image_addr, ram_addr, image_length);

  /* Image contains pointers to VMA addresses for reset vector and
   * stack pointer, so just read them in from ROM and dereference.
   */
  program program_entry = (program) *(image_addr + reset_vector_ptr_offset);
  /* Set the stack pointer to that of the application's definition. */
  __set_MSP(*(image_addr + stack_ptr_offset));

  /* Re-map RAM (eSRAM or LPDDR) if required */
  switch (map) {
  case LPDDR_REMAP:
    /* Re-map DDR address space to 0x00000000 */
    SYSREG->DDR_CR = 0x1;
    break;
  default:
    break;
  }
  /* Go! */
  program_entry();
  /* http://www.keil.com/support/man/docs/armcc/armcc_chr1359124976881.htm */
  while (1);
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

void BooteNVMapp() {
  LoadAndLaunchFromRAM(envm_app_store, lpddr_base, ENVM_APP_STORE_SIZE,
                       LPDDR_REMAP);
}

/* Function to launch ISP application from eSRAM. */
void BooteNVMisp() {
  LoadAndLaunchFromRAM(envm_isp_store, esram_base,
                       (esram_length - boot_loader_ram_allocation),
                       DEFAULT_REMAP);
}

int main() {
  /* Initialize external DDR RAM */
  init_mddr_lpddr();

  /* Initialize MSS GPIOs. */
  MSS_GPIO_init();

  /* Configure MSS GPIOs. */
  MSS_GPIO_config(DS3_LED_GPIO, MSS_GPIO_OUTPUT_MODE);
  MSS_GPIO_config(DS4_LED_GPIO, MSS_GPIO_OUTPUT_MODE);
  MSS_GPIO_config(S2_BTN_GPIO, MSS_GPIO_INPUT_MODE);

  /* Set LEDs to initial state. */
  MSS_GPIO_set_outputs(MSS_GPIO_get_outputs() | DS3_LED_MASK | DS4_LED_MASK);

  if (MSS_GPIO_get_inputs() & S2_BTN_MASK) {
    BooteNVMapp();
  } else {
    BooteNVMisp();
  }

  while (1) {
    // execution never comes here
  }

  return 0;
}

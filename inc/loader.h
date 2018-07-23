/* Copyright 2018 Sean Keys

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at
      https://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#ifndef LOADER_H_
#define LOADER_H_

#include <stdint.h>

/* Needs to match eNVM ISP client, which sees store addresses with eNVM
 * re-mapped to 0x0. Example, 0x60003200 is seen as 0x3200. */
#define ENVM_ISP_STORE_ADDRESS   0x60003200
#define ENVM_APP_STORE_ADDRESS   0x60010000
#define ENVM_APP_STORE_SIZE      (1024 * 192)

/* LEDs GPIO TODO move to common platform header. */
#define DS3_LED_GPIO  MSS_GPIO_1
#define DS4_LED_GPIO  MSS_GPIO_2
#define DS3_LED_MASK  MSS_GPIO_1_MASK
#define DS4_LED_MASK  MSS_GPIO_2_MASK
#define S2_BTN_GPIO   MSS_GPIO_0
#define S2_BTN_MASK   MSS_GPIO_0_MASK

/* Linker defined symbols. */
extern const uint32_t __esram_base_address;
extern const uint32_t __bootloader_ram_size;
extern const uint32_t __esram_length;
extern const uint32_t __lpddr_base_addresses;

/* Useful functions to be called from application firmware. */
void BooteNVMapp() __attribute__((noreturn));
void BooteNVMisp() __attribute__((noreturn));

#endif /* LOADER_H_ */

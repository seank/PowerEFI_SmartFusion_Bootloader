/*
 * loader.h
 *
 *  Created on: Feb 25, 2018
 *      Author: seank
 */

#ifndef LOADER_H_
#define LOADER_H_


/* TODO once we have the loader, isp, and main application locations defined,
 * we should be able to pull these locations and sizes from the linker file.
 * Eg. extern RAM_SIZE
 *
 * I'm holding off a bit, until we figure out how we want to use the memory
 * remap functions of the SoC.
 */

/* Addresses relevant for loading to ESRAM and launching application form there*/
#define ESRAM0_BASE_ADDR       0x20000000  /* Start of 64K ESRAM */
/* Typically the first address in the start of the linker file */
#define ESRAM0_STACK_PTR       0x20000000  /* Start of 64K ESRAM */
/* Program pointer is right after the stack pointer. */
#define ESRAM0_PROGRAM_PTR     (ESRAM0_STACK_PTR + 4)

/* Needs to match eNVM ISP client */
#define ENVM_ISP_STORE_ADDRESS 0x60020000;

#define BOOT_LOADER_RAM_LENGTH (1024 * 4);  /* Must match production linker file! */
#define ESRAM_LENGTH           (1024 * 64);


/* LEDs GPIO TODO move to common platform header. */
#define DS3_LED_GPIO  MSS_GPIO_1
#define DS4_LED_GPIO  MSS_GPIO_2
#define DS3_LED_MASK  MSS_GPIO_1_MASK
#define DS4_LED_MASK  MSS_GPIO_2_MASK

typedef void(*program)(void);

void launch_from_eSRAM_0(void) __attribute__((noreturn));

#endif /* LOADER_H_ */

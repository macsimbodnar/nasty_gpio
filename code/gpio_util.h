/****************************************************************************
									NASTY
	Macros taken from:
	https://github.com/Pieter-Jan/PJ_RPI
									
*****************************************************************************/
#ifndef NASTY_GPIO_UTIL_H

#include <stdio.h>

#include <string.h>
#include <dirent.h>
#include <fcntl.h>

#include <sched.h>		// To set the priority on linux

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <unistd.h>

#define GPIO_SLOW 1

#if GPIO_SLOW
#define Assert(expression) if (!(expression)) { *(int *)0 = 0; }
#else
#define Assert(expression)
#endif

#define ArrayCount(array) (sizeof(array) / sizeof(array[0]))
#define Kilobytes(value) ((value) * 1024)
#define Megabytes(value) (Kilobytes(value) * 1024)
#define Gigabytes(value) (Megabytes(value) * 1024)
#define Terabytes(value) (Gigabytes(value) * 1024)

#define PAGE_SIZE (4 * 1024)
#define BLOCK_SIZE (4 * 1024)

#define RASP3
//#define RASP2
//#define RASP1

// NOTE (max): The start periferical address 
// and the I2C buss Broadcom Serial Controller (BSC). There are 
// BSC0 and BSC1 we will use BSC0
#if defined(RASP3)
#define BCM2708_BASE_PER_ADDRESS 0x3F000000
#define BSC0_BASE_ADDRESS (BCM2708_BASE_PER_ADDRESS +  0x804000)

#elif defined(RASP2)
#define BCM2708_BASE_PER_ADDRESS 0x3F000000
#define BSC0_BASE_ADDRESS (BCM2708_BASE_PER_ADDRESS +  0x804000)

#elif defined(RASP1)
#define BCM2708_BASE_PER_ADDRESS 0x20000000
#define BSC0_BASE_ADDRESS (BCM2708_BASE_PER_ADDRESS +  0x205000)

#endif

// GPIO controller
#define GPIO_BASE_ADDRESS (BCM2708_BASE_PER_ADDRESS + 0x200000)


// NOTE (max): Input/Output Acces structure.
// Contain information about the location of the register
struct bcm_peripheral
{
	unsigned int physical_address;
	int memory_file_descriptor;
	void * map;
	volatile unsigned int * address;
};


extern struct bcm_peripheral gpio;
extern struct bcm_peripheral bsc0;


// GPIO Setup macros
#define INPUT_GPIO(g) *(gpio.address + ((g)/10)) &= ~(7<<(((g)%10)*3))
#define OUTPUT_GPIO(g) *(gpio.address + ((g)/10)) |=  (1<<(((g)%10)*3))
#define SET_GPIO_ALT(g,a) *(gpio.address + (((g)/10))) |= (((a)<=3?(a) + 4:(a)==4?3:2)<<(((g)%10)*3))

// NOTE (max): 
// Sets   bits which are 1 ignores bits which are 0
// Clears bits which are 1 ignores bits which are 0
#define GPIO_SET *(gpio.address + 7)  
#define GPIO_CLR *(gpio.address + 10) 

#define GPIO_READ(g) *(gpio.address + 13) &= (1<<(g))


// I2C macros
#define BSC0_C *(bsc0.address + 0x00)
#define BSC0_S *(bsc0.address + 0x01)
#define BSC0_DLEN *(bsc0.address + 0x02)
#define BSC0_A *(bsc0.address + 0x03)
#define BSC0_FIFO *(bsc0.address + 0x04)

#define BSC_C_READ 1
#define BSC_C_CLEAR (1 << 4)
#define BSC_C_ST (1 << 7)
#define BSC_C_INTD (1 << 8)
#define BSC_C_INTT (1 << 9)
#define BSC_C_INTR (1 << 10)
#define BSC_C_I2CEN (1 << 15)

#define START_READ BSC_C_I2CEN|BSC_C_ST|BSC_C_CLEAR|BSC_C_READ
#define START_WRITE BSC_C_I2CEN|BSC_C_ST

#define BSC_S_TA 1
#define BSC_S_DONE (1 << 1)
#define BSC_S_TXW (1 << 2)
#define BSC_S_RXR (1 << 3)
#define BSC_S_TXD (1 << 4)
#define BSC_S_RXD (1 << 5)
#define BSC_S_TXE (1 << 6)
#define BSC_S_RXF (1 << 7)
#define BSC_S_ERR (1 << 8)
#define BSC_S_CLKT (1 << 9)

#define CLEAR_STATUS BSC_S_CLKT | BSC_S_ERR | BSC_S_DONE

// Function prototypes
int map_peripheral(struct bcm_peripheral * p);
void unmap_peripheral(struct bcm_peripheral * p);

// I2C
void dump_bsc_status();
void wait_i2c_done();
void i2c_init();

// Priority
int setProgramPriority(int priorityLevel);

#define NASTY_GPIO_UTIL_H
 #endif
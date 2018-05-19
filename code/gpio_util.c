#include "gpio_util.h"

struct bcm_peripheral gpio = { GPIO_BASE_ADDRESS };
struct bcm_peripheral bsc0 = { BSC0_BASE_ADDRESS };

int map_peripheral(struct bcm_peripheral * p)
{
	p->memory_file_descriptor = open("/dev/mem", O_RDWR | O_SYNC);
	if (p->memory_file_descriptor < 0) {
		printf("Failed to open /dev/mem, try checking permissions.\n");
		return -1;
	}


	p->map = mmap(0, BLOCK_SIZE, PROT_READ | PROT_WRITE, 
								MAP_SHARED, p->memory_file_descriptor,
								p->physical_address);


	if (p->map == MAP_FAILED) {
		perror("mmap");
		return -1;
	}


	p->address = (volatile unsigned int *) p->map;

	return 0;
}


void unmap_peripheral(struct bcm_peripheral * p)
{
	munmap(p->map, BLOCK_SIZE);
	close(p->memory_file_descriptor);
}


// I2C
void dump_bsc_status()
{
	unsigned int s = BSC0_S;

	printf("BSC0_S: ERR=%d  RXF=%d  TXE=%d  RXD=%d  TXD=%d  RXR=%d  TXW=%d  DONE=%d  TA=%d\n",
        (s & BSC_S_ERR) != 0,
        (s & BSC_S_RXF) != 0,
        (s & BSC_S_TXE) != 0,
        (s & BSC_S_RXD) != 0,
        (s & BSC_S_TXD) != 0,
        (s & BSC_S_RXR) != 0,
        (s & BSC_S_TXW) != 0,
        (s & BSC_S_DONE) != 0,
        (s & BSC_S_TA) != 0 );
}


void wait_i2c_done()
{
	int timeout = 50;

    while((!((BSC0_S) & BSC_S_DONE)) && --timeout) {
        usleep(1000);
    }

    if(timeout == 0)
        printf("wait_i2c_done() timeout. Something went wrong.\n");
}


void i2c_init()
{
	INPUT_GPIO(0);
    SET_GPIO_ALT(0, 0);
    INPUT_GPIO(1);
    SET_GPIO_ALT(1, 0);
}


// Priority
int setProgramPriority(int priorityLevel)
{
	struct sched_param sched;

    memset (&sched, 0, sizeof(sched));

    if (priorityLevel > sched_get_priority_max (SCHED_RR)) {
    	priorityLevel = sched_get_priority_max (SCHED_RR);
    }

    sched.sched_priority = priorityLevel;

    return sched_setscheduler (0, SCHED_RR, &sched);
}
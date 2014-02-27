// read pwm
// the ReadPWM.bin will read pwm input from R31 
// the firmware will put pluse width value, frame width in pru share memory.
// the first channel value will be put into 
// 0x10100 in pru, which is 0x4a310100
// each channel has 16 bytes, 
// 0x4a310100  	1st channel
// 0x4a310110	2nd channel
// 0x4a310120	3rd channel,
// ...
// 0x4a3101E0	15th channel
// for each channel, 
// the first 4 bytes are flag, reserved for future use
// the seconds 4 bytes are pluse width, it is timer count number, timer run at 200mhz, 1us is 200.
// the third 4 byts are the frame width, it is timer count number
// the fourth 4 bytes are reserved for future use


/******************************************************************************
* Include Files                                                               *
******************************************************************************/

// Standard header files
#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>


/******************************************************************************
* Explicit External Declarations                                              *
******************************************************************************/

/******************************************************************************
* Local Macro Declarations                                                    *
******************************************************************************/

#define PRU_NUM 	 0

#define OFFSET_SHAREDRAM 2048		//equivalent with 0x00002000


/******************************************************************************
* Local Typedef Declarations                                                  *
******************************************************************************/


/******************************************************************************
* Local Function Declarations                                                 *
******************************************************************************/

static int LOCAL_exampleInit ( );

/******************************************************************************
* Local Variable Definitions                                                  *
******************************************************************************/


/******************************************************************************
* Intertupt Service Routines                                                  *
******************************************************************************/


/******************************************************************************
* Global Variable Definitions                                                 *
******************************************************************************/

static int mem_fd;
static void *ddrMem, *sharedMem;


/******************************************************************************
* Global Function Definitions                                                 *
******************************************************************************/

int main (void)
{
    unsigned int ret;
    
    printf("Start to read channel values\n");
    /* Initialize example */
    LOCAL_exampleInit();
	long count = 0;
    while(1)
	{
		int i = 0;
		int j = 0;
		unsigned long  rate = 0;
		unsigned long   r1[8], r2[8];
		unsigned long   r3[8];
		for(j =0; j < 8; j++)
		{
			r3[j] = 0;
		}
		for(j = 0; j < 8 ; j++)
		{
			for( i =0; i < 7; i ++)
			{
				//unsigned long n = (*(unsigned long *)(sharedMem + 114 + i * 0x10));
				r1[i] = (*(unsigned long *)(sharedMem + 0x114 + i * 0x10));
				r2[i]  = (*(unsigned long *)(sharedMem + 0x118 + i * 0x10));
				rate = r1[i];
				r3[i] = r3[i] + rate/20;
			}
			usleep(10000);
		}
		printf("%3d ", count);
		count ++;
		for(j = 0; j < 4; j ++)
		{
			//unsigned long 	 n = (*(unsigned long *)(sharedMem + 0x114 + j * 0x10));
			printf("%8d ", (r3[j]/8));
			//printf("%8d ", n/200 );
		}
		printf("\r\n");
		//sleep(1);
	}
    munmap(ddrMem, 0x2000);
    close(mem_fd);

    return(0);
}

/*****************************************************************************
* Local Function Definitions                                                 *
*****************************************************************************/

static int LOCAL_exampleInit (  )
{

    /* open the device */
    mem_fd = open("/dev/mem", O_RDWR);
    if (mem_fd < 0) {
        printf("Failed to open /dev/mem (%s)\n", strerror(errno));
        return -1;
    }	

    /* map the shared memory */
    sharedMem = mmap(0, 0x2000, PROT_WRITE | PROT_READ, MAP_SHARED, mem_fd, 0x4a310000);
    if (sharedMem == NULL) {
        printf("Failed to map the device (%s)\n", strerror(errno));
        close(mem_fd);
        return -1;
    }
    

    return(0);
}


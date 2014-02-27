/*
 * PRU_memAccess_DDR_PRUsharedRAM.c
 *
 * Copyright (C) 2012 Texas Instruments Incorporated - http://www.ti.com/ 
 * 
 * 
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions 
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the   
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
*/

/*
 * ============================================================================
 * Copyright (c) Texas Instruments Inc 2010-12
 *
 * Use of this software is controlled by the terms and conditions found in the
 * license agreement under which this software has been supplied or provided.
 * ============================================================================
 */

/******************************************************************************
* PRU_memAccess_DDR_PRUsharedRAM.c
*
* The PRU reads three values from external DDR memory and stores these values 
* in shared PRU RAM using the programmable constant table entries.  The example 
* initially loads 3 values into the external DDR RAM.  The PRU configures its 
* Constant Table Programmable Pointer Register 0 and 1 (CTPPR_0, 1) to point 
* to appropriate locations in the DDR memory and the PRU shared RAM.  The 
* values are then read from the DDR memory and stored into the PRU shared RAM 
* using the values in the 28th and 31st entries of the constant table.
*
******************************************************************************/


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
#define ADDEND1	 	 0x98765400u
#define ADDEND2		 0x12345678u
#define ADDEND3		 0x10210210u

#define DDR_BASEADDR     0x80000000
#define OFFSET_DDR	 0x00001000 
#define OFFSET_SHAREDRAM 2048		//equivalent with 0x00002000

#define PRUSS0_SHARED_DATARAM    4

/******************************************************************************
* Local Typedef Declarations                                                  *
******************************************************************************/


/******************************************************************************
* Local Function Declarations                                                 *
******************************************************************************/

static int LOCAL_exampleInit ( );
static unsigned short LOCAL_examplePassed ( unsigned short pruNum );

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

static unsigned int *sharedMem_int;

    void *DDR_regaddr1, *DDR_regaddr2, *DDR_regaddr3;	

/******************************************************************************
* Global Function Definitions                                                 *
******************************************************************************/

int main (void)
{
    unsigned int ret;
 //   tpruss_intc_initdata pruss_intc_initdata = PRUSS_INTC_INITDATA;
    
    printf("\nINFO: Starting %s example.\r\n", "PRU_memAccess_DDR_PRUsharedRAM");
    /* Initialize example */
    printf("\tINFO: Initializing example.\r\n");
    LOCAL_exampleInit();
	int count = 0;
    while(1)
	{
		int i = 0;
		int j = 0;
		double rate = 0;
		double r1[8], r2[8];
		double r3[8];
		for(j =0; j < 8; j++)
		{
			r3[j] = 0;
		}
		for(j = 0; j < 8 ; j++)
		{
			for( i =0; i < 8; i ++)
			{
				r1[i] = (double)(*(unsigned long *)(sharedMem + 4 + i * 0x10));
				r2[i]  = (double)(*(unsigned long *)(sharedMem + 8 + i * 0x10));
				rate = r1[i]/r2[i];
				r3[i] = r3[i] + rate;
			}
			usleep(10000);
		}
		printf("%3d ", count);
		count ++;
		for(j = 0; j < 4; j ++)
		{
			printf("%9.4f", r3[j]/8.0);
		}
		printf("\r\n");
		//sleep(1);
	}
    munmap(ddrMem, 0x100);
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
    sharedMem = mmap(0, 0x100, PROT_WRITE | PROT_READ, MAP_SHARED, mem_fd, 0x4a310000);
    if (sharedMem == NULL) {
        printf("Failed to map the device (%s)\n", strerror(errno));
        close(mem_fd);
        return -1;
    }
    
    /* Store Addends in DDR memory location */
    DDR_regaddr1 = sharedMem + 0;
    DDR_regaddr2 = sharedMem + 0x00000004;
    DDR_regaddr3 = sharedMem + 0x00000008;


    return(0);
}


// PruProxy.cpp
#include <stdio.h>
#include <iostream>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#include "PruProxy.h"

#define PRUSS_MAX_IRAM_SIZE	0x2000

using namespace std;

PruProxy::PruProxy()
{
	Enabled = true;
	mem_fd = -1;	
}

PruProxy::~PruProxy()
{
	if(mem_fd >= 0)
	{
		munmap(sharedMem, 0x400);
		close(mem_fd);
	}
}

// Disable the pru
void PruProxy::DisablePru()
{
	Enabled = false;
}

// initialize the pru
bool PruProxy::Init()
{
	Input1 = 1500;
	Input2 = 1500;
	Input3 = 1500;
	Input4 = 1500;
	Input5 = 1500;
	Input6 = 1500;
	
	if(Enabled)
	{

		
		/* open the device */	
		// map the device and init the varible before use it
		mem_fd = open("/dev/mem", O_RDWR);
		if (mem_fd < 0) {
			printf("Failed to open /dev/mem (%s)\n", strerror(errno));
			return false;
		}	
		
		LoadImageToPru0("ReadPwmPru0.bin");
		LoadImageToPru1("PwmOutPru1.bin");		

		/* map the shared memory */
		sharedMem = mmap(0, 0x400, PROT_WRITE | PROT_READ, MAP_SHARED, mem_fd, 0x4a310000);
		if (sharedMem == NULL) {
			printf("Failed to map the device (%s)\n", strerror(errno));
			close(mem_fd);
			return false;
		}
		InitOutput();
		
		RunPru0();
		RunPru1();	
		
	}
   return true;
}

// update input
bool PruProxy::UpdateInput()
{
	if(Enabled)
	{
		Input1 = (*(unsigned long *)(sharedMem + 0x114))/200;
		Input2 = (*(unsigned long *)(sharedMem + 0x124)/200);
		Input3 = (*(unsigned long *)(sharedMem + 0x134)/200);
		Input4 = (*(unsigned long *)(sharedMem + 0x144)/200);
		Input5 = (*(unsigned long *)(sharedMem + 0x154)/200);
		Input6 = (*(unsigned long *)(sharedMem + 0x164)/200);
		// if(Input1 > 2000 || Input1 < 1000)
		// {
			// Input1 = 1050;
		// }
		// if(Input2 > 2000 || Input2 < 1000)
		// {
			// Input2 = 1050;
		// }
		// if(Input3 > 2000 || Input3 < 1000)
		// {
			// Input3 = 1050;
		// }
		// if(Input4 > 2000 || Input4 < 1000)
		// {
			// Input4 = 1050;
		// }
	}
	return true;
	
}

// Init Output, set the esc to low throttle value as 1000
// set pulse width as 20 ms
// S1	pin 29		REAR_R		S4
// S2	pin 30		FRONT_R		S3
// S3	pin 27		READ)L		S2
// S4 	pin 28 		FRONT_L		S1
// Pin 20 21 won't work as it is high when reboot, esc wont' work
bool PruProxy::InitOutput()
{
	if(Enabled)
	{
		*(unsigned long *)(sharedMem + 0x240) = 1000 * 200;
		*(unsigned long *)(sharedMem + 0x250) = 1000 * 200;
		*(unsigned long *)(sharedMem + 0x220) = 1000 * 200;
		*(unsigned long *)(sharedMem + 0x230) = 1000 * 200;
		
		*(unsigned long *)(sharedMem + 0x244) = 5000 * 200;
		*(unsigned long *)(sharedMem + 0x254) = 5000 * 200;
		*(unsigned long *)(sharedMem + 0x224) = 5000 * 200;
		*(unsigned long *)(sharedMem + 0x234) = 5000 * 200;
	
	}
	return true;
}

bool PruProxy::UpdateOutput()
{
	if(Enabled)
	{
		*(unsigned long *)(sharedMem + 0x240) = Output1;
		*(unsigned long *)(sharedMem + 0x250) = Output2;
		*(unsigned long *)(sharedMem + 0x220) = Output3;
		*(unsigned long *)(sharedMem + 0x230) = Output4;
	
	}
	return true;
}

// load Image file to Pru0, the image will not run until ResetPru0 is called
bool PruProxy::LoadImageToPru0(char * filename)
{
	if(Enabled)
	{
		if(!ResetPru0())
		{
			cout << "fail to reset pru0 " << endl;
			return false;
		}
		return LoadImage(0x4a334000, filename);	// 0x4a334000 is address of instruction in Pru0
	}
	return true;
}

// load Image file to Pru1, the image will not run until ResetPru1 is called
bool PruProxy::LoadImageToPru1(char * filename)
{
	if(Enabled)
	{
		if(!ResetPru1())
		{
			cout << "Fail to reset Pru1 " << endl;
			return false;
		}
		return LoadImage(0x4a338000, filename);	// 0x4a334000 is address of instruction in Pru0
	}
	return true;
}

// Run image on Pru0
bool PruProxy::RunPru0()
{
	return WriteUInt32(0x4a322000, 0xa);
}

// Run Image to Pru1
bool PruProxy::RunPru1()
{
	return WriteUInt32(0x4a324000, 0xa);
}

// Reset Pru0,the image won't run
bool PruProxy::ResetPru0()
{
	printf("Reset Pru0\n");
	return WriteUInt32(0x4a322000, 0x10a);
}

// Reset Pru1, the image wont' run
bool PruProxy::ResetPru1()
{
	return WriteUInt32(0x4a324000, 0x10a);
}


// write a unsigned long to memory
bool PruProxy::WriteUInt32(unsigned long addr, unsigned long data)
{
	if(Enabled)
	{
		/* Initialize example */
		/* map the shared memory */
		void * pMem = mmap(0, 0x2000, PROT_WRITE | PROT_READ, MAP_SHARED, mem_fd, addr);
		if (pMem == NULL) 
		{
			printf("Failed to map the device (%s)\n", strerror(errno));
			close(mem_fd);
			return false;
		}
		*(unsigned long *)(pMem) = data;
		cout << "write data to memory" << endl;
		munmap(pMem, 0x2000);	
		return true;
	
	}
	return true;
}

// read a unsigned long from memroy
bool PruProxy::ReadUInt32(unsigned long addr, unsigned long & data)
{
	if(Enabled)
	{
		/* Initialize example */
		/* map the shared memory */
		void * pMem = mmap(0, 0x4, PROT_WRITE | PROT_READ, MAP_SHARED, mem_fd, addr);
		if (pMem == NULL) 
		{
			printf("Failed to map the device (%s)\n", strerror(errno));
			close(mem_fd);
			return false;
		}
		data = *(unsigned long *)(pMem);
		cout << "read data to memory" << endl;
		munmap(pMem, 0x4);	
		return true;
	}
	return true;
}

// load image file
bool PruProxy::LoadImage(unsigned long addr, char * filename)
{
	if(Enabled)
	{
		FILE *fPtr;
		
		// Open an File from the hard drive
		fPtr = fopen(filename, "rb");
		if (fPtr == NULL) {
			printf("Image File %s open failed\n", filename);
		} else {
			printf("Image File %s open passed\n", filename);
		}
		// Read file size
		fseek(fPtr, 0, SEEK_END);
		// read file
		unsigned char fileDataArray[PRUSS_MAX_IRAM_SIZE];
		int fileSize = 0;		
		fileSize = ftell(fPtr);

		if (fileSize == 0) {
			printf("File read failed.. Closing program\n");
			fclose(fPtr);
			return -1;
		}

		fseek(fPtr, 0, SEEK_SET);

		if (fileSize !=
			fread((unsigned char *) fileDataArray, 1, fileSize, fPtr)) {
			printf("WARNING: File Size mismatch\n");
		}	
		fclose(fPtr);
		/* Initialize example */
		/* map the shared memory */
		void * pMem = mmap(0, 0x2000, PROT_WRITE | PROT_READ, MAP_SHARED, mem_fd, addr);
		if (pMem == NULL) {
			printf("Failed to map the device (%s)\n", strerror(errno));
			close(mem_fd);
			return false;
		}
		char * p = (char*)pMem;
		for(int i = 0; i < fileSize; i ++)
		{
			*(p + i) = fileDataArray[i];
		}
		cout << "write file to memory" << endl;
		munmap(pMem, 0x2000);	
		return true;
	
	}
	return true;
}
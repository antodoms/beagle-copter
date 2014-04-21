#ifndef __PRU_PROXY_HH
#define __PRU_PROXY_HH
//PruProxy.h
class PruProxy
{
public :
	unsigned long Input1, Input2, Input3, Input4, Input5, Input6;
	unsigned long Output1, Output2, Output3, Output4;
	PruProxy();
	~PruProxy();
	// disable Pru, function for debug program without getting bus error
	void DisablePru();
	// init the Pru
	bool Init();
	// update input, call this method before using Input1 to Input4
	bool UpdateInput();
	// Init the output pulse width and set low value
	bool InitOutput();	
	// update output, call this method after updating Output1 to Output4
	bool UpdateOutput();
	// load Image file to Pru0, the image will not run until ResetPru0 is called
	bool LoadImageToPru0(char * filename);
	// load Image file to Pru1, the image will not run until ResetPru1 is called
	bool LoadImageToPru1(char * filename);
	// Run image on Pru0
	bool RunPru0();
	// Run Image to Pru1
	bool RunPru1();
	// Reset Pru0,the image won't run
	bool ResetPru0();
	// Reset Pru1, the image wont' run
	bool ResetPru1();
	// write a unsigned long to memory
	bool WriteUInt32(unsigned long addr, unsigned long data);
	// read a unsigned long from memroy
	bool ReadUInt32(unsigned long addr, unsigned long & data);
private:
	int mem_fd;
	void *sharedMem;

	bool Enabled;	// flag for if the pru is enabled 
	// load image file
	bool LoadImage(unsigned long addr, char * filename);
	
};
#endif
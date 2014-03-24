//=============================================================================
// File: output2.p 
// Desc: Create PWM output in PRU
// Vers: 1.1
//

#define IEP_COUNT_ADDR	0x2e00c
#define	IEP_CFG_ADDR	0x2e000
#define	IEP_CFG_VALUE	0x111

//#define PRU0
#define PRU1

.origin 0
.entrypoint PWM_OUT_INIT

// put iep count value into reg
.macro	GetIepCount
.mparam	reg
	// copy the current timer counter
	MOV R8, IEP_COUNT_ADDR	
	LBBO reg, R8, 0, 4	// copy content of ecap r0
.endm

.macro CreatePulse
.mparam		dataOffset, bitNum
	MOV R8, dataOffset 
	LBBO R4, R8, 0, 0x04	// load data to register
	MOV R10, dataOffset + 0x10000
	LBBO R5, R10, 0, 8		// load pulse width and frame width
	SUB R1, R2, R4			// calculate the elpse time and put it to r1
	QBGE	SET_LOW, R5, R1
SET_HIGH:
	SET R30, bitNum
	JMP SET_END
SET_LOW:
	CLR R30, bitNum
	QBLT	SET_END, R6, R1
UPDATE_START_TIME:
	MOV R4, R2
	SET R30, bitNum
	// save data back
	SBBO R4, R8, 0, 0x4		// only save start time and pulse and frame width
SET_END:
	
.endm

// R1	time elpse
// R2	curent time
// R3
// R4	start time
// R5	pulse width
// R6	frame width
// R7
// R8	temp for addrss
// R9	status of output pin

PWM_OUT_INIT:
	// start ecap
	MOV R8, IEP_CFG_ADDR		// copy ecap config register address
	MOV R2, IEP_CFG_VALUE		// copy the same value to current frame Reg
	SBBO R2, R8, 0, 4			// set register of ECCTL2, the timer will start now	
	GetIepCount R2
	MOV R8, 0x00				// save start time to first channel
	SBBO R2, R8, 0, 4
	// copy the current timer counter
PWM_OUT_INTERVAL:
	// copy the current timer counter
	GetIepCount R2
#ifdef PRU0

	CreatePulse 0x200, 7		//	1st channel			PRU0_R30_7		P9-25
	CreatePulse 0x210, 5		//	2nd channel			PRU0_R30_5		P9-27
	CreatePulse 0x220, 3		//	3rd channel			PRU0_R30_3		P9-28
	CreatePulse 0x230, 1		//	4th channel			PRU0_R30_1		P9-29
	CreatePulse 0x240, 2		//	5th channel			PRU0_R30_2		P9-30
	CreatePulse 0x250, 0		//	6th channel			PRU0_R30_0		P9-31
	
#endif

#ifdef PRU1
	CreatePulse	0x200, 13		// 1st channel			//PRU0_R30_13	P8-20
	CreatePulse	0x210, 12		// 2nd channel			//PRU0_R30_12	P8-21
	CreatePulse	0x220, 8		// 3rd channel			//PRU0_R30_8	P8-27
	CreatePulse	0x230, 10		// 4th channel			//PRU0_R30_10	P8-28
	CreatePulse	0x240, 9		// 5th channel			//PRU0_R30_9	P8-29
	CreatePulse	0x250, 11		// 6th channel			//PRU0_R30_11	P8-30
	CreatePulse	0x260, 6		// 7th channel			//PRU0_R30_6	P8-39
	CreatePulse	0x270, 7		// 8th channel			//PRU0_R30_7	P8-40
	CreatePulse	0x280, 4		// 9th channel			//PRU0_R30_4	P8-41		
	CreatePulse	0x290, 5		// 10th channel			//PRU0_R30_5	P8-42		
	CreatePulse	0x2A0, 2		// 11th channel			//PRU0_R30_2	P8-43		
	CreatePulse	0x2B0, 3		// 12th channel			//PRU0_R30_3	P8-44		
	CreatePulse	0x2C0, 0		// 13th channel			//PRU0_R30_0	P8-45		
	CreatePulse	0x2D0, 1		// 14th channel			//PRU0_R30_1	P8-46	
#endif	
	// update R30
	//MOV R30, R9
	JMP PWM_OUT_INTERVAL
	HALT

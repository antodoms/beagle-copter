// control.cpp

#include <math.h>
#include <vector>
#include <string.h>
#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <time.h>
#include <pthread.h>
#include <stdint.h>

using namespace std;

#include "MPU6050_6Axis_MotionApps20.h"
#include "pid.h"

// class default I2C address is 0x68
// specific I2C addresses may be passed as a parameter here
// AD0 low = 0x68 (default for SparkFun breakout and InvenSense evaluation board)
// AD0 high = 0x69
MPU6050 mpu;

#include "PruProxy.h"


// ******************
// rc functions
// ******************
#define MINCHECK 1050
#define MAXCHECK 1950

//Sensor sensor;
static PruProxy Pru;
 
// Radio min/max values for each stick for my radio (worked out at beginning of article)
#define RC_THR_MIN   1070
#define RC_YAW_MIN   1068
#define RC_YAW_MAX   1915
#define RC_PIT_MIN   1077
#define RC_PIT_MAX   1915
#define RC_ROL_MIN   1090
#define RC_ROL_MAX   1913

// Motor numbers definitions
#define MOTOR_FL   2    // Front left    
#define MOTOR_FR   0    // Front right
#define MOTOR_BL   1    // back left
#define MOTOR_BR   3    // back right


// MPU control/status vars
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer
int16_t g[3];
float gyro[3];
// orientation/motion vars
Quaternion q;           // [w, x, y, z]         quaternion container
VectorInt16 aa;         // [x, y, z]            accel sensor measurements
VectorInt16 aaReal;     // [x, y, z]            gravity-free accel sensor measurements
VectorInt16 aaWorld;    // [x, y, z]            world-frame accel sensor measurements
VectorFloat gravity;    // [x, y, z]            gravity vector
float euler[3];         // [psi, theta, phi]    Euler angle container
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector

// packet structure for InvenSense teapot demo
uint8_t teapotPacket[14] = { '$', 0x02, 0,0, 0,0, 0,0, 0,0, 0x00, 0x00, '\r', '\n' };



///////////////////////////////////

// Arduino map function
long map(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

#define wrap_180(x) (x < -180 ? x+360 : (x > 180 ? x - 360: x))
#define ToDeg(x) ((x)*57.2957795131) // *180/pi
#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))

// PID array (6 pids, two for each axis)
PID pids[6];
#define PID_PITCH_RATE 0
#define PID_ROLL_RATE 1
#define PID_PITCH_STAB 2
#define PID_ROLL_STAB 3
#define PID_YAW_RATE 4
#define PID_YAW_STAB 5


///////////////////////////////////////

// ================================================================
// ===                      INITIAL SETUP                       ===
// ================================================================

void setup() 
{

// PID Configuration

 pids[PID_PITCH_RATE].set_Kpid(0.5,0.003,0.09);
  pids[PID_ROLL_RATE].set_Kpid(0.5,0.003,0.09);
  //pids[PID_YAW_RATE].set_Kpid(2.7,1,0);
  
  pids[PID_PITCH_STAB].set_Kpid(6.5,0.1,1.2);
  pids[PID_ROLL_STAB].set_Kpid(6.5,0.1,1.2);
  //pids[PID_YAW_STAB].set_Kpid(10,0,0);

/*
  pids[PID_PITCH_RATE].set_Kpid(0.7,1,0);
  pids[PID_ROLL_RATE].set_Kpid(0.7,1,0);
  pids[PID_YAW_RATE].set_Kpid(2.7,1,0);
  
  pids[PID_PITCH_STAB].set_Kpid(4.5,0,0);
  pids[PID_ROLL_STAB].set_Kpid(4.5,0,0);
  pids[PID_YAW_STAB].set_Kpid(10,0,0);
*/
////////////////////////////////// MPU INIT ////////////////////////////////////////

 // initialize device
    printf("Initializing I2C devices...\n");
    mpu.initialize();

    // verify connection
    printf("Testing device connections...\n");
    printf(mpu.testConnection() ? "MPU6050 connection successful\n" : "MPU6050 connection failed\n");

    // load and configure the DMP
    printf("Initializing DMP...\n");
    devStatus = mpu.dmpInitialize();
    
    // make sure it worked (returns 0 if so)
    if (devStatus == 0) {
        // turn on the DMP, now that it's ready
        printf("Enabling DMP...\n");
        mpu.setDMPEnabled(true);

        // enable Arduino interrupt detection
        //Serial.println(F("Enabling interrupt detection (Arduino external interrupt 0)..."));
        //attachInterrupt(0, dmpDataReady, RISING);
        mpuIntStatus = mpu.getIntStatus();

        // set our DMP Ready flag so the main loop() function knows it's okay to use it
        printf("DMP ready!\n");
        dmpReady = true;

        // get expected DMP packet size for later comparison
        packetSize = mpu.dmpGetFIFOPacketSize();
    } else {
        // ERROR!
        // 1 = initial memory load failed
        // 2 = DMP configuration updates failed
        // (if it's going to break, usually the code will be 1)
        printf("DMP Initialization failed (code %d)\n", devStatus);
    }
	
/////////////////////////MPU OVER /////////////////////////////////
}





// ******** Main Loop *********
void loop () 
{
	
 long rcthr, rcyaw, rcpit, rcroll,yaw, pitch, roll, gyroYaw, gyroPitch, gyroRoll;  // Variables to store radio in
  static float yaw_target = 0; 
 
  uint16_t channels[4];
   float motor[4];

	if(Pru.UpdateInput())
	{
		// #define ROLL       0
		// #define PITCH      1
		// #define YAW        2
		// #define THROTTLE   3
		// #define AUX1       4
		// #define AUX2       5
		// #define AUX3       6
		// #define AUX4       7	
	
		
		channels[2] = Pru.Input3;
		channels[3] = Pru.Input4;  //4
		channels[1] = Pru.Input1; // 1
		channels[0] = Pru.Input2;	//2
		//cout << "C1: " << rcData[0]<< " C2:" << rcData[1] << " C3:" << rcData[2] << " C4:" << rcData[3] << endl;
	}
	else
	{
		cout << "Pru failed update" << endl;
	}

  rcthr = channels[2];
  rcyaw = map(channels[0], RC_YAW_MIN, RC_YAW_MAX, -180, 180)+10;
  rcpit =  -map(channels[3], RC_PIT_MIN, RC_PIT_MAX, -45, 45)-11;
  rcroll = -map(channels[1], RC_ROL_MIN, RC_ROL_MAX, -45, 45)-7;
   
  //cout<< " rc-> " << rcthr << " " << rcyaw << " " << rcpit <<" "<< rcroll<<endl;
////////////////////////////////////// MPU START ///////////////////////////////////////

    if (!dmpReady) return;

  // wait for FIFO count > 42 bits
  do {
  fifoCount = mpu.getFIFOCount();
  }while (fifoCount<42);

  if (fifoCount == 1024) {
    // reset so we can continue cleanly
    mpu.resetFIFO();
    printf("FIFO overflow!\n");

    // otherwise, check for DMP data ready interrupt
    //(this should happen frequently)
  } else  {
    //read packet from fifo
    mpu.getFIFOBytes(fifoBuffer, packetSize);

    mpu.dmpGetQuaternion(&q, fifoBuffer);
    mpu.dmpGetGravity(&gravity, &q);
    mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);

    //scaling for degrees output
    for (int i=0;i<3;i++){
      ypr[i]*=180/M_PI;
    }

yaw=ypr[0]; pitch=ypr[1];  roll=ypr[2];

//cout<< " ypr-> " << yaw << " " << pitch <<" "<< roll<<endl;

    mpu.dmpGetGyro(g, fifoBuffer);

    //0=gyroX, 1=gyroY, 2=gyroZ
    //swapped to match Yaw,Pitch,Roll
    //Scaled from deg/s to get tr/s
     for (int i=0;i<3;i++){
       gyro[i]   = ToDeg(g[3-i-1]);
     }
gyroYaw=-gyro[0];   gyroPitch=-gyro[1]; gyroRoll=gyro[2];
//cout<< " Gyroypr-> " << gyroYaw << " " << gyroPitch <<" "<< gyroRoll<<endl;
  }

/////////////////////////// MPU ENDS ///////////////////////////////////


// Do the magic
  if(rcthr > RC_THR_MIN + 100) {  // Throttle raised, turn on stablisation.
    // Stablise PIDS
   float pitch_stab_output = constrain(pids[PID_PITCH_STAB].update_pid_std((float)rcpit, pitch, 1), -250, 250); 
    float roll_stab_output = constrain(pids[PID_ROLL_STAB].update_pid_std((float)rcroll, roll, 1), -250, 250);
    float yaw_stab_output = constrain(pids[PID_YAW_STAB].update_pid_std(wrap_180(yaw_target), wrap_180(yaw), 1), -360, 360);
  
    // is pilot asking for yaw change - if so feed directly to rate pid (overwriting yaw stab output)
    if(abs(rcyaw ) > 5) {
      yaw_stab_output = rcyaw;
      yaw_target = yaw;   // remember this yaw for when pilot stops
    }
    
    // rate PIDS
    long pitch_output =  (long) constrain(pids[PID_PITCH_RATE].update_pid_std(pitch_stab_output, gyroPitch, 1), - 500, 500);  
    long roll_output =  (long) constrain(pids[PID_ROLL_RATE].update_pid_std(roll_stab_output, gyroRoll, 1), -500, 500);  
    long yaw_output =  (long) constrain(pids[PID_YAW_RATE].update_pid_std(yaw_stab_output, gyroYaw, 1), -500, 500);  

    // mix pid outputs and send to the motors.

    motor[MOTOR_FL] = rcthr + roll_output + pitch_output - yaw_output;
    motor[MOTOR_BL] = rcthr + roll_output - pitch_output + yaw_output;
    motor[MOTOR_FR] = rcthr - roll_output + pitch_output + yaw_output;
    motor[MOTOR_BR] = rcthr - roll_output - pitch_output - yaw_output;

        Pru.Output1 = motor[MOTOR_BR]*200;
	Pru.Output2 = motor[MOTOR_FR]*200;
	Pru.Output3 = motor[MOTOR_BL]*200;
	Pru.Output4 = motor[MOTOR_FL]*200;
	Pru.UpdateOutput();

  } else {
    // motors off
   
        Pru.Output1 = 1000*200;
	Pru.Output2 = 1000*200;
	Pru.Output3 = 1000*200;
	Pru.Output4 = 1000*200;
	Pru.UpdateOutput();
       
    // reset yaw target so we maintain this on takeoff
    yaw_target = yaw;
    
    

  }
	
}


int main(int argc, char ** args)
{
	cout<<"start"<<endl;	
	
	cout << "setup " << endl;
	//	Pru.DisablePru(); 	// disable pru and keep programming running for debug
	if(Pru.Init())
	{
		cout << "Pru init success." << endl;
	}
	else
	{
		cout << "Pru init failed." << endl;
	}
    	
	setup();

	while(1)
	{
		loop();
		usleep(1);
	}
	//loop();
	
	return 0;
}

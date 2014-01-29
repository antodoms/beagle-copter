#! /usr/bin/python
################################################################################################################
# Enable PWM for Brushless DC Motor using Beaglebone Black Kernal 3.8.13 on Angstrom   			       #
#													       #
# Obtained help from  https://groups.google.com/forum/#!msg/beagleboard/alKf67dwMHI/b9W2igN6Lr4J               #
#													       #
# Brushless DC Motor ESC Python Library to Configure PINS for Quadcopter motors P8_13 , P9_14 , P9_21 , P9_42  #
################################################################################################################
######### Python Script Coded by ANTO DOMINIC ################## www.antodominic.com ###########################
################################################################################################################

from subprocess import call
import struct
import os

class ESC:
	def enable(self):

		"""
		enable() Function is used to Enable PWM for Beagle Bone Black for the PINS Mentioned below. Currently I am only 
		able to configure the below mentioned PINS but you can change the PINS according to your needs, since some of the
		PINS share the PWM channel only 1 from each that is sharing memory can be accesed at a time. 

		1) EHRPWM1A (P9_14) or EHRPWM1B (P9_16) or EHRPWM1B (P8_34) or EHRPWM1A (P8_36)  --> 4 sharing same memory
		2) EHRPWM0B (P9_21) or EHRPWM0A (P9_22) or EHRPWM0B (P9_29) or EHRPWM0A (P9_31)  --> 4 sharing same memory
		3) ECAPPWM0 (P9_42) or ECAPPWM2 (P9_28)                                          --> 2 sharing same memory
		4) EHRPWM2B (P8_13) or EHRPWM2A (P8_19) or EHRPWM2A (P8_45) or EHRPWM2B (P8_46)  --> 4 sharing same memory

		Infact in Total we can access 8 PWM channels at a time maximum, if you remove the HDMI driver from 
		/sys/devices/bone_capemgr.9/slots. If not you can access 7 PWM channels after editing and compiling the firmware.
		I am not going to do that here, but will tell you how, just incase you are trying to build Hexcopter or octocopter.

		But when we include any 2 from each category say EHRPWM1A (P9_14)and EHRPWM1B (P9_16) from category 1 then we will no
		longer be able to change the period of PWM signal since when both from same category are enabled they both share the 
		same memory for Period by you can change duty cycle for each P9_14 and P9_16.

		Now when we include any 1 from each category say EHRPWM1A (P9_14) then we wont have any trouble in changing any of the
		PWM values including Period of the PWM signal. But when we Once change the default values of period we will not be able
		to do anything if we add in capemgr.9 EHRPWM1B (P9_16). 

		The Above problem can be corrected by editing the default values of the PWM firmware of each PWM_PINS with A and B 
		derivatives. The fix for this can be found here : 

		I have not yet been able to get it worked from the tutorial mentioned above but it has worked for many. What it does is, 
		it changes the default period from 500000 to the one you mentioned but again the default values must be same for A and B
		PWM derivative.
 
		
		"""	
		LoadCapemgrLocation = "/sys/devices/bone_capemgr.9/slots"
		open(LoadCapemgrLocation, 'w').write("am33xx_pwm")
		open(LoadCapemgrLocation, 'w').write("bone_pwm_P8_13")
		path=self.getdirectory("pwm_test_P8_13")
		self.initpwmvalues(path)
		open(LoadCapemgrLocation, 'w').write("bone_pwm_P9_14")
		path=self.getdirectory("pwm_test_P9_14")
		self.initpwmvalues(path)
		open(LoadCapemgrLocation, 'w').write("bone_pwm_P9_21")
		path=self.getdirectory("pwm_test_P9_21")
		self.initpwmvalues(path)
		open(LoadCapemgrLocation, 'w').write("bone_pwm_P9_42")
		path=self.getdirectory("pwm_test_P9_42")
		self.initpwmvalues(path)
		
	
		# You can Exchange the values for pins you want to use for PWM. I have added
		# the rest of the pins as comented to show how everything works using Device 
		# tree concept.

		#open("/sys/devices/bone_capemgr.9/slots", 'w').write("bone_pwm_P9_16")
		#path=getdirectory("pwm_test_P9_16")
		#initpwmvalues(path)
		#open("/sys/devices/bone_capemgr.9/slots", 'w').write("bone_pwm_P8_19")
		#path=getdirectory("pwm_test_P8_19")
		#initpwmvalues(path)
		#open("/sys/devices/bone_capemgr.9/slots", 'w').write("bone_pwm_P9_28")
		#path=getdirectory("pwm_test_P9_28")
		#initpwmvalues(path)
		#open("/sys/devices/bone_capemgr.9/slots", 'w').write("bone_pwm_P9_29")
		#path=getdirectory("pwm_test_P9_29")
		#initpwmvalues(path)
		#open("/sys/devices/bone_capemgr.9/slots", 'w').write("bone_pwm_P9_31")
		#path=getdirectory("pwm_test_P9_31")
		#initpwmvalues(path)
		#open("/sys/devices/bone_capemgr.9/slots", 'w').write("bone_pwm_P9_22")
		#path=getdirectory("pwm_test_P9_22")
		#initpwmvalues(path)
		#open("/sys/devices/bone_capemgr.9/slots", 'w').write("bone_pwm_P9_28")
		#path=getdirectory("pwm_test_P9_28")
		#initpwmvalues(path)
		#open("/sys/devices/bone_capemgr.9/slots", 'w').write("bone_pwm_P9_21")
		#path=getdirectory("pwm_test_P9_21")
		#initpwmvalues(path)
		#open("/sys/devices/bone_capemgr.9/slots", 'w').write("bone_pwm_P8_34")
		#path=getdirectory("pwm_test_P8_34")
		#initpwmvalues(path)
		#open("/sys/devices/bone_capemgr.9/slots", 'w').write("bone_pwm_P8_36")
		#path=getdirectory("pwm_test_P8_36")
		#initpwmvalues(path)
		#open("/sys/devices/bone_capemgr.9/slots", 'w').write("bone_pwm_P8_45")
		#path=getdirectory("pwm_test_P8_45")
		#initpwmvalues(path)
		#open("/sys/devices/bone_capemgr.9/slots", 'w').write("bone_pwm_P8_46")
		#path=getdirectory("pwm_test_P8_46")
		#initpwmvalues(path)
	

	def getdirectory(self,pwm_test_name):
		"""
		getdirectory(pwm_test_pinname) function is used to search for send the pwm_test folder for 
		each pins and return the full directory link to the path variable in enable() function
		"""
		pwmdir="/sys/devices/ocp.3/"
		for file in os.listdir(pwmdir):
			if file.startswith(pwm_test_name):
				return pwmdir+file

	
	def initpwmvalues(self,path):
		"""
		initpwmvalues(path) will change the period of the PWM pin to 20000000 = 50 Hz ,
		duty to 1000000 = 1 millisecond (which is the least value to be given to esc since
		servo pwm values range from 1ms to 2ms.

		Also with the new update the polarity of the PWM is reversed to you need to change
		the polarity to 0.
		"""
		open(path+"/period", 'w').write("20000000")
		open(path+"/duty", 'w').write("1000000")
		open(path+"/polarity", 'w').write("0")
		open(path+"/run", 'w').write("1")

	def escspeed(self,pin,speed):
		"""
		ThrottleSpeed(pin,speed) this function is used to change the motor speed with pin number 
		same as Adafruit_BBIO library pins. eg : pin = P9_14
		"""
		path=self.getdirectory("pwm_test_"+str(pin))
		speed=1000000+speed*10000
		open(path+"/duty", 'w').write(str(speed)) 

	def clearesc(self):
		"""
		
		"""
		
		# print call(["modprobe","-r","pwm_test"])
		text=open("/sys/devices/bone_capemgr.9/slots", 'r').read()
		text=text.split(",")[1:][-1]
		print text
		



from time import sleep
import pwm #Used to enable PWM
#from pwm import ESC



esc = pwm.ESC()
"""esc.clearesc()
"""
#esc.enable()
#sleep(5)
esc.escspeed("P9_14",0)
esc.escspeed("P8_13",0)

"""
for x in range(20,80):
	esc.escspeed("P9_14",x)
	esc.escspeed("P8_13",x)
	sleep(0.5)



for x in range(20,80):
	esc.escspeed("P9_14",100-x)
	esc.escspeed("P8_13",100-x)
	sleep(0.5)
"""

import time


location ="/sys/class/gpio/gpio30/value"

text=open(location, 'r').read()
text=int(text)

while 1:
	i=50
	dump=0
	while (i!=0):
		while ( int(text) == 0 ):
			text=open(location, 'r').read()
	
		start= time.time()
		while ( int(text) == 1 ):
			text=open(location, 'r').read()
			#time.sleep(5/100000)
	
		elapsed = ( time.time() - start )
		dump=dump+elapsed*1000000
		#dump=int(dump)
		i=i-1
	speed=dump/50
	print (int(((speed/1000)-1)*1000))

	#time.sleep(0.5)

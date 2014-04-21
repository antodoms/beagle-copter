#all: 
#	g++ control.cpp dumpData.cpp itg3200.cpp PruProxy.cpp -o control

CC=g++
CFLAGS=-c -w
#CFLAGS=
LDFLAGS= 
SOURCES=control.cpp PruProxy.cpp I2Cdev.cpp MPU6050.cpp pid.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=controls

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS)  -lrt -lpthread -o $@ 

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@
	

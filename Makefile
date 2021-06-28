CC = gcc

all: liftBody liftSensor liftCtrl liftMng
	make clean

liftSensor: liftSensor.o sensor.o messageQueue.o
	${CC} $^ -o $@ 

liftSensor.o: liftSensor.c
	$(CC) -c $<
##################
liftBody: liftBody.o messageQueue.o commandQueue.o command.o
	${CC} $^ -o $@ 

liftBody.o: liftBody.c
	$(CC) -c $<
##################
liftCtrl: liftCtrl.o messageQueue.o command.o commandQueue.o request.o
	${CC} $^ -o $@ 

liftCtrl.o: liftCtrl.c
	$(CC) -c $<
############
liftMng: liftMng.o messageQueue.o request.o
	${CC} $^ -o $@ -pthread 
liftMng.o: liftMng.c
	$(CC) -pthread -c $<
################## LIBRARIES
sensor.o: lib/sensor.c 
	$(CC) -c $<
messageQueue.o: lib/messageQueue.c
	$(CC) -c $<
commandQueue.o: lib/commandQueue.c
	$(CC) -c $<
command.o: lib/command.c
	$(CC) -c $<
request.o: lib/request.c
	$(CC) -c $<

.PHONY: clean
clean:
	rm -f *.o
TARGET = server
OBJECTS = server.o mq.o

$(TARGET):$(OBJECTS)
	gcc -o server server.o mq.o -lpthread

server.o:server.c server.h
	gcc -c server.c

mq.o:mq.c mq.h PV.h
	gcc -c mq.c

clean:
	-rm $(OBJECTS) $(TARGET)

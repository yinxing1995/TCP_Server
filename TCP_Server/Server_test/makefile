OBJS = server.o ringbuffer_reusable.o dataprocessing.o

iot_server: $(OBJS)
	gcc $(OBJS) -o iot_server -lpthread
server.o: server.c ringbuffer_reusable.h dataprocessing.h
	gcc -c server.c -o server.o -lpthread
ringbuffer_reusable.o: ringbuffer_reusable.c ringbuffer_reusable.h
	gcc -c ringbuffer_reusable.c -o ringbuffer_reusable.o
dataproessing.o: dataprocessing.c dataprocessing.h ringbuffer_reusable.h
	gcc -c dataprocessing.c -o dataprocessing.o -lpthread
clean:
	rm -rf *.o iot_server

OBJS = server.o ringbuffer_reusable.o

iot_server: $(OBJS)
	gcc $(OBJS) -o iot_server -lpthread
server.o: server.c ringbuffer_reusable.h
	gcc -c server.c -o server.o -lpthread
ringbuffer_reusable.o: ringbuffer_reusable.c ringbuffer_reusable.h
	gcc -c ringbuffer_reusable.c -o ringbuffer_reusable.o

clean:
	rm -rf *.o iot_server

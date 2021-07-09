<<<<<<< HEAD
#ifndef RINGBUFFER_REUSABLE_H
#define RINGBUFFER_REUSABLE_H

#include <stdint.h>

typedef struct buffer
{
    uint8_t *position;
    uint16_t writepointer;
    uint16_t readpointer;
    uint16_t max;
}Ringbuf;

//Ringbuf Buffer;

Ringbuf *BufferInit(char *p, uint16_t size);
int BufferWrite(Ringbuf *, void *sour, uint16_t len);
int BufferRead(Ringbuf *,void *dest, uint16_t len);
int BufferSeek(Ringbuf *,void *dest, uint16_t len);
void BufferRelease(Ringbuf *);

=======
#ifndef RINGBUFFER_REUSABLE_H
#define RINGBUFFER_REUSABLE_H

#include <stdint.h>

typedef struct buffer
{
    uint8_t *position;
    uint16_t writepointer;
    uint16_t readpointer;
    uint16_t max;
}Ringbuf;

//Ringbuf Buffer;

Ringbuf *BufferInit(char *p, uint16_t size);
int BufferWrite(Ringbuf *, void *sour, uint16_t len);
int BufferRead(Ringbuf *,void *dest, uint16_t len);
int BufferSeek(Ringbuf *,void *dest, uint16_t len);
void BufferRelease(Ringbuf *);

>>>>>>> 0d9093f262b1f1dae9978811b686b828eb809365
#endif
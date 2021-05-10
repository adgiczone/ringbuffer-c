#ifndef _FIFO_H__
#define _FIFO_H__

struct kfifo
{
        unsigned int in;
        unsigned int out;
        unsigned int mask;
        unsigned int esize;
        void *data;
};

#define kfifo_size(fifo) ((fifo)->mask + 1)
#define kfifo_len(fifo) ({ (fifo)->in - (fifo)->out;})

int kfifo_alloc(struct kfifo *fifo, unsigned int size, size_t esize);

void kfifo_free(struct kfifo *fifo);

unsigned int kfifo_in(struct kfifo *fifo, const void *buf, unsigned int len);

unsigned int kfifo_out(struct kfifo *fifo, void *buf, unsigned int len);

int kfifo_unused(struct kfifo *fifo);

unsigned int kfifo_out_peek(struct kfifo *fifo, void *buf, unsigned int len);

#endif
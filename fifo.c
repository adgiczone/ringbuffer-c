/* referrence linux kfifo.c
 */

#include <stdlib.h>
#include <string.h>
#include "fifo.h"

#define min(x, y) ((x) < (y) ? (x) : (y))
#define is_power_of_2(x) ((x) != 0 && (((x) & ((x)-1)) == 0))

static unsigned int roundup_pow_of_two(unsigned int x)
{
        int position = 0;
        int i;

        for (i = (x - 1); i != 0; ++position)
                i >>= 1;

        return 1UL << position;
}

int kfifo_alloc(struct kfifo *fifo, unsigned int size, size_t esize)
{
        fifo->in = 0;
        fifo->out = 0;
        fifo->esize = esize;

        if (size < 2)
        {
                fifo->data = NULL;
                fifo->mask = 0;
                return -1;
        }

        if (!is_power_of_2(size))
                size = roundup_pow_of_two(size);

        fifo->data = calloc(size, esize);

        if (!fifo->data)
        {
                fifo->mask = 0;
                return -1;
        }
        fifo->mask = size - 1;

        return 0;
}

void kfifo_free(struct kfifo *fifo)
{
        free(fifo->data);
        fifo->in = 0;
        fifo->out = 0;
        fifo->esize = 0;
        fifo->data = NULL;
        fifo->mask = 0;
}

int kfifo_unused(struct kfifo *fifo)
{
        return (fifo->mask + 1) - (fifo->in - fifo->out);
}

static void kfifo_copy_in(struct kfifo *fifo, const void *src, unsigned int len, unsigned int off)
{
        unsigned int size = fifo->mask + 1;
        unsigned int esize = fifo->esize;
        unsigned int l;

        /* that is why request size is power of two, instead of off %= fifo->size */
        off &= fifo->mask;
        if (esize != 1)
        {
                off *= esize;
                size *= esize;
                len *= esize;
        }
        l = min(len, size - off);

        memcpy(fifo->data + off, src, l);
        memcpy(fifo->data, src + l, len - l);

        /* make sure that the data in the fifo is up to date before
         * incrementing the fifo->in index counter
         */
}

unsigned int kfifo_in(struct kfifo *fifo, const void *buf, unsigned int len)
{
        unsigned int l;

        l = kfifo_unused(fifo);
        if (len > l)
                len = l;

        kfifo_copy_in(fifo, buf, len, fifo->in);
        fifo->in += len;
        return len;
}

static void kfifo_copy_out(struct kfifo *fifo, void *dst, unsigned int len, unsigned int off)
{
        unsigned int size = fifo->mask + 1;
        unsigned int esize = fifo->esize;
        unsigned int l;

        off &= fifo->mask;
        if (esize != 1)
        {
                off *= esize;
                size *= esize;
                len *= esize;
        }
        l = min(len, size - off);

        memcpy(dst, fifo->data + off, l);
        memcpy(dst + l, fifo->data, len - l);

        /* make sure that the data is copied before
         * incrementing the fifo->out index counter
         */
}

unsigned int kfifo_out_peek(struct kfifo *fifo, void *buf, unsigned int len)
{
        unsigned int l;

        l = fifo->in - fifo->out;
        if (len > l)
                len = l;

        kfifo_copy_out(fifo, buf, len, fifo->out);
        return len;
}

unsigned int kfifo_out(struct kfifo *fifo, void *buf, unsigned int len)
{
        unsigned int l;

        l = fifo->in - fifo->out;
        if (len > l)
                len = l;

        kfifo_copy_out(fifo, buf, len, fifo->out);
        fifo->out += len;
        return len;
}



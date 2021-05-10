#include <stdio.h>
#include <pthread.h>
#include "fifo.h"

#define FIFO_SIZE 32

struct kfifo temp = {
    .in = 0,
    .out = 0,
    .mask = 0,
    .esize = 0,
    .data = NULL,
};

typedef struct _test_st {
    int b;
    char a;
} test_st;

static const test_st expected_result[FIFO_SIZE] = {
    {3, '3'},
    {4, '4'},
    {5, '5'},
    {6, '6'},
    {7, '7'},
    {8, '8'},
    {9, '9'},
    {0, '0'},
    {1, '1'},
    {20, 'a'},
    {21, 'b'},
    {22, 'c'},
    {23, 'd'},
    {24, 'e'},
    {25, 'f'},
    {26, 'g'},
    {27, 'h'},
    {28, 'i'},
    {29, 'j'},
    {30, 'k'},
    {31, 'l'},
    {32, 'm'},
    {33, 'n'},
    {34, 'o'},
    {35, 'p'},
    {36, 'q'},
    {37, 'r'},
    {38, 's'},
    {39, 't'},
    {40, 'u'},
    {41, 'v'},
    {42, 'w'},
};



int main(int argc, char *argv[])
{
    struct kfifo test;
    kfifo_alloc(&test, FIFO_SIZE, sizeof(test_st));
    printf("int fifo test start, size:%d\n", kfifo_size(&test));

    int buf[6];
    int i = 0, j = 0;
    unsigned int ret;
    /* put values into the fifo */
    for (i = 0; i != 32; i++)
    {
        if (!kfifo_unused(&test)) {
            printf("full %d %d\n", kfifo_len(&test), kfifo_unused(&test));
            break;
        }

        kfifo_in(&test, &expected_result[i], sizeof(expected_result[i]));
    }
    printf("out %d %d\n", kfifo_len(&test), kfifo_unused(&test));

    test_st temp = {0};
    for (i = 0; i < 10; i++) {
        ret = kfifo_out(&test, &temp, 1);
        printf("peek %d, %c %d, %d\n", temp.b, temp.a, ret, kfifo_unused(&test));
    }




    kfifo_free(&test);
}
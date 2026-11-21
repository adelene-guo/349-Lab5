/** @file main.c
 *
 *  @brief  Lab 5 Fall 2024
 */
#include <349_lib.h>
#include <349_peripheral.h>
#include <349_threads.h>
#include <stdio.h>

#define UNUSED __attribute__((unused))

int main(UNUSED int argc, UNUSED char const *argv[])
{

    while (1)
    {
        printf("Hello World from User : %s\n", argv[1]);
    }

    return RET_0349;
}

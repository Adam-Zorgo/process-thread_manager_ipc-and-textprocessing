#include <time.h>

double get_execution_time(void (*function)(void)) {
    clock_t start_time = clock();
    function();
    clock_t end_time = clock();
    return (double)(end_time - start_time) / CLOCKS_PER_SEC;
}
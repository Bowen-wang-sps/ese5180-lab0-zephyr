#include <zephyr/sys/printk.h>
#include "sum_printk.h"

void sum_printk(int a, int b, int result)
{
    printk("%d + %d = %d\n", a, b, result);
}
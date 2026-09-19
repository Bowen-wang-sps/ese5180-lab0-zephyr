#include <zephyr/logging/log.h>
#include "sum_log.h"

LOG_MODULE_REGISTER(sum_log);

void sum_log(int a, int b, int result)
{
    int inputs[2] = {a, b};

    LOG_INF("result: %d", result);
    LOG_WRN("warning");
    LOG_ERR("error");

    LOG_HEXDUMP_INF(inputs, sizeof(inputs), "inputs");
}
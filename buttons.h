#include "main.h"

extern TaskHandle_t task_s1_handle;
extern TaskHandle_t task_s2_handle;

void task_s1(void *pvParameters);
void task_s2(void *pvParameters);

void init_s1s2(void);

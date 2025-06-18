#include "bsp_common.h"
#include "common_api.h"
#include "soc_service.h"
#include "FreeRTOS.h"
#include "task.h"

#include <math.h>

void* task1_handle;
void* task2_handle;

// #define soc_printf(...)

static void hw_demoA_init(void)
{
    soc_printf("this hw demo1");
}

static void hw_demoB_init(void)
{
    soc_printf("this hw demo2");
}

static void dr_demoC_init(void)
{
    soc_printf("this dr demo1");
}

static void dr_demoD_init(void)
{
    soc_printf("this dr demo2");
}

static void task1(void *param)
{
    while(1)
    {
        vTaskDelay(1000);
        soc_printf("task1 loop");
    }
}

#ifdef STATIC_RECORDS
static TaskRunTimeRecord_t record[TASK_RECORD_MAX];
#endif

static void task2(void *param)
{
    StaticTask_t *handle;
    uint32_t per;

    while(1)
    {
        // 间隔 1s 打印 CPU 占用率
        vTaskDelay(1000);
        uint64_t now_time = soc_get_poweron_time_tick();
        #ifndef STATIC_RECORDS
        TaskRunTimeRecord_t *record = malloc(sizeof(TaskRunTimeRecord_t) * TASK_RECORD_MAX);
        #endif
        if(record)
        {
            vTaskSuspendAll();
            if(soc_task_record_get(record, TASK_RECORD_MAX))
            {
                xTaskResumeAll();
                for(int i = 0; i < TASK_RECORD_MAX; i++)
                {
                    if(record[i].tcb)
                    {
                        handle = (StaticTask_t *)record[i].tcb;
                        per = record[i].RunTime * 1000 / now_time;
                        if(0)
                        {
                            soc_printf("task %s \t ticks %llu \t cpu time percent %u‰", handle->ucDummy7, record[i].RunTime, per);
                        }
                        else
                        {
                            soc_printf("task %s \t cpu time percent %u‰", handle->ucDummy7, per, atan(2));
                        }
                    }
                }
            }
            else
            {
                xTaskResumeAll();
            }
            #ifndef STATIC_RECORDS
            free(record);
            #endif
        }
    }
}

static void task_demoE_init(void)
{
    task1_handle = create_event_task(task1, NULL, 2 * 1024, 50, 0, "task1");
}

static void task_demoF_init(void)
{
    soc_task_record_on_off(1);
    task2_handle = create_event_task(task2, NULL, 2 * 1024, 50, 0, "task2");
}

//启动hw_demoA_init，启动位置硬件初始1级
INIT_HW_EXPORT(hw_demoA_init, "1");
//启动hw_demoB_init，启动位置硬件初始2级
INIT_HW_EXPORT(hw_demoB_init, "2");
//启动dr_demoC_init，启动位置驱动1级
INIT_DRV_EXPORT(dr_demoC_init, "1");
//启动dr_demoD_init，启动位置驱动2级
INIT_DRV_EXPORT(dr_demoD_init, "2");
// 启动task_demoE_init，启动位置任务1级
INIT_TASK_EXPORT(task_demoE_init, "1");
//启动task_demoF_init，启动位置任务2级
INIT_TASK_EXPORT(task_demoF_init, "2");

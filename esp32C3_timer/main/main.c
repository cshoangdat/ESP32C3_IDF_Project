#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/timer.h"

#define TIMER_DIVIDER         16
#define TIMER_SCALE           TIMER_BASE_CLK / TIMER_DIVIDER  // 5Mhz

// typedef struct {
//     int timer_group;
//     int timer_idx;
//     int alarm_interval;
//     bool auto_reload;
// } timer_info_t;

// typedef struct {
//     timer_info_t info;
//     uint64_t timer_counter_value;
// } timer_event_t;

// static xQueueHandle s_timer_queue;

static void inline print_timer_counter(uint64_t counter_value)
{
    printf("Counter: 0x%08x%08x\r\n", (uint32_t) (counter_value >> 32),
           (uint32_t) (counter_value));
    printf("Time   : %.8f s\r\n", (double) counter_value / TIMER_SCALE);
}

// static void timer_init(int group, int timer, bool auto_reload, int timer_interval_sec)
// {
//     timer_config_t config = {
//         .divider = TIMER_DIVIDER,
//         .counter_dir = TIMER_COUNT_UP,
//         .counter_en = TIMER_PAUSE,
//         .alarm_en = TIMER_ALARM_EN,
//         .auto_reload = auto_reload,
//     };
//     timer_init(group, timer, &config);
//     timer_set_counter_value(group, timer, 0);
//     timer_set_alarm_value(group, timer, timer_interval_sec * TIMER_SCALE);
//     timer_enable_intr(group, timer);

//     example_timer_info_t *timer_info = calloc(1, sizeof(example_timer_info_t));
//     timer_info->timer_group = group;
//     timer_info->timer_idx = timer;
//     timer_info->auto_reload = auto_reload;
//     timer_info->alarm_interval = timer_interval_sec;
//     timer_isr_callback_add(group, timer, timer_group_isr_callback, timer_info, 0);

//     timer_start(group, timer);
// }


void app_main(void)
{
    timer_config_t config = {
        .divider = TIMER_DIVIDER,
        .counter_dir = TIMER_COUNT_UP,
        .counter_en = TIMER_START,
        .alarm_en = TIMER_ALARM_EN,
        .auto_reload = true
    };
    timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, 10*TIMER_SCALE);
    timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0);
    timer_init(TIMER_GROUP_0, TIMER_0, &config);
    uint64_t counter_value;
    while(1){
        timer_get_counter_value(TIMER_GROUP_0, TIMER_0, &counter_value);
        print_timer_counter(counter_value);
    }
}

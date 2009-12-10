#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

MODULE_AUTHOR("ROGER ORIOL GARCIA ALVAREZ & MIGUEL ANGEL VICO MOYA");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Intercept syscalls and calculate statistics");

#define proso_rdtsc(low,high) \
__asm__ __volatile__("rdtsc" : "=a" (low), "=d" (high))

static inline unsigned long long proso_get_cycles (void) {
  unsigned long eax, edx;

  proso_rdtsc(eax, edx);

  return ((unsigned long long) edx << 32) + eax;
}

#define SYSCALLS_MONITORIZED 5

/* Module Data */
struct statistics {
    int total_calls;
    int ok_calls;
    int error_calls;
    unsigned long long total_time;
};

struct thred_info_extended {
    struct thred_info *info;
    struct statistic stats[SYSCALLS_MONITORIZED];
};

/* Module Operations */


/* Module Initialization */
static int __init syscallStatsCalcMod_init(void) {
}

/* Module Download */
static void __exit syscallStatsCalcMod_exit(void) {
}

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/perf_event.h>
#include <asm/unistd.h>

static int fd;

static long
perf_event_open(struct perf_event_attr *hw_event, pid_t pid,
                int cpu, int group_fd, unsigned long flags) {
    int ret;
    ret = syscall(__NR_perf_event_open, hw_event, pid, cpu,
                  group_fd, flags);
    return ret;
}

void init_count() {
    struct perf_event_attr pe;

    memset(&pe, 0, sizeof(struct perf_event_attr));
    pe.type = PERF_TYPE_HARDWARE;
    pe.size = sizeof(struct perf_event_attr);
    pe.config = PERF_COUNT_HW_INSTRUCTIONS;
    pe.disabled = 1;
    pe.exclude_kernel = 1;
    pe.exclude_hv = 1;

    fd = perf_event_open(&pe, 0, -1, -1, 0);
    if (fd == -1) {
        fprintf(stderr,
                "Error opening leader %llx\n", pe.config);
        fprintf(stderr,
                "execute \"echo -1 | sudo tee /proc/sys/kernel/perf_event_paranoid\"\n");
        exit(EXIT_FAILURE);
    }
}

void start_count() {
    ioctl(fd, PERF_EVENT_IOC_RESET,  0);
    ioctl(fd, PERF_EVENT_IOC_ENABLE, 0);
}

void stop_count() {
    ioctl(fd, PERF_EVENT_IOC_DISABLE, 0);
}

void read_count(unsigned long long* count) {
    read(fd, count, sizeof(unsigned long long));
}

#define _DEFAULT_SOURCE
#include "../include/stats.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <sys/statvfs.h>

void stats_sample(SystemStats *stats) {
    static unsigned long long prev_idle = 0, prev_total = 0;

    //  CPU
    FILE *fp = fopen("/proc/stat", "r");
    if (fp) {
        unsigned long long user, nice, system, idle, iowait, irq, softirq, steal;
        int n = fscanf(fp, "cpu %llu %llu %llu %llu %llu %llu %llu %llu",
                        &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal);
        fclose(fp);

        if (n == 8) {
            unsigned long long total = user + nice + system + idle + iowait + irq + softirq + steal;
            if (prev_total > 0 && total > prev_total) {
                stats->cpu_usage = 100.0f * (1.0f - (float)(idle - prev_idle) / (float)(total - prev_total));
            }
            prev_idle = idle;
            prev_total = total;
        }
    }

    //  Memory
    struct sysinfo info;
    if (sysinfo(&info) == 0) {
        stats->memory_usage = 100.0f * (1.0f - (float)info.freeram / (float)info.totalram);
        stats->uptime = info.uptime;
    }

    //  Disk (root filesystem)
    struct statvfs vfs;
    if (statvfs("/", &vfs) == 0) {
        unsigned long long total_blocks = vfs.f_blocks;
        unsigned long long free_blocks  = vfs.f_bfree;
        if (total_blocks > 0) {
            stats->disk_usage = 100.0f * (1.0f - (float)free_blocks / (float)total_blocks);
        }
    }

    //  Hostname
    gethostname(stats->hostname, sizeof(stats->hostname));
}

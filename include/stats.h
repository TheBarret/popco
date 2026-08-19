#ifndef STATS_H
#define STATS_H

typedef struct {
    float cpu_usage;     // 0-100
    float memory_usage;  // 0-100
    float disk_usage;    // 0-100, root filesystem
    long  uptime;        // seconds
    char  hostname[256];
} SystemStats;

// Samples /proc/stat, sysinfo(), statvfs("/") and gethostname() into stats.
// Safe to call repeatedly; CPU usage is a delta since the previous call.
void stats_sample(SystemStats *stats);

#endif

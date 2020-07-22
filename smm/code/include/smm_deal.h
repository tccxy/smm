/**
 * @file smm_deal.h
 * @author zhao.wei (hw)
 * @brief smm处理函数集头文件
 * @version 0.1
 * @date 2020-07-17
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef _SMM_DEAL_H_
#define _SMM_DEAL_H_

#define FS_STAT "/proc/stat"
#define FS_PID_STAT "/proc/%u/stat"
#define FS_PID_IO "/proc/%u/io"
#define FS_MEMINFO "/proc/meminfo"

extern unsigned int kb_shift;

/*
 * kB <-> number of pages.
 * Page size depends on machine architecture (4 kB, 8 kB, 16 kB, 64 kB...)
 */
#define KB_TO_PG(k)	((k) >> kb_shift)
#define PG_TO_KB(k)	((k) << kb_shift)
/**
 * @brief cpu状态信息
 * 
 */
struct smm_cpu_mem_stat
{
    u64 cpu_user;
    u64 cpu_nice;
    u64 cpu_kernel;
    u64 cpu_idle;
    u64 cpu_iowait;
    u64 cpu_irq;
    u64 cpu_softirq;
    u64 cpu_stealstolen;
    u64 cpu_gust;
    u64 mem_total;
    u64 mem_free;
    u64 mem_buffer;
    u64 mem_cache;
    u64 mem_sreclaimable;

    u64 pid_utime;
    u64 pid_stime;
    u64 pid_cutime;
    u64 pid_cstime;
    u64 pid_rss;
};



#define total_stat(stat)                                                  \
    stat->cpu_user + stat->cpu_nice + stat->cpu_kernel + stat->cpu_idle + \
        stat->cpu_iowait + stat->cpu_irq + stat->cpu_softirq + stat->cpu_stealstolen

void cpu_stat_update(u32 pid, struct smm_cpu_mem_stat *stat, struct smm_contrl *contrl);
void cpu_ratio(u32 pid, struct smm_contrl *contrl, int index, void *data);
void cpu_usr_ratio(u32 pid, struct smm_contrl *contrl, int index, void *data);
void cpu_kernel_ratio(u32 pid, struct smm_contrl *contrl, int index, void *data);
void cpu_si_ratio(u32 pid, struct smm_contrl *contrl, int index, void *data);
void mem_ratio(u32 pid, struct smm_contrl *contrl, int index, void *data);
void mem_cache(u32 pid, struct smm_contrl *contrl, int index, void *data);
void mem_buffer(u32 pid, struct smm_contrl *contrl, int index, void *data);
//void pid_stat_updata(u32 pid, struct smm_pid_stat *stat);
void pid_cpu_ratio(u32 pid, struct smm_contrl *contrl, int index, void *data);
void pid_cpu_usr_ratio(u32 pid, struct smm_contrl *contrl, int index, void *data);
void pid_cpu_kernel_ratio(u32 pid, struct smm_contrl *contrl, int index, void *data);
void pid_mem_ratio(u32 pid, struct smm_contrl *contrl, int index, void *data);
void pid_net_sd_rate(u32 pid, struct smm_contrl *contrl, int index, void *data);
void pid_net_rc_rate(u32 pid, struct smm_contrl *contrl, int index, void *data);
void smm_deal_result(struct smm_contrl *contrl);

#endif
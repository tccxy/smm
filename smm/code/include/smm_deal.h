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

/**
 * @brief cpu状态信息
 * 
 */
struct smm_cpu_mem_stat
{
    u32 cpu_user;
    u32 cpu_nice;
    u32 cpu_kernel;
    u32 cpu_idle;
    u32 cpu_iowait;
    u32 cpu_irq;
    u32 cpu_softirq;
    u32 cpu_stealstolen;
    u32 cpu_gust;
    u64 mem_total;
    u64 mem_free;
    u32 mem_buffer;
    u32 mem_cache;
    u32 mem_sreclaimable;
};


/**
 * @brief 进程状态信息
 * 
 */
struct smm_pid_stat
{
};

#define total_stat(stat)                                                 \
    stat->cpu_user + stat->cpu_nice + stat->cpu_kernel + stat->cpu_idle + \
    stat->cpu_iowait + stat->cpu_irq + stat->cpu_softirq + stat->cpu_stealstolen

void cpu_stat_update(struct smm_cpu_mem_stat *stat);
void cpu_ratio(u32 pid, struct smm_contrl *contrl, int type, void *data);
void cpu_usr_ratio(u32 pid, struct smm_contrl *contrl, int type, void *data);
void cpu_kernel_ratio(u32 pid, struct smm_contrl *contrl, int type, void *data);
void cpu_si_ratio(u32 pid, struct smm_contrl *contrl, int type, void *data);
void mem_ratio(u32 pid, struct smm_contrl *contrl, int type, void *data);
void mem_cache(u32 pid, struct smm_contrl *contrl, int type, void *data);
void mem_buffer(u32 pid, struct smm_contrl *contrl, int type, void *data);
void pid_stat_updata(u32 pid, struct smm_pid_stat *stat);
void pid_cpu_ratio(u32 pid, struct smm_contrl *contrl, int type, void *data);
void pid_cpu_index(u32 pid, struct smm_contrl *contrl, int type, void *data);
void pid_mem_ratio(u32 pid, struct smm_contrl *contrl, int type, void *data);
void pid_mem_rss(u32 pid, struct smm_contrl *contrl, int type, void *data);
void pid_mem_vir(u32 pid, struct smm_contrl *contrl, int type, void *data);
void pid_io_rd_ratio(u32 pid, struct smm_contrl *contrl, int type, void *data);
void pid_io_wr_ratio(u32 pid, struct smm_contrl *contrl, int type, void *data);
void pid_net_sd_rate(u32 pid, struct smm_contrl *contrl, int type, void *data);
void pid_net_rc_rate(u32 pid, struct smm_contrl *contrl, int type, void *data);
void smm_deal_result(struct smm_contrl *contrl);

#endif
/**
 * @file smm_deal.c
 * @author zhao.wei (hw)
 * @brief smm实体处理函数集
 * @version 0.1
 * @date 2020-07-17
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include "pub.h"

/**
 * @brief 获取cpu的stat信息
 * 
 * @param stat 状态信息
 * @return u32 
 */
static u32 cpu_stat_get(struct smm_cpu_mem_stat *stat)
{
    FILE *pfile = NULL;
    char line[128];
    //cpu
    pfile = fopen(FS_STAT, "r");

    fscanf(pfile, "cpu %d %d %d %d %d %d %d %d", &stat->cpu_user,
           &stat->cpu_nice, &stat->cpu_kernel, &stat->cpu_idle,
           &stat->cpu_iowait, &stat->cpu_irq, &stat->cpu_softirq,
           &stat->cpu_stealstolen);

    fclose(pfile);

    //mem
    pfile = fopen(FS_MEMINFO, "r");
    while (fgets(line, sizeof(line), pfile) != NULL)
    {
        if (!strncmp(line, "MemTotal:", 9))
        {
            /* Read the total amount of memory in kB */
            sscanf(line + 9, "%llu", &stat->mem_total);
        }
        else if (!strncmp(line, "MemFree:", 8))
        {
            /* Read the amount of free memory in kB */
            sscanf(line + 8, "%llu", &stat->mem_free);
        }
        else if (!strncmp(line, "Buffers:", 8))
        {
            /* Read the amount of buffered memory in kB */
            sscanf(line + 8, "%llu", &stat->mem_buffer);
        }
        else if (!strncmp(line, "Cached:", 7))
        {
            /* Read the amount of cached memory in kB */
            sscanf(line + 7, "%llu", &stat->mem_cache);
        }
        else if (!strncmp(line, "SReclaimable:", 13))
        {
            sscanf(line + 7, "%llu", &stat->mem_sreclaimable);
        }
    }
    fclose(pfile);
    return SUCCESS;
}

/**
 * @brief 更新CPU 信息
 * 
 * @param stat 状态信息
 */
void cpu_stat_update(struct smm_cpu_mem_stat *stat)
{
    struct smm_cpu_mem_stat *stat_pre = NULL;
    struct smm_cpu_mem_stat *stat_cur = NULL;

    stat_pre = stat;
    stat_cur = stat_pre + 1;

    cpu_stat_get(stat_pre);
    usleep(SAMPLE_MS * 1000);
    cpu_stat_get(stat_cur);
}
/**
 * @brief cpu利用率
 * 
 * @param pid pid
 * @param contrl 控制消息数据结构
 * @param type 类型
 * @param data 
 */
void cpu_ratio(u32 pid, struct smm_contrl *contrl, int type, void *data)
{
    u64 total_pre, total_cur;
    u32 cpu_idle, total;
    struct smm_cpu_mem_stat *stat_pre = (struct smm_cpu_mem_stat *)data;
    struct smm_cpu_mem_stat *stat_cur = stat_pre + 1;

    total_pre = total_stat(stat_pre);

    total_cur = total_stat(stat_cur);

    total = total_cur - total_pre;

    cpu_idle = stat_cur->cpu_idle - stat_pre->cpu_idle;

    zlog_debug(zc, "total %d  cpu_idle %d\r\n", total, cpu_idle);
    contrl->result.r_cpu_ratio = 100.f * (total - cpu_idle) / total;
    zlog_debug(zc, "contrl->result.r_cpu_ratio is %.2f \r\n",
               contrl->result.r_cpu_ratio);
}

/**
 * @brief cpu用户态占比
 * 
 * @param pid pid
 * @param contrl 控制消息数据结构
 * @param type 类型
 * @param data 
 */
void cpu_usr_ratio(u32 pid, struct smm_contrl *contrl, int type, void *data)
{
    u64 total_pre, total_cur;
    u32 usr, total;
    struct smm_cpu_mem_stat *stat_pre = (struct smm_cpu_mem_stat *)data;
    struct smm_cpu_mem_stat *stat_cur = stat_pre + 1;

    total_pre = total_stat(stat_pre);

    total_cur = total_stat(stat_cur);

    total = total_cur - total_pre;
    usr = stat_cur->cpu_user - stat_pre->cpu_user;

    zlog_debug(zc, "total %d  usr %d\r\n", total, usr);
    contrl->result.r_cpu_usr_ratio = 100.f * (usr) / total;
    zlog_debug(zc, "contrl->result.r_cpu_usr_ratio is %.2f \r\n",
               contrl->result.r_cpu_usr_ratio);
}

/**
 * @brief cpu内核态占比
 * 
 * @param pid pid
 * @param contrl 控制消息数据结构
 * @param type 类型
 * @param data 
 */
void cpu_kernel_ratio(u32 pid, struct smm_contrl *contrl, int type, void *data)
{
    u64 total_pre, total_cur;
    u32 cpu_kernel, total;
    struct smm_cpu_mem_stat *stat_pre = (struct smm_cpu_mem_stat *)data;
    struct smm_cpu_mem_stat *stat_cur = stat_pre + 1;

    total_pre = total_stat(stat_pre);

    total_cur = total_stat(stat_cur);

    total = total_cur - total_pre;
    cpu_kernel = stat_cur->cpu_kernel - stat_pre->cpu_kernel;

    zlog_debug(zc, "total %d  cpu_kernel %d\r\n", total, cpu_kernel);
    contrl->result.r_cpu_kernel_ratio = 100.f * (cpu_kernel) / total;
    zlog_debug(zc, "contrl->result.r_cpu_kernel_ratio is %.2f \r\n",
               contrl->result.r_cpu_kernel_ratio);
}

/**
 * @brief 软中断的cpu占比
 * 
 * @param pid pid
 * @param contrl 控制消息数据结构
 * @param type 类型
 * @param data 
 */
void cpu_si_ratio(u32 pid, struct smm_contrl *contrl, int type, void *data)
{
    u64 total_pre, total_cur;
    u32 si, total;
    struct smm_cpu_mem_stat *stat_pre = (struct smm_cpu_mem_stat *)data;
    struct smm_cpu_mem_stat *stat_cur = stat_pre + 1;

    total_pre = total_stat(stat_pre);

    total_cur = total_stat(stat_cur);

    total = total_cur - total_pre;
    si = stat_cur->cpu_softirq - stat_pre->cpu_softirq;

    zlog_debug(zc, "total %d  si %d\r\n", total, si);
    contrl->result.r_cpu_si_ratio = 100.f * (si) / total;
    zlog_debug(zc, "contrl->result.r_cpu_si_ratio is %.2f \r\n",
               contrl->result.r_cpu_si_ratio);
}

/**
 * @brief 内存利用率
 * 
 * @param pid pid
 * @param contrl 控制消息数据结构
 * @param type 类型
 * @param data 
 */
void mem_ratio(u32 pid, struct smm_contrl *contrl, int type, void *data)
{
    u64 mem_idle, total;
    struct smm_cpu_mem_stat *stat_pre = (struct smm_cpu_mem_stat *)data;
    struct smm_cpu_mem_stat *stat_cur = stat_pre + 1;

    mem_idle = stat_pre->mem_free + stat_pre->mem_cache + stat_pre->mem_buffer + stat_cur->mem_free + stat_cur->mem_cache + stat_cur->mem_buffer;

    total = stat_pre->mem_total + stat_cur->mem_total;

    zlog_debug(zc, "mem_idle %llu  total %llu\r\n", mem_idle, total);
    contrl->result.r_mem_ratio = 100.f - (mem_idle * 100.f) / total;
    zlog_debug(zc, "contrl->result.r_mem_ratio is %.2f \r\n",
               contrl->result.r_mem_ratio);
}

/**
 * @brief 获取cache
 * 
 * @param pid pid
 * @param contrl 控制消息数据结构
 * @param type 类型
 * @param data 
 */
void mem_cache(u32 pid, struct smm_contrl *contrl, int type, void *data)
{
    u32 cache;
    struct smm_cpu_mem_stat *stat_pre = (struct smm_cpu_mem_stat *)data;
    struct smm_cpu_mem_stat *stat_cur = stat_pre + 1;

    cache = (stat_pre->mem_cache + stat_cur->mem_cache) / 2;

    zlog_debug(zc, "cache %llu \r\n", cache);
    contrl->result.r_mem_cache = cache / 1024.f;
    zlog_debug(zc, "contrl->result.r_mem_cache is %.2f \r\n",
               contrl->result.r_mem_cache);
}

/**
 * @brief 获取buffer
 * 
 * @param pid pid
 * @param contrl 控制消息数据结构
 * @param type 类型
 * @param data 
 */
void mem_buffer(u32 pid, struct smm_contrl *contrl, int type, void *data)
{
    u32 buffer;
    struct smm_cpu_mem_stat *stat_pre = (struct smm_cpu_mem_stat *)data;
    struct smm_cpu_mem_stat *stat_cur = stat_pre + 1;

    buffer = (stat_pre->mem_buffer + stat_cur->mem_buffer) / 2;

    zlog_debug(zc, "buffer %llu \r\n", buffer);
    contrl->result.r_mem_buffer = buffer / 1024.f;
    zlog_debug(zc, "contrl->result.r_mem_buffer is %.2f \r\n",
               contrl->result.r_mem_buffer);
}

/**
 * @brief 更新pid的状态信息
 * 
 */
void pid_stat_updata(u32 pid, struct smm_pid_stat *stat)
{
}
/**
 * @brief 指定进程的cpu利用率
 * 
 * @param pid pid
 * @param contrl 控制消息数据结构
 * @param type 类型
 * @param data 
 */
void pid_cpu_ratio(u32 pid, struct smm_contrl *contrl, int type, void *data)
{
}

/**
 * @brief 指定进程运行的CPU号
 * 
 * @param pid pid
 * @param contrl 控制消息数据结构
 * @param type 类型
 * @param data 
 */
void pid_cpu_index(u32 pid, struct smm_contrl *contrl, int type, void *data)
{
}

/**
 * @brief 进程内存使用率
 * 
 * @param pid pid
 * @param contrl 控制消息数据结构
 * @param type 类型
 * @param data 
 */
void pid_mem_ratio(u32 pid, struct smm_contrl *contrl, int type, void *data)
{
}

/**
 * @brief 进程物理内存使用率
 * 
 * @param pid pid
 * @param contrl 控制消息数据结构
 * @param type 类型
 * @param data 
 */
void pid_mem_rss(u32 pid, struct smm_contrl *contrl, int type, void *data)
{
}

/**
 * @brief 进程虚拟内存使用率
 * 
 * @param pid pid
 * @param contrl 控制消息数据结构
 * @param type 类型
 * @param data 
 */
void pid_mem_vir(u32 pid, struct smm_contrl *contrl, int type, void *data)
{
}

/**
 * @brief 进程io读速率
 * 
 * @param pid pid
 * @param contrl 控制消息数据结构
 * @param type 类型
 * @param data 
 */
void pid_io_rd_ratio(u32 pid, struct smm_contrl *contrl, int type, void *data)
{
}

/**
 * @brief 进程io写效率
 * 
 * @param pid pid
 * @param contrl 控制消息数据结构
 * @param type 类型
 * @param data 
 */
void pid_io_wr_ratio(u32 pid, struct smm_contrl *contrl, int type, void *data)
{
}

/**
 * @brief 进程网络发送速率
 * 
 * @param pid pid
 * @param contrl 控制消息数据结构
 * @param type 类型
 * @param data 
 */
void pid_net_sd_rate(u32 pid, struct smm_contrl *contrl, int type, void *data)
{
}

/**
 * @brief 进程网络接收速率
 * 
 * @param pid pid
 * @param contrl 控制消息数据结构
 * @param type 类型
 * @param data 
 */
void pid_net_rc_rate(u32 pid, struct smm_contrl *contrl, int type, void *data)
{
}
/**
 * @brief smm的结果处理
 * 
 * @param contrl 控制信息数据结构
 */
void smm_deal_result(struct smm_contrl *contrl)
{
    smm_deal(zc, contrl->dealmode, "%%CPU(s)  %5.2f%% use %5.2f%% usr %5.2f%% sys %5.2f%% si",
             contrl->result.r_cpu_ratio, contrl->result.r_cpu_usr_ratio,
             contrl->result.r_cpu_kernel_ratio, contrl->result.r_cpu_si_ratio);
    smm_deal(zc, contrl->dealmode, "%%MEM(MB) %5.2f%% use %.2f/%.2f buff/cache ",
             contrl->result.r_mem_ratio, contrl->result.r_mem_buffer, contrl->result.r_mem_cache);
}
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
static void cpu_stat_get(struct smm_cpu_mem_stat *stat)
{
    FILE *pfile_cpu = NULL;
    FILE *pfile_mem = NULL;
    char line[128];
    //cpu
    pfile_cpu = fopen(FS_STAT, "r");
    if (NULL != pfile_cpu)
    {
        fscanf(pfile_cpu, "cpu %llu %llu %llu %llu %llu %llu %llu %llu", &stat->cpu_user,
               &stat->cpu_nice, &stat->cpu_kernel, &stat->cpu_idle,
               &stat->cpu_iowait, &stat->cpu_irq, &stat->cpu_softirq,
               &stat->cpu_stealstolen);

        fclose(pfile_cpu);
    }
    //mem
    pfile_mem = fopen(FS_MEMINFO, "r");
    if (NULL != pfile_mem)
    {
        while (fgets(line, sizeof(line), pfile_mem) != NULL)
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
        fclose(pfile_mem);
    }
}

/**
 * @brief 获取pid的状态信息
 * 
 */
u32 pid_stat_get(u32 pid, struct smm_cpu_mem_stat *stat)
{
    FILE *pfile = NULL;
    u8 filename[128] = {0};
    u8 line[4096] = {0};
    u8 pid_items = 0;
    u32 ret = ERROR;

    sprintf(filename, FS_PID_STAT, pid);
    pfile = fopen(filename, "r");
    if (pfile != NULL)
    {
        while (fgets(line, sizeof(line), pfile) != NULL)
        {
            zlog_debug(zc, "strlen(line) %d\r\n", strlen(line));
            zlog_debug(zc, "line ->%s\r\n", line);
            for (u32 i = 0; i < strlen(line); i++)
            {
                if (line[i] == ' ')
                    pid_items++;
                if (pid_items == 13 && stat->pid_utime == 0)
                {
                    //zlog_debug(zc, "line %s\r\n", line + i);
                    sscanf(line + i, "%llu", &stat->pid_utime);
                }
                if (pid_items == 14 && stat->pid_stime == 0)
                {
                    //zlog_debug(zc, "line %s\r\n", line + i);
                    sscanf(line + i, "%llu", &stat->pid_stime);
                }
                if (pid_items == 15 && stat->pid_cutime == 0)
                    sscanf(line + i, "%llu", &stat->pid_cutime);
                if (pid_items == 16 && stat->pid_cstime == 0)
                    sscanf(line + i, "%llu", &stat->pid_cstime);
                if (pid_items == 23 && stat->pid_rss == 0)
                {
                    zlog_debug(zc, "line %s\r\n", line + i);
                    sscanf(line + i, "%llu", &stat->pid_rss);
                }
            }
        }

        fclose(pfile);
        ret = SUCCESS;
    }
    return ret;
}

/**
 * @brief 更新CPU 信息
 * 
 * @param stat 状态信息
 */
u32 cpu_stat_update(u32 pid, struct smm_cpu_mem_stat *stat, struct smm_contrl *contrl)
{
    u32 ret = ERROR;
    struct smm_cpu_mem_stat *stat_pre = NULL;
    struct smm_cpu_mem_stat *stat_cur = NULL;

    stat_pre = stat;
    stat_cur = stat_pre + 1;

    cpu_stat_get(stat_pre);
    if (pid != 0)
        ret = pid_stat_get(pid, stat_pre);
    usleep(SAMPLE_MS * 1000);
    if (pid != 0)
        ret = pid_stat_get(pid, stat_cur);
    cpu_stat_get(stat_cur);
    contrl->result.weight_times++; //加权次数加一
    return ret;
}
/**
 * @brief cpu利用率
 * 
 * @param pid pid
 * @param contrl 控制消息数据结构
 * @param type 类型
 * @param data 
 */
void cpu_ratio(u32 pid, struct smm_contrl *contrl, int index, void *data)
{
    u64 total_pre, total_cur;
    u32 cpu_idle, total;
    double cpu_ratio;
    struct smm_cpu_mem_stat *stat_pre = (struct smm_cpu_mem_stat *)data;
    struct smm_cpu_mem_stat *stat_cur = stat_pre + 1;

    total_pre = total_stat(stat_pre);

    total_cur = total_stat(stat_cur);

    total = total_cur - total_pre;

    cpu_idle = stat_cur->cpu_idle - stat_pre->cpu_idle;

    zlog_debug(zc, "total %d  cpu_idle %d\r\n", total, cpu_idle);
    if (total != 0) //避免除数为0
        cpu_ratio = 100.f * (total - cpu_idle) / total;
    else
        cpu_ratio = 0;
    contrl->result.r_cpu_ratio += cpu_ratio;
    zlog_debug(zc, "cpu_ratio is %.2f \r\n", cpu_ratio);
}

/**
 * @brief cpu用户态占比
 * 
 * @param pid pid
 * @param contrl 控制消息数据结构
 * @param type 类型
 * @param data 
 */
void cpu_usr_ratio(u32 pid, struct smm_contrl *contrl, int index, void *data)
{
    u64 total_pre, total_cur;
    u32 usr, total;
    double cpu_usr_ratio;
    struct smm_cpu_mem_stat *stat_pre = (struct smm_cpu_mem_stat *)data;
    struct smm_cpu_mem_stat *stat_cur = stat_pre + 1;

    total_pre = total_stat(stat_pre);

    total_cur = total_stat(stat_cur);

    total = total_cur - total_pre;
    usr = stat_cur->cpu_user - stat_pre->cpu_user;

    zlog_debug(zc, "total %d  usr %d\r\n", total, usr);
    if (total != 0) //避免除数为0
        cpu_usr_ratio = 100.f * (usr) / total;
    else
        cpu_usr_ratio = 0;
    contrl->result.r_cpu_usr_ratio += cpu_usr_ratio;
    zlog_debug(zc, "cpu_usr_ratio is %.2f \r\n", cpu_usr_ratio);
}

/**
 * @brief cpu内核态占比
 * 
 * @param pid pid
 * @param contrl 控制消息数据结构
 * @param type 类型
 * @param data 
 */
void cpu_kernel_ratio(u32 pid, struct smm_contrl *contrl, int index, void *data)
{
    u64 total_pre, total_cur;
    u32 cpu_kernel, total;
    double cpu_kernel_ratio;
    struct smm_cpu_mem_stat *stat_pre = (struct smm_cpu_mem_stat *)data;
    struct smm_cpu_mem_stat *stat_cur = stat_pre + 1;

    total_pre = total_stat(stat_pre);

    total_cur = total_stat(stat_cur);

    total = total_cur - total_pre;
    cpu_kernel = stat_cur->cpu_kernel - stat_pre->cpu_kernel;

    zlog_debug(zc, "total %d  cpu_kernel %d\r\n", total, cpu_kernel);
    if (total != 0) //避免除数为0
        cpu_kernel_ratio = 100.f * (cpu_kernel) / total;
    else
        cpu_kernel_ratio = 0;
    contrl->result.r_cpu_kernel_ratio += cpu_kernel_ratio;
    zlog_debug(zc, "cpu_kernel_ratio is %.2f \r\n", cpu_kernel_ratio);
}

/**
 * @brief 软中断的cpu占比
 * 
 * @param pid pid
 * @param contrl 控制消息数据结构
 * @param type 类型
 * @param data 
 */
void cpu_si_ratio(u32 pid, struct smm_contrl *contrl, int index, void *data)
{
    u64 total_pre, total_cur;
    u32 si, total;
    double cpu_si_ratio;
    struct smm_cpu_mem_stat *stat_pre = (struct smm_cpu_mem_stat *)data;
    struct smm_cpu_mem_stat *stat_cur = stat_pre + 1;

    total_pre = total_stat(stat_pre);

    total_cur = total_stat(stat_cur);

    total = total_cur - total_pre;
    si = stat_cur->cpu_softirq - stat_pre->cpu_softirq;

    zlog_debug(zc, "total %d  si %d\r\n", total, si);
    if (total != 0) //避免除数为0
        cpu_si_ratio = 100.f * (si) / total;
    else
        cpu_si_ratio = 0;
    contrl->result.r_cpu_si_ratio += cpu_si_ratio;
    zlog_debug(zc, "cpu_si_ratio is %.2f \r\n", cpu_si_ratio);
}

/**
 * @brief 内存利用率
 * 
 * @param pid pid
 * @param contrl 控制消息数据结构
 * @param type 类型
 * @param data 
 */
void mem_ratio(u32 pid, struct smm_contrl *contrl, int index, void *data)
{
    u64 mem_idle, total;
    double mem_ratio;
    struct smm_cpu_mem_stat *stat_pre = (struct smm_cpu_mem_stat *)data;
    struct smm_cpu_mem_stat *stat_cur = stat_pre + 1;

    mem_idle = stat_pre->mem_free + stat_pre->mem_cache + stat_pre->mem_buffer + stat_cur->mem_free + stat_cur->mem_cache + stat_cur->mem_buffer;

    total = stat_pre->mem_total + stat_cur->mem_total;

    zlog_debug(zc, "mem_idle %llu  total %llu\r\n", mem_idle, total);
    if (total != 0) //避免除数为0
        mem_ratio = 100.f - (mem_idle * 100.f) / total;
    else
        mem_ratio = 0;
    contrl->result.r_mem_ratio += mem_ratio;
    zlog_debug(zc, "mem_ratio is %.2f \r\n", mem_ratio);
}

/**
 * @brief 获取cache
 * 
 * @param pid pid
 * @param contrl 控制消息数据结构
 * @param type 类型
 * @param data 
 */
void mem_cache(u32 pid, struct smm_contrl *contrl, int index, void *data)
{
    u32 cache;
    double mem_cache;
    struct smm_cpu_mem_stat *stat_pre = (struct smm_cpu_mem_stat *)data;
    struct smm_cpu_mem_stat *stat_cur = stat_pre + 1;

    cache = (stat_pre->mem_cache + stat_cur->mem_cache) / 2;

    zlog_debug(zc, "cache %llu \r\n", cache);
    mem_cache = cache / 1024.f;
    contrl->result.r_mem_cache += mem_cache;
    zlog_debug(zc, "mem_cache is %.2f \r\n", mem_cache);
}

/**
 * @brief 获取buffer
 * 
 * @param pid pid
 * @param contrl 控制消息数据结构
 * @param type 类型
 * @param data 
 */
void mem_buffer(u32 pid, struct smm_contrl *contrl, int index, void *data)
{
    u32 buffer;
    double mem_buffer;
    struct smm_cpu_mem_stat *stat_pre = (struct smm_cpu_mem_stat *)data;
    struct smm_cpu_mem_stat *stat_cur = stat_pre + 1;

    buffer = (stat_pre->mem_buffer + stat_cur->mem_buffer) / 2;

    zlog_debug(zc, "buffer %llu \r\n", buffer);
    mem_buffer = buffer / 1024.f;
    contrl->result.r_mem_buffer += mem_buffer;
    zlog_debug(zc, "mem_buffer is %.2f \r\n", mem_buffer);
}

/**
 * @brief 指定进程的cpu利用率
 * 
 * @param pid pid
 * @param contrl 控制消息数据结构
 * @param type 类型
 * @param data 
 */
void pid_cpu_ratio(u32 pid, struct smm_contrl *contrl, int index, void *data)
{
    u64 total_pre, total_cur;
    u64 pid_pre, pid_cur;
    double cpu_pid_ratio;
    struct smm_cpu_mem_stat *stat_pre = (struct smm_cpu_mem_stat *)data;
    struct smm_cpu_mem_stat *stat_cur = stat_pre + 1;

    total_pre = total_stat(stat_pre);

    total_cur = total_stat(stat_cur);

    pid_pre = stat_pre->pid_cstime + stat_pre->pid_cutime + stat_pre->pid_stime + stat_pre->pid_utime;
    pid_cur = stat_cur->pid_cstime + stat_cur->pid_cutime + stat_cur->pid_stime + stat_cur->pid_utime;
    if ((total_cur - total_pre) != 0) //避免除数为0
        cpu_pid_ratio = 100.f * (pid_cur - pid_pre) / (total_cur - total_pre);
    else
        cpu_pid_ratio = 0;
    zlog_debug(zc, "pid_cpu_ratio is %.2f \r\n", cpu_pid_ratio);
    contrl->pid_result[index].r_pid_cpu_ratio = cpu_pid_ratio;
}

/**
 * @brief 
 * 
 * @param pid pid
 * @param contrl 控制消息数据结构
 * @param type 类型
 * @param data 
 */
void pid_cpu_usr_ratio(u32 pid, struct smm_contrl *contrl, int index, void *data)
{
    u64 total_pre, total_cur;
    u64 pid_pre, pid_cur;
    double cpu_pid_usr_ratio;
    struct smm_cpu_mem_stat *stat_pre = (struct smm_cpu_mem_stat *)data;
    struct smm_cpu_mem_stat *stat_cur = stat_pre + 1;

    total_pre = total_stat(stat_pre);

    total_cur = total_stat(stat_cur);

    pid_pre = stat_pre->pid_utime;
    pid_cur = stat_cur->pid_utime;
    if ((total_cur - total_pre) != 0) //避免除数为0
        cpu_pid_usr_ratio = 100.f * (pid_cur - pid_pre) / (total_cur - total_pre);
    else
        cpu_pid_usr_ratio = 0;
    zlog_debug(zc, "cpu_pid_usr_ratio is %.2f \r\n", cpu_pid_usr_ratio);
    contrl->pid_result[index].r_pid_cpu_usr_ratio = cpu_pid_usr_ratio;
}

/**
 * @brief 
 * 
 * @param pid pid
 * @param contrl 控制消息数据结构
 * @param type 类型
 * @param data 
 */
void pid_cpu_kernel_ratio(u32 pid, struct smm_contrl *contrl, int index, void *data)
{
    u64 total_pre, total_cur;
    u64 pid_pre, pid_cur;
    double cpu_pid_sys_ratio;
    struct smm_cpu_mem_stat *stat_pre = (struct smm_cpu_mem_stat *)data;
    struct smm_cpu_mem_stat *stat_cur = stat_pre + 1;

    total_pre = total_stat(stat_pre);

    total_cur = total_stat(stat_cur);

    pid_pre = stat_pre->pid_stime;
    pid_cur = stat_cur->pid_stime;
    if ((total_cur - total_pre) != 0) //避免除数为0
        cpu_pid_sys_ratio = 100.f * (pid_cur - pid_pre) / (total_cur - total_pre);
    else
        cpu_pid_sys_ratio = 0;
    zlog_debug(zc, "cpu_pid_sys_ratio is %.2f \r\n", cpu_pid_sys_ratio);
    contrl->pid_result[index].r_pid_cpu_sys_ratio = cpu_pid_sys_ratio;
}
/**
 * @brief 进程内存使用率
 * 
 * @param pid pid
 * @param contrl 控制消息数据结构
 * @param type 类型
 * @param data 
 */
unsigned int kb_shift;
void get_kb_shift(void)
{
    int shift = 0;
    long size;

    /* One can also use getpagesize() to get the size of a page */
    if ((size = sysconf(_SC_PAGESIZE)) == -1)
    {
        perror("sysconf");
    }

    size >>= 10; /* Assume that a page has a minimum size of 1 kB */

    while (size > 1)
    {
        shift++;
        size >>= 1;
    }

    kb_shift = (unsigned int)shift;
}
void pid_mem_ratio(u32 pid, struct smm_contrl *contrl, int index, void *data)
{
    u64 rss, total;
    double pid_rss_ratio;
    struct smm_cpu_mem_stat *stat_pre = (struct smm_cpu_mem_stat *)data;
    struct smm_cpu_mem_stat *stat_cur = stat_pre + 1;

    rss = stat_pre->pid_rss + stat_cur->pid_rss;
    total = stat_pre->mem_total + stat_cur->mem_total;

    get_kb_shift();
    if (total != 0) //避免除数为0
        pid_rss_ratio = 100.f * (PG_TO_KB(rss)) / total;
    else
        pid_rss_ratio = 0;
    zlog_debug(zc, "pid_rss_ratio is %.2f \r\n", pid_rss_ratio);
    contrl->pid_result[index].r_pid_mem_ratio = pid_rss_ratio;
}

/**
 * @brief smm的结果处理
 * 
 * @param contrl 控制信息数据结构
 */
void smm_deal_result(struct smm_contrl *contrl)
{
    u8 pid_index = 0;
    u8 info[4096] = {0};
    u32 info_len = 0;
    static int clear_screen_flag = 0, clear_screen_item = 0;
    zlog_debug(zc, "smm_deal_result weight times %d\r\n", contrl->result.weight_times);
    if (contrl->dealmode == DISPLAY_MODE)
    {
        if (clear_screen_flag != 0)
        {
            for (pid_index = 0; pid_index < clear_screen_item; pid_index++)
            {
                printf("\033[1A"); //光标上移
                printf("\033[K");
            }
        }
        printf("%%CPU(s)  %5.2f%% use %5.2f%% usr %5.2f%% sys %5.2f%% si \r\n",
               contrl->result.r_cpu_ratio / contrl->result.weight_times,
               contrl->result.r_cpu_usr_ratio / contrl->result.weight_times,
               contrl->result.r_cpu_kernel_ratio / contrl->result.weight_times,
               contrl->result.r_cpu_si_ratio / contrl->result.weight_times);
        printf("%%MEM(MB) %5.2f%% use %.2f/%.2f buff/cache \r\n",
               contrl->result.r_mem_ratio / contrl->result.weight_times,
               contrl->result.r_mem_buffer / contrl->result.weight_times,
               contrl->result.r_mem_cache / contrl->result.weight_times);
        for (pid_index = 0; pid_index < contrl->pidnum; pid_index++)
        {
            printf("PID:%s (%d) \r\n", contrl->pid_name[pid_index].name, contrl->smm_pid[pid_index]);
            printf("CPU(s)  %5.2f%% use %5.2f%% usr %5.2f%% sys \r\n",
                   contrl->pid_result[pid_index].r_pid_cpu_ratio, contrl->pid_result[pid_index].r_pid_cpu_usr_ratio,
                   contrl->pid_result[pid_index].r_pid_cpu_sys_ratio);
            printf("MEM %5.2f%%\r\n", contrl->pid_result[pid_index].r_pid_mem_ratio);
        }
        clear_screen_flag = 1;
        clear_screen_item = (contrl->pidnum * 3) + 2;
        //fflush(stdout);
    }
    //以md的格式进行记录
    sprintf(info, "|%%CPU(s)| %5.2f%% |use |%5.2f%% |usr |%5.2f%% |sys |%5.2f%% |si"
                  "|%%MEM(MB)| %5.2f%% |use |%.2f/%.2f |buff/cache ",
            contrl->result.r_cpu_ratio / contrl->result.weight_times,
            contrl->result.r_cpu_usr_ratio / contrl->result.weight_times,
            contrl->result.r_cpu_kernel_ratio / contrl->result.weight_times,
            contrl->result.r_cpu_si_ratio / contrl->result.weight_times,
            contrl->result.r_mem_ratio / contrl->result.weight_times,
            contrl->result.r_mem_buffer / contrl->result.weight_times,
            contrl->result.r_mem_cache / contrl->result.weight_times);

    for (pid_index = 0; pid_index < contrl->pidnum; pid_index++)
    {
        info_len = strlen(info);
        sprintf(&info[info_len], "|PID:%s (%d)"
                                 "|CPU(s) | %5.2f%% |use |%5.2f%% |usr |%5.2f%% |sys "
                                 "|MEM |%5.2f%%",
                contrl->pid_name[pid_index].name, contrl->smm_pid[pid_index],
                contrl->pid_result[pid_index].r_pid_cpu_ratio,
                contrl->pid_result[pid_index].r_pid_cpu_usr_ratio,
                contrl->pid_result[pid_index].r_pid_cpu_sys_ratio,
                contrl->pid_result[pid_index].r_pid_mem_ratio);
    }
    info_len = strlen(info);
    sprintf(&info[info_len], "|");
    zlog_info(zc, "%s ", info);
}
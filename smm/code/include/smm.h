/**
 * @file smm.h
 * @author zhao.wei (hw)
 * @brief smm头文件
 * @version 0.1
 * @date 2020-07-17
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef _SMM_H_
#define _SMM_H_

#define smm_deal(dealmode, format, args...)                                             \
    zlog(cat, __FILE__, sizeof(__FILE__) - 1, __func__, sizeof(__func__) - 1, __LINE__, \
         dealmode, format, ##args)

#define DISPLAY_MODE ZLOG_LEVEL_DEBUG //使用zlog的debug模式
#define LOG_MODE ZLOG_LEVEL_INFO      //使用zlog的info模式

#define MAX_PID_NUM 128

/**
 * @brief 系统级监控数据
 * 
 */
struct smm_result
{
    double r_cpu_ratio;
    double r_cpu_usr_ratio;
    double r_cpu_kernel_ratio;
    double r_cpu_vir_ratio;
    double r_mem_ratio;
};

/**
 * @brief 进程级监控数据
 * 
 */
struct smm_pid_result
{
    double r_pid_cpu_ratio;
    double r_pid_cpu_index;
    double r_pid_mem_ratio;
    double r_pid_mem_rss;
    double r_pid_mem_vir;
    double r_pid_io_rd_ratio;
    double r_pid_io_wr_ratio;
    double r_pid_net_sd_rate;
    double r_pid_net_rc_rate;
};
/**
 * @brief smm的控制数据结构
 * 
 */
struct smm_contrl
{
    u32 interval; //监控的时间间隔 单位：mm
    u8 dealmode;  //0 实时显示 1 记录日志
    u8 pidnum;    //监控的进程数
    u16 pad;      //保留
    struct smm_result result;
    u32 smm_pid[MAX_PID_NUM]; //进程PID
    struct smm_pid_result pid_result[MAX_PID_NUM];
};

typedef void (*dealfun)(unsigned int, struct smm_contrl *, void *); //钩子

typedef enum e_dealtype
{
    CPU_RATIO = 0,
    CPU_USR_RATIO,
    CPU_KERNEL_RATIO,
    CPU_VIR_RATIO,
    MEM_RATIO,

    PID_CPU_RATIO = 20,
    PID_CPU_INDEX,
    PID_MEM_RATIO,
    PID_MEM_RSS,
    PID_MEM_VIR,
    PID_IO_RD_RATE,
    PID_IO_WR_RATE,
    PID_NET_SD_RATE,
    PID_NET_RC_RATE
} dealtype;

struct smm_dealentity
{
    int deal_project;
    dealfun p_dealfun;
}

#endif
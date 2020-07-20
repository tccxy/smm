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
 * @brief cpu利用率
 * 
 * @param pid pid
 * @param contrl 控制消息数据结构
 * @param type 类型
 */
void cpu_ratio(u32 pid, struct smm_contrl *contrl, int type)
{
}

/**
 * @brief cpu用户态利用率
 * 
 * @param pid pid
 * @param contrl 控制消息数据结构
 * @param data 预留
 */
void cpu_usr_ratio(u32 pid, struct smm_contrl *contrl, int type)
{
}

/**
 * @brief cpu内核态利用率
 * 
 * @param pid pid
 * @param contrl 控制消息数据结构
 * @param data 预留
 */
void cpu_kernel_ratio(u32 pid, struct smm_contrl *contrl, int type)
{
}

/**
 * @brief 虚拟cpu利用率
 * 
 * @param pid pid
 * @param contrl 控制消息数据结构
 * @param data 预留
 */
void cpu_vir_ratio(u32 pid, struct smm_contrl *contrl, int type)
{
}

/**
 * @brief 内存利用率
 * 
 * @param pid pid
 * @param contrl 控制消息数据结构
 * @param data 预留
 */
void mem_ratio(u32 pid, struct smm_contrl *contrl, int type)
{
}

/**
 * @brief 指定进程的cpu利用率
 * 
 * @param pid pid
 * @param contrl 控制消息数据结构
 * @param data 预留
 */
void pid_cpu_ratio(u32 pid, struct smm_contrl *contrl, int type)
{
}

/**
 * @brief 指定进程运行的CPU号
 * 
 * @param pid pid
 * @param contrl 控制消息数据结构
 * @param data 预留
 */
void pid_cpu_index(u32 pid, struct smm_contrl *contrl, int type)
{
}

/**
 * @brief 进程内存使用率
 * 
 * @param pid pid
 * @param contrl 控制消息数据结构
 * @param data 预留
 */
void pid_mem_ratio(u32 pid, struct smm_contrl *contrl, int type)
{
}

/**
 * @brief 进程物理内存使用率
 * 
 * @param pid pid
 * @param contrl 控制消息数据结构
 * @param data 预留
 */
void pid_mem_rss(u32 pid, struct smm_contrl *contrl, int type)
{
}

/**
 * @brief 进程虚拟内存使用率
 * 
 * @param pid pid
 * @param contrl 控制消息数据结构
 * @param data 预留
 */
void pid_mem_vir(u32 pid, struct smm_contrl *contrl, int type)
{
}

/**
 * @brief 进程io读速率
 * 
 * @param pid pid
 * @param contrl 控制消息数据结构
 * @param data 预留
 */
void pid_io_rd_ratio(u32 pid, struct smm_contrl *contrl, int type)
{
}

/**
 * @brief 进程io写效率
 * 
 * @param pid pid
 * @param contrl 控制消息数据结构
 * @param data 预留
 */
void pid_io_wr_ratio(u32 pid, struct smm_contrl *contrl, int type)
{
}

/**
 * @brief 进程网络发送速率
 * 
 * @param pid pid
 * @param contrl 控制消息数据结构
 * @param data 预留
 */
void pid_net_sd_rate(u32 pid, struct smm_contrl *contrl, int type)
{
}

/**
 * @brief 进程网络接收速率
 * 
 * @param pid pid
 * @param contrl 控制消息数据结构
 * @param data 预留
 */
void pid_net_rc_rate(u32 pid, struct smm_contrl *contrl, int type)
{
}
/**
 * @brief smm的结果处理
 * 
 * @param contrl 控制信息数据结构
 */
void smm_deal_result(struct smm_contrl *contrl)
{

}
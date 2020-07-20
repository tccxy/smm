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


void cpu_ratio(u32 pid, struct smm_contrl *contrl, void *data);
void cpu_usr_ratio(u32 pid, struct smm_contrl *contrl, void *data);
void cpu_kernel_ratio(u32 pid, struct smm_contrl *contrl, void *data);
void cpu_vir_ratio(u32 pid, struct smm_contrl *contrl, void *data);
void mem_ratio(u32 pid, struct smm_contrl *contrl, void *data);
void pid_cpu_ratio(u32 pid, struct smm_contrl *contrl, void *data);
void pid_cpu_index(u32 pid, struct smm_contrl *contrl, void *data);
void pid_mem_ratio(u32 pid, struct smm_contrl *contrl, void *data);
void pid_mem_rss(u32 pid, struct smm_contrl *contrl, void *data);
void pid_mem_vir(u32 pid, struct smm_contrl *contrl, void *data);
void pid_io_rd_ratio(u32 pid, struct smm_contrl *contrl, void *data);
void pid_io_wr_ratio(u32 pid, struct smm_contrl *contrl, void *data);
void pid_net_sd_rate(u32 pid, struct smm_contrl *contrl, void *data);
void pid_net_rc_rate(u32 pid, struct smm_contrl *contrl, void *data);


#endif
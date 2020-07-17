#include "pub.h"

static u8 help[] =
    "\
        \r\n Usage   : \
        \r\n    smm [options] -t <Monitoring period> -p {[app_name1];[name2];..}\
        \r\n    options\
        \r\n        -h,--help                          get app help\
        \r\n        -d,--display                       display monitoring results\
        \r\n        -l,--log                           record monitoring results use zlog\
        ";

static u8 exit_prese_msg[] =
    "\
    \r\n Usage   : \
    \r\n    smm [options] -t <Monitoring period> -p {[app_name1];[name2];..}\
    \r\nTry `smm -h,--help' for more information.\
    ";

static struct option long_options[] =
    {
        {"help", no_argument, NULL, 'h'},
        {"display", no_argument, NULL, 'd'},
        {"log", no_argument, NULL, 'l'},
        {"timeinterval", required_argument, NULL, 't'},
        {"Process name", required_argument, NULL, 'p'},
        {NULL, 0, NULL, 0},
};

struct smm_dealentity entry_register[] =
    {
        {CPU_RATIO, cpu_ratio},
        {CPU_USR_RATIO, cpu_usr_ratio},
        {CPU_KERNEL_RATIO, cpu_kernel_ratio},
        {CPU_VIR_RATIO, cpu_vir_ratio},
        {MEM_RATIO, mem_ratio},

        {PID_CPU_RATIO, pid_cpu_ratio},
        {PID_CPU_INDEX, pid_cpu_index},
        {PID_MEM_RATIO, pid_mem_ratio},
        {PID_MEM_RSS, pid_mem_rss},
        {PID_MEM_VIR, pid_mem_vir},
        {PID_IO_RD_RATE, pid_io_rd_ratio},
        {PID_IO_WR_RATE, pid_io_wr_ratio},
        {PID_NET_SD_RATE, pid_net_sd_rate},
        {PID_NET_RC_RATE, pid_net_rc_rate}};

/**
 * @brief smm的命令参数解析
 * 
 * @param opt 
 * @param optarg 
 */
static void smm_cmd_parse(int opt, char *optarg)
{
}

/**
 * @brief 主函数
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */
int main(int argc, char *argv[])
{
    u32 opt;
    u32 rc = 0;
    u32 option_index = 0;
    u8 *string = "";

    rc = zlog_init("/usp/usp_log.conf");
    if (rc)
    {
        printf("init failed\n");
        return -1;
    }

    zc = zlog_get_category("smm");
    if (!zc)
    {
        printf("get cat fail\n");
        zlog_fini();
        return -2;
    }

    while ((opt = getopt_long_only(argc, argv, string, long_options, &option_index)) != -1)
    {
        //printf("opt = %c\t\t", opt);
        //printf("optarg = %s\t\t", optarg);
        //printf("optind = %d\t\t", optind);
        //printf("argv[optind] =%s\t\t", argv[optind]);
        //printf("option_index = %d\n", option_index);
        switch (opt)
        {
        case 'l':
        case 'd':
        case 't':
        case 'p':
            smm_cmd_parse(opt, optarg);
        default:
            exit_usage(0);
            break;
        }
    }

    zlog_fini();
    return 0;
}
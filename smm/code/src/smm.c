#include "pub.h"

/**
 * @brief smm控制信息数据结构定义
 * 
 */
struct smm_contrl g_smm_contrl = {0};

zlog_category_t *zc = NULL; /**日志描述符*/

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
    \r\n    smm [options] -t <Monitoring period> -p {[app_name1]:[name2];..}\
    \r\nTry `smm -h,--help' for more information.\
    ";

static struct option long_options[] =
    {
        {"help", no_argument, NULL, 'h'},
        {"display", no_argument, NULL, 'd'},
        {"log", no_argument, NULL, 'l'},
        {"timeu32erval", required_argument, NULL, 't'},
        {"Process name", optional_argument, NULL, 'p'},
        {NULL, 0, NULL, 0},
};

struct smm_dealentity entry_register[] =
    {
        {CPU_RATIO, cpu_ratio},
        {CPU_USR_RATIO, cpu_usr_ratio},
        {CPU_KERNEL_RATIO, cpu_kernel_ratio},
        {CPU_VIR_RATIO, cpu_vir_ratio},
        {MEM_RATIO, mem_ratio},
        {SMM_M_END, NULL},

        {PID_CPU_RATIO, pid_cpu_ratio},
        {PID_CPU_INDEX, pid_cpu_index},
        {PID_MEM_RATIO, pid_mem_ratio},
        {PID_MEM_RSS, pid_mem_rss},
        {PID_MEM_VIR, pid_mem_vir},
        {PID_IO_RD_RATE, pid_io_rd_ratio},
        {PID_IO_WR_RATE, pid_io_wr_ratio},
        {PID_NET_SD_RATE, pid_net_sd_rate},
        {PID_NET_RC_RATE, pid_net_rc_rate},
};

/**
 * @brief 通过进程名字获取pid
 * 
 * @param pid 
 * @param task_name 
 * @return u32 
 */
u32 get_pid_byname(pid_t *pid, char *task_name)
{
#define BUF_SIZE 1024
    DIR *dir;
    struct dirent *ptr;
    FILE *fp;
    char filepath[50];
    char cur_task_name[50];
    char buf[BUF_SIZE];
    u32 ret = ERROR;

    dir = opendir("/proc");
    if (NULL != dir)
    {
        while ((ptr = readdir(dir)) != NULL) //循环读取/proc下的每一个文件/文件夹
        {
            //如果读取到的是"."或者".."则跳过，读取到的不是文件夹名字也跳过
            if ((strcmp(ptr->d_name, ".") == 0) || (strcmp(ptr->d_name, "..") == 0))
                continue;
            if (DT_DIR != ptr->d_type)
                continue;

            sprintf(filepath, "/proc/%s/status", ptr->d_name); //生成要读取的文件的路径
            fp = fopen(filepath, "r");
            if (NULL != fp)
            {
                if (fgets(buf, BUF_SIZE - 1, fp) == NULL)
                {
                    fclose(fp);
                    continue;
                }
                sscanf(buf, "%*s %s", cur_task_name);

                //如果文件内容满足要求则打印路径的名字（即进程的PID）
                if (!strcmp(task_name, cur_task_name))
                {
                    sscanf(ptr->d_name, "%d", pid);
                    ret = SUCCESS;
                }
                fclose(fp);
            }
        }
        closedir(dir);
    }
    return ret;
}

/**
 * @brief smm 从传入的参数解析pid的数据
 * 
 * @param contrl smm contrl数据结构
 * @param optarg 传入的参数
 * @return u32 0 is success
 */
u32 smm_parse_pid_data(struct smm_contrl *contrl, u8 *arg)
{
    u32 ret = SUCCESS;
    u8 pidnum = 0;
    u8 *data = NULL;
    u32 pid = 0;
    struct smm_pid_msg pid_msg = {0};

    zlog_debug(zc, "arg is %s \r\n", arg);
    for (data = arg; *data != '\0'; data++)
    {
        if (':' == *data)
            pidnum++; //初步统计一下进程数
    }

    zlog_debug(zc, "pidnum is %d \r\n", pidnum);
    data = arg;
    zlog_debug(zc, "data is %s \r\n", data);
    for (u8 i = 0; i <= pidnum; i++)
    {
        sscanf(data, "%[^:]%*c", pid_msg.pid_name[i].name);
        data += strlen(pid_msg.pid_name[i].name) + 1;
        zlog_debug(zc, "pid_msg.pid_name %s \r\n ", pid_msg.pid_name[i].name);
        if (SUCCESS == get_pid_byname(&pid, pid_msg.pid_name[i].name))
        {
            contrl->pidnum++;
            contrl->smm_pid[i] = pid;
        }
        zlog_debug(zc, "pid_msg.pid_name %s pid %d \r\n ", pid_msg.pid_name[i].name, pid);
    }

    return ret;
}
/**
 * 
 * @brief 打印帮助信息
 * 
 */
void printf_help_usage()
{
    printf("smm_v%d.%d \r\n %s \r\n", MAJOR_VER, MINOR_VER, help);
    exit(1);
}

/**
 * @brief 异常退出消息提示
 * 
 */
void exit_usage()
{
    printf("smm_v%d.%d \r\n %s \r\n", MAJOR_VER, MINOR_VER, exit_prese_msg);
    //日志系统资源释放
    zlog_fini();
    exit(1);
}

/**
 * @brief smm的命令参数解析
 * 
 * @param opt 
 * @param optarg 
 * @param argv 
 */
static void smm_cmd_parse(u32 opt, u8 *optarg, u8 *argv)
{
    //默认参数
    g_smm_contrl.interval = DEFAULT_INTERVAL;
    g_smm_contrl.dealmode = DISPLAY_MODE;
    g_smm_contrl.pidnum = 0;

    if (opt == 'l')
        g_smm_contrl.dealmode = LOG_MODE;

    if (opt == 't')
    {
        if (atoi(optarg) > MIN_INTERVAL)
            g_smm_contrl.interval = atoi(optarg);

        zlog_debug(zc, "g_smm_contrl.interval %d \r\n", g_smm_contrl.interval);
    }

    if (opt == 'p')
    {
        smm_parse_pid_data(&g_smm_contrl, argv);
    }
}

/**
 * @brief ms级别延时
 * 
 * @param mSec 
 */
void ms_sleep(u32 mSec)
{
    struct timeval tv;
    tv.tv_sec = mSec / 1000;
    tv.tv_usec = (mSec % 1000) * 1000;
    int err;
    do
    {
        err = select(0, NULL, NULL, NULL, &tv);
    } while (err < 0 && errno == EINTR);
}

/**
 * @brief 监控实体线程
 * 
 * @param arg 
 */
void monitor_task(void *arg)
{
    struct smm_contrl *contrl = (struct smm_contrl *)arg;
    u8 index = 0, pid_index = 0;
    u32 pid;

    zlog_debug(zc, "monitor_task in\r\n");


    /*系统级监控*/
    for (index = CPU_RATIO; index < SMM_M_END; index++)
    {
        entry_register[index].p_dealfun(0, contrl, index);
    }

    /*进程级监控*/
    for (pid_index = 0; pid_index < contrl->pidnum; pid_index++)
    {
        pid = contrl->smm_pid[pid_index];
        for (index = PID_CPU_RATIO; index < SMM_PID_M_END; index++)
        {
            entry_register[index].p_dealfun(pid, contrl, index);
        }
    }

    smm_deal_result(contrl);
}

/**
 * @brief 主函数
 * 
 * @param argc 
 * @param argv 
 * @return u32 
 */
int main(int argc, char *argv[])
{
    u32 opt;
    u32 rc = 0;
    u32 option_index = 0;
    u8 *string = "lt:p::";

    rc = zlog_init("/home/ab64/test/usp_log.conf");
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
        printf("opt = %c\t\t", opt);
        printf("optarg = %s\t\t", optarg);
        printf("optind = %d\t\t", optind);
        printf("argv[optind] =%s\t\t", argv[optind]);
        printf("option_index = %d\n", option_index);
        switch (opt)
        {
        case 'l':
        case 'd':
        case 't':
        case 'p':
            smm_cmd_parse(opt, optarg, argv[optind]);
            break;
        case 'h':
            printf_help_usage();
            break;
        default:
            exit_usage(0);
            break;
        }
    }

    while (1)
    {
        ms_sleep(g_smm_contrl.interval);
        monitor_task((void *)&g_smm_contrl);
    }
    zlog_fini();
    return 0;
}
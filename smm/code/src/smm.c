#include "pub.h"

/**
 * @brief smm控制信息数据结构定义
 * 
 */
struct smm_contrl g_smm_contrl = {0};
struct smm_pid_msg parse_pid_msg = {0}; //从输入参数解析的进程信息
zlog_category_t *zc = NULL;             /**日志描述符*/

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
        {CPU_SI_RATIO, cpu_si_ratio},
        {MEM_RATIO, mem_ratio},
        {MEM_CAHE, mem_buffer},
        {MEM_BUFFER, mem_cache},
        {SMM_M_END, NULL},

        {PID_CPU_RATIO, pid_cpu_ratio},
        {PID_CPU_USR_RATIO, pid_cpu_usr_ratio},
        {PID_CPU_KERNEL_RATIO, pid_cpu_kernel_ratio},
        {PID_MEM_RATIO, pid_mem_ratio},
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
 * @brief smm通过进程名字校验是否为有效的pid
 * 
 * @param contrl 控制信息数据结构
 * @param pid 从参数解析的数据结构
 * @return u32 
 */
void smm_checkpid_by_name(struct smm_contrl *contrl, struct smm_pid_msg *pid_msg)
{
    u32 pid = 0;
    u32 pid_num = 0;
    static u32 pid_old = 0;
    memset(contrl->smm_pid_valid, 0, sizeof(contrl->smm_pid_valid));
    for (u8 i = 0; i <= pid_msg->parse_pid_num; i++)
    {
        if (SUCCESS == get_pid_byname(&pid, pid_msg->parse_pid_name[i].name))
        {
            pid_num++;
            contrl->smm_pid_valid[i] = ACTIVE; //该进程真实有效
            contrl->smm_pid[i] = pid;
            memcpy(contrl->pid_name[i].name, pid_msg->parse_pid_name[i].name, strlen(pid_msg->parse_pid_name[i].name));
        }
        else
        {
            if (pid_old != contrl->smm_pid[i])
                zlog_notice(zc, "pid_name (%s) has dead!! ", pid_msg->parse_pid_name[i].name);
            else
                zlog_debug(zc, "pid_msg->pid_name %s has dead!! ", pid_msg->parse_pid_name[i].name);
            pid_old = contrl->smm_pid[i];
        }
        zlog_debug(zc, "pid_msg->pid_name %s pid %d \r\n ", pid_msg->parse_pid_name[i].name, pid);
    }
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

    zlog_debug(zc, "arg is %s \r\n", arg);
    for (data = arg; *data != '\0'; data++)
    {
        if (':' == *data)
            pidnum++; //初步统计一下进程数
    }

    zlog_debug(zc, "pidnum is %d \r\n", pidnum);
    data = arg;
    zlog_debug(zc, "data is %s \r\n", data);
    parse_pid_msg.parse_pid_num = pidnum; //从输入参数解析而来的进程数
    for (u8 i = 0; i <= pidnum; i++)
    {
        sscanf(data, "%[^:]%*c", parse_pid_msg.parse_pid_name[i].name);
        data += strlen(parse_pid_msg.parse_pid_name[i].name) + 1;
        zlog_debug(zc, "pid_msg.pid_name %s \r\n ", parse_pid_msg.parse_pid_name[i].name);
        zlog_debug(zc, "parse_pid_msg.pid_name %s  \r\n ", parse_pid_msg.parse_pid_name[i].name);
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
void monitor_task(void *arg, struct smm_pid_msg *pid_msg)
{
    struct smm_contrl *contrl = (struct smm_contrl *)arg;
    u8 index = 0, pid_index = 0;
    u32 pid;
    struct smm_cpu_mem_stat cpu_stat[2] = {0}; //存放两个相近时刻的状态，用作计算

    /*系统级监控*/
    memset((void *)cpu_stat, 0, sizeof(cpu_stat));
    cpu_stat_update(0, cpu_stat, contrl);

    for (index = CPU_RATIO; index < SMM_M_END; index++)
    {
        entry_register[index].p_dealfun(0, contrl, index, (void *)cpu_stat);
    }

    /*进程级监控*/
    for (pid_index = 0; pid_index <= pid_msg->parse_pid_num; pid_index++)
    {
        if (ACTIVE == contrl->smm_pid_valid[pid_index]) //有效
        {
            pid = contrl->smm_pid[pid_index];
            memset((void *)cpu_stat, 0, sizeof(cpu_stat));
            if (cpu_stat_update(pid, cpu_stat, contrl) != SUCCESS)
            {
                continue; //说明此时有进程发生退出
            }
            for (index = CPU_RATIO; index < SMM_PID_M_END; index++)
            {
                if (NULL != entry_register[index].p_dealfun)
                    entry_register[index].p_dealfun(pid, contrl, pid_index, (void *)cpu_stat);
            }
        }
    }
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

    rc = zlog_init("/home/ab64/test/smm_log.conf");
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
        smm_checkpid_by_name(&g_smm_contrl, &parse_pid_msg);
        memset((void *)&g_smm_contrl.result, 0, sizeof(g_smm_contrl.result));
        memset((void *)&g_smm_contrl.pid_result, 0, sizeof(g_smm_contrl.pid_result));
        monitor_task((void *)&g_smm_contrl, &parse_pid_msg);
        smm_deal_result((void *)&g_smm_contrl, &parse_pid_msg);
    }
    zlog_fini();
    return 0;
}
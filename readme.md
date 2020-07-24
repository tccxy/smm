# 系统监控模块
## 概述
系统监控模块用于对片上linux系统资源监控，基于proc文件系统实现对CPU和内存资源的监控，并记录日志
到本地，监控分为两大部分进行，
1） 对整个片上资源的监控，包括

|系统级监控|
|----|
|CPU占用率|
|用户态CPU占用率|
|内核态CPU占用率|
|内存占用率|
|buff空间大小|
|cache空间大小|
2） 对所设置的进程（程序）进行资源消耗的监控,包括

|进程级监控|
|----|
|进程CPU占用率|
|进程用户态CPU占用率|
|进程内核态CPU占用率|
|进程内存使用率|

对系统级的监控是伴随着程序的启动进行的，对进程级的监控进程名字通过传参进行配置，如无则不进行监控，
监控的时间间隔采用程序传参进行配置，同时，可以通过传参配置选择监控结果为实时显示还是记录日志，日
志记录系统基于zlog进行管理，代码以c进行开发，实现更加快速，轻量化的监控

监控处理程序关注的是一种“态势”

![Alt text](./read.png)

如上图示，为监控两个进程是示图，则
pid_1的cpu及内存的占用率对应的总值为t1-t2
pid_2的cpu及内存的占用率对应的总值为t2-t3

而这一次的系统级的监控则为
t0-t1 t1-t2 t2-t3的三次平均的结果，


```bash
smm_v1.0 
         
 Usage   :         
    smm [options] -t <Monitoring period> -p {[app_name1];[name2];..}        
    options        
        -h,--help                          get app help        
        -d,--display                       display monitoring results        
        -l,--log                           record monitoring results use zlog 
```


记录的日志采用markdown的格式，需要自己添加一个头即可形成表格的形式。
通过下面的网址，可以很方便的转换成多种形式的表格进行数据分析。
https://tableconvert.com
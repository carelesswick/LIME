#include "sysinfolinuximpl.h"
#include <sys/types.h>
#include <sys/sysinfo.h>
#include <QFile>
#include <QDebug>
#include <QProcess>
sysinfolinuximpl::sysinfolinuximpl()
{

}
/*计算CPU占用率的值,通过读取/proc/stat中的数据进行计算,其具体公式在报告中*/
double sysinfolinuximpl::cpuLoadAverage()
{
    QProcess process;
    process.start("cat /proc/stat");
    process.waitForFinished();
    QString str = process.readLine();
    str.replace("\n","");
    str.replace(QRegExp("( ){1,}")," ");
    auto lst = str.split(" ");
    if(lst.size() > 3)
    {
        double use = lst[1].toDouble() + lst[2].toDouble() + lst[3].toDouble();
        double total = 0;
        for(int i = 1;i < lst.size();++i)
            total += lst[i].toDouble();
        if(total - pre_total > 0)
        {
            cpu_rate =(use - pre_user) / (total - pre_total) * 100.0;
            pre_total = total;
            pre_user = use;


        }
    }
    return cpu_rate;
}
/*计算内存的值*/
double sysinfolinuximpl::get_mem_usage__()
{
    QProcess process;
    double free =0.0;
    double total =0.0;
    process.start("free -m"); //使用free完成获取
    process.waitForFinished();
    process.readLine();
    QString str = process.readLine();
    str.replace("\n","");
    str.replace(QRegExp("( ){1,}")," ");//将连续空格替换为单个空格 用于分割
    auto lst = str.split(" ");
    if(lst.size() > 6)
    {
        free = lst[6].toDouble();
        total = lst[1].toDouble();
        mem_rate = (total-free) / total * 100.0;
        return mem_rate;
    }
    return false;
}

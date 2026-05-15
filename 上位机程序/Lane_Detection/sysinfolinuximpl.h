#ifndef SYSINFOLINUXIMPL_H
#define SYSINFOLINUXIMPL_H
#include <QtGlobal>
#include <QVector>
#include <QProcess>

class sysinfolinuximpl
{
public:
     sysinfolinuximpl();
     double cpuLoadAverage();
     double get_mem_usage__();
private:
   // QVector<qulonglong> cpuRawData();
private:
     double pre_user=0;
     double pre_total=0;
     double cpu_rate = 0.0;
     double mem_rate = 0.0;

};

#endif // SYSINFOLINUXIMPL_H

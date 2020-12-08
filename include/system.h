#ifndef SYSTEM_H
#define SYSTEM_H

#include <string>
#include <vector>

#include "process.h"
#include "processor.h"
#include "usage_stats.h"

class System : public UsageStats {
 public:
  System();
  std::vector<Process>& Processes();  // TODO: See src/system.cpp
  float MemoryUtilization() const;    // TODO: See src/system.cpp
  long UpTime() const;                // TODO: See src/system.cpp
  float CpuUtilization();
  int TotalProcesses();           // TODO: See src/system.cpp
  int RunningProcesses();         // TODO: See src/system.cpp
  std::string Kernel();           // TODO: See src/system.cpp
  std::string OperatingSystem();  // TODO: See src/system.cpp

  // TODO: Define any necessary private members
 private:
  Processor cpu_ = {};
  std::vector<Process> processes_ = {};
  std::string kernel_;
  std::string os_;
};

#endif

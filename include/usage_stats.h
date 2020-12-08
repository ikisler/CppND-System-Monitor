#ifndef STATS_H
#define STATS_H

#include <string>
#include <vector>

class UsageStats {
 public:
  virtual float MemoryUtilization() const = 0;
  virtual float CpuUtilization() = 0;
  virtual long UpTime() const = 0;
};

#endif

#include "process.h"
#include "linux_parser.h"

#include <unistd.h>

#include <cctype>
#include <sstream>
#include <string>
#include <vector>

using std::string;
using std::to_string;
using std::vector;

// Return this process's ID
int Process::Pid() {
  return pid_;
}

// Return this process's CPU utilization
/*
  NOTE!
  The algorithm to calculate this is derived from the guide here:
  https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat/16736599#16736599

  Retrieved 12/05/2020
*/
float Process::CpuUtilization() {
  cpu_ = LinuxParser::ProcessCpuTimeUsage(pid_) / UpTime();
  return cpu_;
}

// Return the command that generated this process
string Process::Command() {
  return LinuxParser::Command(pid_);
}

// Return this process's memory utilization
string Process::Ram() {
  return LinuxParser::Ram(pid_);
}

// Return the user (name) that generated this process
string Process::User() {
  return LinuxParser::User(pid_);
}

// Return the age of this process (in seconds)
long int Process::UpTime() {
  return LinuxParser::UpTime(pid_);
}

// Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const {
  return cpu_ > a.cpu_; // ">" is NOT a typo -- want to sort the processes in DESC not ASC order
}

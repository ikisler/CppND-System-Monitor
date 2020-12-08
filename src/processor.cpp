#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#include <string>
#include <vector>

#include "processor.h"
#include "linux_parser.h"

using std::vector;
using std::string;

float Processor::CalculateIdle(vector<string> values) {
  return std::stof(values[LinuxParser::CPUStates::kIdle_]) + std::stof(values[LinuxParser::CPUStates::kIOwait_]);
}

float Processor::CalculateNonIdle(vector<string> values) {
  return std::stof(values[LinuxParser::CPUStates::kUser_]) + std::stof(values[LinuxParser::CPUStates::kNice_]) + std::stof(values[LinuxParser::CPUStates::kSystem_]) + std::stof(values[LinuxParser::CPUStates::kIRQ_]) + std::stof(values[LinuxParser::CPUStates::kSoftIRQ_]) + std::stof(values[LinuxParser::CPUStates::kSteal_]);
}

// Return the aggregate CPU utilization
float Processor::Utilization() {
  if (old_idle_ == 0 || old_non_idle_ == 0) {
    // This branch is for when the program is initially started and we have no info to based our utilization on
    vector<string> old_values = LinuxParser::CpuUtilization();
    old_idle_ = CalculateIdle(old_values);
    old_non_idle_ = CalculateNonIdle(old_values);
    sleep(1);
  }
  vector<string> new_values = LinuxParser::CpuUtilization();

  /*
    PLEASE NOTE!
    The below algorithm for determining the CPU utilization was adapted from
    the answer provided on StackOverflow here:
    https://stackoverflow.com/a/23376195
    Retrieved: 12/01/2020
    If the thread seems familiar, it's because it was linked in the project
    description as a resource available for use
  */

  float new_idle = CalculateIdle(new_values);
  float new_non_idle = CalculateNonIdle(new_values);

  float old_total = old_idle_ + old_non_idle_;
  float new_total = new_idle + new_non_idle;

  float total_difference = new_total - old_total;
  float idle_difference = new_idle - old_idle_;

  // Update our old usage numbers for the next time we need to calculate this
  old_idle_ = new_idle;
  old_non_idle_ = new_non_idle;

  return (total_difference - idle_difference) / total_difference;
}

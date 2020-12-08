#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// Read and return the system memory utilization
// Returns the percentage of memory utilized as a float (so 50% used would return 0.5)
// Calculates usage like the `free` command: total memory - free memory - buffer - cache
float LinuxParser::MemoryUtilization() {
  string line, key, value;
  vector<string> values;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    while(std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      values.push_back(value);
      if (key == "Cached:") {
          // Only need the first four lines, so getta outta here once we hit this line
          break;
      }
    }
  }

  int intMemTotal = std::stoi(values[0]);

  return float(intMemTotal - std::stoi(values[1]) - std::stoi(values[2]) - std::stoi(values[3])) / intMemTotal;
}

// Read and return the system uptime
long LinuxParser::UpTime() {
  string uptime;
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
  }
  return std::stoll(uptime);
}

// Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
// investigate CPUStates enum
  vector<string> values;
  string value, line;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while(linestream >> value) {
      if (value != "cpu") {
        values.push_back(value);
      }
    }
  }
  return values;
}

// Gets and sums values used for calculating CPU usage
// utime
// stime
// cutime
// cstime
// Returns a float measured in SECONDS not clockticks
float LinuxParser::ProcessCpuTimeUsage(int pid) {
  string value, line;
  int i = 1;
  long sum;
  std::ifstream stream(kProcDirectory + "/" + to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while(linestream >> value) {
      // Need to get the 14 - 17th values from the file
      if (i >= 14) {
        sum += stof(value);
      }

      // Not interested in any values after the 17th
      if (i == 17) {
        break;
      }
      i++;
    }
  }

  return sum / sysconf(_SC_CLK_TCK);
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  return std::stoi(LinuxParser::getValueFromFileWithKey(kProcDirectory + kStatFilename, "processes"));
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  return std::stoi(LinuxParser::getValueFromFileWithKey(kProcDirectory + kStatFilename, "procs_running"));
}

// Read and return the command associated with a process
string LinuxParser::Command(int pid) {
  string line;
  std::ifstream stream(kProcDirectory + "/" + to_string(pid) + kCmdlineFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
  }
  return line;
}

// Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  string value = LinuxParser::getValueFromFileWithKey(kProcDirectory + "/" + to_string(pid) + kStatusFilename, "VmSize:");

  // Convert value to MB
  if (value != "") {
    return to_string(std::stoi(value) / 1000);
  }

  return string();
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  return LinuxParser::getValueFromFileWithKey(kProcDirectory + "/" + to_string(pid) + kStatusFilename, "Uid:");
}

// Read and return the user associated with a process
string LinuxParser::User(int pid) {
  string line, username, x, uid, process_uid;

  process_uid = LinuxParser::Uid(pid);

  std::ifstream stream(kPasswordPath);
  if (stream.is_open()) {
    while(std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> username >> x >> uid;
      if (uid == process_uid) {
        return username;
      }
    }
  }
  return string();
}

// Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  string line, uptime, value;
  int const index_location_of_uptime_value = 22; // Indexed starting at 1, not 0
  int i = 1;

  std::ifstream stream(kProcDirectory + "/" + to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while(linestream >> value) {
      if (i == index_location_of_uptime_value) {
        return LinuxParser::UpTime() - (std::stoll(value) / sysconf(_SC_CLK_TCK));
      }
      i++;
    }
  }
  return 0; // shouldn't get here, but just in case
}

string LinuxParser::getValueFromFileWithKey(string filepath, string key) {
  string line, keyFound, value;
  std::ifstream stream(filepath);
  if (stream.is_open()) {
    while(std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> keyFound >> value;
      if (keyFound == key) {
        return value;
      }
    }
  }
  return string();
}

#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <string>
#include <vector>

class Processor {
 public:
  float Utilization();  // TODO: See src/processor.cpp

  // TODO: Declare any necessary private members
 private:
  float old_idle_;
  float old_non_idle_;
  float CalculateIdle(std::vector<std::string> values);
  float CalculateNonIdle(std::vector<std::string> values);
};

#endif

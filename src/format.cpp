#include "format.h"

#include <string>

using std::string;

string Format::PadIntToTwoCharacters(int n) {
  if (n < 10) {
    return "0" + std::to_string(n);
  }
  return std::to_string(n);
}

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) {
  int total_minutes = seconds / 60;  // 60 seconds in a minute
  int remaining_seconds =
      seconds %
      60;  // what's left after the minutes, when 60 seconds are in a minute
  int hours = total_minutes / 60;  // 60 minutes in an hour
  int remaining_minutes =
      total_minutes - (hours * 60);  // subtract hours converted to minutes

  return PadIntToTwoCharacters(hours) + ":" +
         PadIntToTwoCharacters(remaining_minutes) + ":" +
         PadIntToTwoCharacters(remaining_seconds);
}

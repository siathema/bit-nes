#include <stdio.h>
#include "b_log.h"

namespace BITNES
{
  bool Logged = false;

  void Log(const char* message) {
    FILE* logFile = 0;

    if(!Logged) {
      logFile = fopen(LOG_PATH, "w");
      Logged = true;
    }
    else {
      logFile = fopen(LOG_PATH, "a");
    }
    if(logFile == 0) {
      if(Logged) Logged = false;
      return;
    }
    else {
      fputs(message, logFile);
    }
    fclose(logFile);
  }

  void LogERROR(const char* message) {
    Log("ERROR: ");
    Log(message);
  }
}

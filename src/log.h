#ifndef _B_LOG_H
#define _B_LOG_H

#define LOG_PATH "bitnes.log"

extern bool Logged;

void Log(const char* message);
void LogERROR(const char* message);
#endif

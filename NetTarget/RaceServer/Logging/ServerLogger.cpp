// ServerLogger.cpp : Logging implementation

#include "stdafx.h"
#include "ServerLogger.h"
#include <time.h>
#include <stdarg.h>

MR_ServerLogger::MR_ServerLogger()
    : mpLogFile(NULL), mMinLevel(MR_LOG_DEBUG)
{
    InitializeCriticalSection(&mLock);
}

MR_ServerLogger::~MR_ServerLogger()
{
    Close();
    DeleteCriticalSection(&mLock);
}

BOOL MR_ServerLogger::Initialize(const char* logfilePath, int minLevel)
{
    EnterCriticalSection(&mLock);

    mMinLevel = minLevel;
    
    // Open log file in append mode
    errno_t err = fopen_s(&mpLogFile, logfilePath, "a");
    if (err != 0 || mpLogFile == NULL) {
        LeaveCriticalSection(&mLock);
        return FALSE;
    }

    // Write header
    fprintf(mpLogFile, "\n");
    fprintf(mpLogFile, "===========================================\n");
    time_t now = time(NULL);
    fprintf(mpLogFile, "RaceServer started at: %s", ctime(&now));
    fprintf(mpLogFile, "===========================================\n");
    fflush(mpLogFile);

    LeaveCriticalSection(&mLock);
    return TRUE;
}

const char* MR_ServerLogger::LevelToString(int level)
{
    switch (level) {
        case MR_LOG_DEBUG: return "DEBUG";
        case MR_LOG_INFO:  return "INFO";
        case MR_LOG_WARN:  return "WARN";
        case MR_LOG_ERROR: return "ERROR";
        default:           return "????";
    }
}

void MR_ServerLogger::Log(int level, const char* format, ...)
{
    if (level < mMinLevel) {
        return;  // Skip logs below minimum level
    }

    EnterCriticalSection(&mLock);

    if (mpLogFile) {
        // Get current time
        time_t now = time(NULL);
        struct tm timeinfo;
        localtime_s(&timeinfo, &now);
        char timestr[32];
        strftime(timestr, sizeof(timestr), "%Y-%m-%d %H:%M:%S", &timeinfo);

        // Write formatted message
        fprintf(mpLogFile, "[%s] [%s] ", timestr, LevelToString(level));

        va_list args;
        va_start(args, format);
        vfprintf(mpLogFile, format, args);
        va_end(args);

        fprintf(mpLogFile, "\n");
        fflush(mpLogFile);
    }

    // Also print to console for errors
    if (level >= MR_LOG_ERROR) {
        va_list args;
        va_start(args, format);
        vprintf(format, args);
        va_end(args);
        printf("\n");
    }

    LeaveCriticalSection(&mLock);
}

void MR_ServerLogger::Flush()
{
    EnterCriticalSection(&mLock);
    if (mpLogFile) {
        fflush(mpLogFile);
    }
    LeaveCriticalSection(&mLock);
}

void MR_ServerLogger::Close()
{
    EnterCriticalSection(&mLock);
    if (mpLogFile) {
        fclose(mpLogFile);
        mpLogFile = NULL;
    }
    LeaveCriticalSection(&mLock);
}

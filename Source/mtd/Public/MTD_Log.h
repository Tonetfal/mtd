#pragma once

DECLARE_LOG_CATEGORY_CLASS(LogMtd, All, All);

#define EXEC_INFO_FORMAT "%s::%d: "
#define EXEC_INFO *FString(__FUNCTION__), __LINE__

#define _MTD_LOG_IMPL(VERBOSITY, FORMAT, ...) \
	UE_LOG(LogMtd, VERBOSITY, TEXT(EXEC_INFO_FORMAT FORMAT), EXEC_INFO, __VA_ARGS__)

#define MTD_FATAL(FORMAT, ...)		_MTD_LOG_IMPL(Fatal,		FORMAT, __VA_ARGS__)
#define MTD_ERROR(FORMAT, ...)		_MTD_LOG_IMPL(Error,		FORMAT, __VA_ARGS__)
#define MTD_WARN(FORMAT, ...)		_MTD_LOG_IMPL(Warning,		FORMAT, __VA_ARGS__)
#define MTD_DISPLAY(FORMAT, ...)	_MTD_LOG_IMPL(Display,		FORMAT, __VA_ARGS__)
#define MTD_LOG(FORMAT, ...)		_MTD_LOG_IMPL(Log,			FORMAT, __VA_ARGS__)
#define MTD_VERBOSE(FORMAT, ...)	_MTD_LOG_IMPL(Verbose,		FORMAT, __VA_ARGS__)
#define MTD_VVERBOSE(FORMAT, ...)	_MTD_LOG_IMPL(VeryVerbose,	FORMAT, __VA_ARGS__)

#define S_EXEC_INFO_FORMAT "%s::%d %s: "
#define S_EXEC_INFO *FString(__FUNCTION__), __LINE__, *GetNameSafe(this)

#define _MTDS_LOG_IMPL(VERBOSITY, FORMAT, ...) \
    UE_LOG(LogMtd, VERBOSITY, TEXT(S_EXEC_INFO_FORMAT FORMAT), S_EXEC_INFO, __VA_ARGS__)

#define MTDS_FATAL(FORMAT, ...)     do { _MTDS_LOG_IMPL(Fatal,			FORMAT, __VA_ARGS__); } while(0)
#define MTDS_ERROR(FORMAT, ...)     do { _MTDS_LOG_IMPL(Error,			FORMAT, __VA_ARGS__); } while(0)
#define MTDS_WARN(FORMAT, ...)      do { _MTDS_LOG_IMPL(Warning,		FORMAT, __VA_ARGS__); } while(0)
#define MTDS_DISPLAY(FORMAT, ...)   do { _MTDS_LOG_IMPL(Display,		FORMAT, __VA_ARGS__); } while(0)
#define MTDS_LOG(FORMAT, ...)       do { _MTDS_LOG_IMPL(Log,			FORMAT, __VA_ARGS__); } while(0)
#define MTDS_VERBOSE(FORMAT, ...)   do { _MTDS_LOG_IMPL(Verbose,		FORMAT, __VA_ARGS__); } while(0)
#define MTDS_VVERBOSE(FORMAT, ...)  do { _MTDS_LOG_IMPL(VeryVerbose,	FORMAT, __VA_ARGS__); } while(0)

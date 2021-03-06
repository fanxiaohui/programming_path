/*
 * clog.h
 *
 *  Created on: 2017年3月25日
 *      Author: cj
 */

#ifndef SRC_CLOG_H_
#define SRC_CLOG_H_

#undef CONFIG_CLOG_BASIC

#ifdef CONFIG_CLOG_BASIC

#include <stdio.h>
#include <stdarg.h>
#include <fcntl.h>
#include <sys/prctl.h>

#define log_i(format,...) do { \
		printf("I:%s,%s,%d:"format"\n", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__);  fflush(stdout); } while(0)

#define log_d(format,...)  do { \
		printf("D:%s,%s,%d:"format"\n", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__);  fflush(stdout); } while(0)

#define log_e(format,...)  do { \
		printf("e:%s,%s,%d:"format"\n", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__);  fflush(stdout); } while(0)

#else

#include <stdio.h>
#include <stdarg.h>
#include <sys/syslog.h>

//GCC标准规定必须为定义过的符号,因此，不能使用##连接字符串
//#define LOGFRM(STR,F)	STR##F
//openlog("mode", LOG_CONS | LOG_PID, 0);
//syslog(LOG_DEBUG, "...");
//closelog();
#define logs_d(format,...)	syslog(LOG_DEBUG,"D:%s,%d,"format,__FUNCTION__, __LINE__, ##__VA_ARGS__)
#define logs_i(format,...)	syslog(LOG_INFO,"I:%s,%d,"format,__FUNCTION__, __LINE__, ##__VA_ARGS__)
#define logs_e(format,...)	syslog(LOG_ERR,"E:%s,%d,"format,__FUNCTION__, __LINE__, ##__VA_ARGS__)

//日志极别
typedef enum log_level {
	CLOG_NON = 0x00,
	CLOG_INFO = 0x01,
	CLOG_WRING = 0x02,
	CLOG_ERR = 0x03,
	CLOG_DEBUG = 0x04,
	CLOG_HEX = 0x05,
	CLOG_ALL = 0xFF,
} LOG_LEVEL;

void log_setout_file(const char *file);

//tcp://192.168.0.12:8080
void log_setout_tcp(const char *ipv4str);

extern int log_setlevel(int level);

typedef int log_backcallfun(int level, const char *mode, const char *file,
		const char *fun, int line, const char *fmt, va_list ap);

extern void log_setbackfun(log_backcallfun *fun);

extern void log_get_pthread_curname(char name[16]);

extern int log_printf(int level, const char *mode, const char *file,
		const char *fun, int line, const char *format, ...);

extern int log_printf_hex(int _level, const char *file, const char *function,
		int line, const void *data, int dlen, const char *format, ...);

extern int log_printf2(unsigned char flag, int level, const char *file,
		const char *function, int line, const char *format, ...);

#define log_i(format,...) log_printf(CLOG_INFO,NULL,__FILE__,__FUNCTION__,__LINE__,format, ##__VA_ARGS__)
#define log_d(format,...) log_printf(CLOG_DEBUG,NULL,__FILE__,__FUNCTION__,__LINE__,format, ##__VA_ARGS__)
#define log_e(format,...) log_printf(CLOG_ERR,NULL,__FILE__,__FUNCTION__,__LINE__,format, ##__VA_ARGS__)
#define log_w(format,...) log_printf(CLOG_WRING,NULL,__FILE__,__FUNCTION__,__LINE__,format, ##__VA_ARGS__)

#define log_d2(flag, format, ...)  log_printf2(flag, CLOG_DEBUG,__FILE__,__FUNCTION__,__LINE__,format, ##__VA_ARGS__)

#define log_hex(data,dlen,format,...) log_printf_hex(CLOG_HEX,__FILE__,__FUNCTION__,__LINE__,data,dlen, format, ##__VA_ARGS__)

#endif

#endif /* SRC_CLOG_H_ */

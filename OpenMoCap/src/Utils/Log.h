/*
 * Log.h
 *
 *  Created on: 25/09/2009
 *      Author: laertemateus
 */

#ifndef LOG_H_
#define LOG_H_

#include <iostream>
#include <cstdarg>
#include <cstdio>
#include <ctime>
#include <qobject.h>
#include <pantheios/pantheios.hpp>
#include <pantheios/backends/be.lrsplit.h>
#include <pantheios/backends/bec.fprintf.h>
#include <pantheios/backends/bec.file.h>

#define log(args...) Log::getInstance()->printLog(Log::getInstance()->getDefaultSeverity(),__LINE__,__FILE__,args)
#define logALERT(args...) Log::getInstance()->printLog(pantheios::SEV_ALERT,__LINE__,__FILE__,args)
#define logDEBUG(args...) Log::getInstance()->printLog(pantheios::SEV_DEBUG,__LINE__,__FILE__,args)
#define logCRITICAL(args...) Log::getInstance()->printLog(pantheios::SEV_CRITICAL,__LINE__,__FILE__,args)
#define logEMERGENCY(args...) Log::getInstance()->printLog(pantheios::SEV_EMERGENCY,__LINE__,__FILE__,args)
#define logERROR(args...) Log::getInstance()->printLog(pantheios::SEV_ERROR,__LINE__,__FILE__,args)
#define logINFORMATIONAL(args...) Log::getInstance()->printLog(pantheios::SEV_INFORMATIONAL,__LINE__,__FILE__,args)
#define logNOTICE(args...) Log::getInstance()->printLog(pantheios::SEV_NOTICE,__LINE__,__FILE__,args)
#define logWARNING(args...) Log::getInstance()->printLog(pantheios::SEV_WARNING,__LINE__,__FILE__,args)

class Log {

private:
	static Log* _instance;

	pantheios::pan_severity_t _defaultSeverity;
	std::string _pattern;

	Log();
public:
	static Log* getInstance();

	void printLog(pantheios::pan_severity_t sev,int,const char*,...);

	void setPattern(std::string);
	void setDefaultSeverity(pantheios::pan_severity_t);
	void setLogFile(const char*);
	pantheios::pan_severity_t getDefaultSeverity();
};

#endif /* LOG_H_ */

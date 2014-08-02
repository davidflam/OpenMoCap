/*
 * Log.cpp
 *
 *  Created on: 25/09/2009
 *      Author: Laerte Mateus
 *      Author: David Lunardi Flam
 */

#include "Log.h"

Log* Log::_instance = NULL;

Log* Log::getInstance() {
	if (!_instance) {
		_instance = new Log;
	}
	return _instance;
}

Log::Log() {
	_defaultSeverity = pantheios::SEV_INFORMATIONAL;
	_pattern = "%D %T %S: ";
}

void Log::setPattern(std::string pattern) {
	_pattern = pattern;
}

void Log::setLogFile(const char* file) {
	pantheios_be_file_setFilePath(file, PANTHEIOS_BE_FILE_F_TRUNCATE,
			PANTHEIOS_BE_FILE_F_TRUNCATE, PANTHEIOS_BEID_LOCAL);
}

void Log::printLog(pantheios::pan_severity_t sev, int line, const char* source,
		...) {
	va_list args; //Function arguments
	std::string out; //Output
	std::string message; //full format message [_pattern + messagePattern]
	char buff[40]; //buffer
	const char* messagePattern;
	time_t tim; //Date
	tm *now;

	//Get the actual timestamp
	tim = time(NULL);
	now = localtime(&tim);

	va_start(args,source);
	messagePattern = va_arg(args,const char*);

	//Builde message
	message = _pattern;
	message += messagePattern;

	//Change %D %T %S by your values
	for (unsigned int i = 0; i < strlen(message.c_str()); i++) {
		char c = *(message.c_str() + i);

		if (c == '%') {
			c = *(message.c_str() + (++i)); //Next char

			switch (c) {
			case 'D': //Date
				sprintf(buff, "%2.2d/%2.2d/%2.2d", now->tm_mday, now->tm_mon
						+ 1, now->tm_year + 1900);
				out += buff;
				break;
			case 'T': //Time
				sprintf(buff, "%2.2d:%2.2d:%2.2d", now->tm_hour, now->tm_min,
						now->tm_sec);
				out += buff;
				break;
			case 'L': //Line of source code
				itoa(line, buff, 10);
				out += buff;
				break;
			case 'F': //Nome do arquivo fonte
				out += source;
				break;
			case 'S': //Severity
				out += "[";
				switch (sev) {
				case pantheios::SEV_ALERT:
					out += "ALERT";
					break;
				case pantheios::SEV_CRITICAL:
					out += "CRITICAL";
					break;
				case pantheios::SEV_DEBUG:
					out += "DEBUG";
					break;
				case pantheios::SEV_EMERGENCY:
					out += "EMERGENCY";
					break;
				case pantheios::SEV_ERROR:
					out += "ERROR";
					break;
				case pantheios::SEV_INFORMATIONAL:
					out += "INFORMATIONAL";
					break;
				case pantheios::SEV_NOTICE:
					out += "NOTICE";
					break;
				case pantheios::SEV_WARNING:
					out += "WARNING";
					break;
				}
				out += "]";
				break;
			default: //Reprint pattern to show in vsprintf
				out += "%";
				out += c;
				break;
			}
		} else {
			out += c;
		}
	}

	pantheios::pantheios_logvprintf(sev, out.c_str(), args);
}

void Log::setDefaultSeverity(pantheios::pan_severity_t defaultSeverity) {
	_defaultSeverity = defaultSeverity;
}

pantheios::pan_severity_t Log::getDefaultSeverity() {
	return _defaultSeverity;
}

PANTHEIOS_CALL(void) pantheios_be_fprintf_getAppInit(int backEndId,
		pan_be_fprintf_init_t* init) {
	init->flags = PANTHEIOS_BE_INIT_F_CUSTOM_MASK
			| PANTHEIOS_BE_INIT_F_NO_DATETIME
			| PANTHEIOS_BE_INIT_F_NO_PROCESS_ID
			| PANTHEIOS_BE_INIT_F_NO_SEVERITY
			| PANTHEIOS_BE_INIT_F_NO_THREAD_ID;
}

PANTHEIOS_CALL(void) pantheios_be_file_getAppInit(int backEndId,
		pan_be_file_init_t* init) {
	init->flags = PANTHEIOS_BE_INIT_F_CUSTOM_MASK
			| PANTHEIOS_BE_INIT_F_NO_DATETIME
			| PANTHEIOS_BE_INIT_F_NO_PROCESS_ID
			| PANTHEIOS_BE_INIT_F_NO_SEVERITY
			| PANTHEIOS_BE_INIT_F_NO_THREAD_ID;

	init->fileName = "OpenMoCap.log";
}

PANTHEIOS_CALL(int) pantheios_fe_init(void* reserved, void** ptoken) {
	*ptoken = NULL;
	return 0;
}

PANTHEIOS_CALL(void) pantheios_fe_uninit(void* token) {
}

PANTHEIOS_CALL(char const*) pantheios_fe_getProcessIdentity(void* token) {
	return "OpenMoCap";
}

PANTHEIOS_CALL(int) pantheios_fe_isSeverityLogged(void* token, int severity,
		int backEndId) {
	return 1;
}

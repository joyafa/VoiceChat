/************************FreeBSD license header*****************************
 * Copyright (c) 2012, Wilston Oreo
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ***********************************************************************/


#pragma once 
#define LOGGING

 // general features switches

#define OUTPUT_TIME
#define OUTPUT_COUNTER
//#define OUTPUT_TIMER
#define OUTPUT_COLOR
#define OUTPUT_MSG
#define OUTPUT_SRC
//#define THREAD_SAFE

// in case you use OUTPUT_COLOR ...

#define COLOR_ERR 31 //31
#define COLOR_MSG 37 //37
#define COLOR_WRN 33 //33
#define COLOR_FILE 35 //35
#define COLOR_LINE 32 //32
#define COLOR_FUNC 36 //36
#define DELIMITER '|'

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <map>
#include <time.h>

#ifdef THREAD_SAFE
#include <boost/thread/recursive_mutex.hpp>
#endif 

using namespace std;

// From:
// http://www2.hawaii.edu/~yucheng/projects/clocks/jade-netdelay/?file=jade-netdelay.c
#ifdef OUTPUT_TIMER
static void timespec_normalize(timespec * t)
{ 	/* Eliminate overflows. */
	while (t->tv_nsec > 1e9L) { t->tv_nsec -= 1e9L; t->tv_sec++; }
	/* Eliminate underflows. */
	while (t->tv_nsec < 0L) { t->tv_nsec += 1e9L; t->tv_sec--; }
}
#endif


/// @brief: Simple struct for formatting strings with parameters
struct fmt
{
	fmt(const string& s) { this->operator()(s); }
	fmt(const char* c) { this->operator()(c); }

	void operator()(const char* c)
	{
		this->operator()(string(c));
	}

	void operator()(const string& s)
	{
		nParams = 0; buf = s; size_t n = 0;
		for (size_t i = 0; i < s.length(); i++)
			if (s[i] == '%')
			{
				if (i > 0)
					if (s[i - 1] == '\\') continue;
				n++;
			}
		params.resize(n);
	}

	template<class T> fmt& operator%(const T &t)
	{
		if (nParams < params.size())
		{
			stringstream ss; ss << t;
			params[nParams] = ss.str();
			nParams++;
		}
		return *this;
	}

	string str() const
	{
		stringstream ss; size_t n = 0;
		for (size_t i = 0; i < buf.length(); i++)
		{
			char c = buf[i];
			switch (c) {
			case '%':
				if (i > 0)
					if (buf[i - 1] == '\\') continue;
				if (n >= params.size()) continue;
				ss << params[n]; n++;
				break;
			case '\\':
				if (i < buf.length() - 1)
					if (buf[i + 1] == '%')
					{
						ss << '%'; continue;
					}
			default:
				ss << c;
			}
		}
		return ss.str();
	}

	void reset()
	{
		nParams = 0;
		for (size_t i = 0; i < params.size(); i++)
			params[i] = "";
	}
private:
	string buf;
	vector<string> params;
	size_t nParams;
};

typedef unsigned int UINT;
/// @brief: Singleton class for Logging
class Log
{
	int level_;
	int curLevel;
	UINT _nCounter;
#ifdef OUTPUT_TIMER
	timespec ts;
#endif
	ostream* output_;
	static bool destroyed_;
	static Log *_instance;
	//文件方式写入日志
	ofstream _of;
	Log()
	{ 
		init(); 
	}

	Log(const Log&) 
	{
	}

	Log& operator=(const Log&) { return *this; }
	~Log()
	{
#ifdef THREAD_SAFE
		(*output_) << threadBuffers[boost::this_thread::get_id()];
#endif
		(*output_) << endl;
		_of.close();
		_instance = 0;
		destroyed_ = true;
	}

	static void create()
	{
		static Log theInstance;
		_instance = &theInstance;
	}

	static void onDeadReference()
	{
		create();

		new(_instance) Log;

#ifdef ATEXIT_FIXED
		atexit(killPhoenixLog);
#endif
		destroyed_ = false;
	}

	static void killPhoenixLog()
	{
		_instance->~Log();
	}

#ifdef THREAD_SAFE
	boost::recursive_mutex lock;
#define LOCK boost::recursive_mutex::scoped_lock l(lock);
#else
    #define LOCK
#endif

	template<class T> string colorStr(int color, int bold, char delimiter, const T& t) const
	{
		stringstream ss;
#ifdef OUTPUT_COLOR
		if (output_ == &cout)
		{
			ss << "\033[" << bold << ";" << color << "m";
		}
#endif
		ss << t;
#ifdef OUTPUT_COLOR
		if (output_ == &cout)
		{
			ss << "\033[0m";
		}
#endif
		ss << delimiter;

		return ss.str();
	}

	string logHeader(
		string& file,
		string& function,
		int linenumber,
		int _level,
		string& type);

	template <typename T> void appendThreadBuffer(const T& t)
	{
		stringstream ss; ss << t;
#ifdef THREAD_SAFE
		threadBuffers[boost::this_thread::get_id()] += ss.str();
		//换行符
		threadBuffers[boost::this_thread::get_id()] += "\r\n";
#endif 
#ifndef THREAD_SAFE
		(*output_) << ss.str() << endl;
#endif
	}

public:
	void init();

	ostream* GetOutputMode() const
	{
		return output_;
	}

	//外部可以设置输出方式
	void SetOutputMode(ostream* _output)
	{
		output_ = _output;
	}

	void resetTimer()
	{
#ifdef OUTPUT_TIMER
		clock_gettime(CLOCK_REALTIME, &ts);
#endif		
	}

	int GetLogLevel() const 
	{
		return level_; 
	}

	void SetLogLevel(int _level)
	{
		level_ = (_level > 10) ? 10 : (_level < 0) ? 0 : _level;
	}

	Log& log(
		string file,
		string function,
		int linenumber,
		int _level,
		string type);

	inline Log& operator<< (fmt& f)
	{
#ifdef LOGGING
		if (curLevel <= level_)
			appendThreadBuffer(f.str());
#endif
		return *this;
	}

	template <typename T> inline Log& operator<< (const T& t)
	{
#ifdef LOGGING
		if (curLevel <= level_)
		{
			appendThreadBuffer(t);
		}
#endif
		return *this;
	}

	static Log* instance()
	{
		if (!_instance)
		{
			if (destroyed_)
				onDeadReference();
			else
				create();
		}
		return _instance;
	}


#ifdef THREAD_SAFE
	map<boost::thread::id, std::string> threadBuffers;
#endif
};

#define LOG Log::instance() 
#define LOG_INIT 


#define LOG_(lvl,type) LOG->log(__FILE__,__FUNCTION__,__LINE__,(lvl),(type))

//外部调用的宏定义
#define LOG_SETLEVEL(lv) LOG->SetLogLevel((lv))
#define LOG_MSG LOG_(0,"MSG")
#define LOG_WRN LOG_(1,"WRN")
#define LOG_ERR LOG_(2,"ERR")

#define LOG_MSG_(lvl) LOG_((lvl),"MSG")
#define LOG_WRN_(lvl) LOG_((lvl),"WRN") 
#define LOG_ERR_(lvl) LOG_((lvl),"ERR")
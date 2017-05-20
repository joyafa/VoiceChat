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
#include "StdAfx.h"
#include "Log.h"
Log *Log::_instance = 0;
bool Log::destroyed_ = false;

string Log::logHeader(
	string& file,
	string& function,
	int linenumber,
	int _level,
	string& type)
{
#ifdef LOGGING
	stringstream ss;
	int color = 39;
#ifdef OUTPUT_COLOR				    
	//former "default" values	
	if (type == "ERR")
	{
		color = COLOR_ERR; //31;  
	}
	if (type == "MSG")
	{
		color = COLOR_MSG; //37; 
	}
	if (type == "WRN")
	{
		color = COLOR_WRN; //33;
	}
#endif 

#ifdef OUTPUT_COUNTER
	ss << colorStr(color, 0, DELIMITER, _nCounter);
	_nCounter++;
#endif 

#ifdef OUTPUT_TIME
	time_t rawtime;
	struct tm * timeinfo;
	char timeBuffer[30];
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	strftime(timeBuffer, 30, "%Y/%m/%d-%H:%M:%S", timeinfo);
	ss << colorStr(color, 0, DELIMITER, timeBuffer);
#endif

#ifdef OUTPUT_TIMER
	timespec newTime;
	clock_gettime(CLOCK_REALTIME, &newTime);
	timespec difference;
	/* Add and two values. */
	difference.tv_sec = newTime.tv_sec - ts.tv_sec;
	difference.tv_nsec = newTime.tv_nsec - ts.tv_nsec; /* Return the normalized difference. */
	timespec_normalize(&difference);
	double diff = difference.tv_sec + difference.tv_nsec / 1000000000.0;
	ss << colorStr(color, 0, DELIMITER, diff);
#endif 

#ifdef OUTPUT_MSG
	stringstream st; st << type << _level;
	ss << colorStr(color, 1, DELIMITER, st.str());
#endif

#ifdef OUTPUT_SRC
	int pPos = 0;
	for (int i = file.length() - 1; i > 0; i--)
		if (file.substr(i, 1) == string("/"))
		{
			pPos = i + 1; break;
		}
	file = file.substr(pPos, file.length() - pPos);
	ss << colorStr(COLOR_FILE, 0, ':', file); //35
	ss << colorStr(COLOR_LINE, 1, ':', linenumber); //32
	ss << colorStr(COLOR_FUNC, 0, DELIMITER, function); //36
#endif
	return ss.str();
#endif
#ifndef LOGGING		
	return string();
#endif
}

void Log::init()
{
	_nCounter = 0;
	//缺省等级为1
	SetLogLevel(1);
	resetTimer();

	//TODO:修改, 文件或控制台
	time_t rawtime;
	struct tm * timeinfo;
	char timeBuffer[30];
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	strftime(timeBuffer, 30, "%Y%m%d", timeinfo);
	stringstream ss;
	ss << "Log" << timeBuffer << ".log";
	_of.open(ss.str(), ios::out | ios::app);
	output_ = &_of;
}

Log& Log::log(
	string file,
	string function,
	int linenumber,
	int _level,
	string type)
{
#ifdef LOGGING
	LOCK;
	curLevel = _level;
	//比设置等级低的输出
	if (curLevel > level_)
	{
		return *this;
	}
#ifdef THREAD_SAFE
	if (threadBuffers.count(boost::this_thread::get_id()))
	{
		(*output_) << threadBuffers[boost::this_thread::get_id()];
	}
#endif
	//	(*output_) << endl;
	*output_ << logHeader(file, function, linenumber, _level, type).c_str();

#ifdef THREAD_SAFE
	threadBuffers.erase(boost::this_thread::get_id());
#endif
#endif
	return *this;
}

/*
 * Copyright 1999-2004 The Apache Software Foundation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */



// Base header file.  Must be first.
#include <xalanc/Include/PlatformDefinitions.hpp>



#include <ctime>
#include <iomanip>



#include "Logger.hpp"



using namespace std;



Logger::Logger(ostream & stream) :
		m_stream(stream)
{	
	logText[eMessage] = "Message";
	logText[eWarning] = "Warning";
	logText[eError] = "Error";
}



ostream& 
Logger::message()
{
	return log(eMessage);
}



ostream& 
Logger::warning()
{
	return log(eWarning);
}



ostream& 
Logger::error()
{
	return log(eError);
}


ostream& 
Logger::log(eLogType logType)
{
	time_t theTime;
	time(&theTime);

	// Not thread safe
	char * timeStr = ctime(&theTime);
	timeStr[24] = '\0';

	m_stream << timeStr << setw(10) << logText[logType] << ": ";
	return m_stream;
}



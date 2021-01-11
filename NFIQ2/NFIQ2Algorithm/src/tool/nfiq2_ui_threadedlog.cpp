/******************************************************************************
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 ******************************************************************************/

#include <tool/nfiq2_ui_log.h>
#include <tool/nfiq2_ui_threadedlog.h>
#include <tool/nfiq2_ui_types.h>

#include <string>

// Responsible for logging within Multi-threaded operations
NFIQ2UI::ThreadedLog::ThreadedLog(const Flags &flags)
    : NFIQ2UI::Log::Log(flags, "")
{
	this->out = &ss;
}

// Gets the print result of the last processed image
std::string
NFIQ2UI::ThreadedLog::getAndClearLastScore()
{
	const std::string score = ss.str();
	ss.str("");
	return score;
}

NFIQ2UI::ThreadedLog::~ThreadedLog()
{
	this->out = nullptr;
}

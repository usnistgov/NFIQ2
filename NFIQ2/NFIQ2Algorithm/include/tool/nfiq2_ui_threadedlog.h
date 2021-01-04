/******************************************************************************
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 ******************************************************************************/

#ifndef NFIQ2_UI_THREADEDLOG_H_
#define NFIQ2_UI_THREADEDLOG_H_

#include "nfiq2_ui_log.h"
#include "nfiq2_ui_types.h"

#include <sstream>
#include <string>

namespace NFIQ2UI {

/**
 *  @brief
 *  Subclass of Log used for Multi-threaded batch operations.
 *
 *  @details
 *  Allows for scores to be logged by executeSingle and getImages and uses a
 *  stringstream instead of stdout or a filestream to generate those scores.
 */
class ThreadedLog : public Log {
    public:
	/**
	 *  @brief
	 *  Construct a ThreadedLog object with flags Argument passed in from
	 *  the command line.
	 *
	 *  @param[in] flags
	 *      Argument flags passed into the command line.
	 */
	ThreadedLog(const Flags &flags);

	/**
	 *  @brief
	 *  Gets the last score produced by a Multi-threaded operation.
	 *
	 *  @return
	 *      String version of the last score produced by a worker thread.
	 */
	std::string getAndClearLastScore();

	virtual ~ThreadedLog();

    private:
	/** Internal stringstream that scores get written to */
	std::stringstream ss;
};

} // namespace NFIQ2UI

#endif /* NFIQ2_UI_THREADEDLOG_H_ */

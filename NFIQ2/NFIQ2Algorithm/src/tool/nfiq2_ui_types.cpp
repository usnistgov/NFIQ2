/******************************************************************************
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 ******************************************************************************/

#include <be_io_recordstore.h>
#include <tool/nfiq2_ui_types.h>

#include <string>
#include <vector>

namespace BE = BiometricEvaluation;

// Windows specific macro
#ifdef min
#undef min
#endif

// Constructor for executeBatch splitQueue
NFIQ2UI::SafeSplitPathsQueue::SafeSplitPathsQueue(
    std::vector<std::string> &content,
    const std::vector<std::string>::size_type splittingFactor)
    : NFIQ2UI::SafeQueue<std::vector<std::string>>()
{
	while (!content.empty()) {
		std::vector<std::string> split;

		for (std::vector<std::string>::size_type i { 0 };
		     i < std::min(splittingFactor, content.size()); ++i) {
			split.emplace_back(content.back());
			content.pop_back();
		}

		pushUnsafe(split);
	}
}

// Constructor for executeRecordStore splitQueue
NFIQ2UI::SafeSplitPathsQueue::SafeSplitPathsQueue(
    std::shared_ptr<BE::IO::RecordStore> rs,
    const std::vector<std::string>::size_type splittingFactor)
    : NFIQ2UI::SafeQueue<std::vector<std::string>>()
{
	std::vector<std::string> content;

	for (auto i = rs->begin(); i != rs->end(); i++) {
		content.push_back(i->key);
	}

	while (!content.empty()) {
		std::vector<std::string> split;

		for (std::vector<std::string>::size_type i { 0 };
		     i < std::min(splittingFactor, content.size()); ++i) {
			split.emplace_back(content.back());
			content.pop_back();
		}

		pushUnsafe(split);
	}
}

/******************************************************************************
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 ******************************************************************************/

#ifndef NFIQ2_UI_TYPES_H_
#define NFIQ2_UI_TYPES_H_

#include <be_image_image.h>
#include <be_io_recordstore.h>
#include <nfiq2_algorithm.hpp>

#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <vector>

namespace NFIQ2UI {

/**
 *  @brief
 *  File containing enums, structs and data structures used throughout
 *  this NFIQ2 tool.
 */

/**
 *  @brief
 *  FileType Enumeration.
 *
 *  @details
 *  Outlines the different FileTypes that can be processed.
 */
enum class FileType { Standard = 0, AN2K = 1, ANSI2004 = 2, Unknown = 3 };

/**
 *  @brief
 *  Flags struct.
 *
 *  @details
 *  Contains the values of the optional flag arguments that can be passed
 *  into the command line.
 */
struct Flags {
	/** Verbose Flag value */
	bool verbose { false };
	/** Debug Flag value */
	bool debug { false };
	/** Speed Flag value */
	bool speed { false };
	/** Force Flag value */
	bool force { false };
	/** Recursion Flag value */
	bool recursion { false };
	/** Used if an alternative Machine Learning model is to be used */
	std::string model { "" };
	/** Actionable Flag value */
	bool actionable { false };
	/** Number of threads used for multi-threading */
	unsigned int numthreads { 1 };
};

/**
 *  @brief
 *  Contains the arguments needed for executeSingle to generate an NFIQ2 score.
 *
 *  @details
 *  The vectors contain the parsed files and directories
 *  provided in the command line.
 */
struct Arguments {
	/** Parsed Flags from the command line */
	NFIQ2UI::Flags flags;
	/** Name of executable (argv[0]) */
	std::string argv0;
	/** Optional path used to print all output to a file */
	std::string output;
	/** Stores all Single images that will get processed */
	std::vector<std::string> vecSingle;
	/** Stores all Directories that will get processed */
	std::vector<std::string> vecDirs;
	/** Stores all Batch file paths that will get processed */
	std::vector<std::string> vecBatch;
	/** Stores all RecordStores that will get processed */
	std::vector<std::string> vecRecordStore;
};

/**
 *  @brief
 *  This is the core return type for the image gathering functions.
 *
 *  @details
 *  This struct contains information for each image including a
 *  shared pointer to the image in memory, the name of the image,
 *  its finger position, and an optional warning message if applicable.
 */
struct ImgCouple {
	/** Shared pointer to image in memory */
	std::shared_ptr<BiometricEvaluation::Image::Image> img {};
	/** Finger position of image. Defaults to 0 (unknown) */
	uint8_t fingerPosition { 0 };
	/** Name of image */
	std::string imgName;
	/** Optional warning message */
	std::string warning { "" };

	ImgCouple(std::shared_ptr<BiometricEvaluation::Image::Image> img_,
	    uint8_t fingerPosition_, std::string imgName_, std::string warning_)
	    : img { img_ }
	    , fingerPosition { fingerPosition_ }
	    , imgName { imgName_ }
	    , warning { warning_ }
	{
	}
};

/**
 *  @brief
 *  Contains dimension information about an image
 *
 *  @details
 *  Contains image height, width and dpi. Required dpi is set
 *  to a default of 500 for NFIQ 2 compliance
 */
struct DimensionInfo {
	uint32_t imageHeight {};
	uint32_t imageWidth {};
	uint16_t imagePPI {};
	uint16_t requiredPPI { 500 };

	DimensionInfo(uint32_t imageHeight_, uint32_t imageWidth_,
	    uint16_t imagePPI_, uint16_t requiredPPI_)
	    : imageHeight { imageHeight_ }
	    , imageWidth { imageWidth_ }
	    , imagePPI { imagePPI_ }
	    , requiredPPI { requiredPPI_ }
	{
	}
};

/**
 *  @brief
 *  Contains image property information
 *
 *  @details
 *  Contains image name, its finger position, whether its been quantized
 *  or resampled and whether it was passed to NFIQ 2 as a single image
 */
struct ImageProps {
	std::string name {};
	uint8_t fingerPosition {};
	bool quantized {};
	bool resampled {};
	bool singleImage {};

	ImageProps(std::string name_, uint8_t fingerPosition_, bool quantized_,
	    bool resampled_, bool singleImage_)
	    : name { name_ }
	    , fingerPosition { fingerPosition_ }
	    , quantized { quantized_ }
	    , resampled { resampled_ }
	    , singleImage { singleImage_ }
	{
	}
};

/**
 *  @brief
 *  Thread Safe Concurrent Queue used for multi threaded batch operations.
 *
 *  @details
 *  A wrapper around a normal queue but with locking to ensure multiple threads
 *  can safely read and write from the queue.
 *
 *  Features an pushUnsafe method for use cases that do not require
 *  thread safe code.
 */
template <typename T> class SafeQueue {
    public:
	/**
	 *  @brief
	 *  Safely pushes an item onto the queue.
	 *
	 *  @param[in] item
	 *      The item to be pushed into the queue.
	 */
	void push(const T &item)
	{
		std::unique_lock<std::mutex> ulock(mutex_);
		queue_.push(item);
		ulock.unlock();
		cond_.notify_one();
	}

	/**
	 *  @brief
	 *  Safely pops an element from the front of the queue and returns it.
	 *
	 *  @return
	 *      Element at the front of the queue.
	 */
	T pop()
	{
		std::unique_lock<std::mutex> ulock(mutex_);
		while (queue_.empty()) {
			cond_.wait(ulock);
		}
		auto val = queue_.front();
		queue_.pop();
		ulock.unlock();
		return val;
	}

	/**
	 *  @brief
	 *  Checks if the queue is empty.
	 *
	 *  @return
	 *      Boolean value indicating whether the queue is empty.
	 */
	bool isEmpty()
	{
		std::unique_lock<std::mutex> ulock(mutex_);
		auto status = queue_.empty();
		ulock.unlock();
		cond_.notify_one();
		return status;
	}

	/**
	 *  @brief
	 *  Sets the number of threads being used for a multi-threaded
	 * application.
	 *
	 *  @param[in] threads
	 *      The number of threads being used.
	 */
	void setNumThreads(unsigned int numThreads)
	{
		numThreads_ = numThreads;
	}

	/**
	 *  @brief
	 *  Gets the number of threads to be used for a multi-threaded
	 * application.
	 *
	 *  @return
	 *      The number of threads being used.
	 */
	unsigned int getNumThreads() { return numThreads_; }

	/** Default Constructor */
	SafeQueue() = default;
	/** Prevents copying */
	SafeQueue(const SafeQueue &) = delete;

    protected:
	/**
	 *  @brief
	 *  Unsafely pushes an item to the queue.
	 *
	 *  @details
	 *  Used in operations limited to a single thread that does not require
	 *  thread safety elements.
	 *
	 *  @param[in] item
	 *      The item to be pushed into the queue.
	 */
	void pushUnsafe(const T &item) { queue_.push(item); }

    private:
	/** Standard queue wrapped around with locks */
	std::queue<T> queue_;
	/** Standard mutex */
	std::mutex mutex_;
	/** Allows for thread to thread communication */
	std::condition_variable cond_;
	/** Number of threads being used for a multi-threaded application */
	unsigned int numThreads_;
};

/**
 *  @brief
 *  Subclass of SafeQueue used for splitting up workloads.
 *
 *  @details
 *  Used in Multi-threaded operations to split up work into chunks
 *  of size splittingFactor.
 */
class SafeSplitPathsQueue : public SafeQueue<std::vector<std::string>> {
    public:
	/**
	 *  @brief
	 *  Splits up the workload for a Multi-threaded Batch operation.
	 *
	 *  @param[in] content
	 *      The work that is to be split up.
	 *  @param[in] splittingFactor
	 *      The size of each chunk of work.
	 */
	SafeSplitPathsQueue(std::vector<std::string> &content,
	    const std::vector<std::string>::size_type splittingFactor);

	/**
	 *  @brief
	 *  Splits up the workload for a Multi-threaded RecordStore operation.
	 *
	 *  @param[in] rs
	 *      Shared Pointer to a RecordStore that is to be split up.
	 *  @param[in] splittingFactor
	 *      The size of each chunk of work.
	 */
	SafeSplitPathsQueue(
	    std::shared_ptr<BiometricEvaluation::IO::RecordStore> rs,
	    const std::vector<std::string>::size_type splittingFactor);
};

} // namespace NFIQ2UI

#endif /* NFIQ2_UI_TYPES_H_ */

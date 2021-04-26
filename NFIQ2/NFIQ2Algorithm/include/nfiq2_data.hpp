#ifndef NFIQ2_DATA_HPP_
#define NFIQ2_DATA_HPP_

#include <nfiq2_nfiqexception.hpp>

#include <fstream>
#include <string>
#include <vector>

namespace NFIQ2 {
/**
******************************************************************************
* @class Data
* @brief This class manages binary data and offers functions for easy handling
* of data. Examples of binary data are proprietary fingerprint templates or
* quality feature values in binary format.
******************************************************************************/
class Data : public std::basic_string<uint8_t> {
    public:
	/******************************************************************************/
	// --- Constructor / Destructor --- //
	/******************************************************************************/

	/**
	 * @brief Default constructor of Data
	 */
	Data();

	/**
	 * @brief Constructor of data with available pointer to data
	 */
	Data(const uint8_t *pData, uint32_t dataSize);

	/**
	 * @brief Copy constructor
	 */
	Data(const Data &otherData);

	/**
	 * @brief Copy constructor
	 */
	explicit Data(const std::basic_string<uint8_t> &otherData);

	/**
	 * @brief Destructor
	 */
	virtual ~Data();

	/******************************************************************************/
	// --- Functions --- //
	/******************************************************************************/

	/**
	 * @fn readFromfile
	 * @brief Reads the content from the specified filename into the data
	 * structure.
	 * @param filename The path and name of the file which shall be read.
	 * @throws NFIQException
	 */
	void readFromFile(const std::string &filename);

	/**
	 * @fn writeToFile
	 * @brief Writes the content to a file.
	 * @param filename The path and name of the file which shall be written.
	 * @throws NFIQException
	 */
	void writeToFile(const std::string &filename) const;

	/**
	 * @fn toHexString
	 * @brief Generates a string in hexadecimal format of the buffer.
	 * @return The content of the buffer as hexadecimal string.
	 * @throws NFIQException
	 */
	std::string toHexString() const;

	/**
	 * @fn fromBase64String
	 * @brief Imports data from a Base64 encoded string.
	 * @param base64String The Base64 encoded string.
	 * @throws NFIQException
	 */
	void fromBase64String(const std::string &base64String);

	/**
	 * @fn toBase64String
	 * @brief Generates a string in base64 format of the buffer.
	 * @return The content of the buffer as base64 encoded string.
	 * @throws NFIQException
	 */
	std::string toBase64String() const;
};
} // namespace NFIQ

#endif

/******************************************************************************/

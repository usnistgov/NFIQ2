/*
 * This file is part of NIST Fingerprint Image Quality (NFIQ) 2. For more
 * information on this project, refer to:
 *   - https://nist.gov/services-resources/software/nfiq2
 *   - https://github.com/usnistgov/NFIQ2
 *
 * This work is in the public domain. For complete licensing details, refer to:
 *   - https://github.com/usnistgov/NFIQ2/blob/master/LICENSE.md
 */

#ifndef NFIQ2_DATA_HPP_
#define NFIQ2_DATA_HPP_

#include <nfiq2_exception.hpp>

#include <cstdint>
#include <fstream>
#include <string>
#include <vector>

namespace NFIQ2 {

/** Binary data */
class Data {
    public:
	/** Default Data constructor. */
	Data();

	/**
	 * @brief
	 * Constructor with available pointer to data.
	 *
	 * @param pData
	 * Data pointer.
	 *
	 * @param dataSize
	 * Size of data at data pointer.
	 */
	Data(const std::uint8_t *pData, std::uint32_t dataSize);

	/** Destructor. */
	virtual ~Data();

	/**
	 * @return
	 * Pointer to underlying storage.
	 */
	const std::uint8_t *data() const;

	/**
	 * @return
	 * Number of elements in underlying storage.
	 */
	std::vector<std::uint8_t>::size_type size() const;

	/**
	 * @return
	 * Reference to the element at pos in underlying storage, with
	 * bounds checking.
	 *
	 * @throw std::out_of_range
	 * pos is not within range of underlying storage.
	 */
	std::vector<std::uint8_t>::reference at(
	    std::vector<std::uint8_t>::size_type pos);

	/**
	 * Resize the underlying storage container.
	 *
	 * @param count
	 * Number of elements underlying storage will hold.
	 */
	void resize(std::vector<std::uint8_t>::size_type count);

	/**
	 * @brief
	 * Reads the content from the a file into this object.
	 *
	 * @param filename
	 * The path of the file that will be read.
	 *
	 * @throws NFIQ2::Exception
	 * File cannot be opened.
	 */
	void readFromFile(const std::string &filename);

	/**
	 * @brief
	 * Writes the content to a file.
	 *
	 * @param filename
	 * The path of the file that will be written to.
	 *
	 * @throws NFIQ2::Exception
	 * File cannot be opened.
	 */
	void writeToFile(const std::string &filename) const;

	/**
	 * @brief
	 * Generates a string in hexadecimal format of the buffer.
	 *
	 * @return
	 * The content of the buffer as hexadecimal string.
	 *
	 * @throws NFIQ2::Exception
	 * No data available in buffer.
	 */
	std::string toHexString() const;

	/**
	 * @brief
	 * Imports data from a Base64 encoded string.
	 *
	 * @param base64String
	 * The Base64 encoded string.
	 *
	 * @throws NFIQ2::Exception
	 * If invalid character is detected in string.
	 */
	void fromBase64String(const std::string &base64String);

	/**
	 * @brief
	 * Generates a string in Base64 format of the buffer.
	 *
	 * @return
	 * The content of the buffer as Base64 encoded string.
	 */
	std::string toBase64String() const;

    private:
	std::vector<std::uint8_t> buffer {};
};
} // namespace NFIQ

static_assert(std::is_same<std::uint8_t, char>::value ||
	std::is_same<std::uint8_t, unsigned char>::value,
    "Implementation requires std::uint8_t to be implemented as char or "
    "unsigned char.");

#endif /* NFIQ2_DATA_HPP_ */

/*
 * This file is part of NIST Fingerprint Image Quality (NFIQ) 2. For more
 * information on this project, refer to:
 *   - https://nist.gov/services-resources/software/nfiq2
 *   - https://github.com/usnistgov/NFIQ2
 *
 * This work is in the public domain. For complete licensing details, refer to:
 *   - https://github.com/usnistgov/NFIQ2/blob/master/LICENSE.md
 */

#ifndef NFIQ2_MODELINFO_HPP_
#define NFIQ2_MODELINFO_HPP_

#include <string>

namespace NFIQ2 {
/** Information about a random forest parameter model. */
class ModelInfo {
    public:
	/** Default constructor. */
	ModelInfo();

	/**
	 * Constructor
	 *
	 * @param modelInfoFilePath
	 * Path to model information file.
	 */
	ModelInfo(const std::string &modelInfoFilePath);

	/**
	 * @brief
	 * Obtain the model name.
	 *
	 * @return
	 * Returns model name.
	 */
	std::string getModelName() const;

	/**
	 * @brief
	 * Obtain the entity that trained the random forest model.
	 *
	 * @return
	 * Returns model trainer.
	 */
	std::string getModelTrainer() const;

	/**
	 * @brief
	 * Obtain additional description information for model.
	 *
	 * @return
	 * Returns model description.
	 */
	std::string getModelDescription() const;

	/**
	 * @brief
	 * Obtain the version number of the model.
	 *
	 * @return
	 * Returns model version.
	 */
	std::string getModelVersion() const;

	/**
	 * @brief
	 * Obtain the file path of the model.
	 *
	 * @return
	 * Returns model file path.
	 */
	std::string getModelPath() const;

	/**
	 * @brief
	 * Obtain the md5 checksum of the model
	 *
	 * @return
	 * Returns model md5 checksum.
	 */
	std::string getModelHash() const;

	/** Name Key. */
	static const char ModelInfoKeyName[];
	/** Trainer Key. */
	static const char ModelInfoKeyTrainer[];
	/** Description Key. */
	static const char ModelInfoKeyDescription[];
	/** Version Key. */
	static const char ModelInfoKeyVersion[];
	/** Path Key. */
	static const char ModelInfoKeyPath[];
	/** Hash Key. */
	static const char ModelInfoKeyHash[];

    private:
	/** Name member. */
	std::string modelName;
	/** Trainer member. */
	std::string modelTrainer;
	/** Description member. */
	std::string modelDescription;
	/** Version member. */
	std::string modelVersion;
	/** Path member. */
	std::string modelPath;
	/** Hash member. */
	std::string modelHash;
};

} // namespace NFIQ

#endif /* NFIQ2_MODELINFO_HPP_ */

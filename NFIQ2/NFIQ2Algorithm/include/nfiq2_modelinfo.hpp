#ifndef NFIQ2_MODELINFO_HPP_
#define NFIQ2_MODELINFO_HPP_

#include <string>

namespace NFIQ2 {
/** Model Class containing Model Information. */
class ModelInfo {
    public:
	/** Default ModelInfo constructor. */
	ModelInfo();

	/** Constructor utilizing file path pointing to model info. */
	ModelInfo(const std::string &modelInfoFilePath);

	/**
	 * @brief
	 * Getter for private member #modelName.
	 *
	 * @return
	 * Returns #modelName.
	 */
	std::string getModelName() const;

	/**
	 * @brief
	 * Getter for private member #modelTrainer.
	 *
	 * @return
	 * Returns #modelTrainer.
	 */
	std::string getModelTrainer() const;

	/**
	 * @brief
	 * Getter for private member #modelDescription.
	 *
	 * @return
	 * Returns #modelDescription.
	 */
	std::string getModelDescription() const;

	/**
	 * @brief
	 * Getter for private member #modelVersion.
	 *
	 * @return
	 * Returns #modelVersion.
	 */
	std::string getModelVersion() const;

	/**
	 * @brief
	 * Getter for private member #modelPath.
	 *
	 * @return
	 * Returns #modelPath.
	 */
	std::string getModelPath() const;

	/**
	 * @brief
	 * Getter for private member #modelHash.
	 *
	 * @return
	 * Returns #modelHash.
	 */
	std::string getModelHash() const;

	/** Name Key. */
	static const std::string ModelInfoKeyName;
	/** Trainer Key. */
	static const std::string ModelInfoKeyTrainer;
	/** Description Key. */
	static const std::string ModelInfoKeyDescription;
	/** Version Key. */
	static const std::string ModelInfoKeyVersion;
	/** Path Key. */
	static const std::string ModelInfoKeyPath;
	/** Hash Key. */
	static const std::string ModelInfoKeyHash;

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

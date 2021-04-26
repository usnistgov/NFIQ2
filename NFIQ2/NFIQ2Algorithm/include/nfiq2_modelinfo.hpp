#ifndef NFIQ2_MODELINFO_HPP_
#define NFIQ2_MODELINFO_HPP_

#include <string>

namespace NFIQ2 {
/** Model Class containing Model Information */
class ModelInfo {
    public:
	ModelInfo();
	ModelInfo(const std::string &modelInfoFilePath);

	/**
	 * @brief
	 * Getter for private member modelName
	 *
	 * @return
	 * Returns model name
	 */
	std::string getModelName() const;

	/**
	 * @brief
	 * Getter for private member modelTrainer
	 *
	 * @return
	 * Returns model trainer
	 */
	std::string getModelTrainer() const;

	/**
	 * @brief
	 * Getter for private member modelDescription
	 *
	 * @return
	 * Returns model description
	 */
	std::string getModelDescription() const;

	/**
	 * @brief
	 * Getter for private member modelVersion
	 *
	 * @return
	 * Returns model version
	 */
	std::string getModelVersion() const;

	/**
	 * @brief
	 * Getter for private member modelPath
	 *
	 * @return
	 * Returns model path
	 */
	std::string getModelPath() const;

	/**
	 * @brief
	 * Getter for private member modelHash
	 *
	 * @return
	 * Returns model hash
	 */
	std::string getModelHash() const;

	static const std::string ModelInfoKeyName;
	static const std::string ModelInfoKeyTrainer;
	static const std::string ModelInfoKeyDescription;
	static const std::string ModelInfoKeyVersion;
	static const std::string ModelInfoKeyPath;
	static const std::string ModelInfoKeyHash;

    private:
	std::string modelName;
	std::string modelTrainer;
	std::string modelDescription;
	std::string modelVersion;
	std::string modelPath;
	std::string modelHash;
};

} // namespace NFIQ

#endif

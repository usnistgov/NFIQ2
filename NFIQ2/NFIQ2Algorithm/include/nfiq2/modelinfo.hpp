#ifndef NFIQ2_MODELINFO_H_
#define NFIQ2_MODELINFO_H_

#include <string>
#include <unordered_map>

namespace NFIQ {
/** Model Class containing Model Information */
class ModelInfo {
    public:
	ModelInfo(const std::string &modelInfoFilePath);
	~ModelInfo();

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

	/**
	 * @brief
	 * Setter for private member modelPath
	 *
	 * @param newPath
	 * Updated path containing the model
	 *
	 */
	void setModelPath(const std::string &newPath);

	/** Static strings for Model Info File Keys */
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

	ModelInfo(const ModelInfo &) = delete;
	ModelInfo &operator=(const ModelInfo &) = delete;
};

/**
 * @brief
 * Generates a Key/Value Pair structure containing information from the model
 * info text file
 *
 * @param modelInfoFilePath
 * Path to model info text file
 *
 * @return
 * Map of Key/Value pairs
 */
std::unordered_map<std::string, std::string> parseModelInfoFile(
    const std::string &modelInfoFilePath);

} // namespace NFIQ

#endif

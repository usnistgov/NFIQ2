#ifndef NFIQ2_ALGORITHM_IMPL_HPP_
#define NFIQ2_ALGORITHM_IMPL_HPP_

#include <nfiq2_algorithm.hpp>
#include <nfiq2_constants.hpp>
#include <nfiq2_exception.hpp>
#include <nfiq2_fingerprintimagedata.hpp>
#include <prediction/RandomForestML.h>

#include <fstream>
#include <list>
#include <string>
#include <vector>

namespace NFIQ2 {

/** Internal implementation of NFIQ2::Algorithm */
class Algorithm::Impl {
    public:
	/**
	 * @brief
	 * Default constructor of Algorithm.
	 *
	 * @note
	 * May load from parameters compiled into source code, in which case
	 * this can be slow.
	 */
	Impl();

	/**
	 * @brief
	 * Constructor that loads random forest parameters from disk.
	 *
	 * @param fileName
	 * The file path containing the random forest model.
	 * @param fileHash
	 * The md5 checksum of the provided file.
	 */
	Impl(const std::string &fileName, const std::string &fileHash);

#ifdef __ANDROID__
	/**
	 * @brief
	 * Constructor that loads random forest parameters from AAR.
	 *
	 * @param assets
	 * The Android Asset Manager, provided by the App.
	 * @param fileName
	 * The file path containing the random forest model.
	 * @param fileHash
	 * The md5 checksum of the provided file.
	 */
	Impl(AAssetManager *assets, const std::string &fileName,
	    const std::string &fileHash);
#endif

	/** Destructor. */
	virtual ~Impl();

	unsigned int computeUnifiedQualityScore(
	    const NFIQ2::FingerprintImageData &rawImage) const;

	unsigned int computeUnifiedQualityScore(const std::vector<
	    std::shared_ptr<NFIQ2::QualityMeasures::Algorithm>> &algorithms)
	    const;

	static std::unordered_map<std::string, unsigned int>
	getQualityBlockValues(const std::unordered_map<std::string, double>
		&nativeQualityMeasureValues);

	static unsigned int
	getQualityBlockValue(const std::string &featureIdentifier,
	    const double nativeQualityMeasureValue);

	unsigned int computeUnifiedQualityScore(
	    const std::unordered_map<std::string, double> &algorithms) const;

	std::string getParameterHash() const;

	bool isEmbedded() const;

	bool isInitialized() const;

	unsigned int getEmbeddedFCT() const;

    private:
	/** Indicates whether random forest parameters have been loaded. */
	bool initialized { false };

	/**
	 * @brief
	 * Retrieves unified quality score from a map of feature data.
	 *
	 * @param nativeQualityMeasureValues
	 * Map of keys representing identifiers for the native quality measures
	 * (from nfiq2_constants.hpp), and values representing native quality
	 * measure values.
	 *
	 * @return
	 * Computed unified quality score.
	 *
	 * @throws Exception
	 * Failure to compute (OpenCV reason contained within message string) or
	 * called before random forest parameters loaded.
	 */
	double getQualityPrediction(
	    const std::unordered_map<std::string, double>
		&nativeQualityMeasureValues) const;

	/**
	 * @brief
	 * Throw an exception if random forest parameters have not been
	 * loaded.
	 *
	 * @throw NFIQ2::Exception
	 * Random forest parameters have not been loaded.
	 */
	void throwIfUninitialized() const;

	/** RandomForest parameters. */
	NFIQ2::Prediction::RandomForestML m_RandomForestML;

	/** RandomForest parameter md5 hash. */
	std::string m_parameterHash {};
};
} // namespace NFIQ2

#endif /* NFIQ2_ALGORITHM_IMPL_HPP_ */

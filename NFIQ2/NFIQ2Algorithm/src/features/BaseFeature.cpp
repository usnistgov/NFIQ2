#include <features/BaseFeature.h>
#include <nfiq2_interfacedefinitions.hpp>

#include <vector>

const std::string NFIQ2::QualityFeatures::BaseFeature::HistogramBin0Suffix {
	"0"
};
const std::string NFIQ2::QualityFeatures::BaseFeature::HistogramBin1Suffix {
	"1"
};
const std::string NFIQ2::QualityFeatures::BaseFeature::HistogramBin2Suffix {
	"2"
};
const std::string NFIQ2::QualityFeatures::BaseFeature::HistogramBin3Suffix {
	"3"
};
const std::string NFIQ2::QualityFeatures::BaseFeature::HistogramBin4Suffix {
	"4"
};
const std::string NFIQ2::QualityFeatures::BaseFeature::HistogramBin5Suffix {
	"5"
};
const std::string NFIQ2::QualityFeatures::BaseFeature::HistogramBin6Suffix {
	"6"
};
const std::string NFIQ2::QualityFeatures::BaseFeature::HistogramBin7Suffix {
	"7"
};
const std::string NFIQ2::QualityFeatures::BaseFeature::HistogramBin8Suffix {
	"8"
};
const std::string NFIQ2::QualityFeatures::BaseFeature::HistogramBin9Suffix {
	"9"
};
const std::string NFIQ2::QualityFeatures::BaseFeature::StdDevSuffix {
	"StdDev"
};
const std::string NFIQ2::QualityFeatures::BaseFeature::MeanSuffix { "Mean" };

NFIQ2::QualityFeatures::BaseFeature::BaseFeature() = default;

NFIQ2::QualityFeatures::BaseFeature::~BaseFeature() = default;

NFIQ2::QualityFeatureSpeed
NFIQ2::QualityFeatures::BaseFeature::getSpeed() const
{
	return this->speed;
}

std::unordered_map<std::string, double>
NFIQ2::QualityFeatures::BaseFeature::getFeatures() const
{
	return this->features;
}

void
NFIQ2::QualityFeatures::BaseFeature::setSpeed(
    const NFIQ2::QualityFeatureSpeed &featureSpeed)
{
	this->speed = featureSpeed;
}

void
NFIQ2::QualityFeatures::BaseFeature::setFeatures(
    const std::unordered_map<std::string, double> &featureResult)
{
	this->features = featureResult;
}

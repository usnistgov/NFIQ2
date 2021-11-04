#include <nfiq2_constants.hpp>

/* ISO/IEC JTC 1 SC 37 Biometrics. */
const unsigned int NFIQ2::Identifiers::CBEFF::Vendor { 0x0101 };

const unsigned int NFIQ2::Identifiers::CBEFF::NFIQ2Rev0 { 0x01 };
const unsigned int NFIQ2::Identifiers::CBEFF::NFIQ2Rev1 { 0x17 };
const unsigned int NFIQ2::Identifiers::CBEFF::NFIQ2Rev2 { 0x18 };
const unsigned int NFIQ2::Identifiers::CBEFF::UnifiedQualityScore { NFIQ2Rev2 };

const unsigned int NFIQ2::Identifiers::CBEFF::OrientationCertainty::Mean {
	0x02
};
const unsigned int NFIQ2::Identifiers::CBEFF::OrientationCertainty::StdDev {
	0x03
};

const unsigned int NFIQ2::Identifiers::CBEFF::LocalClarity::Mean { 0x04 };
const unsigned int NFIQ2::Identifiers::CBEFF::LocalClarity::StdDev { 0x05 };

const unsigned int NFIQ2::Identifiers::CBEFF::FrequencyDomainAnalysis::Mean {
	0x06
};
const unsigned int NFIQ2::Identifiers::CBEFF::FrequencyDomainAnalysis::StdDev {
	0x07
};

const unsigned int NFIQ2::Identifiers::CBEFF::RidgeValleyUniformity::Mean {
	0x08
};
const unsigned int NFIQ2::Identifiers::CBEFF::RidgeValleyUniformity::StdDev {
	0x09
};

const unsigned int NFIQ2::Identifiers::CBEFF::OrientationFlow::Mean { 0x0A };
const unsigned int NFIQ2::Identifiers::CBEFF::OrientationFlow::StdDev { 0x0B };

const unsigned int NFIQ2::Identifiers::CBEFF::Contrast::Mean { 0x0C };
const unsigned int NFIQ2::Identifiers::CBEFF::Contrast::MeanBlock { 0x0D };

const unsigned int NFIQ2::Identifiers::CBEFF::Minutiae::Count { 0x0E };
const unsigned int NFIQ2::Identifiers::CBEFF::Minutiae::CountCOM { 0x0F };
const unsigned int NFIQ2::Identifiers::CBEFF::Minutiae::QualityMu2 { 0x10 };
const unsigned int NFIQ2::Identifiers::CBEFF::Minutiae::QualityOCL80 { 0x11 };

const unsigned int NFIQ2::Identifiers::CBEFF::RegionOfInterest::Mean { 0x12 };
const unsigned int NFIQ2::Identifiers::CBEFF::RegionOfInterest::CoherenceSum {
	0x13
};
const unsigned int NFIQ2::Identifiers::CBEFF::RegionOfInterest::CoherenceMean {
	0x14
};

const unsigned int NFIQ2::Identifiers::CBEFF::RadialPowerSpectrum { 0x15 };
const unsigned int NFIQ2::Identifiers::CBEFF::Gabor { 0x16 };

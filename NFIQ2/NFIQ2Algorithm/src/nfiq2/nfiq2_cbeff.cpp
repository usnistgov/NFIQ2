#include <nfiq2_constants.hpp>

/* ISO/IEC JTC 1 SC 37 Biometrics. */
const unsigned int NFIQ2::Identifiers::CBEFF::Vendor { 0x0101 };

const unsigned int NFIQ2::Identifiers::CBEFF::NFIQ2Rev0 { 0x21 };
const unsigned int NFIQ2::Identifiers::CBEFF::NFIQ2Rev1 { 0x37 };
const unsigned int NFIQ2::Identifiers::CBEFF::NFIQ2Rev2 { 0x38 };
const unsigned int NFIQ2::Identifiers::CBEFF::NFIQ2Rev3 { 0x39 };

const unsigned int NFIQ2::Identifiers::CBEFF::OrientationCertainty::Mean {
	0x22
};
const unsigned int NFIQ2::Identifiers::CBEFF::OrientationCertainty::StdDev {
	0x23
};

const unsigned int NFIQ2::Identifiers::CBEFF::LocalClarity::Mean { 0x24 };
const unsigned int NFIQ2::Identifiers::CBEFF::LocalClarity::StdDev { 0x25 };

const unsigned int NFIQ2::Identifiers::CBEFF::FrequencyDomainAnalysis::Mean {
	0x26
};
const unsigned int NFIQ2::Identifiers::CBEFF::FrequencyDomainAnalysis::StdDev {
	0x27
};

const unsigned int NFIQ2::Identifiers::CBEFF::RidgeValleyUniformity::Mean {
	0x28
};
const unsigned int NFIQ2::Identifiers::CBEFF::RidgeValleyUniformity::StdDev {
	0x29
};

const unsigned int NFIQ2::Identifiers::CBEFF::OrientationFlow::Mean { 0x2A };
const unsigned int NFIQ2::Identifiers::CBEFF::OrientationFlow::StdDev { 0x2B };

const unsigned int NFIQ2::Identifiers::CBEFF::Contrast::ImageMean { 0x2C };
const unsigned int NFIQ2::Identifiers::CBEFF::Contrast::MeanOfBlockMeans {
	0x2D
};

const unsigned int NFIQ2::Identifiers::CBEFF::Minutiae::Count { 0x2E };
const unsigned int NFIQ2::Identifiers::CBEFF::Minutiae::CountCOM { 0x2F };
const unsigned int NFIQ2::Identifiers::CBEFF::Minutiae::PercentImageMean50 {
	0x30
};
const unsigned int
    NFIQ2::Identifiers::CBEFF::Minutiae::PercentOrientationCertainty80 { 0x31 };

const unsigned int NFIQ2::Identifiers::CBEFF::RegionOfInterest::Mean { 0x32 };
const unsigned int
    NFIQ2::Identifiers::CBEFF::RegionOfInterest::OrientationMapCoherenceSum {
	    0x33
    };
const unsigned int
    NFIQ2::Identifiers::CBEFF::RegionOfInterest::OrientationMapCoherenceMean {
	    0x34
    };

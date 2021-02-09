#include <nfiq2/nfiq2.hpp>
#include <nfiq2/nfiqexception.hpp>
#include <opencv2/core/core_c.h>
#include <opencv2/core/version.hpp>

#include "nfiq2api.h"

#include <cstdlib>
#include <memory>
#include <string>

#ifndef _WIN32
#include <dlfcn.h>
#endif

// external vars for the version values
extern char product_name[128];
extern char product_vendor[128];
extern int version_major;
extern int version_minor;
extern int version_patch;

// static object to load the algorithm only once (random forest init!)
std::unique_ptr<NFIQ::NFIQ2Algorithm> g_nfiq2;

std::string
GetYamlFilePath()
{
	std::string p;

#ifdef _WIN32
	HMODULE hmodule = NULL;

	GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
		GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
	    reinterpret_cast<LPCSTR>(GetYamlFilePath), &hmodule);
	char buffer[MAX_PATH];
	int n = GetModuleFileNameA(hmodule, buffer, sizeof(buffer));
	if (n > 0 && n < (int)sizeof(buffer)) {
		char *c = strrchr(buffer, '\\');
		if (c != nullptr) {
			*c = 0;
			p = buffer;
			p += "\\nist_plain_tir-ink.yaml";
		}
	}
#else
	Dl_info info;
	if (dladdr((void *)GetYamlFilePath, &info) != 0 &&
	    info.dli_fname != nullptr) {
		char *c = (char *)strrchr(info.dli_fname, '/');
		if (c != nullptr) {
			*c = 0;
			p = info.dli_fname;
			p += "/nist_plain_tir-ink.yaml";
		}
	}
#endif
	std::cout << "YAML file: " << p << std::endl;
	return p;
}

extern "C" {
DLLEXPORT void STDCALL
GetNfiq2Version(int *major, int *minor, int *patch, const char **ocv)
{
	*major = version_major;
	*minor = version_minor;
	*patch = version_patch;
#if CV_MAJOR_VERSION <= 2
	const char *m = nullptr;
	cvGetModuleInfo(nullptr, ocv, &m);
#else
	std::stringstream ss;
	ss << cv::getVersionMajor() << "." << cv::getVersionMinor() << "."
	   << cv::getVersionRevision();
	static char buf[128];
	memset(buf, 0, 128);
	strncpy(buf, ss.str().c_str(), ss.str().length());
	*ocv = buf;
#endif
}
// Require an allocated buffer of size 33 to accomodate md5 hash
DLLEXPORT const char *STDCALL
InitNfiq2(char* paramHash)
{
	try {
		if (g_nfiq2.get() == nullptr) {
#ifdef EMBED_RANDOMFOREST_PARAMETERS
			g_nfiq2 = std::unique_ptr<NFIQ::NFIQ2Algorithm>(
			    new NFIQ::NFIQ2Algorithm());
#else
			g_nfiq2 = std::unique_ptr<NFIQ::NFIQ2Algorithm>(
			    new NFIQ::NFIQ2Algorithm(GetYamlFilePath(),
				"ccd75820b48c19f1645ef5e9c481c592"));
#endif
			strncpy(paramHash, g_nfiq2->getParameterHash().c_str(), 33);
			return paramHash;
		}
	} catch (std::exception &exc) {
		std::cerr << "NFIQ2 ERROR => " << exc.what() << std::endl;
	}
	return nullptr;
}
DLLEXPORT int STDCALL
ComputeNfiq2Score(int fpos, const unsigned char *pixels, int size, int width,
    int height, int ppi)
{
	try {
		if (g_nfiq2.get() != nullptr) {
			NFIQ::FingerprintImageData rawImage(
			    pixels, size, width, height, fpos, ppi);
			std::list<NFIQ::ActionableQualityFeedback>
			    actionableQuality;
			std::list<NFIQ::QualityFeatureData> featureVector;
			std::list<NFIQ::QualityFeatureSpeed> featureTimings;
			int qualityScore = (int)g_nfiq2->computeQualityScore(
			    rawImage, true, actionableQuality, false,
			    featureVector, false, featureTimings);
			return qualityScore;
		}
	} catch (const NFIQ::NFIQException &exc) {
		std::cerr << "NFIQ2 ERROR => Return code ["
			  << exc.getReturnCode()
			  << "]: " << exc.getErrorMessage() << std::endl;
		return 255;
	} catch (std::exception &exc) {
		std::cerr << "NFIQ2 ERROR => " << exc.what() << std::endl;
		return -1;
	}
	return -2; // not initialized
}
}

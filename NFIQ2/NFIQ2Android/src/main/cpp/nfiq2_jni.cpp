#include <android/asset_manager_jni.h>
#include <jni.h>
#include <nfiq2_algorithm.hpp>
#include <nfiq2_exception.hpp>

#include <exception>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

namespace {
std::mutex algorithmMutex {};
std::unique_ptr<NFIQ2::Algorithm> algorithm {};

void
throwJava(JNIEnv *env, const char *type, const char *message)
{
	jclass exception = env->FindClass(type);
	if (exception != nullptr) {
		env->ThrowNew(exception, message);
		env->DeleteLocalRef(exception);
	}
}

void
throwNfiq2Error(JNIEnv *env, const NFIQ2::Exception &error)
{
	const std::string message = "NFIQ2 (" +
	    std::to_string(static_cast<int>(error.getErrorCode())) +
	    "): " + error.what();
	throwJava(env, "java/lang/IllegalStateException", message.c_str());
}
} // namespace

extern "C" JNIEXPORT void JNICALL
Java_gov_nist_nfiq2_Nfiq2_initialize(JNIEnv *env, jclass, jobject assetManager,
    jstring modelAsset, jstring modelHash)
{
	std::lock_guard<std::mutex> lock(algorithmMutex);
	if (algorithm != nullptr)
		return;

	try {
#ifdef NFIQ2_EMBED_RANDOM_FOREST_PARAMETERS
		(void)assetManager;
		(void)modelAsset;
		(void)modelHash;
		algorithm.reset(new NFIQ2::Algorithm {});
#else
		if (assetManager == nullptr || modelAsset == nullptr ||
		    modelHash == nullptr) {
			throwJava(env, "java/lang/IllegalArgumentException",
			    "An asset manager, model asset, and model hash are required.");
			return;
		}

		AAssetManager *assets = AAssetManager_fromJava(env,
		    assetManager);
		if (assets == nullptr) {
			throwJava(env, "java/lang/IllegalArgumentException",
			    "Could not access the Android asset manager.");
			return;
		}

		const char *assetChars = env->GetStringUTFChars(modelAsset,
		    nullptr);
		if (assetChars == nullptr)
			return;
		const std::string assetName { assetChars };
		env->ReleaseStringUTFChars(modelAsset, assetChars);

		const char *hashChars = env->GetStringUTFChars(modelHash,
		    nullptr);
		if (hashChars == nullptr)
			return;
		const std::string hash { hashChars };
		env->ReleaseStringUTFChars(modelHash, hashChars);

		if (assetName.empty() || hash.empty()) {
			throwJava(env, "java/lang/IllegalArgumentException",
			    "The model asset and hash must not be empty.");
			return;
		}
		algorithm.reset(
		    new NFIQ2::Algorithm { assets, assetName, hash });
#endif
	} catch (const NFIQ2::Exception &error) {
		throwNfiq2Error(env, error);
	} catch (const std::exception &error) {
		throwJava(env, "java/lang/IllegalStateException", error.what());
	} catch (...) {
		throwJava(env, "java/lang/IllegalStateException",
		    "Unexpected native initialization error.");
	}
}

extern "C" JNIEXPORT jint JNICALL
Java_gov_nist_nfiq2_Nfiq2_score(JNIEnv *env, jclass, jbyteArray image,
    jint width, jint height, jint ppi)
{
	if (image == nullptr || width < 32 || height < 32 || width > 4096 ||
	    height > 4096 || static_cast<int64_t>(width) * height > 4000000 ||
	    ppi != 500 ||
	    env->GetArrayLength(image) !=
		static_cast<int64_t>(width) * height) {
		throwJava(env, "java/lang/IllegalArgumentException",
		    "A 500 PPI grayscale image, 32-4096 pixels per dimension and no more than 4 million pixels, is required.");
		return -1;
	}

	try {
		std::lock_guard<std::mutex> lock(algorithmMutex);
		if (algorithm == nullptr) {
			throwJava(env, "java/lang/IllegalStateException",
			    "NFIQ2 must be initialized before scoring.");
			return -1;
		}

		std::vector<uint8_t> pixels(
		    static_cast<size_t>(width) * height);
		env->GetByteArrayRegion(image, 0,
		    static_cast<jsize>(pixels.size()),
		    reinterpret_cast<jbyte *>(pixels.data()));
		if (env->ExceptionCheck())
			return -1;

		const NFIQ2::FingerprintImageData fingerprint(pixels.data(),
		    static_cast<uint32_t>(pixels.size()), width, height, 0,
		    ppi);
		return static_cast<jint>(
		    algorithm->computeUnifiedQualityScore(fingerprint));
	} catch (const NFIQ2::Exception &error) {
		throwNfiq2Error(env, error);
	} catch (const std::exception &error) {
		throwJava(env, "java/lang/IllegalStateException", error.what());
	} catch (...) {
		throwJava(env, "java/lang/IllegalStateException",
		    "Unexpected native analysis error.");
	}
	return -1;
}

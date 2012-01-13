#include "KeyNative.h"
#include "marisa.h"

extern jlong get_handle_value(JNIEnv *env, jobject pthis, const char* class_name);

marisa::Key* get_key(JNIEnv *env, jobject pthis) {
	jlong handle = get_handle_value(env, pthis, "Ljp/marisa/KeyNative;");
	if (handle == -1) {
		return NULL;
	}

	return (marisa::Key*)(handle);
}

/*
 * Class:     jp_marisa_KeyNative
 * Method:    id
 * Signature: ()J
 */
extern "C"
JNIEXPORT jlong JNICALL Java_jp_marisa_KeyNative_id(JNIEnv *env, jobject pthis) {
	marisa::Key* key = get_key(env, pthis);

	return key->id();
}

/*
 * Class:     jp_marisa_KeyNative
 * Method:    text
 * Signature: ()[B
 */
extern "C"
JNIEXPORT jbyteArray JNICALL Java_jp_marisa_KeyNative_text__(JNIEnv *env, jobject pthis) {
	marisa::Key* key = get_key(env, pthis);

	const char* ptr = key->ptr();
	const std::size_t length = key->length();

	jbyteArray java_array = env->NewByteArray(length);
	jbyte* native_array = env->GetByteArrayElements(java_array, NULL);

	memcpy(native_array, ptr, length);

	env->ReleaseByteArrayElements(java_array, native_array, 0);

	return java_array;
}

/*
 * Class:     jp_marisa_KeyNative
 * Method:    text
 * Signature: (Ljava/nio/ByteBuffer;)V
 */
extern "C"
JNIEXPORT void JNICALL Java_jp_marisa_KeyNative_text__Ljava_nio_ByteBuffer_2(JNIEnv *env, jobject pthis, jobject buff) {
	marisa::Key* key = get_key(env, pthis);

	void* native_buff = env->GetDirectBufferAddress(buff);

	const char* ptr = key->ptr();
	const std::size_t length = key->length();

	memcpy(native_buff, ptr, length);
}

/*
 * Class:     jp_marisa_KeyNative
 * Method:    textLength
 * Signature: ()J
 */
extern "C"
JNIEXPORT jlong JNICALL Java_jp_marisa_KeyNative_textLength(JNIEnv *env, jobject pthis) {
	marisa::Key* key = get_key(env, pthis);
	const std::size_t length = key->length();
	return static_cast<jlong>(length);
}

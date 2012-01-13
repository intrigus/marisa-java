#include "QueryNative.h"
#include "marisa.h"

extern jlong get_handle_value(JNIEnv *env, jobject pthis, const char* class_name);

marisa::Query* get_query(JNIEnv *env, jobject pthis) {
	jlong handle = get_handle_value(env, pthis, "Ljp/marisa/QueryNative;");
	if (handle == -1) {
		return NULL;
	}

	return (marisa::Query*)(handle);
}

/*
 * Class:     jp_marisa_QueryNative
 * Method:    id
 * Signature: ()J
 */
extern "C"
JNIEXPORT jlong JNICALL Java_jp_marisa_QueryNative_id(JNIEnv *env, jobject pthis) {
	marisa::Query *query = get_query(env, pthis);
	return static_cast<jlong>(query->id());
}

/*
 * Class:     jp_marisa_QueryNative
 * Method:    text
 * Signature: ()[B
 */
extern "C"
JNIEXPORT jbyteArray JNICALL Java_jp_marisa_QueryNative_text__(JNIEnv *env, jobject pthis) {
	marisa::Query *query = get_query(env, pthis);

	const char* ptr = query->ptr();
	const std::size_t length = query->length();

	jbyteArray java_array = env->NewByteArray(length);
	jbyte* native_array = env->GetByteArrayElements(java_array, NULL);
	memcpy(native_array, ptr, length);

	// commit
	env->ReleaseByteArrayElements(java_array, native_array, 0);

	return java_array;
}

/*
 * Class:     jp_marisa_QueryNative
 * Method:    text
 * Signature: (Ljava/nio/ByteBuffer;)V
 */
extern "C"
JNIEXPORT void JNICALL Java_jp_marisa_QueryNative_text__Ljava_nio_ByteBuffer_2(JNIEnv *env, jobject pthis, jobject buff) {
	marisa::Query *query = get_query(env, pthis);
	void* native_buff = env->GetDirectBufferAddress(buff);

	const char* ptr = query->ptr();
	const std::size_t length = query->length();

	memcpy(native_buff, ptr, length);
}

/*
 * Class:     jp_marisa_QueryNative
 * Method:    textLength
 * Signature: ()J
 */
extern "C"
JNIEXPORT jlong JNICALL Java_jp_marisa_QueryNative_textLength(JNIEnv *env, jobject pthis) {
	marisa::Query *query = get_query(env, pthis);
	const std::size_t length = query->length();
	return static_cast<jlong>(length);
}

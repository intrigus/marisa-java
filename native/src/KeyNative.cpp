/*--------------------------------------------------------------------------
 * Copyright (c) 2012, NAKANO Hideo
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 * 
 * - Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer. 
 * - Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution. 
 * - Neither the name of the <ORGANIZATION> nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
 *
 *THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *--------------------------------------------------------------------------*/
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

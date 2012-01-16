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
#include "TrieNative.h"
#include "marisa.h"

extern jlong get_handle_value(JNIEnv *env, jobject pthis, const char* class_name);
extern void set_handle_value(JNIEnv *env, jobject pthis, const char* class_name, jlong handle_value);

marisa::Keyset* ensure_create_keyset(JNIEnv *env, jobject pthis) {
	jlong handle = get_handle_value(env, pthis, "Ljp/marisa/KeysetNative;");
	if (handle == -1) {
		marisa::Keyset* keyset = new marisa::Keyset();
		set_handle_value(env, pthis, "Ljp/marisa/KeysetNative;", (jlong)keyset);
		return keyset;
	}

	return (marisa::Keyset*)(handle);
}

marisa::Keyset* get_keyset(JNIEnv *env, jobject pthis) {
	jlong handle = get_handle_value(env, pthis, "Ljp/marisa/KeysetNative;");
	if (handle == -1) {
		return NULL;
	}

	return (marisa::Keyset*)(handle);
}

extern marisa::Key* get_key(JNIEnv *env, jobject pthis);
extern void throw_NullPointerException(JNIEnv *env, const char* message);
extern void throw_MarisaException(JNIEnv *env, const char *filename, int line, marisa::ErrorCode error_code, const char *error_message);

/*
 * Class:     jp_marisa_KeysetNative
 * Method:    push_back
 * Signature: (Ljp/marisa/KeyNative;)V
 */
extern "C"
JNIEXPORT void JNICALL Java_jp_marisa_KeysetNative_push_1back__Ljp_marisa_KeyNative_2(JNIEnv *env, jobject pthis, jobject key) {
	marisa::Keyset* native_keyset = ensure_create_keyset(env, pthis);
	if (native_keyset == NULL) {
		throw_NullPointerException(env, "handle is null");
		return;
	}

	if (key == NULL) {
		throw_NullPointerException(env, "key is null");
		return;
	}

	marisa::Key* native_key = get_key(env, key);
	if (native_keyset == NULL) {
		throw_NullPointerException(env, "key is null");
		return;
	}

	try {
		native_keyset->push_back(*native_key);
	} catch (const marisa::Exception& ex) {
		throw_MarisaException(env, ex.filename(), ex.line(), ex.error_code(), ex.error_message());
		return;
	}
}

/*
 * Class:     jp_marisa_KeysetNative
 * Method:    push_back
 * Signature: (Ljp/marisa/KeyNative;B)V
 */
extern "C"
JNIEXPORT void JNICALL Java_jp_marisa_KeysetNative_push_1back__Ljp_marisa_KeyNative_2B(JNIEnv *env, jobject pthis, jobject key, jbyte end_marker) {
	marisa::Keyset* native_keyset = ensure_create_keyset(env, pthis);
	if (native_keyset == NULL) {
		throw_NullPointerException(env, "handle is null");
		return;
	}

	if (key == NULL) {
		throw_NullPointerException(env, "key is null");
		return;
	}

	marisa::Key* native_key = get_key(env, key);
	if (native_keyset == NULL) {
		throw_NullPointerException(env, "key is null");
		return;
	}

	try {
		native_keyset->push_back(*native_key, (char)end_marker);
	} catch (const marisa::Exception& ex) {
		throw_MarisaException(env, ex.filename(), ex.line(), ex.error_code(), ex.error_message());
		return;
	}
}

/*
 * Class:     jp_marisa_KeysetNative
 * Method:    push_back
 * Signature: ([BIF)V
 */
extern "C"
JNIEXPORT void JNICALL Java_jp_marisa_KeysetNative_push_1back___3BIIF(JNIEnv *env, jobject pthis, jbyteArray ptr, jint start, jint length, jfloat weight) {
	marisa::Keyset* native_keyset = ensure_create_keyset(env, pthis);
	if (native_keyset == NULL) {
		throw_NullPointerException(env, "handle is null");
		return;
	}

	if (ptr == NULL) {
		throw_NullPointerException(env, "ptr is null");
		return;
	}

	jbyte* native_ptr = env->GetByteArrayElements(ptr, NULL);
	native_ptr += start;

	try {
		native_keyset->push_back((const char *)native_ptr, length, weight);
	} catch (const marisa::Exception& ex) {
		env->ReleaseByteArrayElements(ptr, native_ptr, JNI_ABORT);

		throw_MarisaException(env, ex.filename(), ex.line(), ex.error_code(), ex.error_message());
		return;
	}

	env->ReleaseByteArrayElements(ptr, native_ptr, JNI_ABORT);
}

jobject attach_handle(JNIEnv *env, const char *class_name, jlong handle_value) {
	jclass clazz = env->FindClass(class_name);
	if (clazz == NULL)
		return NULL;

	jmethodID default_constructor = env->GetMethodID(clazz, "<init>", "()V");
	if (default_constructor == NULL) {
		env->DeleteLocalRef(clazz);
		return NULL;
	}

	jobject obj = env->NewObject(clazz, default_constructor);
	set_handle_value(env, obj, class_name, handle_value);

	return obj;
}

/*
 * Class:     jp_marisa_KeysetNative
 * Method:    get
 * Signature: (I)Ljp/marisa/KeyNative;
 */
extern "C"
JNIEXPORT jobject JNICALL Java_jp_marisa_KeysetNative_get(JNIEnv *env, jobject pthis, jint i) {
	marisa::Keyset* native_keyset = get_keyset(env, pthis);
	if (native_keyset == NULL) {
		return NULL;
	}

	try {
		marisa::Key& native_key = (*native_keyset)[i];

		jobject obj = attach_handle(env, "Ljp/marisa/KeyNative;", (jlong)&native_key);
		return obj;
	} catch (const marisa::Exception& ex) {
		throw_MarisaException(env, ex.filename(), ex.line(), ex.error_code(), ex.error_message());
		return NULL;
	}
}

/*
 * Class:     jp_marisa_KeysetNative
 * Method:    num_keys
 * Signature: ()J
 */
extern "C"
JNIEXPORT jlong JNICALL Java_jp_marisa_KeysetNative_num_1keys(JNIEnv *env, jobject pthis) {
	marisa::Keyset* native_keyset = get_keyset(env, pthis);
	if (native_keyset == NULL) {
		return 0;
	}

	jlong ret = 0;
	try {
		ret = native_keyset->num_keys();
	} catch (const marisa::Exception& ex) {
		throw_MarisaException(env, ex.filename(), ex.line(), ex.error_code(), ex.error_message());
		return 0;
	}

	return ret;
}

/*
 * Class:     jp_marisa_KeysetNative
 * Method:    empty
 * Signature: ()Z
 */
extern "C"
JNIEXPORT jboolean JNICALL Java_jp_marisa_KeysetNative_empty(JNIEnv *env, jobject pthis) {
	marisa::Keyset* native_keyset = get_keyset(env, pthis);
	if (native_keyset == NULL) {
		return JNI_TRUE;
	}

	bool ret;
	try {
		ret = native_keyset->empty();
	} catch (const marisa::Exception& ex) {
		throw_MarisaException(env, ex.filename(), ex.line(), ex.error_code(), ex.error_message());
		return JNI_FALSE;
	}

	return ret ? JNI_TRUE : JNI_FALSE;
}

/*
 * Class:     jp_marisa_KeysetNative
 * Method:    size
 * Signature: ()J
 */
extern "C"
JNIEXPORT jlong JNICALL Java_jp_marisa_KeysetNative_size(JNIEnv *env, jobject pthis) {
	marisa::Keyset* native_keyset = get_keyset(env, pthis);
	if (native_keyset == NULL) {
		return 0;
	}

	size_t ret;
	try {
		ret = native_keyset->size();
	} catch (const marisa::Exception& ex) {
		throw_MarisaException(env, ex.filename(), ex.line(), ex.error_code(), ex.error_message());
		return 0;
	}

	return static_cast<jlong>(ret);
}

/*
 * Class:     jp_marisa_KeysetNative
 * Method:    total_length
 * Signature: ()J
 */
extern "C"
JNIEXPORT jlong JNICALL Java_jp_marisa_KeysetNative_total_1length(JNIEnv *env, jobject pthis) {
	marisa::Keyset* native_keyset = get_keyset(env, pthis);
	if (native_keyset == NULL) {
		return 0;
	}

	size_t ret;
	try {
		ret = native_keyset->total_length();
	} catch (const marisa::Exception& ex) {
		throw_MarisaException(env, ex.filename(), ex.line(), ex.error_code(), ex.error_message());
		return 0;
	}

	return static_cast<jlong>(ret);
}

/*
 * Class:     jp_marisa_KeysetNative
 * Method:    reset
 * Signature: ()V
 */
extern "C"
JNIEXPORT void JNICALL Java_jp_marisa_KeysetNative_reset(JNIEnv *env, jobject pthis) {
	marisa::Keyset* native_keyset = get_keyset(env, pthis);
	if (native_keyset == NULL) {
		return;
	}

	try {
		native_keyset->reset();
	} catch (const marisa::Exception& ex) {
		throw_MarisaException(env, ex.filename(), ex.line(), ex.error_code(), ex.error_message());
		return;
	}
}

/*
 * Class:     jp_marisa_KeysetNative
 * Method:    clear
 * Signature: ()V
 */
extern "C"
JNIEXPORT void JNICALL Java_jp_marisa_KeysetNative_clear(JNIEnv *env, jobject pthis) {
	marisa::Keyset* native_keyset = get_keyset(env, pthis);
	if (native_keyset == NULL) {
		return;
	}

	try {
		native_keyset->clear();
	} catch (const marisa::Exception& ex) {
		throw_MarisaException(env, ex.filename(), ex.line(), ex.error_code(), ex.error_message());
		return;
	}
}

/*
 * Class:     jp_marisa_KeysetNative
 * Method:    swap
 * Signature: (Ljp/marisa/KeysetNative;)V
 */
extern "C"
JNIEXPORT void JNICALL Java_jp_marisa_KeysetNative_swap(JNIEnv *env, jobject pthis, jobject rhs) {
	marisa::Keyset* native_keyset = ensure_create_keyset(env, pthis);
	if (native_keyset == NULL) {
		throw_NullPointerException(env, "handle is null");
		return;
	}

	if (rhs == NULL) {
		throw_NullPointerException(env, "rhs is null");
		return;
	}

	marisa::Keyset* native_rhs = ensure_create_keyset(env, rhs);
	if (native_rhs == NULL) {
		throw_NullPointerException(env, "rhs is null");
		return;
	}

	try {
		native_keyset->swap(*native_rhs);
	} catch (const marisa::Exception& ex) {
		throw_MarisaException(env, ex.filename(), ex.line(), ex.error_code(), ex.error_message());
		return;
	}
}

/*
 * Class:     jp_marisa_KeysetNative
 * Method:    dispose
 * Signature: ()V
 */
extern "C"
JNIEXPORT void JNICALL Java_jp_marisa_KeysetNative_dispose(JNIEnv *env, jobject pthis) {
	marisa::Keyset* native_keyset = get_keyset(env, pthis);
	if (native_keyset == NULL) {
		return;
	}

	delete native_keyset;
	set_handle_value(env, pthis, "Ljp/marisa/KeysetNative;", (jlong)-1);
}

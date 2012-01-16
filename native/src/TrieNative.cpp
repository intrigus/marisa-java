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
#if defined( _MSC_VER )
#include <malloc.h> 
#else
#include <alloca.h> 
#endif

jlong get_handle_value(JNIEnv *env, jobject pthis, const char* class_name) {
	jclass clsj = env->FindClass(class_name);
	if (clsj == NULL) {
		return -1;
	}

	jfieldID fj = env->GetFieldID(clsj, "handle", "J");
	if (fj == NULL) {
		env->DeleteLocalRef(clsj);
		return -1;
	}

	jlong handle = env->GetLongField(pthis, fj);
	env->DeleteLocalRef(clsj);

	return handle;
}

void set_handle_value(JNIEnv *env, jobject pthis, const char* class_name, jlong handle_value) {
	jclass clsj = env->FindClass(class_name);
	if (clsj == NULL)
		return;

	jfieldID fj = env->GetFieldID(clsj, "handle", "J");
	if (fj == NULL) {
		env->DeleteLocalRef(clsj);
		return;
	}

	env->SetLongField(pthis, fj, handle_value);
	env->DeleteLocalRef(clsj);
}

void throw_exception(JNIEnv *env, const char *class_name, const char *message) {
	jclass clsj = env->FindClass(class_name);
	if (clsj == NULL)
		return;

	env->ThrowNew(clsj, message);
	env->DeleteLocalRef(clsj);
}

void throw_NullPointerException(JNIEnv *env, const char* message) {
	throw_exception(env, "Ljava/lang/NullPointerException;", message);
}

void throw_MarisaException(JNIEnv *env, const char *filename, int line, marisa::ErrorCode error_code, const char *error_message) {
	jclass exception_class = env->FindClass("Ljp/marisa/MarisaException;");
	if (exception_class == NULL)
		return;

	jmethodID constructor = env->GetMethodID(exception_class, "<init>", "(Ljava/lang/String;IILjava/lang/String;)V");
	if (constructor == NULL) {
		env->DeleteLocalRef(exception_class);
		return;
	}

	jstring x = env->NewStringUTF(filename);
	jstring y = env->NewStringUTF(error_message);

	jobject e = env->NewObject(exception_class, constructor, x, (jint)line, (jint)error_code, y);
	env->DeleteLocalRef(exception_class);
	env->DeleteLocalRef(x);
	env->DeleteLocalRef(y);

	if (e == NULL)
		return;

	env->Throw((jthrowable)e);
}

marisa::Trie* ensure_create_trie(JNIEnv *env, jobject pthis) {
	jlong handle = get_handle_value(env, pthis, "Ljp/marisa/TrieNative;");
	if (handle == -1) {
		marisa::Trie* trie = new marisa::Trie();
		set_handle_value(env, pthis, "Ljp/marisa/TrieNative;", (jlong)trie);
		return trie;
	}

	return (marisa::Trie*)(handle);
}

marisa::Trie* get_trie(JNIEnv *env, jobject pthis) {
	jlong handle = get_handle_value(env, pthis, "Ljp/marisa/TrieNative;");
	if (handle == -1) {
		return NULL;
	}

	return (marisa::Trie*)(handle);
}

extern marisa::Keyset* get_keyset(JNIEnv *env, jobject pthis);
extern marisa::Agent* get_agent(JNIEnv *env, jobject pthis);

/*
 * Class:     jp_marisa_TrieNative
 * Method:    build
 * Signature: (Ljp/marisa/KeysetNative;I)V
 */
extern "C"
JNIEXPORT void JNICALL Java_jp_marisa_TrieNative_build(JNIEnv *env, jobject pthis, jobject keyset, jint config_flags) {
	marisa::Trie* native_trie = ensure_create_trie(env, pthis);
	if (native_trie == NULL) {
		throw_NullPointerException(env, "handle is null");
		return;
	}

	if (keyset == NULL) {
		throw_NullPointerException(env, "keyset is null");
		return;
	}

	marisa::Keyset* native_keyset = get_keyset(env, keyset);
	if (native_keyset == NULL) {
		throw_NullPointerException(env, "keyset is null");
		return;
	}

	try {
		native_trie->build(*native_keyset, config_flags);
	} catch (const marisa::Exception& ex) {
		throw_MarisaException(env, ex.filename(), ex.line(), ex.error_code(), ex.error_message());
		return;
	}
}
/*
 * Class:     jp_marisa_TrieNative
 * Method:    mmap
 * Signature: ([B)V
 */
extern "C"
JNIEXPORT void JNICALL Java_jp_marisa_TrieNative_mmap(JNIEnv *env, jobject pthis, jbyteArray fileName) {
	marisa::Trie* native_trie = ensure_create_trie(env, pthis);
	if (native_trie == NULL) {
		throw_NullPointerException(env, "handle is null");
		return;
	}

	jbyte* native_fileName = env->GetByteArrayElements(fileName, NULL);
	jsize fileNameLength = env->GetArrayLength(fileName);

	char* tmp = (char *)alloca(fileNameLength + 1);
	memcpy(tmp, native_fileName, fileNameLength);
	tmp[fileNameLength] = 0;

	env->ReleaseByteArrayElements(fileName, native_fileName, JNI_ABORT);

	try {
		native_trie->mmap(tmp);
	} catch (const marisa::Exception& ex) {
		throw_MarisaException(env, ex.filename(), ex.line(), ex.error_code(), ex.error_message());
		return;
	}
}

/*
 * Class:     jp_marisa_TrieNative
 * Method:    map
 * Signature: ([BI)V
 */
extern "C"
JNIEXPORT void JNICALL Java_jp_marisa_TrieNative_map(JNIEnv *env, jobject pthis, jbyteArray chunk, jint start, jint length) {
	marisa::Trie* native_trie = ensure_create_trie(env, pthis);
	if (native_trie == NULL) {
		throw_NullPointerException(env, "handle is null");
		return;
	}

	jbyte* native_chunk = env->GetByteArrayElements(chunk, NULL);
	native_chunk += start;

	try {
		native_trie->map(native_chunk, length);
	} catch (const marisa::Exception& ex) {
		env->ReleaseByteArrayElements(chunk, native_chunk, JNI_ABORT);

		throw_MarisaException(env, ex.filename(), ex.line(), ex.error_code(), ex.error_message());
		return;
	}

	env->ReleaseByteArrayElements(chunk, native_chunk, JNI_ABORT);
}

/*
 * Class:     jp_marisa_TrieNative
 * Method:    load
 * Signature: ([B)V
 */
extern "C"
JNIEXPORT void JNICALL Java_jp_marisa_TrieNative_load(JNIEnv *env, jobject pthis, jbyteArray fileName) {
	marisa::Trie* native_trie = ensure_create_trie(env, pthis);
	if (native_trie == NULL) {
		throw_NullPointerException(env, "handle is null");
		return;
	}

	jbyte* native_fileName = env->GetByteArrayElements(fileName, NULL);
	jsize fileNameLength = env->GetArrayLength(fileName);

	char* tmp = (char *)alloca(fileNameLength + 1);
	memcpy(tmp, native_fileName, fileNameLength);
	tmp[fileNameLength] = 0;

	env->ReleaseByteArrayElements(fileName, native_fileName, JNI_ABORT);

	try {
		native_trie->load(tmp);
	} catch (const marisa::Exception& ex) {
		throw_MarisaException(env, ex.filename(), ex.line(), ex.error_code(), ex.error_message());
		return;
	}
}

/*
 * Class:     jp_marisa_TrieNative
 * Method:    read
 * Signature: (I)V
 */
extern "C"
JNIEXPORT void JNICALL Java_jp_marisa_TrieNative_read(JNIEnv *env, jobject pthis, jint fd) {
	marisa::Trie* native_trie = ensure_create_trie(env, pthis);
	if (native_trie == NULL) {
		throw_NullPointerException(env, "handle is null");
		return;
	}

	try {
		native_trie->read(fd);
	} catch (const marisa::Exception& ex) {
		throw_MarisaException(env, ex.filename(), ex.line(), ex.error_code(), ex.error_message());
		return;
	}
}

/*
 * Class:     jp_marisa_TrieNative
 * Method:    save
 * Signature: ([B)V
 */
extern "C"
JNIEXPORT void JNICALL Java_jp_marisa_TrieNative_save(JNIEnv *env, jobject pthis, jbyteArray fileName) {
	marisa::Trie* native_trie = ensure_create_trie(env, pthis);
	if (native_trie == NULL) {
		throw_NullPointerException(env, "handle is null");
		return;
	}

	jbyte* native_fileName = env->GetByteArrayElements(fileName, NULL);
	jsize fileNameLength = env->GetArrayLength(fileName);

	// force zero terminate
	char* tmp = (char *)alloca(fileNameLength + 1);
	memcpy(tmp, native_fileName, fileNameLength);
	tmp[fileNameLength] = 0;

	env->ReleaseByteArrayElements(fileName, native_fileName, JNI_ABORT);

	try {
		native_trie->save(tmp);
	} catch (const marisa::Exception& ex) {
		throw_MarisaException(env, ex.filename(), ex.line(), ex.error_code(), ex.error_message());
		return;
	}
}

/*
 * Class:     jp_marisa_TrieNative
 * Method:    write
 * Signature: (I)V
 */
extern "C"
JNIEXPORT void JNICALL Java_jp_marisa_TrieNative_write(JNIEnv *env, jobject pthis, jint fd) {
	marisa::Trie* native_trie = ensure_create_trie(env, pthis);
	if (native_trie == NULL) {
		throw_NullPointerException(env, "handle is null");
		return;
	}

	try {
		native_trie->write(fd);
	} catch (const marisa::Exception& ex) {
		throw_MarisaException(env, ex.filename(), ex.line(), ex.error_code(), ex.error_message());
		return;
	}
}

/*
 * Class:     jp_marisa_TrieNative
 * Method:    lookup
 * Signature: (Ljp/marisa/AgentNative;)V
 */
extern "C"
JNIEXPORT jboolean JNICALL Java_jp_marisa_TrieNative_lookup(JNIEnv *env, jobject pthis, jobject agent) {
	marisa::Trie* native_trie = get_trie(env, pthis);
	if (native_trie == NULL) {
		throw_NullPointerException(env, "handle is null");
		return JNI_FALSE;
	}

	if (agent == NULL) {
		throw_NullPointerException(env, "agent is null");
		return JNI_FALSE;
	}

	marisa::Agent* native_agent = get_agent(env, agent);
	if (native_agent == NULL) {
		throw_NullPointerException(env, "agent is null");
		return JNI_FALSE;
	}

	bool ret = false;
	try {
		ret = native_trie->lookup(*native_agent);
	} catch (const marisa::Exception& ex) {
		throw_MarisaException(env, ex.filename(), ex.line(), ex.error_code(), ex.error_message());
		return JNI_FALSE;
	}

	return ret ? JNI_TRUE : JNI_FALSE;
}

/*
 * Class:     jp_marisa_TrieNative
 * Method:    revese_lookup
 * Signature: (Ljp/marisa/AgentNative;)V
 */
extern "C"
JNIEXPORT void JNICALL Java_jp_marisa_TrieNative_reverse_1lookup(JNIEnv *env, jobject pthis, jobject agent) {
	marisa::Trie* native_trie = get_trie(env, pthis);
	if (native_trie == NULL) {
		return;
	}

	if (agent == NULL) {
		throw_NullPointerException(env, "agent is null");
		return;
	}

	marisa::Agent* native_agent = get_agent(env, agent);
	if (native_agent == NULL) {
		throw_NullPointerException(env, "agent is null");
		return;
	}

	try {
		native_trie->reverse_lookup(*native_agent);
	} catch (const marisa::Exception& ex) {
		throw_MarisaException(env, ex.filename(), ex.line(), ex.error_code(), ex.error_message());
		return;
	}
}

/*
 * Class:     jp_marisa_TrieNative
 * Method:    common_prefix_search
 * Signature: (Ljp/marisa/AgentNative;)Z
 */
extern "C"
JNIEXPORT jboolean JNICALL Java_jp_marisa_TrieNative_common_1prefix_1search(JNIEnv *env, jobject pthis, jobject agent) {
	marisa::Trie* native_trie = get_trie(env, pthis);
	if (native_trie == NULL) {
		return JNI_FALSE;
	}

	if (agent == NULL) {
		throw_NullPointerException(env, "agent is null");
		return JNI_FALSE;
	}

	marisa::Agent* native_agent = get_agent(env, agent);
	if (native_agent == NULL) {
		throw_NullPointerException(env, "agent is null");
		return JNI_FALSE;
	}

	jboolean ret = 0;
	try {
		ret = native_trie->common_prefix_search(*native_agent);
	} catch (const marisa::Exception& ex) {
		throw_MarisaException(env, ex.filename(), ex.line(), ex.error_code(), ex.error_message());
		return JNI_FALSE;
	}

	return ret ? JNI_TRUE : JNI_FALSE;
}

/*
 * Class:     jp_marisa_TrieNative
 * Method:    predictive_search
 * Signature: (Ljp/marisa/AgentNative;)Z
 */
extern "C"
JNIEXPORT jboolean JNICALL Java_jp_marisa_TrieNative_predictive_1search(JNIEnv *env, jobject pthis, jobject agent) {
	marisa::Trie* native_trie = get_trie(env, pthis);
	if (native_trie == NULL) {
		return JNI_FALSE;
	}

	if (agent == NULL) {
		throw_NullPointerException(env, "agent is null");
		return JNI_FALSE;
	}

	marisa::Agent* native_agent = get_agent(env, agent);
	if (native_agent == NULL) {
		throw_NullPointerException(env, "agent is null");
		return JNI_FALSE;
	}

	jboolean ret = 0;
	try {
		ret = native_trie->predictive_search(*native_agent);
	} catch (const marisa::Exception& ex) {
		throw_MarisaException(env, ex.filename(), ex.line(), ex.error_code(), ex.error_message());
		return JNI_FALSE;
	}

	return ret ? JNI_TRUE : JNI_FALSE;
}

/*
 * Class:     jp_marisa_TrieNative
 * Method:    num_tries
 * Signature: ()J
 */
extern "C"
JNIEXPORT jlong JNICALL Java_jp_marisa_TrieNative_num_1tries(JNIEnv *env, jobject pthis) {
	marisa::Trie* native_trie = get_trie(env, pthis);
	if (native_trie == NULL) {
		return 0;
	}

	jlong ret = 0;
	try {
		ret = native_trie->num_tries();
	} catch (const marisa::Exception& ex) {
		throw_MarisaException(env, ex.filename(), ex.line(), ex.error_code(), ex.error_message());
		return 0;
	}

	return ret;
}

/*
 * Class:     jp_marisa_TrieNative
 * Method:    num_keys
 * Signature: ()J
 */
extern "C"
JNIEXPORT jlong JNICALL Java_jp_marisa_TrieNative_num_1keys(JNIEnv *env, jobject pthis) {
	marisa::Trie* native_trie = get_trie(env, pthis);
	if (native_trie == NULL) {
		return 0;
	}

	jlong ret = 0;
	try {
		ret = native_trie->num_keys();
	} catch (const marisa::Exception& ex) {
		throw_MarisaException(env, ex.filename(), ex.line(), ex.error_code(), ex.error_message());
		return 0;
	}

	return ret;
}

/*
 * Class:     jp_marisa_TrieNative
 * Method:    num_nodes
 * Signature: ()J
 */
extern "C"
JNIEXPORT jlong JNICALL Java_jp_marisa_TrieNative_num_1nodes(JNIEnv *env, jobject pthis) {
	marisa::Trie* native_trie = get_trie(env, pthis);
	if (native_trie == NULL) {
		return 0;
	}

	jlong ret = 0;
	try {
		ret = native_trie->num_nodes();
	} catch (const marisa::Exception& ex) {
		throw_MarisaException(env, ex.filename(), ex.line(), ex.error_code(), ex.error_message());
		return 0;
	}

	return ret;
}

/*
 * Class:     jp_marisa_TrieNative
 * Method:    tail_mode
 * Signature: ()I
 */
extern "C"
JNIEXPORT jint JNICALL Java_jp_marisa_TrieNative_tail_1mode(JNIEnv *env, jobject pthis) {
	marisa::Trie* native_trie = get_trie(env, pthis);
	if (native_trie == NULL) {
		return MARISA_DEFAULT_TAIL;
	}

	jint ret = MARISA_DEFAULT_TAIL;
	try {
		ret = native_trie->tail_mode();
	} catch (const marisa::Exception& ex) {
		throw_MarisaException(env, ex.filename(), ex.line(), ex.error_code(), ex.error_message());
		return 0;
	}

	return ret;
}

/*
 * Class:     jp_marisa_TrieNative
 * Method:    node_order
 * Signature: ()I
 */
extern "C"
JNIEXPORT jint JNICALL Java_jp_marisa_TrieNative_node_1order(JNIEnv *env, jobject pthis) {
	marisa::Trie* native_trie = get_trie(env, pthis);
	if (native_trie == NULL) {
		return MARISA_DEFAULT_ORDER;
	}

	jint ret = MARISA_DEFAULT_ORDER;
	try {
		ret = native_trie->node_order();
	} catch (const marisa::Exception& ex) {
		throw_MarisaException(env, ex.filename(), ex.line(), ex.error_code(), ex.error_message());
		return 0;
	}

	return ret;
}

/*
 * Class:     jp_marisa_TrieNative
 * Method:    empty
 * Signature: ()Z
 */
extern "C"
JNIEXPORT jboolean JNICALL Java_jp_marisa_TrieNative_empty(JNIEnv *env, jobject pthis) {
	marisa::Trie* native_trie = get_trie(env, pthis);
	if (native_trie == NULL) {
		return JNI_TRUE;
	}

	jboolean ret = 0;
	try {
		ret = native_trie->empty();
	} catch (const marisa::Exception& ex) {
		throw_MarisaException(env, ex.filename(), ex.line(), ex.error_code(), ex.error_message());
		return JNI_FALSE;
	}

	return ret ? JNI_TRUE : JNI_FALSE;
}

/*
 * Class:     jp_marisa_TrieNative
 * Method:    size
 * Signature: ()J
 */
extern "C"
JNIEXPORT jlong JNICALL Java_jp_marisa_TrieNative_size(JNIEnv *env, jobject pthis) {
	marisa::Trie* native_trie = get_trie(env, pthis);
	if (native_trie == NULL) {
		return 0;
	}

	jlong ret = 0;
	try {
		ret = native_trie->size();
	} catch (const marisa::Exception& ex) {
		throw_MarisaException(env, ex.filename(), ex.line(), ex.error_code(), ex.error_message());
		return 0;
	}

	return ret;
}

/*
 * Class:     jp_marisa_TrieNative
 * Method:    io_size
 * Signature: ()J
 */
extern "C"
JNIEXPORT jlong JNICALL Java_jp_marisa_TrieNative_io_1size(JNIEnv *env, jobject pthis) {
	marisa::Trie* native_trie = get_trie(env, pthis);
	if (native_trie == NULL) {
		return 0;
	}

	jlong ret = 0;
	try {
		ret = native_trie->io_size();
	} catch (const marisa::Exception& ex) {
		throw_MarisaException(env, ex.filename(), ex.line(), ex.error_code(), ex.error_message());
		return 0;
	}

	return ret;
}

/*
 * Class:     jp_marisa_TrieNative
 * Method:    clear
 * Signature: ()V
 */
extern "C"
JNIEXPORT void JNICALL Java_jp_marisa_TrieNative_clear(JNIEnv *env, jobject pthis) {
	marisa::Trie* native_trie = get_trie(env, pthis);
	if (native_trie == NULL) {
		return;
	}

	try {
		native_trie->clear();
	} catch (const marisa::Exception& ex) {
		throw_MarisaException(env, ex.filename(), ex.line(), ex.error_code(), ex.error_message());
		return;
	}
}

/*
 * Class:     jp_marisa_TrieNative
 * Method:    swap
 * Signature: (Ljp/marisa/TrieNative;)V
 */
extern "C"
JNIEXPORT void JNICALL Java_jp_marisa_TrieNative_swap(JNIEnv *env, jobject pthis, jobject rhs) {
	marisa::Trie* native_trie = get_trie(env, pthis);
	if (native_trie == NULL) {
		return;
	}

	marisa::Trie* native_rhs = ensure_create_trie(env, rhs);
	if (native_trie == NULL) {
		jclass clsj = env->FindClass("Ljava/lang/NullPointerException;");
		if (clsj == NULL)
			return;

		env->ThrowNew(clsj, "handle is null");
		env->DeleteLocalRef(clsj);
		return;
	}

	try {
		native_trie->swap(*native_rhs);
	} catch (const marisa::Exception& ex) {
		throw_MarisaException(env, ex.filename(), ex.line(), ex.error_code(), ex.error_message());
		return;
	}
}

/*
 * Class:     jp_marisa_TrieNative
 * Method:    dispose
 * Signature: ()V
 */
extern "C"
JNIEXPORT void JNICALL Java_jp_marisa_TrieNative_dispose(JNIEnv *env, jobject pthis) {
	marisa::Trie* native_trie = get_trie(env, pthis);
	if (native_trie == NULL) {
		return;
	}

	delete native_trie;
	set_handle_value(env, pthis, "Ljp/marisa/TrieNative;", (jlong)-1);
}

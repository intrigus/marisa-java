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
#include "AgentNative.h"
#include "marisa.h"

extern jlong get_handle_value(JNIEnv *env, jobject pthis, const char* class_name);
extern void set_handle_value(JNIEnv *env, jobject pthis, const char* class_name, jlong handle_value);
extern void throw_NullPointerException(JNIEnv *env, const char* message);
extern void throw_MarisaException(JNIEnv *env, const char *filename, int line, marisa::ErrorCode error_code, const char *error_message);
extern jobject attach_handle(JNIEnv *env, const char *class_name, jlong handle_value);

marisa::Agent* ensure_create_agent(JNIEnv *env, jobject pthis) {
	jlong handle = get_handle_value(env, pthis, "Ljp/marisa/AgentNative;");
	if (handle == -1) {
		marisa::Agent* agent = new marisa::Agent();
		set_handle_value(env, pthis, "Ljp/marisa/AgentNative;", (jlong)agent);
		return agent;
	}

	return (marisa::Agent*)(handle);
}

marisa::Agent* get_agent(JNIEnv *env, jobject pthis) {
	jlong handle = get_handle_value(env, pthis, "Ljp/marisa/AgentNative;");
	if (handle == -1) {
		return NULL;
	}

	return (marisa::Agent*)(handle);
}

jlong get_stringBufferHandle_value(JNIEnv *env, jobject pthis) {
	const char* class_name = "Ljp/marisa/AgentNative;";
	const char* field_name = "stringBufferHandle";

	jclass clsj = env->FindClass(class_name);
	if (clsj == NULL) {
		return -1;
	}

	jfieldID fj = env->GetFieldID(clsj, field_name, "J");
	if (fj == NULL) {
		env->DeleteLocalRef(clsj);
		return -1;
	}

	jlong handle = env->GetLongField(pthis, fj);
	env->DeleteLocalRef(clsj);

	return handle;
}

void set_stringBufferHandle_value(JNIEnv *env, jobject pthis, jlong handle_value) {
	const char* class_name = "Ljp/marisa/AgentNative;";
	const char* field_name = "stringBufferHandle";

	jclass clsj = env->FindClass(class_name);
	if (clsj == NULL)
		return;

	jfieldID fj = env->GetFieldID(clsj, field_name, "J");
	if (fj == NULL) {
		env->DeleteLocalRef(clsj);
		return;
	}

	env->SetLongField(pthis, fj, handle_value);
	env->DeleteLocalRef(clsj);
}

/*
 * Class:     jp_marisa_AgentNative
 * Method:    query
 * Signature: ()Ljp/marisa/QueryNative;
 */
extern "C"
JNIEXPORT jobject JNICALL Java_jp_marisa_AgentNative_query(JNIEnv *env, jobject pthis) {
	marisa::Agent* native_agent = get_agent(env, pthis);
	if (native_agent == NULL) {
		return NULL;
	}

	try {
		const marisa::Query& query = native_agent->query();

		jobject obj = attach_handle(env, "Ljp/marisa/QueryNative;", (jlong)&query);
		return obj;
	} catch (const marisa::Exception& ex) {
		// TODO:
		jclass clsj = env->FindClass("Ljava/lang/UnsupportedOperationException;");
		if (clsj == NULL)
			return NULL;

		env->ThrowNew(clsj, ex.error_message());
		env->DeleteLocalRef(clsj);
		return NULL;
	}
}

/*
 * Class:     jp_marisa_AgentNative
 * Method:    key
 * Signature: ()Ljp/marisa/KeyNative;
 */
extern "C"
JNIEXPORT jobject JNICALL Java_jp_marisa_AgentNative_key(JNIEnv *env, jobject pthis) {
	marisa::Agent* native_agent = get_agent(env, pthis);
	if (native_agent == NULL) {
		return NULL;
	}

	try {
		const marisa::Key& query = native_agent->key();

		jobject obj = attach_handle(env, "Ljp/marisa/KeyNative;", (jlong)&query);
		return obj;
	} catch (const marisa::Exception& ex) {
		// TODO:
		jclass clsj = env->FindClass("Ljava/lang/UnsupportedOperationException;");
		if (clsj == NULL)
			return NULL;

		env->ThrowNew(clsj, ex.error_message());
		env->DeleteLocalRef(clsj);
		return NULL;
	}
}

/*
 * Class:     jp_marisa_AgentNative
 * Method:    set_query
 * Signature: ([BI)V
 */
extern "C"
JNIEXPORT void JNICALL Java_jp_marisa_AgentNative_set_1query___3BII(JNIEnv *env, jobject pthis, jbyteArray str, jint start, jint length) {
	marisa::Agent* native_agent = ensure_create_agent(env, pthis);
	if (native_agent == NULL) {
		throw_NullPointerException(env, "handle is null");
		return;
	}

	// marisa::Agent::set_query set only pointer address.
	// so, we need copy of str.
	jbyte* native_str = env->GetByteArrayElements(str, NULL);
	native_str += start;

	char* copy_str = new char[length];
	memcpy(copy_str, native_str, length);

	env->ReleaseByteArrayElements(str, native_str, JNI_ABORT);

	try {
		native_agent->set_query(copy_str, length);
	} catch (const marisa::Exception& ex) {
		throw_MarisaException(env, ex.filename(), ex.line(), ex.error_code(), ex.error_message());
		return;
	}

	jlong handle = get_stringBufferHandle_value(env, pthis);
	if (handle != -1) {
		delete [] (char *)handle;
	}
	set_stringBufferHandle_value(env, pthis, (jlong)copy_str);
}

/*
 * Class:     jp_marisa_AgentNative
 * Method:    set_query
 * Signature: (I)V
 */
extern "C"
JNIEXPORT void JNICALL Java_jp_marisa_AgentNative_set_1query__I(JNIEnv *env, jobject pthis, jint key_id) {
	marisa::Agent* native_agent = ensure_create_agent(env, pthis);
	if (native_agent == NULL) {
		throw_NullPointerException(env, "handle is null");
		return;
	}

	try {
		native_agent->set_query(key_id);
	} catch (const marisa::Exception& ex) {
		throw_MarisaException(env, ex.filename(), ex.line(), ex.error_code(), ex.error_message());
		return;
	}
}

/*
 * Class:     jp_marisa_AgentNative
 * Method:    set_query
 * Signature: (J)V
 */
extern "C"
JNIEXPORT void JNICALL Java_jp_marisa_AgentNative_set_1query__J(JNIEnv *env, jobject pthis, jlong key_id) {
	marisa::Agent* native_agent = ensure_create_agent(env, pthis);
	if (native_agent == NULL) {
		throw_NullPointerException(env, "handle is null");
		return;
	}

	try {
		native_agent->set_query(static_cast<std::size_t>(key_id));
	} catch (const marisa::Exception& ex) {
		throw_MarisaException(env, ex.filename(), ex.line(), ex.error_code(), ex.error_message());
		return;
	}
}

/*
 * Class:     jp_marisa_AgentNative
 * Method:    dispose
 * Signature: ()V
 */
extern "C"
JNIEXPORT void JNICALL Java_jp_marisa_AgentNative_dispose(JNIEnv *env, jobject pthis) {
	marisa::Agent* native_agent = get_agent(env, pthis);
	if (native_agent != NULL) {
		delete native_agent;
	}

	set_handle_value(env, pthis, "Ljp/marisa/AgentNative;", (jlong)-1);

	jlong handle = get_stringBufferHandle_value(env, pthis);
	if (handle != -1) {
		delete [] (char *)handle;
	}
	set_stringBufferHandle_value(env, pthis, (jlong)-1);
}

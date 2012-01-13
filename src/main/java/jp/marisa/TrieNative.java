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
package jp.marisa;

public class TrieNative {
	//marisa_num_tries
	public static final int MARISA_MIN_NUM_TRIES = 0x00001;
	public static final int MARISA_MAX_NUM_TRIES = 0x0007F;
	public static final int MARISA_DEFAULT_NUM_TRIES = 0x00003;
	
	//marisa_cache_level
	public static final int MARISA_HUGE_CACHE = 0x00080;
	public static final int MARISA_LARGE_CACHE = 0x00100;
	public static final int MARISA_NORMAL_CACHE = 0x00200;
	public static final int MARISA_SMALL_CACHE = 0x00400;
	public static final int MARISA_TINY_CACHE = 0x00800;
	public static final int MARISA_DEFAULT_CACHE = MARISA_NORMAL_CACHE;
	
	// marisa_tail_mode
	public static final int MARISA_TEXT_TAIL = 0x01000;
	public static final int MARISA_BINARY_TAIL = 0x02000;
	public static final int MARISA_DEFAULT_TAIL = MARISA_TEXT_TAIL;
	
	// marisa_node_order
	public static final int MARISA_LABEL_ORDER = 0x10000;
	public static final int MARISA_WEIGHT_ORDER = 0x20000;
	public static final int MARISA_DEFAULT_ORDER = MARISA_WEIGHT_ORDER;
	
	// marisa_config_mask
	public static final int MARISA_NUM_TRIES_MASK = 0x0007F;
	public static final int MARISA_CACHE_LEVEL_MASK = 0x00F80;
	public static final int MARISA_TAIL_MODE_MASK = 0x0F000;
	public static final int MARISA_NODE_ORDER_MASK = 0xF0000;
	public static final int MARISA_CONFIG_MASK = 0xFFFFF;
	
    long handle;
    
    public TrieNative() {
    	handle = -1;
    }
	
	public native void build(KeysetNative keyset, int config_flags) throws MarisaException;
	public void build(KeysetNative keyset) throws MarisaException {
		build(keyset, 0);
	}
	
	public native void mmap(byte[] fileName) throws MarisaException;
	public native void map(byte[] chunk, int start, int length) throws MarisaException;
	public void map(byte[] chunk) throws MarisaException {
		map(chunk, 0, chunk.length);
	}
	
	public native void load(byte[] fileName) throws MarisaException;
	public native void read(int fd) throws MarisaException;
	
	public native void save(byte[] fileName) throws MarisaException;
	public native void write(int fd) throws MarisaException;
	
	public native boolean lookup(AgentNative agent) throws MarisaException;
	public native void reverse_lookup(AgentNative agent) throws MarisaException;
	public native boolean common_prefix_search(AgentNative agent) throws MarisaException;
	public native boolean predictive_search(AgentNative agent) throws MarisaException;
	
	public native long num_tries() throws MarisaException;
	public native long num_keys() throws MarisaException;
	public native long num_nodes() throws MarisaException;
	
	public native int tail_mode() throws MarisaException;
	public native int node_order() throws MarisaException;
	
	public native boolean empty() throws MarisaException;
	public native long size() throws MarisaException;
	public native long io_size() throws MarisaException;
	
	public native void clear() throws MarisaException;
	public native void swap(TrieNative trie) throws MarisaException;
	
	public native void dispose();
}

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

import java.util.ArrayList;
import java.util.Collection;
import java.util.List;

/**
 * This class provides rich interface to marisa.
 * 
 * <pre>
 * {@code 
 * Marisa marisa = new Marisa();
 * try {
 *   List<String> keyset = new ArrayList<String>();
 *   keyset.add("a");
 *   keyset.add("app");
 *   keyset.add("apple");
 *       
 *   marisa.build(keyset);
 *       
 *   final String query = "apple";
 *   List<Marisa.IdKeyPair> results = marisa.commonPrefixSearch(query);
 *   if (results == null) {
 *     System.out.println("not found: " + query);
 *   } else {
 *     for(Marisa.IdKeyPair result : results) {
 *       System.out.println(result.Key + ": " + result.Id);
 *     }
 *   }
 * } finally {
 *   marisa.close();
 * }
 * }
 * </pre>
 */
public class Marisa implements java.io.Closeable {
	
	public static class IdKeyPair {
		public long Id;
		public String Key;
	}
	
	@SuppressWarnings("unused")
	private static MarisaLoader loader = new MarisaLoader();
	
	private TrieNative trie;
	private java.nio.charset.Charset charset;
	private java.nio.charset.Charset environmentCharset;

	public Marisa() {
        try {
        	this.trie = new TrieNative();
        } catch (Exception ex) {
        	ex.printStackTrace();
        	return;
        }
        
		this.charset = java.nio.charset.Charset.forName("UTF-8");
		this.environmentCharset = java.nio.charset.Charset.defaultCharset();
	}
	
	public java.nio.charset.Charset getCharset() {
		return this.charset;
	}
	
	public void setCharset(java.nio.charset.Charset charset) {
		this.charset = charset;
	}
	
	public java.nio.charset.Charset getEnvironmentCharset() {
		return this.environmentCharset;
	}
	
	public void setEnvironmentCharset(java.nio.charset.Charset charset) {
		this.environmentCharset = charset;
	}
	
	public int size() {
		try {
			return (int)this.trie.num_keys();
		} catch (MarisaException ignore) {
			return 0;
		}
	}
	
	public void build(Collection<String> keys) {
		KeysetNative keyset = new KeysetNative();
		try {
			for(String key : keys) {
				byte[] bytes = key.getBytes(this.getCharset());
				keyset.push_back(bytes);
			}
			
			this.trie.build(keyset);
		} finally {
			keyset.dispose();
		}
	}
	
	public void load(String fileName) {
		byte[] bytes = fileName.getBytes(this.getEnvironmentCharset());
		this.trie.load(bytes);
	}
	
	public void save(String fileName) {
		byte[] bytes = fileName.getBytes(this.getEnvironmentCharset());
		this.trie.save(bytes);
	}
	
	public IdKeyPair lookup(String key) {
		return lookup(key, 0);
	}
	
	public IdKeyPair lookup(String key, long id) {
		byte[] bytes = key.getBytes(this.getCharset());
		
		AgentNative agent = new AgentNative();
		try {
			agent.set_query(bytes);
			agent.set_query(id);
			
			boolean matches = this.trie.lookup(agent);
			if (! matches) {
				return null;
			}
			
			KeyNative keyNative = agent.key();
			IdKeyPair ret = new IdKeyPair();
			
			ret.Id = keyNative.id();
			ret.Key = new String(keyNative.text(), this.getCharset());
			
			return ret;
		} finally {
			agent.dispose();
		}
	}
	
	public IdKeyPair reverseLookup(String key) {
		return reverseLookup(key, 0);
	}
	
	public IdKeyPair reverseLookup(String key, long id) {
		byte[] bytes = key.getBytes(this.getCharset());
		
		AgentNative agent = new AgentNative();
		try {
			agent.set_query(bytes);
			agent.set_query(id);
			
			this.trie.reverse_lookup(agent);
			
			KeyNative keyNative = agent.key();
			IdKeyPair ret = new IdKeyPair();
			
			ret.Id = keyNative.id();
			ret.Key = new String(keyNative.text(), this.getCharset());
			
			return ret;
		} finally {
			agent.dispose();
		}
	}
	
	public List<IdKeyPair> commonPrefixSearch(String key) {
		return commonPrefixSearch(key, 0);
	}
	
	public List<IdKeyPair> commonPrefixSearch(String key, long id) {
		byte[] bytes = key.getBytes(this.getCharset());
		
		AgentNative agent = new AgentNative();
		try {
			agent.set_query(bytes);
			agent.set_query(id);
			
			List<IdKeyPair> ret = new ArrayList<IdKeyPair>();
			boolean matches = this.trie.common_prefix_search(agent);
			while (matches) {
				KeyNative keyNative = agent.key();
				IdKeyPair pair = new IdKeyPair();
				
				pair.Id = keyNative.id();
				pair.Key = new String(keyNative.text(), this.getCharset());
				ret.add(pair);
				
				matches = this.trie.common_prefix_search(agent);
			}
			
			return ret;
		} finally {
			agent.dispose();
		}
	}
	
	public List<IdKeyPair> predictiveSearch(String key) {
		return predictiveSearch(key, 0);
	}
	
	public List<IdKeyPair> predictiveSearch(String key, long id) {
		byte[] bytes = key.getBytes(this.getCharset());
		
		AgentNative agent = new AgentNative();
		try {
			agent.set_query(bytes);
			agent.set_query(id);
			
			List<IdKeyPair> ret = new ArrayList<IdKeyPair>();
			boolean matches = this.trie.predictive_search(agent);
			while (matches) {
				KeyNative keyNative = agent.key();
				IdKeyPair pair = new IdKeyPair();
				
				pair.Id = keyNative.id();
				pair.Key = new String(keyNative.text(), this.getCharset());
				ret.add(pair);
				
				matches = this.trie.predictive_search(agent);
			}
			
			return ret;
		} finally {
			agent.dispose();
		}
	}
	
	public void clear() {
		this.trie.clear();
	}
	
	public void close() {
		if (this.trie != null) {
			this.trie.dispose();
			this.trie = null;
		}
	}
}

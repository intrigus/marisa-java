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

import java.io.*;

import org.junit.BeforeClass;
import org.junit.Test;
import static org.junit.Assert.*;
import static org.hamcrest.CoreMatchers.*;

public class TrieNativeTest {
    
	@BeforeClass
	public static void setUp() {
		java.lang.management.RuntimeMXBean runtimeMxBean = java.lang.management.ManagementFactory.getRuntimeMXBean();
		String processId = runtimeMxBean.getName().substring(0, runtimeMxBean.getName().indexOf('@'));
		System.out.println(processId + ": attatch your debugger to this process id, if you want to attach debugger.");
		
		@SuppressWarnings("unused")
		MarisaLoader loader = new MarisaLoader();
	}
	
	@Test
	public void test001() throws UnsupportedEncodingException {
		KeysetNative keyset = new KeysetNative();
		final String text = "APPLE";
		final byte[] text_bytes = text.getBytes("UTF-8");
		keyset.push_back(text_bytes, 0, text_bytes.length, 1.0F);
		assertThat(keyset.size(), is(1L));
		
		{
			KeyNative key = keyset.get(0);
			assertThat(key.textLength(), is(5L));
			byte[] tmp = key.text();
			assertThat(tmp.length, is(5));
			assertThat(new String(tmp, 0, tmp.length, "UTF-8"), is("APPLE"));
		}
		
		TrieNative trie = new TrieNative();
		trie.build(keyset, TrieNative.MARISA_DEFAULT_NUM_TRIES | TrieNative.MARISA_DEFAULT_TAIL | TrieNative.MARISA_DEFAULT_ORDER | TrieNative.MARISA_DEFAULT_CACHE);
		assertThat(trie.num_keys(), is(1L));
		
		AgentNative agent1 = new AgentNative();
		agent1.set_query(text_bytes, 0, text_bytes.length);
		
		{
			QueryNative q = agent1.query();
			assertThat(q.textLength(), is(5L));
			byte[] tmp = q.text();
			assertThat(tmp.length, is(5));
			assertThat(new String(tmp, 0, tmp.length, "UTF-8"), is("APPLE"));
		}
		
		boolean ret = trie.lookup(agent1);
		assertThat(ret, is(true));
		
		final String query2 = "ALICE";
		final byte[] query2_bytes = query2.getBytes("UTF-8");
		AgentNative agent2 = new AgentNative();
		agent2.set_query(query2_bytes, 0, query2_bytes.length);
		ret = trie.lookup(agent2);
		assertThat(ret, is(false));
		
		AgentNative agent4 = new AgentNative();
		agent4.set_query("".getBytes(), 0, 0);
		int num_found = 0;
		ret = trie.predictive_search(agent4);
		while (ret) {
			num_found++;
			KeyNative key = agent4.key();
			assertThat(key.textLength(), is(5L));
			byte[] tmp = key.text();
			assertThat(tmp.length, is(5));
			assertThat(new String(tmp, 0, tmp.length, "UTF-8"), is("APPLE"));

			ret = trie.predictive_search(agent4);
		}
		assertThat(num_found, is(1));
		
		keyset.dispose();
		assertThat(keyset.handle, is(-1L));
		agent1.dispose();
		assertThat(agent1.handle, is(-1L));
		assertThat(agent1.stringBufferHandle, is(-1L));
		agent4.dispose();
		assertThat(agent4.handle, is(-1L));
		assertThat(agent4.stringBufferHandle, is(-1L));
		trie.dispose();
		assertThat(trie.handle, is(-1L));
	}

}

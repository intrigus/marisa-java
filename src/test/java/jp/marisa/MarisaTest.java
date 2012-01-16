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

import static org.junit.Assert.*;

import java.io.File;
import java.nio.charset.Charset;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import org.junit.BeforeClass;
import org.junit.Test;
import static org.hamcrest.CoreMatchers.*;

public class MarisaTest {
	
	@BeforeClass
	public static void setUp() {
		java.lang.management.RuntimeMXBean runtimeMxBean = java.lang.management.ManagementFactory.getRuntimeMXBean();
		String processId = runtimeMxBean.getName().substring(0, runtimeMxBean.getName().indexOf('@'));
		System.out.println(processId + ": attatch your debugger to this process id, if you want to attach debugger.");
	}
	
	static File getWorkFile(String name) {
		File currentDirectory = new File(".");
		File targetDirectory = new File(currentDirectory, "target");
		File workFile;
		if (targetDirectory.exists()) {
			workFile = new File(targetDirectory, name);
		} else {
			workFile = new File(currentDirectory, name);
		}
		
		if (workFile.exists()) {
			workFile.delete();
		}
		
		return workFile;
	}

	@Test
	public void testMarisa() {
		Marisa marisa = new Marisa();
		assertThat(marisa, notNullValue());
		assertThat(marisa.size(), is(0));
		assertThat(marisa.getCharset(), notNullValue());
		assertThat(marisa.getCharset(), sameInstance(Charset.forName("UTF-8")));
		assertThat(marisa.getEnvironmentCharset(), notNullValue());
		assertThat(marisa.getEnvironmentCharset(), sameInstance(Charset.defaultCharset()));
		marisa.close();
	}


	@Test
	public void testSetCharset() {
		Marisa marisa = new Marisa();
		marisa.setCharset(Charset.forName("EUC-JP"));
		assertThat(marisa.getCharset(), notNullValue());
		assertThat(marisa.getCharset(), sameInstance(Charset.forName("EUC-JP")));
	}

	@Test
	public void testSetEnvironmentCharset() {
		Marisa marisa = new Marisa();
		marisa.setEnvironmentCharset(Charset.forName("EUC-JP"));
		assertThat(marisa.getEnvironmentCharset(), notNullValue());
		assertThat(marisa.getEnvironmentCharset(), sameInstance(Charset.forName("EUC-JP")));
		marisa.close();
	}

	@Test
	public void testBuild() {
		Marisa marisa = new Marisa();
		marisa.build(Arrays.asList("APPLE", "ALICE", "ASAHI"));
		assertThat(marisa.size(), is(3));
		marisa.close();
	}

	@Test
	public void testSave() {
		File workFile = getWorkFile("work-1113479346");
		Marisa marisa1 = new Marisa();
		marisa1.build(Arrays.asList("APPLE", "ALICE", "ASAHI"));
		assertThat(marisa1.size(), is(3));
		marisa1.save(workFile.getAbsolutePath());
		marisa1.close();
		
		Marisa marisa2 = new Marisa();
		marisa2.load(workFile.getAbsolutePath());
		assertThat(marisa2.size(), is(3));

		Marisa.IdKeyPair pair1 = marisa2.lookup("APPLE");
		assertThat(pair1, notNullValue());
		assertThat(pair1.Id, is(1L));
		assertThat(pair1.Key, is("APPLE"));
		
		Marisa.IdKeyPair pair2 = marisa2.lookup("ALICE");
		assertThat(pair2, notNullValue());
		assertThat(pair2.Id, is(0L));
		assertThat(pair2.Key, is("ALICE"));
		
		Marisa.IdKeyPair pair3 = marisa2.lookup("ASAHI");
		assertThat(pair3, notNullValue());
		assertThat(pair3.Id, is(2L));
		assertThat(pair3.Key, is("ASAHI"));
		
		Marisa.IdKeyPair pair4 = marisa2.lookup("ATOM");
		assertThat(pair4, nullValue());
		
		marisa2.close();
	}

	@Test
	public void testLookup001() {
		Marisa marisa = new Marisa();
		marisa.build(Arrays.asList("APPLE", "ALICE", "ASAHI"));
		assertThat(marisa.size(), is(3));
		
		Marisa.IdKeyPair pair1 = marisa.lookup("APPLE");
		assertThat(pair1, notNullValue());
		assertThat(pair1.Id, is(1L));
		assertThat(pair1.Key, is("APPLE"));
		
		Marisa.IdKeyPair pair2 = marisa.lookup("ALICE");
		assertThat(pair2, notNullValue());
		assertThat(pair2.Id, is(0L));
		assertThat(pair2.Key, is("ALICE"));
		
		Marisa.IdKeyPair pair3 = marisa.lookup("ASAHI");
		assertThat(pair3, notNullValue());
		assertThat(pair3.Id, is(2L));
		assertThat(pair3.Key, is("ASAHI"));
		
		Marisa.IdKeyPair pair4 = marisa.lookup("ATOM");
		assertThat(pair4, nullValue());
		
		marisa.close();
	}

	@Test
	public void testLookup002() {
		Marisa marisa = new Marisa();
		marisa.build(Arrays.asList("APPLE", "ALICE", "ASAHI"));
		assertThat(marisa.size(), is(3));
		
		Marisa.IdKeyPair pair1 = marisa.lookup("APPLE", 1);
		assertThat(pair1, notNullValue());
		assertThat(pair1.Id, is(1L));
		assertThat(pair1.Key, is("APPLE"));
		
		Marisa.IdKeyPair pair2 = marisa.lookup("APPLE", 2);
		assertThat(pair2, notNullValue());
		assertThat(pair2.Id, is(1L));
		assertThat(pair2.Key, is("APPLE"));
		
		Marisa.IdKeyPair pair3 = marisa.lookup("APPLE", 0);
		assertThat(pair3, notNullValue());
		assertThat(pair3.Id, is(1L));
		assertThat(pair3.Key, is("APPLE"));
		
		marisa.close();
	}

	@Test
	public void testReveseLookup() {
		Marisa marisa = new Marisa();
		marisa.build(Arrays.asList("APPLE", "ALICE", "ASAHI"));
		assertThat(marisa.size(), is(3));
		
		Marisa.IdKeyPair pair1 = marisa.reverseLookup("APPLE");
		assertThat(pair1, notNullValue());
		assertThat(pair1.Id, is(0L));
		assertThat(pair1.Key, is("ALICE"));
		
		marisa.close();
	}
	
	@Test
	public void testExampleCode() {
		Marisa marisa = new Marisa();
		try {
			List<String> keyset = new ArrayList<String>();
			keyset.add("a");
			keyset.add("app");
			keyset.add("apple");

			marisa.build(keyset);

			final String query = "apple";
			List<Marisa.IdKeyPair> results = marisa.commonPrefixSearch(query);
			if (results == null) {
				System.out.println("not found: " + query);
			} else {
				for (Marisa.IdKeyPair result : results) {
					System.out.println(result.Key + ": " + result.Id);
				}
			}
		} finally {
			marisa.close();
		}
	}

	@Test
	public void testCommonPrefixSearch001() {
		Marisa marisa = new Marisa();
		marisa.build(Arrays.asList("APPLE", "ALICE", "ASAHI"));
		assertThat(marisa.size(), is(3));
		
		List<Marisa.IdKeyPair> pairList1 = marisa.commonPrefixSearch("APPLE LUICE");
		assertThat(pairList1, notNullValue());
		assertThat(pairList1.size(), is(1));
		{
			Marisa.IdKeyPair pair0 = pairList1.get(0);
			assertThat(pair0, notNullValue());
			assertThat(pair0.Id, is(1L));
			assertThat(pair0.Key, is("APPLE"));
		}
		
		marisa.close();
	}

	@Test
	public void testCommonPrefixSearch002() {
		Marisa marisa = new Marisa();
		marisa.build(Arrays.asList("北斗", "北斗神拳", "北斗百裂拳", "北斗百裂脚", "北斗柔破斬"));
		assertThat(marisa.size(), is(5));
		
		List<Marisa.IdKeyPair> pairList1 = marisa.commonPrefixSearch("北斗神拳");
		assertThat(pairList1, notNullValue());
		assertThat(pairList1.size(), is(2));
		{
			Marisa.IdKeyPair pair0 = pairList1.get(0);
			assertThat(pair0, notNullValue());
			assertThat(pair0.Id, is(0L));
			assertThat(pair0.Key, is("北斗"));
			
			Marisa.IdKeyPair pair1 = pairList1.get(1);
			assertThat(pair1, notNullValue());
			assertThat(pair1.Id, is(2L));
			assertThat(pair1.Key, is("北斗神拳"));
		}
		
		marisa.close();
	}

	@Test
	public void testPredictiveSearch() {
		Marisa marisa = new Marisa();
		marisa.build(Arrays.asList("APPLE", "ALICE", "ASAHI"));
		assertThat(marisa.size(), is(3));
		
		List<Marisa.IdKeyPair> pairList1 = marisa.predictiveSearch("A");
		assertThat(pairList1, notNullValue());
		assertThat(pairList1.size(), is(3));
		{
			Marisa.IdKeyPair pair0 = pairList1.get(0);
			assertThat(pair0, notNullValue());
			assertThat(pair0.Id, is(0L));
			assertThat(pair0.Key, is("ALICE"));
			
			Marisa.IdKeyPair pair1 = pairList1.get(1);
			assertThat(pair1, notNullValue());
			assertThat(pair1.Id, is(1L));
			assertThat(pair1.Key, is("APPLE"));
			
			Marisa.IdKeyPair pair2 = pairList1.get(2);
			assertThat(pair2, notNullValue());
			assertThat(pair2.Id, is(2L));
			assertThat(pair2.Key, is("ASAHI"));
		}
		
		marisa.close();
	}

	@Test
	public void testClear() {
		Marisa marisa = new Marisa();
		marisa.build(Arrays.asList("APPLE", "ALICE", "ASAHI"));
		assertThat(marisa.size(), is(3));
	
		marisa.clear();
		assertThat(marisa.size(), is(0));
		
		marisa.close();
	}

	@Test
	public void testClose() {
		Marisa marisa = new Marisa();
		marisa.close();
		marisa.close();
		marisa.close();
	}

}

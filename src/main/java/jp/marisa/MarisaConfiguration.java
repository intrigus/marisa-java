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

import java.io.IOException;
import java.io.InputStream;
import java.util.Properties;

public class MarisaConfiguration {
    static Properties props;

    static void init() {
        try {
            InputStream is = MarisaConfiguration.class.getResourceAsStream("jp-marisa.properties");
            if (is == null) {
                // no configuration file is found, use system property 
            	props = System.getProperties();
                return;
            }

            // Load property file
            Properties tmp = new Properties();
            try {
            	tmp.load(is);
            } finally {
            	is.close();
            }

            props = tmp;
        } catch (IOException ex) {
        	ex.printStackTrace();
        	
            // configuration file load error, use system property 
        	props = System.getProperties();
        }
    }

    static {
        init();
    }
    
	static String getString(String key, String defaultValue, boolean allowNull, boolean allowEmpty) {
		String ret = props.getProperty(key);
		if (!allowNull && ret == null)
			return defaultValue;
		if (!allowEmpty && ret != null && ret.isEmpty())
			return defaultValue;
		
		return ret;
	}
	
	static boolean getBoolean(String key, boolean defaultValue) {
		String stringValue = getString(key, null, false, false);
		if (stringValue == null)
			return defaultValue;
		
		if (stringValue.compareToIgnoreCase("true") == 0 || stringValue.compareTo("1") == 0)
			return true;
		if (stringValue.compareToIgnoreCase("false") == 0 || stringValue.compareTo("0") == 0)
			return false;
		
		return defaultValue;
	}
    
    public static String getNativeLibraryFullPath() {
    	return getString("jp.marisa.nativeLibraryFullPath", null, true, false);
    }

    public static boolean getUseSystemLib() {
    	return getBoolean("jp.marisa.useSystemLib", false);
    }

    public static String getTempDirectoryName() {
    	return getString("jp.marisa.tempdir", System.getProperty("java.io.tmpdir"), false, false);
    }
}

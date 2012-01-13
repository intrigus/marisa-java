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

import java.io.BufferedInputStream;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.URL;
import java.security.DigestInputStream;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.Properties;

public final class MarisaLoader {
	static java.util.logging.Logger logger = java.util.logging.Logger.getLogger(MarisaLoader.class.getCanonicalName());
	
	static {
		load();
	}

	public static void load() {
		boolean success = false;
        File nativeLib = findNativeLibrary();
        if (nativeLib != null) {
        	logger.fine("found native library as " + nativeLib);
        	System.load(nativeLib.getAbsolutePath());
        	success = true;
		}
        
        if (!success) {
            // Load preinstalled marisa-java (in the path -Djava.library.path)
        	logger.fine("loading libmarisa-java.so using system default manner.");
    		System.loadLibrary("marisa-java");
        }
	}
	
    static File findNativeLibrary() {
    	String nativeLibraryFullPathName = MarisaConfiguration.getNativeLibraryFullPath();
        if (nativeLibraryFullPathName != null) {
        	logger.fine("jp.marisa.nativeLibraryFullPath=" + nativeLibraryFullPathName);
            File nativeLib = new File(nativeLibraryFullPathName);
            if (nativeLib.exists()) {
                return nativeLib.getAbsoluteFile();
            } else {
            	logger.fine(nativeLibraryFullPathName + " is not exists");
            }
        }
    	
		boolean useSystemLib = MarisaConfiguration.getUseSystemLib();
		if (useSystemLib) {
        	logger.fine("jp.marisa.useSystemLib is spcified.");
			return null;
		}

        // Load an OS-dependent native library inside a jar file
		final String nativeLibraryClassPath = "/jp/marisa/native/" + OSInfo.getNativeLibFolderPathForCurrentOS();
		final String nativeLibraryName = System.mapLibraryName("marisa-java");
		final URL resourceUrl = MarisaLoader.class.getResource(nativeLibraryClassPath + "/" + nativeLibraryName);

        if (resourceUrl != null) {
            // Temporary library folder. Use the value of jp.marisa.tempdir or java.io.tmpdir
            File tempDirectory = new File(MarisaConfiguration.getTempDirectoryName());

            // Extract and load a native library inside the jar file
            return extractLibraryFile(nativeLibraryClassPath, nativeLibraryName, tempDirectory);
        }

        return null; // Use a pre-installed marisa-java
    }
    
    /**
     * Extract the specified library file to the target folder
     * 
     * @param libFolderForCurrentOS
     * @param libraryFileName
     * @param targetFolder
     * @return
     */
    private static File extractLibraryFile(String libFolderForCurrentOS, String libraryFileName, File targetFolder) {
        String nativeLibraryFilePath = libFolderForCurrentOS + "/" + libraryFileName;
        final String prefix = "marisa-" + getVersion() + "-";
        String extractedLibFileName = prefix + libraryFileName;
        File extractedLibFile = new File(targetFolder, extractedLibFileName);

        try {
            if (extractedLibFile.exists()) {
                // test md5sum value
                String md5sum1 = md5sum(MarisaLoader.class.getResourceAsStream(nativeLibraryFilePath));
                String md5sum2 = md5sum(new FileInputStream(extractedLibFile));

				if (md5sum1.equals(md5sum2)) {
		        	logger.fine("native library is already extracted as " + extractedLibFile);
					return extractedLibFile;
				} else {
					// remove old native library file
					boolean deletionSucceeded = extractedLibFile.delete();
					if (!deletionSucceeded) {
						throw new IOException("failed to remove existing native library file: " + extractedLibFile.getAbsolutePath());
					}
				}
            }

            // Extract a native library file into the target directory
            InputStream reader = MarisaLoader.class.getResourceAsStream(nativeLibraryFilePath);
            FileOutputStream writer = new FileOutputStream(extractedLibFile);
            byte[] buffer = new byte[8192];
            int bytesRead = 0;
            while ((bytesRead = reader.read(buffer)) != -1) {
                writer.write(buffer, 0, bytesRead);
            }

            writer.close();
            reader.close();
            
        	logger.fine("native library is extracted " + nativeLibraryFilePath + " as " + extractedLibFile);

            // Set executable (x) flag to enable Java to load the native library
            if (!System.getProperty("os.name").contains("Windows")) {
                try {
                    Runtime.getRuntime().exec(new String[] { "chmod", "755", extractedLibFile.getAbsolutePath() })
                            .waitFor();
                }
                catch (Throwable e) {}
            }

            return extractedLibFile;
        }
        catch (IOException e) {
            e.printStackTrace(System.err);
            return null;
        }
    }
    
    /**
     * Computes the MD5 value of the input stream
     * 
     * @param input
     * @return
     * @throws IOException
     * @throws NoSuchAlgorithmException
     */
	static String md5sum(InputStream input) throws IOException {
		BufferedInputStream in = new BufferedInputStream(input);
		try {
			MessageDigest digest = java.security.MessageDigest
					.getInstance("MD5");
			DigestInputStream digestInputStream = new DigestInputStream(in,
					digest);
			for (; digestInputStream.read() >= 0;) {

			}
			ByteArrayOutputStream md5out = new ByteArrayOutputStream();
			md5out.write(digest.digest());
			return md5out.toString();
		} catch (NoSuchAlgorithmException e) {
			throw new IOException("MD5 algorithm is not available: ", e);
		} finally {
			in.close();
		}
	}
	
    /**
     * Get the marisa-java version by reading pom.properties embedded in jar.
     * This version data is used as a suffix of a dll file extracted from the
     * jar.
     * 
     * @return the version string
     */
	static String getVersion() {
		URL versionFile = MarisaLoader.class.getResource("/META-INF/maven/jp.marisa/marisa-java/pom.properties");
		if (versionFile == null)
			versionFile = MarisaLoader.class.getResource("/jp/marisa/VERSION");

		String version = "unknown";
		try {
			if (versionFile != null) {
				Properties versionData = new Properties();
				versionData.load(versionFile.openStream());
				version = versionData.getProperty("version", version);
				if (version.equals("unknown"))
					version = versionData.getProperty("VERSION", version);
				version = version.trim().replaceAll("[^0-9\\.]", "");
			}
		} catch (IOException e) {
			System.err.println(e);
		}
		return version;
	}
}

package com.cogi.audioprocessor;

public class Mp3Compressor {

    static {
        System.loadLibrary("cogi_jni_audioprocessor");
    }
    /**
     * Initialize LAME
     *
     * @param inSampleRate	input sample rate in samples/second
     * @param quality		quality.  0 (best, slow) to 9 (worst).
     *
     * @return pointer pointing to mp3_compression object created
     */
    public native static long init(int inSampleRate, int quality);

    /**
     * Encodes a buffer to mp3.
     *
     * @param pointer	pointer for the mp3 compression object
     * @param bufferIn	PCM data for the left channel
     * @param mp3buf	result encoded mp3 stream
     *
     * @return	number of bytes put into {@code mp3buf}.  This can be 0. <ul>
     * 				<li>-1 - {@code mp3buf} was too small</li>
     * 				<li>-2 - {@code malloc()} problem</li>
     * 				<li>-3 - {@link #init(int, int)} not called</li>
     * 				<li>-4 - psyco acoustic problems</li>
     * 			</ul>
     */
    public native static int encode(long pointer, short[] bufferIn, byte[] mp3buf, int samples);

    /**
     * Encodes a wav file to MP3
     *
     * @param sourcePath	Path to the source WAV file
     * @param targetPath	Path to the MP3 file
     */
    public native static void encodeFile(long pointer, String sourcePath, String targetPath);

    /**
     * Flush LAME buffer.
     *
     * @param pointer	pointer for the mp3 compression object
     * @param mp3buf	result encoded MP3 stream.  Must be at least 7200 bytes.
     * @return	number of bytes output to mp3buf.  Can be 0
     */
    public native static int flush(long pointer, byte[] mp3buf);

    /**
     * Close LAME.
     *
     * @param pointer	pointer for the mp3 compression object that will be deconstructed
     */
    public native static void close(long pointer);
}


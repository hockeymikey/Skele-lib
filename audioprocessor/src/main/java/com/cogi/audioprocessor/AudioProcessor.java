package com.cogi.audioprocessor;

public class AudioProcessor {

	static {
		System.loadLibrary("cogi_jni_audioprocessor");
	}

	public enum AudioProcessorProcessResult {
		Success,
		FullBuffer,
		PriorityStreamWriterKilledDueToOverflow,
		NonPriorityStreamWriterKilledDueToOverflow,
		StreamWritersNotRunning,
		NonPriorityStreamWriterNotRunning
	}

	public native static long init(int inSampleRate, int quality);

	public native static void initWithKillThresholdSeconds(long pointer, int killThresholdSeconds);

	public native static void setKillThresholdSeconds(long pointer, int killThresholdSeconds);

	public native static boolean samplesAreBeingWrittenToFilepath(long pointer, String filepath);

	public native static void startHighlightWithRecommendedDelay(long pointer, int recommendedDelay);

	public native static void startHighlightWithRecommendedDelay(long pointer, int recommendedDelay, String mp3Path, int compressionQuality);

	public native static void startHighlightWithRecommendedDelay(long pointer, int recommendedDelay, String wavePath, String mp3Path, int compressionQuality);

	public native static void startHighlightWithRecommendedDelay(long pointer, int recommendedDelay, String wavePath);

	public native static void stopHighlightWithQueueFlushed(long pointer, boolean flushQueue);

	public native static double bufferedAudioSizeInSeconds(long pointer);

	public native static double sampleCountLimit();

	public native static AudioProcessorProcessResult processSamples(long pointer, int samples,int sampleCount);
	/**
	 * Close LAME.
	 *
	 * @param pointer	pointer for the mp3 compression object that will be deconstructed
	 */
	public native static void close(long pointer);
}


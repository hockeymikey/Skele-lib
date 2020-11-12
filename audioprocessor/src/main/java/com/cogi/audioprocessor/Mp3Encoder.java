package com.cogi.audioprocessor;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.ShortBuffer;
import java.util.concurrent.Callable;

import android.util.Log;

/**
 * Encodes wav files to MP3.
 */
public class Mp3Encoder implements Callable<File> {
	
	/**
	 * Tag used for logging is {@value}
	 */
	private static final String LOG_TAG = "Mp3Encoder";
	
	/**
	 * wav file
	 */
	private File inputFile;
	
	/**
	 * mp3 file
	 */
	private File outputFile;
	
	/**
	 * Number of MP3 channels
	 */
	private int outChannels = 2;
	
	/**
	 * MP3 sample rate in samples/sec
	 */
	private int outSampleRate = 44100;
	
	/**
	 * MP3 bitrate in kHz
	 */
	private int outBitrate = 128;
	
	/**
	 * Quality - 0 (best, slow) to 9 worst
	 */
	private int quality = 4;
	
	/**
	 * Buffer size
	 */
	private int bufferSize = 4096;

	private long lamePointer;
	
	/**
	 * Specifies the wav file to encode.
	 * 
	 * @param inputFile	wav file
	 */
	public void setInput(File inputFile) {
		this.inputFile = inputFile;
	}
	
	/**
	 * Specifies the file to which to encode the audio
	 * 
	 * @param outputFile	mp3 file.  It doesn't need to exist already.
	 */
	public void setOutput(File outputFile) {
		this.outputFile = outputFile;
	}
	
	/**
	 * Specifies the quality of the audio.
	 * 
	 * <p>Defaults to 3 if not called.</p>
	 * 
	 * 
	 * @param quality	Quality between 0 (best, but slow) and 9 (worst, but faster)
	 */
	public void setQuality(int quality) {
		if (quality < 0 || quality > 9) {
			throw new IllegalArgumentException("quality must be between 0 and 9 inclusive.");
		}
		this.quality = quality;
	}
	
	/**
	 * Specifies the bitrate of the encoded file.
	 * 
	 * @param bitrate	Desired bit rate in kbps
	 */
	public void setBitRate(int bitrate) {
		this.outBitrate = bitrate;
	}
	
	/**
	 * Specify the size of the buffer to be used.
	 * 
	 * <p>Defaults to 4096 if not called.</p>
	 * 
	 * @param bufferSize	Buffer size in bytes.
	 */
	public void setBufferSize(int bufferSize) {
		this.bufferSize = bufferSize;
	}
	

	/**
	 * {@inheritDoc}
	 * 
	 * Encodes the audio.
	 * 
	 * @return 	Encoded file.
	 */
	public File call() throws Exception {
		if (inputFile != null) {
			
			if (outputFile != null) {
				int sampleRate = -1;

				WavPcmStream wavStream = new WavPcmStream(new FileInputStream(inputFile));
				FileOutputStream fos = new FileOutputStream(outputFile);
				try {
					wavStream.init();
					sampleRate = wavStream.getSampleRate();
					if (sampleRate > 0) {
						if (Log.isLoggable(LOG_TAG, Log.VERBOSE)) Log.v(LOG_TAG, "   Lame.init");
						lamePointer = AudioProcessor.init(outBitrate, quality);
						
						short[] inputSamples = new short[bufferSize];
						byte[] inputBuf = new byte[2*bufferSize];
						byte[] mp3Buf = new byte[2*bufferSize];
						int byteSamplesRead;
						int bytesEncoded;

						if (Log.isLoggable(LOG_TAG, Log.VERBOSE)) Log.v(LOG_TAG, "   Encoding");
						try {
							while (true) {
								byteSamplesRead = wavStream.read(inputBuf);
								if (byteSamplesRead > 0) {
									
									ByteBuffer byteBuffer = ByteBuffer.wrap(inputBuf, 0, byteSamplesRead);
									byteBuffer.order(ByteOrder.LITTLE_ENDIAN);
									ShortBuffer shortBuffer = byteBuffer.asShortBuffer();
									int shorts = shortBuffer.remaining();
									shortBuffer.get(inputSamples, 0, shorts);
									
									bytesEncoded = Mp3Compressor.encode(lamePointer, inputSamples, mp3Buf, inputSamples.length);
									
									if (bytesEncoded > 0) {
										fos.write(mp3Buf, 0, bytesEncoded);
										fos.flush();
									}
								} else {
									bytesEncoded = Mp3Compressor.flush(lamePointer, mp3Buf);
									if (bytesEncoded > 0) {
										fos.write(bytesEncoded);
										fos.flush();
									}
									break;
								}
							}
						} finally {
							if (Log.isLoggable(LOG_TAG, Log.VERBOSE)) Log.v(LOG_TAG, "   Lame.close");
							AudioProcessor.close(lamePointer);
						}
						if (Log.isLoggable(LOG_TAG, Log.VERBOSE)) Log.v(LOG_TAG, "   Done encoding");
					}
				} finally {
					if (fos != null) fos.close();
					if (wavStream != null) wavStream.close();
				}
				
				return outputFile;
			} else {
				throw new IllegalStateException("No output file specified");
			}
			
		} else {
			throw new FileNotFoundException("No input wav file to encode.");
		}
	}

}

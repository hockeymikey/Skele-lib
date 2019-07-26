package com.cogi.audioprocessor;

import java.io.IOException;
import java.io.InputStream;

/**
 * {@link PcmStream} for getting PCM data from a WAV file.
 */
public class WavPcmStream extends PcmStream {
	/**
	 * Number of channels
	 */
	private int channels;
	/**
	 * Sample rate
	 */
	private int sampleRate;
	/**
	 * Sample size in bits
	 */
	private int sampleSizePerChannel;
	/**
	 * Total number of samples
	 */
	private int numSamples;
	
	/**
	 * Indicates if {@link #init()} has been previously called.
	 */
	private boolean hasInit = false;
	
	/**
	 * Constructor
	 * 
	 * @param fileStream	audio file input stream
	 */
	public WavPcmStream(InputStream fileStream) {
		super(fileStream);
	}

	/**
	 * {@inheritDoc}
	 */
	@Override
	public synchronized void init() throws IOException {
		if (!hasInit) {
			hasInit = true;
			StringBuilder headerBuilder = new StringBuilder();
			for(int i=0; i<4; i++) {
				headerBuilder.append((char)is.read());
			}
			is.skip(4);
			for(int i=0; i<8; i++) {
				headerBuilder.append((char)is.read());
			}
			if(!"RIFFWAVEfmt ".equals(headerBuilder.toString())) {
				throw new IOException("File is not a wave file, header text was '" + headerBuilder.toString() + "'");
			}
			
			is.skip(6);
			channels = (is.read() & 0xFF) | ((is.read() & 0xFF)<<8);
			
			sampleRate = (is.read() & 0xFF) | ((is.read() & 0xFF)<<8) | ((is.read() & 0xFF)<<16) | ((is.read() & 0xFF)<<24);
			is.skip(6);
			
			sampleSizePerChannel = (is.read() & 0xFF) | ((is.read() & 0xFF)<<8);
			
			headerBuilder = new StringBuilder();
			for(int i=0; i<4; i++) {
				headerBuilder.append((char)is.read());
			}
			if(!"data".equals(headerBuilder.toString())) {
				throw new IOException("File doesn't have data section header: " + headerBuilder.toString());
			}
			
			numSamples = ( (is.read() & 0xFF) | ((is.read() & 0xFF)<<8) | ((is.read() & 0xFF)<<16) | ((is.read() & 0xFF)<<24) ) / (channels * (sampleSizePerChannel >> 3));
		}
	}

	/**
	 * {@inheritDoc}
	 */
	@Override
	public int getChannels() {
		return channels;
	}

	/**
	 * {@inheritDoc}
	 */
	@Override
	public int getSampleSizePerChannel() {
		return sampleSizePerChannel;
	}

	/**
	 * {@inheritDoc}
	 */
	@Override
	public int getSampleRate() {
		return sampleRate;
	}
	
	/**
	 * Gets the total number of samples in the file.
	 * 
	 * @return	Total number of samples
	 */
	public int getNumSamples() {
		return numSamples;
	}

	/**
	 * {@inheritDoc}
	 */
	@Override
	public int read() throws IOException {
		return is.read();
	}

	/**
	 * {@inheritDoc}
	 */
	@Override
	public int read(byte[] buffer, int byteOffset, int byteCount) throws IOException {
		return is.read(buffer, byteOffset, byteCount);
	}

}

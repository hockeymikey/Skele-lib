package com.cogi.audioprocessor;

import java.io.IOException;
import java.io.InputStream;

/**
 * Class for accessing PCM data read from an audio file.
 */
public abstract class PcmStream extends InputStream {
	
	/**
	 * Audio file stream
	 */
	protected final InputStream is;
	
	/**
	 * Constructor
	 * 
	 * @param fileStream	Stream to the audio file
	 */
	protected PcmStream(InputStream fileStream) {
		is = fileStream;
	}

	/**
	 * Initializes reading the file in.
	 * 
	 * This will read in whatever file headers need to be read.
	 */
	public abstract void init() throws IOException;
	
	/**
	 * Returns the number of channels of audio
	 * 
	 * @return	Number of channels of audio
	 */
	public abstract int getChannels();
	
	/**
	 * Returns the number of bits per channel of audio
	 * 
	 * @return	Number of bits per channel of audio
	 */
	public abstract int getSampleSizePerChannel();
	
	/**
	 * Returns the sample rate of the audio in samples per second.
	 * 
	 * @return	Sample rate
	 */
	public abstract int getSampleRate();
	
	/**
	 * {@inheritDoc}
	 */
	@Override
	public abstract int read() throws IOException;
	
	/**
	 * {@inheritDoc}
	 */
	@Override
	public abstract int read(byte[] buffer, int byteOffset, int byteCount) throws IOException;
	
	/**
	 * {@inheritDoc}
	 */
	@Override
	public void close() throws IOException {
		is.close();
	}

}

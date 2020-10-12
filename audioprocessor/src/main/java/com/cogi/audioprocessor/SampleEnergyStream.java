package com.cogi.audioprocessor;

import java.io.IOException;

/**
 * Utility which may be used to read in combined-channel sample energies.
 */
public class SampleEnergyStream {

	/**
	 * Audio input stream
	 */
	private final PcmStream is;
	
	/**
	 * Constructor
	 * @param inputStream
	 */
	public SampleEnergyStream(PcmStream inputStream) {
		this.is = inputStream;
	}
	
	/**
	 * Initializes the input stream.
	 * 
	 * <p>
	 * This should be called prior to calling {@link #read()} or {@link #read(double[], int, int)}
	 * and should only be called once.  However, alternatively, {@link PcmStream#init()} can be called
	 * instead.
	 * </p>
	 * 
	 * @throws IOException
	 */
	public void init() throws IOException {
		is.init();
	}
	
	/**
	 * Gets the sample rate of the stream.
	 * 
	 * @return	Sample rate in samples/second.
	 */
	public int getSampleRate() {
		return is.getSampleRate();
	}
	
	/**
	 * Reads a single sample combined-channel energy.
	 * 
	 * @return	Energy or {@link Double#NaN} if end-of-stream
	 * @throws IOException	
	 */
	public double read() throws IOException {
		//this is pretty inefficient:
		int channels = is.getChannels();
		int size = is.getSampleSizePerChannel();

		if (size % 8 != 0) {
			throw new IOException("Only files with sample sizes with multiples of 8 bits are supported.");
		}
		
		short channelValue;
		double value = 0.0;
		
		int read;
		for (int i=0; i<channels; i++) {
			channelValue = 0;
			for (int j=0; j<size; j+=8) {
				read = is.read();
				if(read == -1) {
					return Double.NaN;
				} else {
					channelValue |= (read & 0xFF) << j;
				}
			}
			value += Math.abs(channelValue);
		}
		value = value / channels;
		
		return value;
	}
	
	
	/**
	 * Reads in some number of combined-channel energies.
	 * 
	 * @param buf		Output buffer
	 * @param offset	Offset in the output buffer to begin
	 * @param count		Number of samples to read
	 * @return	Number of samples read
	 * 
	 * @throws IOException
	 */
	public int read(double[] buf, int offset, int count) throws IOException {
		int read = 0;
		double nrg;
		for (int i=offset; i<count && i<buf.length; i++) {
			nrg = read();
			if (Double.isNaN(nrg)) {
				break;
			}
			buf[i] = nrg;
			read++;
		}
		return read;
	}
	
	/**
	 * Closes the associated streams.
	 * 
	 * @throws IOException
	 */
	public void close() throws IOException {
		is.close();
	}

}

package com.cogi.audioprocessor;

import java.io.BufferedInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

/**
 * Vector of values representing a waveform for an audio file.
 */
public class WaveformVector {
	
	/**
	 * Vector of values
	 */
	private final byte[] vector;
	
	/**
	 * Constructor
	 * 
	 * @param vector	Vector of values
	 */
	WaveformVector(byte[] vector) {
		this.vector = vector;
	}
	
	/**
	 * Writes the vector to an output stream.
	 * 
	 * @param os	Output stream of interest.
	 * 
	 * @throws IOException	If there is a problem writing
	 */
	public void write(OutputStream os) throws IOException {
		os.write(vector);
	}
	
	/**
	 * Returns the size of the vector.
	 * 
	 * @return	Size of the vector
	 */
	public int getSize() {
		return vector.length;
	}
	
	/**
	 * Returns the value of the vector at the specified index
	 * 
	 * @param index	Index, must be less than {@link #getSize()}
	 * 
	 * @return	Waveform vector height expressing the energy in the waveform at the specified position.  
	 * 			Value will be between 0 and 255.
	 */
	public int getValue(int index) {
		return 0xFF & vector[index];
	}
	
	/**
	 * Factory method for generating a vector based on a wav file.
	 * 
	 * @param is	Input stream to the audio file.  Stream will be closed once this method has finished.
	 * @param targetSize	Target size of the vector
	 * @return	vector, won't be {@code null}
	 * @throws IOException	if there is a problem reading the audio file
	 */
	public static WaveformVector generateFromWav(InputStream is, int targetSize) throws IOException {
		return new WaveformVector(normalizeHeights(getHeightsForWav(is, targetSize)));
	}

	
	/**
	 * Factory method for generating a vector based on an mp3 file.
	 * 
	 * @param is	Input stream to the audio file.  Stream will be closed once this method has finished.
	 * @param duration	Duration of the audio file in ms
	 * @param targetSize	Target size of the vector
	 * @return	vector, won't be {@code null}
	 * @throws IOException	if there is a problem reading the audio file
	 */
	public static WaveformVector generateFromMp3(InputStream is, int duration, int targetSize) throws IOException {
		return new WaveformVector(normalizeHeights(getHeightsForMp3(is, duration, targetSize)));
	}
	
	/**
	 * Gets the heights for a wav file
	 * 	
	 * @param in	Stream to the audio file
	 * @param targetSize	Target size of the vector
	 * @return	array of heights
	 * @throws IOException
	 */
	static double[] getHeightsForWav(InputStream in, int targetSize) throws IOException {		
		BufferedInputStream bis = new BufferedInputStream(in);
		WavPcmStream pcm = new WavPcmStream(bis);
		SampleEnergyStream nrg = new SampleEnergyStream(pcm);
		try {
			nrg.init();
			int numSamples = pcm.getNumSamples();
			return getHeightsForStream(nrg, numSamples, targetSize);
		} finally {
			nrg.close();
		}
	}

	/**
	 * Gets the heights for an mp3 file
	 * 	
	 * @param in		Input stream to the audio file
	 * @param duration	Duration in ms of the audio file
	 * @param targetSize	Target size of the vector
	 * @return	array of heights
	 * @throws IOException
	 */
	static double[] getHeightsForMp3(InputStream in, int duration, int targetSize) throws IOException {		
		BufferedInputStream bis = new BufferedInputStream(in);
		Mp3PcmStream pcm = new Mp3PcmStream(bis);
		SampleEnergyStream nrg = new SampleEnergyStream(pcm);
		try {
			nrg.init();
			int numSamples = duration / 1000 * pcm.getSampleRate() * 2/pcm.getChannels();		//I don't really understand why we need the 2/channels thing - maybe this is some sort of bug.
			return getHeightsForStream(nrg, numSamples, targetSize);
		} finally {
			nrg.close();
		}
	}
	
	/**
	 * Gets heights for everything in an energy stream
	 * 
	 * @param nrg			energy stream
	 * @param numSamples	Total number of samples
	 * @param targetSize	Target vector length
	 * 
	 * @return	heights
	 * @throws IOException
	 */
	static double[] getHeightsForStream(SampleEnergyStream nrg, int numSamples, int targetSize) throws IOException {
		int blockSize = 1;
		if (numSamples > targetSize) {
			blockSize = numSamples / targetSize;
		}
		if (blockSize > nrg.getSampleRate() / 10) {
			blockSize = nrg.getSampleRate() / 10;
		}
		
		double[] heights = new double[(int)Math.ceil(numSamples / blockSize)];
		
		final double tau = 1.0 / ((double) (blockSize << 8));	// gain should range from 0-255.
		
		// Calculate block heights
		double height;
		double gain; 
		
		for(int i=0; i<heights.length; i++) {
			height = 0.0;
			for(int b=0; b<blockSize; b++) {
				gain = nrg.read();
				if (Double.isNaN(gain)) {
					break;
				}
				
				// clean up for mp3
				if (gain == 210 || gain == 216 || gain == 222 || gain == 123 || gain == 240 || gain == 146 || gain == 112 || gain == 126 || gain == 241 || gain == 140) {	//fix for mp3
					gain = 0.5;
				}
				
				height += tau * gain;
			}
			heights[i] = height;
		}
		
		double[] filteredHeights;

		// a little bit of post filtering if not already done
		if (blockSize < 3) {
			filteredHeights = new double[heights.length - 2];
			for (int i=1; i<heights.length-1; i++) {
				filteredHeights[i-1] = (heights[i-1] + heights[i] + heights[i+1]) / 3.0;
			}
		} else {
			filteredHeights = heights;
		}
		
		return filteredHeights;
	}
	
	/**
	 * Normalizes array of heights to an array of unsigned bytes.
	 * 
	 * @param heights	Heights
	 * @return	Normalized vector
	 */
	static byte[] normalizeHeights(double[] heights) {
		double maxHeight = 0;
		for (double height : heights) {
			if (height > maxHeight) {
				maxHeight = height;
			}
		}
		
		byte[] normalized = new byte[heights.length];
		for (int i=0; i<heights.length; i++) {
			normalized[i] = (byte) (heights[i] / maxHeight * 255);
		}
		
		return normalized;
	}
}

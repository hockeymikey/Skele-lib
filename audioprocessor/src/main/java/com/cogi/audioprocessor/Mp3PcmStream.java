package com.cogi.audioprocessor;

import java.io.IOException;
import java.io.InputStream;

import javazoom.jl.decoder.Bitstream;
import javazoom.jl.decoder.BitstreamException;
import javazoom.jl.decoder.Decoder;
import javazoom.jl.decoder.DecoderException;
import javazoom.jl.decoder.Header;
import javazoom.jl.decoder.SampleBuffer;

/**
 * {@link PcmStream} that can be used to read data from an MP3 file
 */
public class Mp3PcmStream extends PcmStream {
	
	/**
	 * MP3 bitstream
	 */
	private final Bitstream bs;
	
	/**
	 * MP3 decoder
	 */
	private final Decoder decoder;
	
	/**
	 * Buffer of data for the current frame
	 */
	private SampleBuffer sampleBuffer;
	
	/**
	 * Samples in {@link #sampleBuffer}.
	 */
	private short[] samples;
	
	/**
	 * Current index in {@link #samples}
	 */
	private int sampleIdx = 0;
	
	/**
	 * Indicates if the current sample byte is the least significant byte
	 */
	private boolean isSampleLSB = true;
	
	/**
	 * Sample rate in samples/sec
	 */
	private int sampleRate;
	
	/**
	 * Number of channels
	 */
	private int channels;

	/**
	 * Constructor
	 * @param fileStream	Stream from the audio file
	 */
	public Mp3PcmStream(InputStream fileStream) {
		super(fileStream);
		bs = new Bitstream(is);
		decoder = new Decoder();
	}

	/**
	 * {@inheritDoc}
	 */
	@Override
	public void init() throws IOException {
		loadFrame();
	}
	
	/**
	 * Loads the next frame.
	 * 
	 * @throws IOException
	 */
	private void loadFrame() throws IOException {
		try {
			Header header = bs.readFrame();
			if (header != null) {
				sampleBuffer = (SampleBuffer) decoder.decodeFrame(header, bs);
				
				sampleRate = decoder.getOutputFrequency();
				channels = decoder.getOutputChannels();
				
				samples = sampleBuffer.getBuffer();
				sampleIdx = 0;
				isSampleLSB = true;
			} else {
				throw new IOException("No header");
			}
		} catch (BitstreamException e) {
			throw new IOException(e);
		} catch (DecoderException e) {
			throw new IOException(e);
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
		return 16;
	}

	/**
	 * {@inheritDoc}
	 */
	@Override
	public int getSampleRate() {
		return sampleRate;
	}

	/**
	 * {@inheritDoc}
	 */
	@Override
	public int read() throws IOException {
		if (sampleIdx >= samples.length) {
			bs.closeFrame();
			try {
				loadFrame();
			} catch (IOException e) {
				return -1;
			}
		}
		short sample = samples[sampleIdx];
		if (isSampleLSB) {
			isSampleLSB = false;
			
			return sample & 0xFF;
		} else {
			isSampleLSB = true;
			sampleIdx++;
			
			return (sample >> 8) & 0xFF;
		}
	}

	/**
	 * {@inheritDoc}
	 */
	@Override
	public int read(byte[] buffer, int byteOffset, int byteCount) throws IOException {
		if (sampleIdx >= samples.length) {
			bs.closeFrame();
			try {
				loadFrame();
			} catch (IOException e) {
				return -1;
			}
		}
		int outIdx = byteOffset;
		int read = 0;
		byte val;
		short sample;
		while (sampleIdx < samples.length && outIdx < buffer.length && read < byteCount) {
			sample = samples[sampleIdx];
			if (isSampleLSB) {
				isSampleLSB = false;
				val = (byte) (sample & 0xFF);
			} else {
				isSampleLSB = true;
				sampleIdx++;
				val = (byte) ((sample >> 8) & 0xFF);
			}
			buffer[outIdx++] = val;
			read++;
		}
		return read;
	}

	/**
	 * {@inheritDoc}
	 */
	@Override
	public void close() throws IOException {
		try {
			bs.close();
		} catch (BitstreamException e) {
			throw new IOException(e);
		}
	}
}

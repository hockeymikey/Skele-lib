package com.cogi.skele;

public class Skele {

	static {
		System.loadLibrary("cogi_jni_skele");
	}

	public native static boolean testlocks();
}


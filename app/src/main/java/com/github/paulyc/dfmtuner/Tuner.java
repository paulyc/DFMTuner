package com.github.paulyc.dfmtuner;

import android.hardware.usb.UsbDevice;
import android.util.Log;

public class Tuner {
    private static final String TAG = "Tuner";

    static {
        System.loadLibrary("native-lib");
    }

    private static final TunerMediaSource src = new TunerMediaSource();

    public Tuner() {

    }

    public void tune(int frequency, int program) {

    }
    public void setDevice(UsbDevice usbDevice) {

    }
    public native void start();
    public native void configure(int frequency, int program);
    public native void poll();

    public void onAudioData(byte[] audio_data) {
        //t.onAudioData(audio_data);
    }

    public void onLogMessage(String msg) {
        Log.i(TAG, "onLogMessage(\"" + msg + "\"");
    }
}

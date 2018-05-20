package com.github.paulyc.dfmtuner;

import android.hardware.usb.UsbDevice;
import android.util.Log;

public class Tuner {
    private static final String TAG = "Tuner";

    static {
        System.loadLibrary("native-lib");
    }

    public Tuner() {
        init();
        configure(101100000, 0);
    }
    public void setDevice(UsbDevice usbDevice) {

    }
    private native void init();
    public native void start();
    public native void stop();
    public native void configure(int frequency, int program);
    public native byte[] pollAudioData();

    public void onLogMessage(String msg) {
        Log.i(TAG, "onLogMessage(\"" + msg + "\"");
    }
}

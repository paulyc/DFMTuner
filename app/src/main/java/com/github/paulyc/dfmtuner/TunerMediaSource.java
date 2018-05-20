package com.github.paulyc.dfmtuner;

import android.media.MediaDataSource;
import android.util.Log;

import java.io.IOException;

public class TunerMediaSource extends MediaDataSource
{
    private static final String TAG = "TunerMediaPlayer";

    private byte[] audioChunk = null;
    private int audioChunkRdOffset = 0;
    private long streamPosition = 0;

    private Tuner tuner;

    public TunerMediaSource(Tuner t) {
        tuner = t;
    }

    @Override
    public int readAt(long position, byte[] buffer, int offset, int size) throws IOException {
        Log.d(TAG, "readAt(" + position + "," + buffer + "," + offset + "," + size + ")");

        int bytes_copied = 0;
        while (size > 0) {
            if (audioChunk == null) {
                audioChunk = tuner.pollAudioData();
                if (audioChunk == null) {
                    while (size > 0) {
                        buffer[offset++] = 0;
                        ++bytes_copied;
                        --size;
                    }
                    Log.d(TAG, "returning all zeroes " + bytes_copied);
                    return bytes_copied;
                }
            }
            while (size > 0 && audioChunkRdOffset < audioChunk.length) {
                buffer[offset++] = audioChunk[audioChunkRdOffset++];
                ++bytes_copied;
                --size;
            }
            if (audioChunkRdOffset == audioChunk.length) {
                streamPosition += audioChunk.length;
                audioChunk = null;
                audioChunkRdOffset = 0;
            }
        }
        Log.d(TAG, "returning " + bytes_copied);
        return bytes_copied;
    }

    @Override
    public long getSize() throws IOException {
        Log.d(TAG, "getSize");
        return -1;
    }

    @Override
    public void close() throws IOException {
        audioChunk = null;
    }
}

package com.github.paulyc.dfmtuner;

import android.media.MediaDataSource;
import android.media.MediaPlayer;
import android.util.Log;

import java.io.IOException;
import java.util.ArrayDeque;
import java.util.Deque;
import java.util.NoSuchElementException;
import java.util.concurrent.LinkedBlockingDeque;

public class TunerMediaSource extends MediaDataSource implements Runnable, MediaPlayer.OnPreparedListener, MediaPlayer.OnErrorListener, MediaPlayer.OnInfoListener
{
    private static final String TAG = "TunerMediaPlayer";

    private final MediaPlayer mp = new MediaPlayer();
    private Boolean running = false;
    private long streamPosition = 0;

    @Override
    public boolean onError(MediaPlayer mp, int what, int extra) {
        Log.e(TAG, "MediaPlayer error [what:" + what + "] [extra:" + extra + "]");
        return true;
    }

    @Override
    public boolean onInfo(MediaPlayer mp, int what, int extra) {
        Log.i(TAG, "MediaPlayer info [what:" + what + "] [extra:" + extra + "]");
        return true;
    }

    private class DataChunk {
        byte[] audio_data;
        long stream_position;
        int chunk_offset;
    }

    private Deque<DataChunk> chunkDeque = new LinkedBlockingDeque<>();

    public void start() {
        new Thread(this).start();
    }

    public void stop() {
        //synchronized (this) {
            if (running) {
                mp.stop();
                mp.reset();
                running = false;
            }
       // }
    }

    @Override
    public void run() {
        //synchronized (this) {
            mp.setDataSource(this);
            mp.setOnErrorListener(this);
            mp.setOnPreparedListener(this);
            mp.prepareAsync();
       // }
    }

    @Override
    public void onPrepared(MediaPlayer mp_) {
        Log.d(TAG, "onPrepared");
        //synchronized (this) {
        //    running = true;
            mp_.start();
        //}
    }

    public void onAudioData(byte[] audio_data) {
        Log.d(TAG, "onAudioData");
        //synchronized (this) {
            DataChunk chk = new DataChunk();
            chk.audio_data = audio_data;
            chk.stream_position = streamPosition;
            chk.chunk_offset = 0;
            streamPosition += audio_data.length / 4;
            chunkDeque.addLast(chk);
        //}
    }

    private DataChunk front = null;

    @Override
    public int readAt(long position, byte[] buffer, int offset, int size) throws IOException {
        Log.d(TAG, "readAt(" + position + "," + buffer + "," + offset + "," + size + ")");
        
        //synchronized (this) {
            int bytes_copied = 0;
            //DataChunk front = null;
            while (size > 0) {
                if (front == null) {
                    try {
                        front = chunkDeque.removeFirst();
                    } catch (NoSuchElementException ex) {
                        while (size > 0) {
                            buffer[offset++] = 0;
                            ++bytes_copied;
                            --size;
                        }
                        return bytes_copied;
                    }
                }
                while (size > 0 && front.chunk_offset < front.audio_data.length) {
                    buffer[offset++] = front.audio_data[front.chunk_offset++];
                    ++bytes_copied;
                    --size;
                }
                if (front.chunk_offset == front.audio_data.length) {
                    front = null;
                }
            }
            Log.d(TAG, "returning " + bytes_copied);
            return bytes_copied;
        //}
    }

    @Override
    public long getSize() throws IOException {
        Log.d(TAG, "getSize");
        return 6666666L;
    }

    @Override
    public void close() throws IOException {
        //synchronized (this) {
            while (!chunkDeque.isEmpty()) {
                chunkDeque.pop();
            }
        //}
    }
}

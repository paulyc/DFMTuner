package com.github.paulyc.dfmtuner;

import android.media.MediaPlayer;
import android.util.Log;

public class TunerMediaPlayer extends MediaPlayer implements MediaPlayer.OnPreparedListener, MediaPlayer.OnErrorListener, MediaPlayer.OnInfoListener {

    private static final String TAG = "TunerMediaPlayer";

    private TunerMediaSource src;

    public TunerMediaPlayer(Tuner t) {
        super();

        src = new TunerMediaSource(t);
        setDataSource(src);
        setOnPreparedListener(this);
        setOnInfoListener(this);
        setOnErrorListener(this);
        prepareAsync();
    }

    @Override
    public boolean onError(MediaPlayer mp, int what, int extra) {
        Log.e(TAG, "error [what:" + what + "] [extra:" + extra + "]");
        return true;
    }

    @Override
    public boolean onInfo(MediaPlayer mp, int what, int extra) {
        Log.i(TAG, "info [what:" + what + "] [extra:" + extra + "]");
        return true;
    }

    @Override
    public void onPrepared(MediaPlayer mp_) {
        Log.d(TAG, "onPrepared");
    }
}

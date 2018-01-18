package com.example.anu.wavemaker;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.MotionEvent;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    public native void startEngine();
    public native void stopEngine();
    public native void touchEvent(int action);

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        startEngine();
}

    @Override
    protected void onDestroy() {
        stopEngine();
        super.onDestroy();
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        touchEvent(event.getAction());
        return super.onTouchEvent(event);
    }
}

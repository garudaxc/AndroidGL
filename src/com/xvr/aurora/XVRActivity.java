package com.xvr.aurora;

import android.app.Activity;
import android.view.Choreographer;
import android.view.MotionEvent;
import android.view.KeyEvent;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.os.Bundle;
import android.util.Log;


public class XVRActivity extends Activity implements SurfaceHolder.Callback,  android.view.Choreographer.FrameCallback {	

    static {
        System.loadLibrary("simplejni");
        System.loadLibrary("aurora-native");
    }

    SurfaceView mView;
    public static String TAG = "aurora_MainActivity";

    @Override protected void onCreate(Bundle icicle) {
        super.onCreate(icicle);
		
        Window window = getWindow();
        WindowManager.LayoutParams params = window.getAttributes();
        params.systemUiVisibility = View.SYSTEM_UI_FLAG_HIDE_NAVIGATION;
        window.setAttributes(params);

        //mView = new GL2JNIView(getApplication());
        //setContentView(mView);
        mView = new SurfaceView(this);        
        setContentView(mView);
		mView.getHolder().addCallback( this );
	
        nativeOnCreate();
        GL2JNILib.InitUsbDevice(this);
        
        startVsync(this, this);
    }

    @Override protected void onResume() {
        super.onResume();
        //mView.onResume();
        
        nativeOnResume();
    }
    
    @Override protected void onPause() {
        super.onPause();
        //mView.onPause();
        
        nativeOnPause();    
    }


	@Override
	protected void onDestroy() {
		// TODO Auto-generated method stub
		super.onDestroy();
		
		nativeOnDestroy();
	}

	@Override
	protected void onStart() {
		// TODO Auto-generated method stub
		super.onStart();
	}

	@Override
	protected void onStop() {
		// TODO Auto-generated method stub
		super.onStop();
	}
	
	@Override
	public boolean dispatchTouchEvent (MotionEvent ev) {
		
		/*final int historySize = ev.getHistorySize(); 
		final int pointerCount = ev.getPointerCount();   
		for (int h = 0; h < historySize; h++) {     
			System.out.printf("At time %d:", ev.getHistoricalEventTime(h));     
			
			for (int p = 0; p < pointerCount; p++) {      
				System.out.printf("  pointer %d: (%f,%f)",      
						ev.getPointerId(p), ev.getHistoricalX(p, h), ev.getHistoricalY(p, h));     
				}    
			}    
		System.out.printf("At time %d:", ev.getEventTime());  
		for (int p = 0; p < pointerCount; p++) {   
			System.out.printf("  pointer %d: (%f,%f)", ev.getPointerId(p), ev.getX(p), ev.getY(p));
		} */
		
		int action = ev.getAction();
		float x = ev.getX();
		float y = ev.getY();
		
		nativeOnEvent(action, x, y);
		
		return false;
	}
	
	@Override
	public boolean dispatchKeyEvent (KeyEvent event) {
		return false;
	}
	
	@Override
	public boolean dispatchGenericMotionEvent (MotionEvent ev) {
		return false;
	}

    protected static native void nativeOnCreate();
    protected static native void nativeOnResume();
    protected static native void nativeOnPause();
    protected static native void nativeOnDestroy();
    protected static native void nativeOnEvent(int action, float x, float y);

	@Override
	public void surfaceChanged(SurfaceHolder holder, int format, int width,
			int height) {
		// TODO Auto-generated method stub
		Surface sur = holder.getSurface();
		Log.d(TAG, "surfaceChanged " + sur.toString());	

        GL2JNILib.init(width, height, sur);	
	}

	@Override
	public void surfaceCreated(SurfaceHolder holder) {
		// TODO Auto-generated method stub
		Log.d(TAG, this + " surfaceCreated()");
	}

	@Override
	public void surfaceDestroyed(SurfaceHolder holder) {
		// TODO Auto-generated method stub
		Log.d(TAG, "surfaceDestroyed");
		stopVsync(this, this);
	}
	
	public static native void nativeVsync(long lastVsyncNano);

	public static Choreographer choreographerInstance;

	public static void startVsync( Activity act, final Choreographer.FrameCallback callback ) {
    	act.runOnUiThread( new Thread()
    	{
		 @Override
    		public void run()
    		{
				// Look this up now, so the callback (which will be on the same thread)
				// doesn't have to.
				choreographerInstance = Choreographer.getInstance();

				// Make sure we never get multiple callbacks going.
				choreographerInstance.removeFrameCallback(callback);

				// Start up our vsync callbacks.
				choreographerInstance.postFrameCallback(callback);
    		}
    	});
		
	}

	// It is important to stop the callbacks when the app is paused,
	// because they consume several percent of a cpu core!
	public static void stopVsync( Activity act, final Choreographer.FrameCallback callback ) {
		// This may not need to be run on the UI thread, but it doesn't hurt.
    	act.runOnUiThread( new Thread()
    	{
		 @Override
    		public void run()
    		{
				choreographerInstance.removeFrameCallback(callback);
    		}
    	});		
	}

	int frame = 0;
	public void doFrame(long frameTimeNanos) {
		//Log.d(TAG, "frame " + frame++);
		//nativeVsync(frameTimeNanos);
		
        GL2JNILib.step();
		choreographerInstance.postFrameCallback(this);
	}
	
    
}

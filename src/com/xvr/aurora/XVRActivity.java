
 
package com.xvr.aurora;

import android.app.Activity;
import android.view.MotionEvent;
import android.view.KeyEvent;
import android.os.Bundle;
import android.util.Log;


public class XVRActivity extends Activity {	

    static {
        System.loadLibrary("aurora-native");
    }

    GL2JNIView mView;
    public static String TAG = "aurora_MainActivity";

    @Override protected void onCreate(Bundle icicle) {
        super.onCreate(icicle);
        mView = new GL2JNIView(getApplication());
        setContentView(mView);
	
        nativeOnCreate();
        GL2JNILib.InitUsbDevice(this);
    }

    @Override protected void onResume() {
        super.onResume();
        mView.onResume();
        
        nativeOnResume();
    }
    
    @Override protected void onPause() {
        super.onPause();
        mView.onPause();
        
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
    
}

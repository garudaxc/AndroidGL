
 
package com.xvr.aurora;

import android.app.Activity;
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

    protected static native void nativeOnCreate();
    protected static native void nativeOnResume();
    protected static native void nativeOnPause();
    protected static native void nativeOnDestroy();


}

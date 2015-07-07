package com.xvr.aurora;

import java.io.File;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.util.HashMap;
import java.util.Iterator;

import android.util.Log;
import android.content.Context;
import android.hardware.usb.*;

// Wrapper for native library

public class GL2JNILib {

	static {
		System.loadLibrary("aurora-native");
	}

	static UsbDeviceConnection connection = null;
	UsbInterface intf = null;
	UsbEndpoint endpoint = null;

	public static boolean InitUsbDevice(Context context) {
		UsbManager manager = (UsbManager) context
				.getSystemService(Context.USB_SERVICE);
		HashMap<String, UsbDevice> deviceList = manager.getDeviceList();

		Iterator<UsbDevice> deviceIterator = deviceList.values().iterator();

		while (deviceIterator.hasNext()) {
			UsbDevice device = deviceIterator.next();

			if (device.getVendorId() == 10291 && device.getProductId() == 1) {

				// Log.i(XVRActivity.Tag, "found usb device " +
				// device.getDeviceName());
				connection = manager.openDevice(device);
				String path = properDeviceName(device.getDeviceName());

				int fd = connection.getFileDescriptor();
				Testlibusb(fd, path);
				
				return true;
			}
		}

		return false;
	}

	private final static String DEFAULT_USPFS_PATH = "/dev/bus/usb";

	private final static String properDeviceName(String deviceName) {
		if (deviceName == null)
			return DEFAULT_USPFS_PATH;
		deviceName = deviceName.trim();
		if (deviceName.isEmpty())
			return DEFAULT_USPFS_PATH;

		final String[] paths = deviceName.split("/");
		final StringBuilder sb = new StringBuilder();
		for (int i = 0; i < paths.length - 2; i++)
			if (i == 0)
				sb.append(paths[i]);
			else
				sb.append("/" + paths[i]);
		final String stripped_name = sb.toString().trim();
		if (stripped_name.isEmpty())
			return DEFAULT_USPFS_PATH;
		else
			return stripped_name;
	}

	private static void chmodRecursive(final String dir,
			final OutputStreamWriter osw) throws IOException {
		osw.write("chmod 777 " + dir + "\n");
		osw.flush();

		final String[] files = new File(dir).list();
		if (files == null)
			return;
		for (final String s : files) {
			final String fname = dir + s;
			final File f = new File(fname);

			if (f.isDirectory())
				chmodRecursive(fname + "/", osw);
			else {
				osw.write("chmod 777 " + fname + "\n");
				osw.flush();
			}
		}
	}

	public static void fixRootPermissions() throws Exception {
		Runtime runtime = Runtime.getRuntime();
		OutputStreamWriter osw = null;
		java.lang.Process proc = null;
		try { // Run Script

			proc = runtime.exec("su");

			osw = new OutputStreamWriter(proc.getOutputStream());
			chmodRecursive("/dev/bus/usb/", osw);
			osw.close();

		} catch (IOException ex) {
			throw new Exception();
		} finally {
			if (osw != null) {
				try {
					osw.close();
				} catch (IOException e) {
					e.printStackTrace();
				}
			}
		}
		try {
			if (proc != null)
				proc.waitFor();
		} catch (InterruptedException e) {
		}

		if (proc.exitValue() != 0) {
			// Log.appendLine("Root refused to give permissions.");
			throw new Exception();
		}
	}

	protected static native void Testlibusb(int fd, String usbPath);

	/**
	 * @param width
	 *            the current view width
	 * @param height
	 *            the current view height
	 */
	public static native void init(int width, int height);

	public static native void step();

}

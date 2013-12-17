package com.genymotion.clipboardproxy;

import java.io.IOException;
import java.net.InetAddress;
import java.net.Socket;
import java.net.UnknownHostException;

import android.app.Service;
import android.content.ClipData;
import android.content.Intent;
import android.os.IBinder;
import android.util.Log;
import android.widget.Toast;
import android.content.ClipboardManager;

public class ServiceClipboardProxy extends Service implements
		ClipboardManager.OnPrimaryClipChangedListener {

	private static final int SERVERPORT = 22666;
	private static final String SERVER_IP = "127.0.0.1";
	private Socket socket;

	@Override
	public void onCreate() {
		super.onCreate();

		ClipboardManager clipboardManager = (ClipboardManager) getSystemService(CLIPBOARD_SERVICE);
		clipboardManager.addPrimaryClipChangedListener(this);
	}

	@Override
	public IBinder onBind(Intent intent) {
		// We do not provide a communication channel
		return null;
	}

	@Override
	public int onStartCommand(Intent intent, int flags, int startId) {
		super.onStartCommand(intent, flags, startId);

		Log.d("clipboardproxy", "ServiceClipboardProxy onStartCommand");

		// Tells the system that, should the service be terminated for memory
		// or performance reasons, I’d like to have it started back up.
		return Service.START_STICKY;
	}

	@Override
	public void onDestroy() {
		super.onDestroy();
		
		try {
			socket.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
		
		Toast.makeText(this, R.string.clipboard_proxy_stopped,
				Toast.LENGTH_SHORT).show();
	}

	@Override
	public void onPrimaryClipChanged() {
		ClipboardManager clipboardManager = (ClipboardManager) getSystemService(CLIPBOARD_SERVICE);

		if (clipboardManager.hasPrimaryClip()) {
			ClipData data = clipboardManager.getPrimaryClip();

			String myLabel = getResources().getString(
					R.string.clip_description_label);

			CharSequence label = data.getDescription().getLabel();
			if (label == null
					|| (label != null && !label.toString().equals(myLabel))) {

				Log.d("clipboardproxy",
						"ServiceClipboardProxy onPrimaryClipChanged");

				ClipData.Item item = data.getItemAt(0);

				CharSequence text = item.coerceToText(this);
				StringBuffer reverse = new StringBuffer(text.length());
				for (int i = 0; i < text.length(); i++) {
					reverse.append(text.charAt(text.length() - i - 1));
				}

				clipboardManager.setPrimaryClip(ClipData.newPlainText(myLabel,
						reverse));

				new Thread(new ConnectionThread()).start();
			}
		}

	}
	
	class ConnectionThread implements Runnable {

		@Override
		public void run() {
			try {
				InetAddress serverAddr = InetAddress.getByName(SERVER_IP);
				socket = new Socket(serverAddr, SERVERPORT);
				socket.setTcpNoDelay(true);

			} catch (UnknownHostException e) {
				e.printStackTrace();
			} catch (IOException e) {
				e.printStackTrace();
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
		
	}

	class CloseThread implements Runnable {

		@Override
		public void run() {
			try {
				InetAddress serverAddr = InetAddress.getByName(SERVER_IP);
				Socket socket = new Socket(serverAddr, SERVERPORT);
				socket.setTcpNoDelay(true);

			} catch (UnknownHostException e) {
				e.printStackTrace();
			} catch (IOException e) {
				e.printStackTrace();
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
		
	}
}

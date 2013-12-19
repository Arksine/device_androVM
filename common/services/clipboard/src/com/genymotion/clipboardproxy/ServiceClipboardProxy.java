package com.genymotion.clipboardproxy;

import java.io.IOException;
import java.net.InetAddress;
import java.net.Socket;
import java.net.UnknownHostException;

import android.annotation.SuppressLint;
import android.app.Service;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Handler;
import android.os.IBinder;
import android.util.Log;
import android.widget.Toast;
import android.text.ClipboardManager;

public class ServiceClipboardProxy extends Service {

	private static final int SERVERPORT = 22666;
	private static final String SERVER_IP = "127.0.0.1";
	private Socket socket;
	private ClipboardManager clipboardManager;
	private String clipboardText;
	private BroadcastReceiver receiver;
	private IntentFilter filter;
	private Boolean stopRecursion;

	@Override
	public void onCreate() {
		super.onCreate();

		clipboardManager = (ClipboardManager) getSystemService(CLIPBOARD_SERVICE);
		filter = new IntentFilter("com.genymotion.clipboardproxy.CLIP_CHANGED");
		receiver = new myBroadcastReceiver();
	}

	@Override
	public IBinder onBind(Intent intent) {
		// We do not provide a communication channel
		return null;
	}

	@Override
	public int onStartCommand(Intent intent, int flags, int startId) {
		super.onStartCommand(intent, flags, startId);

		Log.d("Genyd", "ServiceClipboardProxy onStartCommand");

		stopRecursion = false;
		registerReceiver(receiver, filter);

		new Thread(new ConnectionThread()).start();

		// Tells the system that, should the service be terminated for memory
		// or performance reasons, I’d like to have it started back up.
		return Service.START_STICKY;
	}

	@Override
	public void onDestroy() {
		super.onDestroy();

		unregisterReceiver(receiver);

		try {
			if (socket != null && socket.isConnected()) {
				socket.close();
			}
		} catch (IOException e) {
			e.printStackTrace();
		}

		Toast.makeText(this, R.string.clipboard_proxy_stopped,
				Toast.LENGTH_SHORT).show();
	}

	class myBroadcastReceiver extends BroadcastReceiver {
		@Override
		public void onReceive(Context context, Intent intent) {
			if (intent.getAction().equals(
					"com.genymotion.clipboardproxy.CLIP_CHANGED")) {
				synchronized (stopRecursion) {
					if (!stopRecursion) {
						if (clipboardManager.hasText()) {
							Log.d("Genyd", "ServiceClipboardProxy onPrimaryClipChanged");
							clipboardText = clipboardManager.getText().toString();
							new Thread(new WriteThread()).start();
						}
					} else {
						stopRecursion = false;
					}
				}
			}
		}
	}

	protected synchronized boolean connect() {
		if (socket == null || !socket.isConnected()) {
			try {
				InetAddress serverAddr = InetAddress.getByName(SERVER_IP);
				socket = new Socket(serverAddr, SERVERPORT);
				socket.setTcpNoDelay(true);
				socket.setKeepAlive(true);
			} catch (UnknownHostException e) {
				e.printStackTrace();
			} catch (IOException e) {
				e.printStackTrace();
			}
		}

		if (socket == null) {
			return false;
		}

		return socket.isConnected();
	}

	class ConnectionThread implements Runnable {
		@Override
		public void run() {
			if (connect()) {
				new Thread(new ReadThread()).start();
			}
		}
	}

	class WriteThread implements Runnable {
		@Override
		public void run() {
			if (connect()) {
				try {
					socket.getOutputStream().write(clipboardText.getBytes());
					socket.getOutputStream().flush();
					Log.d("Genyd", "WriteThread " + clipboardText);
				} catch (IOException e) {
					e.printStackTrace();
				}
			}
		}

	}

	class ReadThread implements Runnable {
		@Override
		public void run() {
			byte[] buffer = new byte[1024];
			Handler mainHandler = new Handler(getMainLooper());

			while (true) {
				if (connect()) {
					try {
						int len = socket.getInputStream().read(buffer);
						if (len > 0) {
							clipboardText = new String(buffer, 0, len);
							Log.d("Genyd", "ReadThread " + clipboardText);
							synchronized(stopRecursion) {
								stopRecursion = true;
								clipboardManager.setText(clipboardText);
							}
						}
					} catch (IOException e) {
						e.printStackTrace();
					}
				}
			}
		}
	}
}

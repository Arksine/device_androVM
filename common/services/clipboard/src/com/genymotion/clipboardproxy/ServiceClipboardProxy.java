package com.genymotion.clipboardproxy;

import java.io.IOException;
import java.net.InetAddress;
import java.net.Socket;
import java.net.UnknownHostException;
import java.lang.StringBuilder;

import android.app.Service;
import android.content.ClipData;
import android.content.Intent;
import android.os.IBinder;
import android.util.Log;
import android.content.ClipboardManager;

public class ServiceClipboardProxy extends Service implements
		ClipboardManager.OnPrimaryClipChangedListener {

        private static final int BUFFER_SIZE = 32768;
	private static final int SERVERPORT = 22666;
	private static final String SERVER_IP = "127.0.0.1";
	private String myLabel;
	private Socket socket;
	private ClipboardManager clipboardManager;
	private String clipboardText;

	@Override
	public void onCreate() {
		super.onCreate();

		clipboardManager = (ClipboardManager) getSystemService(CLIPBOARD_SERVICE);
		clipboardManager.addPrimaryClipChangedListener(this);

		myLabel = getResources().getString(R.string.clip_description_label);
	}

	@Override
	public IBinder onBind(Intent intent) {
		// We do not provide a communication channel
		return null;
	}

	@Override
	public int onStartCommand(Intent intent, int flags, int startId) {
		super.onStartCommand(intent, flags, startId);

		Log.d("ServiceClipboardProxy", "onStartCommand");

		new Thread(new ConnectionThread()).start();

		// Tells the system that, should the service be terminated for memory
		// or performance reasons, I’d like to have it started back up.
		return Service.START_STICKY;
	}

	@Override
	public void onDestroy() {
		super.onDestroy();

		try {
			if (socket != null && socket.isConnected()) {
				socket.close();
			}
		} catch (IOException e) {

		}

		Log.d("ServiceClipboardProxy", "Destroyed");
	}

	@Override
	public void onPrimaryClipChanged() {

		if (clipboardManager.hasPrimaryClip()) {
			ClipData data = clipboardManager.getPrimaryClip();

			CharSequence label = data.getDescription().getLabel();
			if (label == null
					|| (label != null && !label.toString().equals(myLabel))) {

				Log.d("ServiceClipboardProxy", "onPrimaryClipChanged");

				ClipData.Item item = data.getItemAt(0);
				clipboardText = item.coerceToText(this).toString();
				new Thread(new WriteThread()).start();
			}
		}

	}

	protected synchronized boolean connect() {
		if (socket == null || !socket.isConnected()) {
			Log.d("ServiceClipboardProxy", "Connection...");
			try {
				InetAddress serverAddr = InetAddress.getByName(SERVER_IP);
				socket = new Socket(serverAddr, SERVERPORT);
				socket.setTcpNoDelay(true);
				socket.setKeepAlive(true);
			} catch (UnknownHostException e) {
				Log.d("ServiceClipboardProxy", "Connection UnknownHostException " + e.getMessage());
			} catch (IOException e) {
				Log.d("ServiceClipboardProxy", "Connection IOException " + e.getMessage());
			}
		}

		if (socket == null) {
			return false;
		}

		Log.d("ServiceClipboardProxy", "Connected: " + socket.isConnected());

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
					Log.d("ServiceClipboardProxy", "Write done (" + String.valueOf(clipboardText.length()) + " bytes)");
				} catch (IOException e) {
					Log.d("ServiceClipboardProxy", "write IOException " + e.getMessage());
				}
			}
		}

	}

	class ReadThread implements Runnable {
		@Override
		public void run() {
			byte[] buffer = new byte[BUFFER_SIZE];

			while (true) {
				if (connect()) {
					try {
						int len = socket.getInputStream().read(buffer);
						if (len > 0) {
                                                        StringBuilder clipboardData = new StringBuilder(new String(buffer, 0, len));

							// Handle big clipboard
                                                        while (len == BUFFER_SIZE) {
								len = socket.getInputStream().read(buffer);
								clipboardData.append(new String(buffer, 0, len));
                                                        }

							clipboardText = clipboardData.toString();

							Log.d("ServiceClipboardProxy", "Read " + String.valueOf(clipboardText.length()) + " bytes");
							clipboardManager.setPrimaryClip(ClipData
									.newPlainText(myLabel, clipboardText));
						}

					} catch (IOException e) {
						Log.d("Genyd", "read IOException " + e.getMessage());
					}
				}
			}
		}
	}
}
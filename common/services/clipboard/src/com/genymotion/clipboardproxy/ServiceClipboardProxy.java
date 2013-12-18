package com.genymotion.clipboardproxy;

import java.io.IOException;
import java.net.InetAddress;
import java.net.Socket;
import java.net.UnknownHostException;

import android.app.Service;
import android.content.ClipData;
import android.content.Intent;
import android.os.Handler;
import android.os.IBinder;
import android.util.Log;
import android.widget.Toast;
import android.content.ClipboardManager;

public class ServiceClipboardProxy extends Service implements
		ClipboardManager.OnPrimaryClipChangedListener {

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

		Log.d("Genyd", "ServiceClipboardProxy onStartCommand");

		new Thread(new ConnectionThread()).start();

		// Tells the system that, should the service be terminated for memory
		// or performance reasons, I’d like to have it started back up.
		return Service.START_STICKY;
	}

	@Override
	public void onDestroy() {
		super.onDestroy();

		try {
			if (socket != null) {
				socket.close();
			}
		} catch (IOException e) {
			e.printStackTrace();
		}

		Toast.makeText(this, R.string.clipboard_proxy_stopped,
				Toast.LENGTH_SHORT).show();
	}

	@Override
	public void onPrimaryClipChanged() {

		if (clipboardManager.hasPrimaryClip()) {
			ClipData data = clipboardManager.getPrimaryClip();

			CharSequence label = data.getDescription().getLabel();
			if (label == null
					|| (label != null && !label.toString().equals(myLabel))) {

				Log.d("Genyd","ServiceClipboardProxy onPrimaryClipChanged");

				ClipData.Item item = data.getItemAt(0);
				clipboardText = item.coerceToText(this).toString();
				new Thread(new WriteThread()).start();
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
				new Thread(new ReadThread()).start();
			} catch (UnknownHostException e) {
				e.printStackTrace();
			} catch (IOException e) {
				e.printStackTrace();
			} catch (Exception e) {
				e.printStackTrace();
			}
		}

	}

	class WriteThread implements Runnable {
		@Override
		public void run() {
			try {
				socket.getOutputStream().write(clipboardText.getBytes());
				socket.getOutputStream().flush();
				Log.d("Genyd", "WriteThread " + clipboardText);
			} catch (IOException e) {
				e.printStackTrace();
			} catch (Exception e) {
				e.printStackTrace();
			}
		}

	}

	class ReadThread implements Runnable {
		@Override
		public void run() {
			byte[] buffer = new byte[1024];
			Handler mainHandler = new Handler(getMainLooper());

			while (true) {
				try {
					int len = socket.getInputStream().read(buffer);
					if(len != 0) {
						clipboardText = new String(buffer, 0, len);				
						Log.d("Genyd", "ReadThread " + clipboardText);
						mainHandler.post(new Runnable() {
							@Override
							public void run() {
								clipboardManager.setPrimaryClip(ClipData
										.newPlainText(myLabel, clipboardText));
							}
						});
					}

				} catch (IOException e) {
					e.printStackTrace();
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
		}
	}
}

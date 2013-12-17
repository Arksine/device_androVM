package com.genymotion.clipboardproxy;

import android.os.Bundle;
import android.app.Activity;
import android.content.Intent;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;


// Simple activity to start/stop ClipboardProxy service
public class ActivityStartClipboardProxy extends Activity implements OnClickListener {

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_start_clipboard_proxy);
		
		// Start button
		Button btn = (Button) findViewById(R.id.start_service);
		btn.setOnClickListener(this);

		// Stop button
		btn = (Button) findViewById(R.id.stop_service);
		btn.setOnClickListener(this);
	}

	@Override
	public void onClick(View v) {
		// Intent to start/stop the ClipboardProxy service
		Intent clipboardProxyIntent = 
				new Intent(this, ServiceClipboardProxy.class);
		
		if (v.getId() == R.id.start_service) {
			startService(clipboardProxyIntent);
		} else if (v.getId() == R.id.stop_service) {
			stopService(clipboardProxyIntent);
		}
	}

}

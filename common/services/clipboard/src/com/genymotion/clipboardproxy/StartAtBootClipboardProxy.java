package com.genymotion.clipboardproxy;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

public class StartAtBootClipboardProxy extends BroadcastReceiver {

	@Override
	public void onReceive(Context context, Intent intent) {
		if (intent.getAction().equals(Intent.ACTION_BOOT_COMPLETED)) {
			Intent serviceIntent = new Intent(context, ServiceClipboardProxy.class);
			
			Log.d("clipboardproxy", "StartAtBootClipBoardProxy");
			
			context.startService(serviceIntent);
		}
	}
}

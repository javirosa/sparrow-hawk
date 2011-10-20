package net.sh.android.ibutton;

import java.util.List;

import android.app.Activity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

public class Android_ibutton_appActivity extends Activity {
	
	TextView _txtInfo;
	Button _btnSerialOpen;
	Button _btnSerialClose;
	Button _btnGetIBtnAddr;
	
	OneWireAdapterService _oneWireService;
	
    /** Called when the activity is first created. */
    @Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.main);

		_oneWireService = new OneWireAdapterService("DS9097U",
				"/dev/s3c_serial1");

		_txtInfo = (TextView) findViewById(R.id.txtInfo);
		_btnSerialOpen = (Button) findViewById(R.id.btnSerialOpen);
		_btnSerialClose = (Button) findViewById(R.id.btnSerialClose);
		_btnGetIBtnAddr = (Button) findViewById(R.id.btnGetAddress);
		
		_btnSerialOpen.setEnabled(true);
		_btnSerialClose.setEnabled(false);
		_btnGetIBtnAddr.setEnabled(false);

		
		_btnSerialOpen.setOnClickListener(new View.OnClickListener() {
			public void onClick(View v) {

				_btnSerialOpen.setEnabled(false);
				
				_oneWireService.start();
				
				if(_oneWireService.isStarted()){
					_btnSerialClose.setEnabled(true);
					_btnGetIBtnAddr.setEnabled(true);
				}else{
					_btnSerialOpen.setEnabled(true);
				}
			}
		});
		
		_btnSerialClose.setOnClickListener(new View.OnClickListener() {
			public void onClick(View v) {

				_btnSerialClose.setEnabled(false);
				_btnGetIBtnAddr.setEnabled(false);
				
				_oneWireService.stop();
				
				_btnSerialOpen.setEnabled(true);
			}
		});
		
		_btnGetIBtnAddr.setOnClickListener(new View.OnClickListener() {
			public void onClick(View v) {
				List<String> addrs = _oneWireService.searchAllIButtons();
				if(addrs.isEmpty()){
					_txtInfo.setText("No iButton found!");
				}else{
					StringBuffer sb = new StringBuffer();
					int count = addrs.size();
					sb.append("Fount " + count + "iButtons: \r\n");
					for(int i = 0; i < count; i++){
						sb.append(addrs.get(i) + "\r\n");
					}
					_txtInfo.setText(sb.toString());
				}
			}
		});
	}
}
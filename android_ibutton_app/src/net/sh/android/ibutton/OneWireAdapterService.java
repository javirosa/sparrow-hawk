package net.sh.android.ibutton;

import java.util.ArrayList;
import java.util.Enumeration;
import java.util.List;

import com.dalsemi.onewire.OneWireAccessProvider;
import com.dalsemi.onewire.OneWireException;
import com.dalsemi.onewire.adapter.DSPortAdapter;
import com.dalsemi.onewire.container.OneWireContainer;

import android.util.Log;

public class OneWireAdapterService {
	

	private static final String LOG_TAG = "OneWireAdapterService";
	
	private String adapterName = "DS9097U";
	private String portName = "/dev/ttyS0";
	
	private DSPortAdapter adapter;
	
	private boolean isStarted;
	
	public OneWireAdapterService(String adapterName, String portName){
		
		this.adapterName = adapterName;
		this.portName = portName;
		

		Trace("");
		Trace("Adapter: " + this.adapterName + 
				" Port: " + this.portName);
		Trace("");
		
	}
	
	public boolean isStarted(){
		return isStarted;
	}
	
	
	public void start(){
		
		if(isStarted) return;
		
		if(adapter == null){

			try {
				adapter = OneWireAccessProvider.getAdapter(adapterName, portName);
	
				
			} catch (Exception e) {
				Trace("Couldn't get 1-Wire adapter!");
				Trace(e.toString());
			}
		}
		
		if (adapter == null){
			Trace("No 1-Wire adapter on the system!");
		}else{
			isStarted = true;
		}
	}
	
	public void stop(){
		
		if(isStarted) {

			try {
				adapter.freePort();
			} catch (OneWireException e) {
				Trace(e.toString());
			}
			
			adapter = null;
			
			isStarted = false;
		}
		
	}
	
	public List<String> searchAllIButtons() {

		List<String> ret = new ArrayList<String>();
		
		if (adapter == null){
			Trace("No 1-Wire adapter on the system!");
			return ret;
		}
		
		OneWireContainer owd;

		try {

			// get exclusive use of adapter
			adapter.beginExclusive(true);

			// clear any previous search restrictions
			adapter.setSearchAllDevices();
			adapter.targetAllFamilies();
			adapter.setSpeed(adapter.SPEED_REGULAR);

			// enumerate through all the 1-Wire devices found
			for (Enumeration owd_enum = adapter.getAllDeviceContainers(); owd_enum
					.hasMoreElements();) {
				owd = (OneWireContainer) owd_enum.nextElement();

				String addr = owd.getAddressAsString();
				ret.add(addr);
				Trace("Found iButton: " + addr);
			}

			// end exclusive use of adapter
			adapter.endExclusive();
			
		} catch (Exception e) {
			Trace(e.toString());
		}
		
		return ret;
	}
	
	
	@Override
	protected void finalize() throws Throwable {
		// free port used by adapter
        adapter.freePort();
        
		super.finalize();
	}



	private void Trace(String msg){
		Log.d(LOG_TAG, msg);
		//System.out.println(msg);
	}
	
	

}

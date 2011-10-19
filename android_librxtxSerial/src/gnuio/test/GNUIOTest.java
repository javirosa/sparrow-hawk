package gnuio.test;

import gnu.io.CommPortIdentifier;
import gnu.io.PortInUseException;
import gnu.io.RXTXPort;
import gnu.io.SerialPort;
import gnu.io.SerialPortEvent;
import gnu.io.SerialPortEventListener;
import gnu.io.UnsupportedCommOperationException;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.UnsupportedEncodingException;
import java.util.Enumeration;
import java.util.HashMap;
import java.util.Map;
import java.util.Random;
import java.util.TooManyListenersException;

import android.app.Activity;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.EditText;
import android.widget.LinearLayout.LayoutParams;
import android.widget.TextView;
import android.widget.Toast;

import android.util.Log;

public class GNUIOTest extends Activity {
	

	private static final String LOG_TAG = "GNUIOTest";
	
	private static final String SERIAL_PORT = "/dev/s3c_serial1";

    private CommPortIdentifier portId;
	private SerialPort serialPort;
	private InputStream in;
	private OutputStream out;	
	private Map<String, View> indicatorMap;
	private Handler mHandler;
	//private Random rnd;

	private EditText txtSendData;
	private Button btnSerialSend;
	private Button btnSerialOpen;
	private Button btnSerialClose;

	private TextView txtIndication; 
	private Button btnClear;
	
	private boolean keepReading;
	private Thread readingThread;
	
	private CommPortIdentifier findPortId() {
		// Find serial ports...
		CommPortIdentifier portId = null; // will be set if port found
		Enumeration portIdentifiers = CommPortIdentifier.getPortIdentifiers();

		// See what ports are available. and latch on desired port
		toast("Requesting Ports");
		while (portIdentifiers.hasMoreElements()) {
			CommPortIdentifier pid = (CommPortIdentifier) portIdentifiers
					.nextElement();

			toast("Got : " + pid.getName());
			if (pid.getPortType() == CommPortIdentifier.PORT_SERIAL
					&& pid.getName().equals(SERIAL_PORT)) {
				portId = pid;
				break;
			}
		}
		return portId;
	}
	
	private boolean OpenSerialPort(){
		try {
        	
			 serialPort = (SerialPort) portId.open("GNU IO Test", 2000 );
			
			 in  = serialPort.getInputStream();
			 out = serialPort.getOutputStream();
			 
			 serialPort.notifyOnCarrierDetect(true);//CD
			 serialPort.notifyOnCTS(true);
			 serialPort.notifyOnDSR(true);
			 serialPort.notifyOnRingIndicator(true);//RI
			 serialPort.notifyOnBreakInterrupt(true);//BI
			 serialPort.notifyOnDataAvailable(true);
			 serialPort.notifyOnOutputEmpty(false);
			 
			 serialPort.addEventListener(new SerialEventsListener());
			 
			 
			 serialPort.setSerialPortParams(9600, 
					 					    SerialPort.DATABITS_8, 
					 					    SerialPort.STOPBITS_1, 
					 					    SerialPort.PARITY_NONE);
			 
			 //serialPort.setFlowControlMode( SerialPort.FLOWCONTROL_RTSCTS_IN  | SerialPort.FLOWCONTROL_RTSCTS_OUT );
			 serialPort.setFlowControlMode( SerialPort.FLOWCONTROL_NONE );
			 serialPort.setDTR(true);
			 serialPort.setRTS(true);
			 
			 return true;
		
       } catch (IOException e) {
	       	String msg = "I/O Exception " + e.getMessage();
				toast(msg);
		}
		catch (PortInUseException e) {
			toast ( "Port in use by " + e.currentOwner );
	 	    
		} catch (UnsupportedCommOperationException e) {
			 toast ("Unsupported Operation " + e.getMessage());
       				
		} catch (TooManyListenersException e) {
			 toast ("Too many listeners");
		}
		
		return false;
	}
	
	private void CloseSerialPort() {
		if (serialPort != null) {
			serialPort.removeEventListener();
			serialPort.close();

			serialPort = null;
			in = null;
			out = null;
		}
	}
	
	private void StartReadingThread(){
		keepReading = true;
		readingThread = new SerialReadingThread ();
		readingThread.start();
	}
	
	private void StopReadingThread() {
		keepReading = false;
		try {
			Thread.sleep(2000);
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	
	private class SerialReadingThread extends Thread{
		
		public void run() {
			
			while(keepReading) {
				int numBytes;
				int numBytesTotal = 0;
				String sReadBuff = "";
				
				try {
				
					// Read the data until no more available
					byte[] readBuffer = new byte[20];
				
					numBytes = in.read(readBuffer);
				
					numBytesTotal += numBytes;
					//String tmpR = new String(readBuffer);
					//sReadBuff += tmpR.substring(0, numBytes); 
					String tmpR = ConvertCodec.bytesToHexString(readBuffer, 0, numBytes);
					sReadBuff += tmpR;
				} 
				catch (IOException e) {
					Bundle dBundle = new Bundle();
					Message msg = Message.obtain();
					dBundle.putString("ERR", e.getMessage() );
					msg.setData(dBundle);
					mHandler.sendMessage(msg);
				}
				
				/// If any data was read ship it to the UI
				if ( sReadBuff.length() > 0 )
				{
					Bundle dBundle = new Bundle();
					Message msg = Message.obtain();
					dBundle.putString("DATA", sReadBuff );
					msg.setData(dBundle);
					mHandler.sendMessage(msg);
				}
				
				/*
				try {
					Thread.sleep(1000);
				} catch (InterruptedException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
				*/
			}
		
		}
	}
	

	/** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        
    	
		super.onCreate(savedInstanceState);
		setContentView(R.layout.main);

		txtSendData = (EditText) findViewById(R.id.txtSendData);
		btnSerialSend = (Button)findViewById(R.id.btnSerialSend);
		btnSerialOpen = (Button)findViewById(R.id.btnSerialOpen);
		btnSerialClose = (Button)findViewById(R.id.btnSerialClose);

		txtIndication = (TextView) findViewById(R.id.lblIndication);
		btnClear = (Button)findViewById(R.id.btnClear);
		
		btnSerialSend.setEnabled(false);
		btnSerialOpen.setEnabled(false);
		btnSerialClose.setEnabled(false);
		txtSendData.setText("0001020304");
        
		// Create Status Indicators
		final String[] Signals = new String[] { "CD", "CTS", "DSR", "RI" };
		indicatorMap = createIndicators(Signals, (ViewGroup) findViewById(R.id.IndContainer));
		//rnd = new java.util.Random();
        
		// / Find serial ports...
		portId = this.findPortId();

		// / Bail out if we can't get the port..
		if (portId == null) {
			toast("Can't find Serial Port ");
			return;
		}

		// Event Notification listener...
		toast("Good, find the Serial Port!");
		
	   
	   /// Handler object that receives events from read and
	   /// notification threads..
       mHandler = new Handler() {
    	   public void handleMessage(Message msg) 
    	   {
    		   Bundle d = msg.getData();
    		   
    		   /// Check if it is a status IND.  If it is .
    		   /// change the state of the inidicator
    		   for (String s : Signals) {
    			   if ( d.containsKey(s))
    			   {
    				   setIndicator(s, d.getBoolean(s));
    			   }    
    			}
    		   
    		   /// Notify user of the error message
    		   if ( d.containsKey("ERR"))
    		   {
    			   toast ( "Error: " + d.getString("ERR"));
    		   }
    		   
    		   /// Append data to the data area..
    		   if ( d.containsKey("DATA"))
    		   {   
    			   txtIndication.append(d.getString("DATA"));
    		   }
    		   
    		   /// Notify user that sending is complete...
    		   if ( d.containsKey("OUT"))
    		   {
    			   toast ( "OUT " + d.getString("OUT"));
    		   }
    		   
    		   if ( d.containsKey("BI"))
    		   {
    			   toast ( "BI: " + d.getString("BI"));
    		   }
    	   };
       };
       
      
       
		btnSerialOpen.setOnClickListener(new Button.OnClickListener() {
			public void onClick(View v) {
				
				btnSerialOpen.setEnabled(false);

				if (OpenSerialPort()) {
					setIndicator("CD", serialPort.isCD());
					setIndicator("RI", serialPort.isRI());
					setIndicator("CTS", serialPort.isCTS());
					setIndicator("DSR", serialPort.isDSR());

					StartReadingThread();
				}

				btnSerialClose.setEnabled(true);
				btnSerialSend.setEnabled(true);
			}
		});
		
		btnSerialClose.setOnClickListener(new Button.OnClickListener() {
			public void onClick(View v) {

				btnSerialClose.setEnabled(false);
				
				StopReadingThread();
				
				CloseSerialPort();
				
				btnSerialOpen.setEnabled(true);
			}
		});
		
		
		
		// Attach Send Button Handler
		//findViewById(R.id.btnSerialSend).setOnClickListener(new Button.OnClickListener() {
		btnSerialSend.setOnClickListener(new Button.OnClickListener() {
    		public void onClick(View v) {
    			
    			//sendInputData();
    			oneWirePresent();
    			
    		}
    	});
    
		
		btnClear.setOnClickListener(new Button.OnClickListener() {
			public void onClick(View v) {

				txtIndication.setText("RECV: ");
			}
		});
		
		
		btnSerialOpen.setEnabled(true);
    }

    
    
    private void oneWirePresent(){
    	
    	
    	try {

    		long start, pause1, pause2, stop;
    		
    		start = System.currentTimeMillis();
    		
    		
    		int baudrate = this.serialPort.getBaudRate();
    		toast("Get Baudrate: " + baudrate);
        	
    		((RXTXPort)this.serialPort).changeOspeed(baudrate/ 2);
			toast("Set Baudrate: " + baudrate / 2 + " OK ");
    		
			out.write(new byte[]{(byte) 0x00});
			out.flush();
			Thread.sleep(2);
    		

    		((RXTXPort)this.serialPort).changeOspeed(baudrate);
			toast("Set Baudrate: " + baudrate + " OK ");
    		
    		//serialPort.setBaudBase(9600);
        	//serialPort.sendBreak(2);

        	pause1 = System.currentTimeMillis();
        	
			Thread.sleep(2);
			out.flush();
			
			out.write(new byte[]{(byte) 0xC1});
			Thread.sleep(2);
			out.flush();
			
        	pause2 = System.currentTimeMillis();
			
			//out.write(new byte[] {0x17, 0x45, 0x59, 0x3F, (byte) 0x0F, (byte) 0x95});
			out.write(new byte[] {0x17, 0x45, 0x5B, (byte) 0x0F, (byte) 0x91});
			out.flush();
			
	    	stop = System.currentTimeMillis();
	    	
	    	toast("Consume: " + 
	    			(pause1 - start) + ", " + 
	    			(pause2 - pause1) + ", " + 
	    			(stop - pause2) + ", " + 
	    			(stop - start) + "ms");
			
		} 
    	catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} 
    	catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} 
//    	catch (UnsupportedCommOperationException e) {
//			// TODO Auto-generated catch block
//			e.printStackTrace();
//		} 
    }

	private void sendInputData(){
		// Get Data to Send...
		//txtSendData = (EditText) findViewById(R.id.txtSerialSend);
		//String txt = txtSendData.getText().toString();
		
		String txt = txtSendData.getText().toString();
		
		if(txt == null || txt.equals("")) return;
		
		try {
			
			// Send Data...
			toast("Sending " + txt );
			
			//out.write(txt.getBytes("UTF8"));
			//out.write(0x0A);
			
			byte[] bytes = ConvertCodec.hexStringToBytes(txt);
			out.write(bytes);
			out.flush();
			
			// Clear Input Field
			//txtSendData.getText().clear();
			
			// Pop the toast
			toast("Sent");
			
		} catch (IllegalArgumentException e1){
			toast("Wrong inpu, must be hex string!");
		} catch (UnsupportedEncodingException e2) {
			toast ( "Unsupported Encoding");
		} catch (IOException e3) {
			toast("Sending IO Exception");
		} catch (Exception e4) {
			toast(e4.toString());
		}
	}
    
    
    private final class SerialEventsListener implements SerialPortEventListener {
		private final HashMap<Integer, String> evtLabels;

		private SerialEventsListener() {
			evtLabels = new HashMap<Integer, String>();
			evtLabels.put ( SerialPortEvent.CTS, "CTS" );
			evtLabels.put ( SerialPortEvent.DSR, "DSR" );
			evtLabels.put ( SerialPortEvent.RI, "RI" );
			evtLabels.put ( SerialPortEvent.CD, "CD" );
		}

		@Override
		public void serialEvent(SerialPortEvent ev) {
			
			/// Create message bundle
			Bundle dBundle = new Bundle();
			Message msg = Message.obtain();
			
			/// Get Event type
			int t = ev.getEventType();
			Boolean v = ev.getNewValue();
			
			/// If it is one of the status events just forward it to the
			/// handler as is
			if ( evtLabels.containsKey(t))
			{
				dBundle.putBoolean(evtLabels.get(t), v);
			}	
			//TODO:  Not sure it does anything...
			//       data reading is handled by the separate thread
			else if ( t == SerialPortEvent.DATA_AVAILABLE  )
			{
				byte[] readBuffer = new byte[20];
				int numBytes;
				int numBytesTotal = 0;
				String sReadBuff = "";
				
				try {
					while (in.available() > 0) {
						numBytes = in.read(readBuffer);
						numBytesTotal += numBytes;
						String tmpR = new String(readBuffer);
						sReadBuff += tmpR.substring(0, numBytes); 
					}
					dBundle.putString("DATA", sReadBuff );
				} catch (IOException e) {
					dBundle.putString("ERR", "Error Reading from serial port" );
				}
				

				msg.setData(dBundle);
				mHandler.sendMessage(msg);
				
			}
			else if ( t == SerialPortEvent.OUTPUT_BUFFER_EMPTY )
			{
				dBundle.putString("OUT", "Empty" );

				//msg.setData(dBundle);
				//mHandler.sendMessage(msg);
			}
			else if ( t == SerialPortEvent.BI )
			{
				dBundle.putString("BI", "Break Interupt" );

				msg.setData(dBundle);
				mHandler.sendMessage(msg);
			}
			else
			{
				dBundle.putString("ERR", "Unhandled COMM Event" );
				

				msg.setData(dBundle);
				mHandler.sendMessage(msg);
			}
			
			
		}
	}
    
    
    
    
    // Create line of indicators
	private Map<String, View> createIndicators(final String[] Signals, ViewGroup indCnt) {
		   
		   LayoutParams lp = new LayoutParams(indCnt.getLayoutParams());
		   lp.width  = 40;
		   lp.height = 20;
		   HashMap<String, View> inds = new HashMap<String, View>();
		   
		   for (String s : Signals) {
			   TextView tv = new TextView(this,null,R.style.Ind);
		       tv.setText(s);
		       tv.setBackgroundDrawable(getResources().getDrawable(R.drawable.ind_off));
		       tv.setLayoutParams(new LayoutParams(lp));           
		       indCnt.addView(tv);
		       inds.put(s, tv);
		   }
		   
		   return inds;
	}

	
	// Set Indicator
	private void setIndicator(String key, boolean b) {
		View v = indicatorMap.get(key);
		if ( b )
			v.setBackgroundDrawable(getResources().getDrawable(R.drawable.ind_on));
		else
			v.setBackgroundDrawable(getResources().getDrawable(R.drawable.ind_off));	
		v.invalidate();
		toast( key + ( b ? "On" : "Off"));
	}

	// Show the toast notification..
	private void toast(String msg) {
		Log.d(LOG_TAG, msg);
		
		Toast t3 = Toast.makeText(this, msg, 100);
		t3.show();
	}
}

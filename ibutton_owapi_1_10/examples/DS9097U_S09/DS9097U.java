

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

import com.dalsemi.onewire.utils.ConvertCodec;


public class DS9097U {
	

	private static final String LOG_TAG = "GNUIOTest";
	
	//private static final String SERIAL_PORT = "/dev/s3c_serial1";
	private static final String SERIAL_PORT = "COM2";

    private CommPortIdentifier portId;
	private SerialPort serialPort;
	private InputStream in;
	private OutputStream out;	
	
	private boolean keepReading;
	private Thread readingThread;
	
	private CommPortIdentifier findPortId() {
		// Find serial ports...
		CommPortIdentifier portId = null; // will be set if port found
		Enumeration portIdentifiers = CommPortIdentifier.getPortIdentifiers();

		// See what ports are available. and latch on desired port
		debug("Requesting Ports");
		while (portIdentifiers.hasMoreElements()) {
			CommPortIdentifier pid = (CommPortIdentifier) portIdentifiers
					.nextElement();

			debug("Got : " + pid.getName());
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
			debug(msg);
		}
		catch (PortInUseException e) {
			debug( "Port in use by " + e.currentOwner );
	 	    
		} catch (UnsupportedCommOperationException e) {
			debug("Unsupported Operation " + e.getMessage());
       				
		} catch (TooManyListenersException e) {
			debug("Too many listeners");
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
		//readingThread.start();
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
					e.printStackTrace();
				}
				
				try {
					Thread.sleep(1000);
				} catch (InterruptedException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			}
		
		}
	}
	
    
    
    private void oneWirePresent(){
    	
    	
    	try {

    		long start, pause1, pause2, stop;

    		//int baudrate = RXTXPort.staticGetBaudRate(SERIAL_PORT);
    		
    		//System.out.println("Baudrate: " + baudrate);
    		
    		start = System.currentTimeMillis();
        	
        	serialPort.sendBreak(2);
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
	    	
	    	System.out.println("Consume: " + 
	    			(pause1 - start) + ", " + 
	    			(pause2 - pause1) + ", " + 
	    			(stop - pause2) + ", " + 
	    			(stop - start) + "ms");
			
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} 
//		catch (UnsupportedCommOperationException e) {
//			// TODO Auto-generated catch block
//			e.printStackTrace();
//		} 
    }

	
	private final class SerialEventsListener implements SerialPortEventListener {

		@Override
		public void serialEvent(SerialPortEvent ev) {

			switch (ev.getEventType()) {
			
			case SerialPortEvent.DATA_AVAILABLE:
				//System.out.println("SerialPortEvent: DATA_AVAILABLE!");
				
				break;
			case SerialPortEvent.OUTPUT_BUFFER_EMPTY:
				//System.out.println("SerialPortEvent: OUTPUT_BUFFER_EMPTY!");
				break;
			case SerialPortEvent.CTS:
				System.out.println("SerialPortEvent: CTS!");
				break;
			case SerialPortEvent.DSR:
				System.out.println("SerialPortEvent: DSR!");
				break;
			case SerialPortEvent.RI:
				System.out.println("SerialPortEvent: RI!");
				break;
			case SerialPortEvent.CD:
				System.out.println("SerialPortEvent: CD!");
				break;
			case SerialPortEvent.OE:
				System.out.println("SerialPortEvent: OE!");
				break;
			case SerialPortEvent.PE:
				System.out.println("SerialPortEvent: PE!");
				break;
			case SerialPortEvent.FE:
				System.out.println("SerialPortEvent: FE!");
				break;
			case SerialPortEvent.BI:
				System.out.println("SerialPortEvent: BI!");
				break;
			}

		}
	}
    
    
	private void debug(String msg) {
		System.out.println("Debug: " + msg);
	}
	
	public static void main(String[] args){

		DS9097U test = new DS9097U();
		
		test.start();
		
		for(int i = 0; i< 5; i++){
			test.oneWirePresent();
			try {
				Thread.sleep(1000);
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
		
		try {
			Thread.sleep(60 * 1000);
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		
		
		test.stop();
		
	}
	
	public void start(){
		// / Find serial ports...
		portId = findPortId();

		// / Bail out if we can't get the port..
		if (portId == null) {
			debug("Can't find Serial Port ");
			return;
		}
		
		if (OpenSerialPort()) {
			
			StartReadingThread();
			
		} else {

			debug("Can't Open Serial Port ");
		}

		debug("Serial Port started!");
	}
	
	public void stop(){
		
		StopReadingThread();
		
		CloseSerialPort();

		debug("Serial Port stopped!");
	}
	
}

package com.dalsemi.onewire.utils;


/**
 * @author deven.fan#technodex.com
 * @version 0.9.1
 * @category tool - basic code converter
 * @date 2007-04-04
 */

public class ConvertCodec {

	private static final char[] hexDigits = { '0', '1', '2', '3', '4', '5',
			'6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

	/**
	 * Bytes to hex string.
	 * @param length
	 *            the length
	 * @param offset
	 *            the offset
	 * @param bytes
	 *            the bytes array
	 * @return Returns a string of hexdecimal digits from a byte array.
	 * @note Each byte is converted to 2 hex symbols.
	 * @remark If offset and length are omitted, the whole array is used.
	 */
	public static String bytesToHexString(byte[] bytes, int offset, int length) {
		char[] buf = new char[length * 2];
		int j = 0;
		int k;

		for (int i = offset; i < offset + length; i++) {
			k = bytes[i];
			buf[j++] = ConvertCodec.hexDigits[(k >>> 4) & 0x0F];
			buf[j++] = ConvertCodec.hexDigits[k & 0x0F];
		}
		return new String(buf);
	}

	/**
	 * @param bytes
	 *            the bytes array
	 * @return return the hexs string from byte[] like example
	 * @example "A018" from {(byte)0xA0,(byte)0x18}(byte[])
	 */
	public static String bytesToHexString(byte[] bytes) {
		return ConvertCodec.bytesToHexString(bytes, 0, bytes.length);
	}

	/**
	 * @param ch
	 *            the hex digit input
	 * @return Returns the number from 0 to 15 corresponding to the hex digit
	 */
	private static int fromDigit(char ch) {
		if (ch >= '0' && ch <= '9') {
			return ch - '0';
		}
		if (ch >= 'A' && ch <= 'F') {
			return ch - 'A' + 10;
		}
		if (ch >= 'a' && ch <= 'f') {
			return ch - 'a' + 10;
		}
		throw new IllegalArgumentException("invalid hex digit: '" + ch + "'");
	}

	/**
	 * @param hex
	 *            the hex string
	 * @return Returns a byte array from a string of hexadecimal digits.
	 */
	public static byte[] hexStringToBytes(String hex) {
		int len = hex.length();

		if ((len % 2) == 1) {
			throw new IllegalArgumentException("invalid hex length: '" + len
					+ "'");
		}

		byte[] buf = new byte[(len / 2)];
		int i = 0, j = 0;

		while (i < len) {
			buf[j++] = (byte) ((ConvertCodec.fromDigit(hex.charAt(i++)) << 4) | ConvertCodec
					.fromDigit(hex.charAt(i++)));
		}
		return buf;
	}

}

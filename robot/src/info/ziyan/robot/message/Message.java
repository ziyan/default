/*
 * File: Message.java is part of info.ziyan.robot.message.
 * Author: Ziyan Zhou <zhou@ziyan.info> http://ziyan.info
 *
 * This package is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This package is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * To obtain a copy of the GNU General Public License,
 * see <http://www.gnu.org/licenses/>.
 *
 */
package info.ziyan.robot.message;

import java.net.InetSocketAddress;

/**
 * Base class for all type of Message
 * @author ziyan
 *
 */
public abstract class Message implements Comparable<Message> {
	/**
	 * Message creator class provides method to create a certain type of message
	 * @author ziyan
	 *
	 */
	protected static abstract class Creator {
		protected abstract Message create(final InetSocketAddress source, byte type, byte priority, byte[] data, int offset, int length) throws Exception;
	}
	
	public static final int UDP_MESSAGE_SIZE = 8192;
	public static final byte TYPE = 0x00;

	public static final byte PRIORITY_CRITICAL = 0x03;
	public static final byte PRIORITY_HIGH = 0x02;
	public static final byte PRIORITY_NORMAL = 0x01;
	public static final byte PRIORITY_LOW = 0x00;

	protected byte type = TYPE;
	protected byte[] cache;
	protected byte priority;
	protected InetSocketAddress source, destination;

	public Message(final InetSocketAddress source, final InetSocketAddress destination, final byte type, final byte priority) {
		this.type = (byte)(type & 0x3f);
		this.priority = (byte)(priority & 0x3);
		this.source = source;
		this.destination = destination;
	}

	public byte getType() {
		return type;
	}

	public byte getPriority() {
		return priority;
	}
	
	public InetSocketAddress getSource() {
		return source;
	}
	
	public InetSocketAddress getDestination() {
		return destination;
	}

	public abstract byte[] getData();

	public int compareTo(final Message o) {
		return o.priority - priority;
	}

}

/*
 * File: NodeTerminatingMessage.java is part of info.ziyan.robot.message.
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

import java.net.InetAddress;
import java.net.InetSocketAddress;



public class NodeRegisteredMessage extends Message{
	public static final byte TYPE = 0x30;
	public static final Creator CREATOR = new Creator() {
			@Override
			protected Message create(final InetSocketAddress source, byte type, byte priority, byte[] data, int offset, int length) throws Exception {
				if(type != TYPE || (length !=2 && length < 4)) return null;
				short nodeId = (short)((data[offset] << 8) | data[offset + 1]);
				if(length == 2)
					return new NodeLookupResponseMessage(source, null, nodeId, null);
				int port = ((data[offset + 2] << 8) & 0xff00) | (data[offset + 3] & 0xff);
				byte[] addr = new byte[length - 4];
				System.arraycopy(data, offset + 4, addr, 0, length - 4);
				return new NodeRegisteredMessage(source, null, nodeId, new InetSocketAddress(InetAddress.getByAddress(addr), port));
			}
		};
	
	protected final short nodeId;
	protected final InetSocketAddress address;
	
	public NodeRegisteredMessage(final InetSocketAddress source, final InetSocketAddress destination, short nodeId, InetSocketAddress address) {
		super(source, destination, TYPE, Message.PRIORITY_CRITICAL);
		this.nodeId = nodeId;
		this.address = address;
	}
	
	public short getNodeId() {
		return nodeId;
	}

	public InetSocketAddress getAddress() {
		return address;
	}
	
	@Override
	public byte[] getData() {
		byte[] buffer;
		if(address!=null) {
			int port = address.getPort();
			byte[] addr = address.getAddress().getAddress();
			buffer = new byte[4 + addr.length];
			buffer[2] = (byte)((port & 0xff00) >> 8);
			buffer[3] = (byte)(port & 0xff);
			System.arraycopy(addr, 0, buffer, 4, addr.length);
		} else
			buffer = new byte[2];
		buffer[0] = (byte)((nodeId & 0xff00) >> 8);
		buffer[1] = (byte)(nodeId & 0xff);
		return buffer;
	}
}

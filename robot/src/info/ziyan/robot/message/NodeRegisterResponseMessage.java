/*
 * File: NodeRegisterResponse.java is part of info.ziyan.robot.message.
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



public class NodeRegisterResponseMessage extends Message{
	public static final byte TYPE = 0x11;
	public static final Creator CREATOR = new Creator() {
			@Override
			protected Message create(final InetSocketAddress source, byte type, byte priority, byte[] data, int offset, int length) {
				if(type != TYPE || length != 2) return null;
				short nodeId = (short)((data[offset] << 8) | data[offset + 1]);
				return new NodeRegisterResponseMessage(source, null, nodeId);
			}
		};

	protected final short nodeId;
	
	public NodeRegisterResponseMessage(final InetSocketAddress source, final InetSocketAddress destination, short nodeId) {
		super(source, destination, TYPE, Message.PRIORITY_CRITICAL);
		this.nodeId = nodeId;
	}
	
	public short getNodeId() {
		return nodeId;
	}

	@Override
	public byte[] getData() {
		byte[] buffer = new byte[2];
		buffer[0] = (byte)((nodeId & 0xff00) >> 8);
		buffer[1] = (byte)(nodeId & 0xff);
		return buffer;
	}
}

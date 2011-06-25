/*
 * File: Messaging.java is part of info.ziyan.robot.message.
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

import info.ziyan.robot.message.Message.Creator;

import java.net.InetSocketAddress;


/**
 * @author ziyan
 *
 */
public class Messages {	
	public static final Creator[] CREATORS = new Creator[64];
	static {
		CREATORS[HeadHeartbeatMessage.TYPE & 0x3f] = HeadHeartbeatMessage.CREATOR;
		CREATORS[HeadTerminatingMessage.TYPE & 0x3f] = HeadTerminatingMessage.CREATOR;

		CREATORS[NodeRegisteredMessage.TYPE & 0x3f] = NodeRegisteredMessage.CREATOR;
		CREATORS[NodeHeartbeatMessage.TYPE & 0x3f] = NodeHeartbeatMessage.CREATOR;
		CREATORS[NodeRegisterRequestMessage.TYPE & 0x3f] = NodeRegisterRequestMessage.CREATOR;
		CREATORS[NodeRegisterResponseMessage.TYPE & 0x3f] = NodeRegisterResponseMessage.CREATOR;
		CREATORS[NodeTerminatingMessage.TYPE & 0x3f] = NodeTerminatingMessage.CREATOR;
		CREATORS[NodeLookupRequestMessage.TYPE & 0x3f] = NodeLookupRequestMessage.CREATOR;
		CREATORS[NodeLookupResponseMessage.TYPE & 0x3f] = NodeLookupResponseMessage.CREATOR;
		
		CREATORS[ModuleRegisterRequestMessage.TYPE & 0x3f] = ModuleRegisterRequestMessage.CREATOR;
		CREATORS[ModuleRegisterResponseMessage.TYPE & 0x3f] = ModuleRegisterResponseMessage.CREATOR;
		CREATORS[ModuleRegisteredMessage.TYPE & 0x3f] = ModuleRegisteredMessage.CREATOR;
		CREATORS[ModuleUnregisteringMessage.TYPE & 0x3f] = ModuleUnregisteringMessage.CREATOR;
		CREATORS[ModuleQueryRequestMessage.TYPE & 0x3f] = ModuleQueryRequestMessage.CREATOR;
		CREATORS[ModuleQueryResponseMessage.TYPE & 0x3f] = ModuleQueryResponseMessage.CREATOR;
		CREATORS[ModuleLookupRequestMessage.TYPE & 0x3f] = ModuleLookupRequestMessage.CREATOR;
		CREATORS[ModuleLookupResponseMessage.TYPE & 0x3f] = ModuleLookupResponseMessage.CREATOR;
		
	}
	
	
	public static void register(byte type, Creator creator) {
		CREATORS[type & 0x3f] = creator;
	}

	/**
	 * Get the byte[] representation of the message
	 * This method uses cached byte[] after first call
	 * @param msg
	 * @return
	 */
	public static byte[] toBytes(final Message msg) {
		if(msg.cache != null)
			return msg.cache;
		final byte[] data = msg.getData();
		byte[] cache;
		if(data != null && data.length > 0) {
			cache = new byte[1+data.length];
			for(int i=0;i<data.length;i++)
				cache[i+1] = data[i];
		} else
			cache = new byte[1];
		cache[0] = (byte)(((msg.priority << 6) & 0xc0) | (msg.type & 0x3f));
		msg.cache = cache;
		return cache;
	}

	/**
	 * Create Message from provided byte[]
	 * @param source
	 * @param msg
	 * @param offset
	 * @param length
	 * @return
	 */
	public static Message fromBytes(final InetSocketAddress source, final byte[] msg, final int offset, final int length) {
		if(msg == null || msg.length < offset + length || length < 1) return null;
		final byte header = msg[offset];
		final Creator creator = CREATORS[header & 0x3f];
		try {
			if(creator != null)
				return creator.create(source, (byte)(header & 0x3f), (byte)((header >> 6) & 0x3), msg, offset+1, length-1);
		} catch(final Exception e) {
			e.printStackTrace();
		}
		return null;
	}
}

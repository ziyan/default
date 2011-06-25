/*
 * File: NodeQueryRequestMessage.java is part of info.ziyan.robot.message.
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
import java.util.Collections;
import java.util.LinkedHashMap;
import java.util.Map;

public class ModuleQueryResponseMessage extends Message {
	public static final byte TYPE = 0x26;

	public static final Creator CREATOR = new Creator() {
		@Override
		protected Message create(final InetSocketAddress source, byte type, byte priority, byte[] data, int offset, int length) throws Exception {
			if(type != TYPE || length < 0) return null;
			if(length == 0)
				return new ModuleQueryResponseMessage(source, null, "", null);
			int i = offset;
			int j = i; // take note of begin
			while(data[j] != 0) j++; // find end
			String moduleId = new String(data, j, i - j);
			
			Map<String, Short> map = new LinkedHashMap<String, Short>();
			while(i < offset+length) {
				j = i; // take note of begin
				while(data[i] != 0) i++; // find end
				map.put(new String(data, j, i - j), (short)((data[i+1] << 8) | data[i+2])); // put into map
				i+=3;
			}
			ModuleQueryResponseMessage msg = new ModuleQueryResponseMessage(source, null, moduleId, map);
			return msg;
		}
	};

	protected final Map<String, Short> map;
	protected final String moduleId;

	public ModuleQueryResponseMessage(final InetSocketAddress source, final InetSocketAddress destination, final String moduleId, final Map<String, Short> map) {
		super(source, destination, TYPE, Message.PRIORITY_HIGH);
		if (moduleId == null)
			throw new IllegalArgumentException();
		this.moduleId = moduleId;
		this.map = map == null ? null : Collections.unmodifiableMap(map);
	}

	public Map<String, Short> getMap() {
		return map;
	}

	@Override
	public byte[] getData() {
		byte[] sb = moduleId.getBytes();
		int length = sb.length + 1;
		byte[] buffer;
		if (map != null) {
			int i = length;
			for(String moduleId : map.keySet())
				length += moduleId.length() + 3;
			buffer = new byte[length];
			for(String moduleId : map.keySet()) {
				byte[] sbuffer = moduleId.getBytes();
				System.arraycopy(sbuffer, 0, buffer, i, sbuffer.length);
				i += sbuffer.length;
				buffer[i] = 0;
				short nodeId = map.get(moduleId);
				buffer[i+1] = (byte)((nodeId & 0xff00) >> 8);
				buffer[i+2] = (byte)(nodeId & 0xff);
				i += 3;
			}
		} else
			buffer = new byte[length];
		System.arraycopy(sb, 0, buffer, 0, sb.length);
		buffer[sb.length] = 0;
		return buffer;
	}

}

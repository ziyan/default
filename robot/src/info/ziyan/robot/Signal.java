/*
 * File: Event.java is part of info.ziyan.robot.
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
package info.ziyan.robot;

/**
 * @author ziyan
 *
 */
public class Signal {
	public static final short TYPE_MODULE_RUN = 0x0001;
	public static final short TYPE_MODULE_REGISTERED = 0x0002;
	public static final short TYPE_MODULE_UNREGISTERED = 0x0003;
	public static final short TYPE_MODULE_STOP = 0x0004;
	private final String source, destination;
	private final short type;
	public Signal(final short type) {
		this(null, null, type);
	}
	public Signal(final String source, final String destination, final short type) {
		this.source = source;
		this.destination = destination;
		this.type = type;
	}

	public String getSource() {
		return source;
	}
	public String getDestination() {
		return destination;
	}
	public short getType() {
		return type;
	}
}

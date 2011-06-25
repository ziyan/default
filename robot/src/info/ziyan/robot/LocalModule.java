/*
 * File: Module.java is part of info.ziyan.robot.module.
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


import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;

public abstract class LocalModule implements Module {
	public static enum Status { New, Running, Error, Stopped }

	protected String id;
	protected Status status;
	protected final BlockingQueue<Signal> signals;
	protected final Thread signaler;

	public LocalModule() {
		status = Status.New;
		signals = new LinkedBlockingQueue<Signal>();
		signaler = new Thread(new Runnable() {
			public void run() {	signaler();	}
		});
		signaler.start();
	}

	public String getId() {
		return id;
	}

	/**
	 * Get status of Module
	 * @return
	 */
	public Status getStatus() {
		return status;
	}

	public Event waitForEvent(final short type) {
		return waitForEvent(type, 0);
	}

	public Event waitForEvent(final short type, final long timeout) {
		return null;
	}

	public void addEventCallback(final short type, final Callback<Event> listener) {

	}

	public void removeEventCallback(final short type, final Callback<Event> listener) {

	}

	public void signal(final Signal s) throws InterruptedException {
		signals.put(s);
	}


	protected void run() {
		if (status == Status.New)
			status = Status.Running;
	}
	protected void registered() { }
	protected void unregistered() { }
	protected void stop() {
		status = Status.Stopped;
	}

	protected void handleSignal(final Signal s) throws Exception {
		switch(s.getType()) {
		case Signal.TYPE_MODULE_RUN:
			run();
			break;
		case Signal.TYPE_MODULE_REGISTERED:
			registered();
			break;
		case Signal.TYPE_MODULE_UNREGISTERED:
			unregistered();
			break;
		case Signal.TYPE_MODULE_STOP:
			stop();
			break;
		}
	}

	/**
	 * Thread to run signals
	 *
	 */
	protected void signaler() {
		while(status != Status.Stopped || !signals.isEmpty()) {
			try {
				handleSignal(signals.take());
			} catch (final Exception e) { }
		}
	}


	@Override
	public final String toString() {
		return id;
	}

	/* (non-Javadoc)
	 * @see java.lang.Object#hashCode()
	 */
	@Override
	public int hashCode() {
		// TODO Auto-generated method stub
		return toString().hashCode();
	}

	/* (non-Javadoc)
	 * @see java.lang.Object#equals(java.lang.Object)
	 */
	@Override
	public boolean equals(final Object obj) {
		if(!(obj instanceof LocalModule)) return false;
		return ((LocalModule)obj).toString().equals(toString());
	}


}

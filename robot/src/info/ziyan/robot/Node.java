/*
 * File: Node.java is part of info.ziyan.robot.node.
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

import info.ziyan.robot.message.HeadHeartbeatMessage;
import info.ziyan.robot.message.HeadTerminatingMessage;
import info.ziyan.robot.message.Message;
import info.ziyan.robot.message.Messages;
import info.ziyan.robot.message.ModuleRegisterRequestMessage;
import info.ziyan.robot.message.ModuleRegisterResponseMessage;
import info.ziyan.robot.message.ModuleRegisteredMessage;
import info.ziyan.robot.message.ModuleUnregisteringMessage;
import info.ziyan.robot.message.NodeHeartbeatMessage;
import info.ziyan.robot.message.NodeRegisterRequestMessage;
import info.ziyan.robot.message.NodeRegisterResponseMessage;
import info.ziyan.robot.message.NodeRegisteredMessage;
import info.ziyan.robot.message.NodeTerminatingMessage;

import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.util.Collections;
import java.util.Date;
import java.util.LinkedHashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.PriorityBlockingQueue;

public class Node {
	protected static Node node = null;

	/**
	 * Initialize Node
	 * @throws Exception
	 */
	public static void init() throws Exception {
		if(node == null)
			node = new Node();
	}

	/**
	 * Exit Node
	 */
	public static void exit() {
		node.terminate();
	}

	/**
	 * Execute a Module
	 * @param module
	 * @throws InterruptedException 
	 */
	public static void run(final LocalModule module) throws InterruptedException {
		node.addLocalModule(module);
	}

	/**
	 * Get status of the running Node
	 * @return
	 */
	public static Status status() {
		return node.status;
	}

	/**
	 * Get all running Modules on current node
	 * @return
	 */
	public static LocalModule[] localModules() {
		return node.localModules.values().toArray(new LocalModule[]{});
	}
	
	public static Object module(String id) {
		return node.getModule(id);
	}

	/**
	 * Node Status Enumeration
	 * @author ziyan
	 *
	 */
	public static enum Status { Detached, Attached, Terminated };

	private short id = 1; // node id

	// communication
	private final DatagramSocket udp; // udp sender/listener
	private final InetSocketAddress head; // head address
	private final BlockingQueue<Message> queue; // message out queue
	private final Thread heart, receiver, sender; // threads

	private Status status = Status.Detached; // node status

	// cached messages
	private final Message cachedRegisterRequestMessage; // cached message
	private final Message cachedHeartbeatMessage; // cached message

	private long headHeartbeatTimestamp = -1; // head heartbeat timestamp

	// local modules
	private final Map<String, LocalModule> localModules; // module list, map: module -> registered?
	private final Map<String, RemoteModule> remoteModules; // proxies
	
	private final Map<Short, InetSocketAddress> nodeTable; // remote node addresses
	private final Map<String, Short> moduleTable; // remote module table


	private Node() throws Exception {
		// get node id
		id = Short.parseShort(System.getProperty("robot.node.id", ""+id));

		// setup local variables
		localModules  = Collections.synchronizedMap(new LinkedHashMap<String, LocalModule>());
		remoteModules = Collections.synchronizedMap(new LinkedHashMap<String, RemoteModule>());
		moduleTable = Collections.synchronizedMap(new LinkedHashMap<String, Short>());
		nodeTable = Collections.synchronizedMap(new LinkedHashMap<Short, InetSocketAddress>());


		// get address for head
		head = new InetSocketAddress(
				InetAddress.getByName(System.getProperty("robot.head.host", "localhost")),
				Integer.parseInt(System.getProperty("robot.head.port", ""+Constants.HEAD_DEFAULT_PORT)));

		// setup cached messages
		cachedRegisterRequestMessage = new NodeRegisterRequestMessage(null, head, id);
		cachedHeartbeatMessage = new NodeHeartbeatMessage(null, head, id);

		// let's setup communication
		udp = new DatagramSocket(); // setup udp
		queue = new PriorityBlockingQueue<Message>(); // setup queue


		// thread for the heart
		heart = new Thread(new Runnable() {
			public void run() { heart(); }
		});


		// thread to listen to incoming udp packets
		receiver = new Thread(new Runnable() {
			public void run() {	receiver();	}
		});


		// thread to send udp packets on the queue
		sender = new Thread(new Runnable() {
			public void run() {	sender(); }
		});

		// start all threads
		heart.start();
		receiver.start();
		sender.start();
	}

	private synchronized void terminate() {
		if(status == Status.Terminated) return;
		status = Status.Terminated;
		queue.add(new NodeTerminatingMessage(null, head, id));
		try {
			signalAll(new Signal(Signal.TYPE_MODULE_STOP));
		} catch (final InterruptedException e) { }
		try {
			Thread.sleep(Constants.NODE_TERMINATION_DELAY);
		} catch (final InterruptedException e) { }
		heart.interrupt();
		sender.interrupt();
		udp.close();
		receiver.interrupt();
	}

	private void addLocalModule(final LocalModule module) throws InterruptedException {
		if(module == null || module.getId() == null)
			throw new IllegalArgumentException();
		if(localModules.containsKey(module.getId())) return;
		// add to map
		localModules.put(module.getId(), module);
		// signal module to start
		module.signal(new Signal(Signal.TYPE_MODULE_RUN));
		// register
		if(status == Status.Attached)
			heart.interrupt();
	}
	
	private Module getModule(final String id) {
		if (id == null || id.equals(""))
			return null;
		if (localModules.containsKey(id))
			return localModules.get(id);
		if (remoteModules.containsKey(id))
			return remoteModules.get(id);
		return null;
	}

	private void signalAll(final Signal signal) throws InterruptedException {
		for(final LocalModule module : localModules.values())
			module.signal(signal);
	}

	private void heart() {
		while(status != Status.Terminated) {
			try {
				if(status == Status.Detached) {
					queue.put(cachedRegisterRequestMessage);
					Thread.sleep(Constants.NODE_REGISTER_TRIAL_INTERVAL);
				} else if(status == Status.Attached) {
					if(new Date().getTime() - headHeartbeatTimestamp > Constants.HEAD_HEARTBEAT_TIMEOUT) {
						// heartbeat timeout, head is dead
						status = Status.Detached;
						signalAll(new Signal(Signal.TYPE_MODULE_UNREGISTERED));
						moduleTable.clear();
						nodeTable.clear();
						continue;
					}
					queue.put(cachedHeartbeatMessage);
					for(final LocalModule module : localModules.values())
						if(!moduleTable.containsKey(module.getId()))
							queue.add(new ModuleRegisterRequestMessage(null, head, id, module.getId()));
					Thread.sleep(Constants.NODE_HEARTBEAT_INTERVAL);
				}
			} catch(final InterruptedException e) { }
		}
	}

	private void sender() {
		while(status != Status.Terminated || !queue.isEmpty()) {
			try {
				final Message msg = queue.take();
				final InetSocketAddress destination = msg.getDestination();
				if(destination == null) continue;
				final byte[] buffer = Messages.toBytes(msg);
				udp.send(new DatagramPacket(buffer, buffer.length, destination.getAddress(), destination.getPort()));
			} catch(final Exception e) { }
		}
	}

	private void receiver() {
		while(status != Status.Terminated) {
			try {
				final byte[] buffer = new byte[Message.UDP_MESSAGE_SIZE];
				final DatagramPacket packet = new DatagramPacket(buffer, buffer.length);
				udp.receive(packet);
				final InetSocketAddress address = new InetSocketAddress(packet.getAddress(), packet.getPort());
				final Message msg = Messages.fromBytes(address, packet.getData(), packet.getOffset(), packet.getLength());
				// check if it is a valid message
				if(msg == null) continue;
				handleMessage(msg);
			} catch (final Exception e) {
			}
		}

	}

	private void handleMessage(final Message msg) throws InterruptedException {
		switch(msg.getType()) {
		case NodeRegisterResponseMessage.TYPE:
			handleNodeRegisterResponseMessage((NodeRegisterResponseMessage)msg);
			break;
		case ModuleRegisterResponseMessage.TYPE:
			handleModuleRegisterResponseMessage((ModuleRegisterResponseMessage)msg);
			break;
		case HeadHeartbeatMessage.TYPE:
			handleHeadHeartbeatMessage((HeadHeartbeatMessage)msg);
			break;
		case HeadTerminatingMessage.TYPE:
			handleHeadTerminatingMessage((HeadTerminatingMessage)msg);
			break;
		case NodeRegisteredMessage.TYPE:
			handleNodeRegisteredMessage((NodeRegisteredMessage)msg);
			break;
		case NodeTerminatingMessage.TYPE:
			handleNodeTerminatingMessage((NodeTerminatingMessage)msg);
			break;
		case ModuleRegisteredMessage.TYPE:
			handleModuleRegisteredMessage((ModuleRegisteredMessage)msg);
			break;
		case ModuleUnregisteringMessage.TYPE:
			handleModuleUnregisteringMessage((ModuleUnregisteringMessage)msg);
			break;
		}
	}

	private void handleHeadHeartbeatMessage(final HeadHeartbeatMessage msg) {
		if(!head.equals(msg.getSource())) return;
		headHeartbeatTimestamp = new Date().getTime();
	}
	private void handleHeadTerminatingMessage(final HeadTerminatingMessage msg) throws InterruptedException {
		if(!head.equals(msg.getSource())) return;
		status = Status.Detached;
		signalAll(new Signal(Signal.TYPE_MODULE_UNREGISTERED));
		moduleTable.clear();
		nodeTable.clear();
		heart.interrupt();
		System.out.println("Head terminating");
	}
	private void handleNodeRegisterResponseMessage(final NodeRegisterResponseMessage msg) {
		if(!head.equals(msg.getSource())) return;
		headHeartbeatTimestamp = new Date().getTime();
		status = Status.Attached;
		heart.interrupt();
	}
	private void handleNodeRegisteredMessage(final NodeRegisteredMessage msg) {
		if(!head.equals(msg.getSource())) return;
		final short nodeId = msg.getNodeId();
		if(nodeId == id) return; // myself
		final InetSocketAddress address = msg.getAddress();
		if(address.getAddress().isLoopbackAddress())
			nodeTable.put(nodeId, new InetSocketAddress(head.getAddress(), address.getPort()));
		else
			nodeTable.put(nodeId, address);
		System.out.println("Node registered: "+msg.getAddress().toString());
	}
	private void handleNodeTerminatingMessage(final NodeTerminatingMessage msg) {
		if(!head.equals(msg.getSource())) return;
		final short nodeId = msg.getNodeId();
		if(nodeId == id) return; // myself
		// clean up local cache of remote module table
		final List<String> moduleIds = new LinkedList<String>();
		for(final String moduleId : moduleTable.keySet())
			if(moduleTable.get(moduleId) == nodeId)
				moduleIds.add(moduleId);
		for(final String moduleId : moduleIds)
			moduleTable.remove(moduleId);
		nodeTable.remove(nodeId);

		System.out.println("Node terminating");
	}

	private void handleModuleRegisterResponseMessage(final ModuleRegisterResponseMessage msg) throws InterruptedException {
		if(!head.equals(msg.getSource())) return;
		final LocalModule module = localModules.get(msg.getModuleId());
		if(module == null) return;
		moduleTable.put(module.getId(), id);
		module.signal(new Signal(Signal.TYPE_MODULE_REGISTERED));
		System.out.println("Module registerd");
	}	
	private void handleModuleRegisteredMessage(final ModuleRegisteredMessage msg) {
		if(!head.equals(msg.getSource())) return;
		final short nodeId = msg.getNodeId();
		if(nodeId == id) return; // myself
		moduleTable.put(msg.getModuleId(), nodeId);
		System.out.println("Module registered: "+msg.getModuleId());
	}
	private void handleModuleUnregisteringMessage(final ModuleUnregisteringMessage msg) {
		if(!head.equals(msg.getSource())) return;
		if(msg.getNodeId() == id) return; // myself
		moduleTable.remove(msg.getNodeId());
		System.out.println("Module unregistered");
	}	
}

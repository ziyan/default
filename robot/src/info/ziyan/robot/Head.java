/*
 * File: Head.java is part of info.ziyan.robot.node.
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
import info.ziyan.robot.message.ModuleLookupRequestMessage;
import info.ziyan.robot.message.ModuleLookupResponseMessage;
import info.ziyan.robot.message.ModuleQueryRequestMessage;
import info.ziyan.robot.message.ModuleQueryResponseMessage;
import info.ziyan.robot.message.ModuleRegisterRequestMessage;
import info.ziyan.robot.message.ModuleRegisterResponseMessage;
import info.ziyan.robot.message.ModuleRegisteredMessage;
import info.ziyan.robot.message.ModuleUnregisteringMessage;
import info.ziyan.robot.message.NodeHeartbeatMessage;
import info.ziyan.robot.message.NodeLookupRequestMessage;
import info.ziyan.robot.message.NodeLookupResponseMessage;
import info.ziyan.robot.message.NodeRegisterRequestMessage;
import info.ziyan.robot.message.NodeRegisterResponseMessage;
import info.ziyan.robot.message.NodeRegisteredMessage;
import info.ziyan.robot.message.NodeTerminatingMessage;

import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetSocketAddress;
import java.net.SocketException;
import java.util.Collections;
import java.util.Date;
import java.util.LinkedHashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.PriorityBlockingQueue;
import java.util.regex.Pattern;

public class Head {
	private static Head head;

	/**
	 * Initialize Head
	 * @throws SocketException
	 */
	public static void init() throws SocketException {
		if(head == null) head = new Head();
	}

	/**
	 * Terminate Head
	 *
	 */
	public static void exit() {
		head.terminate();
	}

	/**
	 * Return status of the Head
	 * @return
	 */
	public static boolean running() {
		return head.running;
	}

	private static class Node {
		public Node(final short id, final InetSocketAddress address) {
			this.id = id;
			this.address = address;
			this.heartbeatTimestamp = new Date().getTime();
			this.modules = new LinkedList<String>();
		}
		public final short id;
		public long heartbeatTimestamp;
		public final InetSocketAddress address;
		public final List<String> modules;
	}

	private final Map<Short, Node> nodes;
	private final Map<String, Node> modules;
	private final DatagramSocket udp;
	private final BlockingQueue<Message> queue;
	private final Thread heart, sender, receiver;
	private final Message cachedHeartbeat;
	private boolean running = true;

	private Head() throws SocketException {
		udp = new DatagramSocket(
				Integer.parseInt(System.getProperty("robot.head.port",
						""+Constants.HEAD_DEFAULT_PORT)));
		queue = new PriorityBlockingQueue<Message>();
		nodes = Collections.synchronizedMap(new LinkedHashMap<Short, Node>());
		modules = Collections.synchronizedMap(new LinkedHashMap<String, Node>());
		cachedHeartbeat = new HeadHeartbeatMessage(null, null);

		System.out.println("Head listening on: "+udp.getLocalPort());
		// jump start the heart
		heart = new Thread(new Runnable() {
			public void run() { heart(); }
		});
		heart.start();
		receiver = new Thread(new Runnable() {
			public void run() { receiver(); }
		});
		receiver.start();
		sender = new Thread(new Runnable() {
			public void run() { sender(); }
		});
		sender.start();
	}

	/**
	 * Terminate the Head
	 *
	 */
	private void terminate() {
		// first, we set the running flag to false
		running = false;
		// we send out head terminating message to everyone
		queue.add(new HeadTerminatingMessage(null, null));
		// sleep a few secs to allow critical messages
		// in the queue to finish sending
		try {
			Thread.sleep(Constants.HEAD_TERMINATION_DELAY);
		} catch (final InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		// interrupt all threads and kill them
		heart.interrupt();
		sender.interrupt();
		// kill the udp socket so receiver quits
		udp.close();
		receiver.interrupt();
	}

	/**
	 * The heart process
	 * responsible for sending out heartbeats,
	 * and cleanning up timed out nodes
	 */
	private void heart() {
		while(running) {
			try {
				clean();
				queue.put(cachedHeartbeat);
				Thread.sleep(Constants.HEAD_HEARTBEAT_INTERVAL);
			} catch (final Exception e) { }
		}
	}

	/**
	 * Clean procedure called by heart process
	 * responsible for cleaning up timed out nodes
	 * @throws Exception
	 */
	private void clean() throws Exception {
		// get a current timestamp
		final long now = new Date().getTime();
		// create a list to hold those to be deleted ids
		final List<Short> nodeIds = new LinkedList<Short>();
		// check each node's recent heartbeat
		for(final Node node : nodes.values())
			if(now - node.heartbeatTimestamp > Constants.NODE_HEARTBEAT_TIMEOUT)
				nodeIds.add(node.id); // oh god, it is dead
		// if no one is dead yet, we quit
		if(nodeIds.size() == 0) return;
		// someone is dead
		for(final Short nodeId : nodeIds) {
			final List<String> moduleIds = new LinkedList<String>();
			for(final String moduleId : modules.keySet())
				if(modules.get(moduleId).id == nodeId)
					moduleIds.add(moduleId);
			for(final String moduleId : moduleIds)
				modules.remove(moduleId);
			nodes.remove(nodeId); // remove the dead
			queue.put(new NodeTerminatingMessage(null, null, nodeId)); // notify everyone
		}
	}


	private void sender() {
		while(running || !queue.isEmpty()) {
			try {
				final Message msg = queue.take();
				final byte[] buffer = Messages.toBytes(msg);
				final InetSocketAddress destination = msg.getDestination();
				if(destination == null)
					for(final Node node : nodes.values())
						udp.send(new DatagramPacket(buffer, buffer.length, node.address.getAddress(), node.address.getPort()));
				else
					udp.send(new DatagramPacket(buffer, buffer.length, destination.getAddress(), destination.getPort()));
			} catch(final Exception e) { }
		}
	}

	private void receiver() {
		while(running) {
			try {
				final byte[] buffer = new byte[Message.UDP_MESSAGE_SIZE];
				final DatagramPacket packet = new DatagramPacket(buffer, buffer.length);
				udp.receive(packet);
				final InetSocketAddress address = new InetSocketAddress(packet.getAddress(), packet.getPort());
				final Message msg = Messages.fromBytes(address, packet.getData(), packet.getOffset(), packet.getLength());
				if(msg == null) continue; // check if valid message
				handleMessage(msg);
			} catch (final Exception e) { }
		}
	}

	private void handleMessage(final Message msg) throws InterruptedException{
		switch(msg.getType()) {
		case NodeRegisterRequestMessage.TYPE:
			handleNodeRegisterMessage((NodeRegisterRequestMessage)msg);
			break;
		case NodeHeartbeatMessage.TYPE:
			handleNodeHeartbeatMessage((NodeHeartbeatMessage)msg);
			break;
		case NodeTerminatingMessage.TYPE:
			handleNodeTerminatingMessage((NodeTerminatingMessage)msg);
			break;
		case NodeLookupRequestMessage.TYPE:
			handleNodeLookupRequestMessage((NodeLookupRequestMessage)msg);
			break;
		case ModuleRegisterRequestMessage.TYPE:
			handleModuleRegisterRequestMessage((ModuleRegisterRequestMessage)msg);
			break;
		case ModuleUnregisteringMessage.TYPE:
			handleModuleUnregisteringMessage((ModuleUnregisteringMessage)msg);
			break;
		case ModuleQueryRequestMessage.TYPE:
			handleModuleQueryRequestMessage((ModuleQueryRequestMessage)msg);
			break;
		case ModuleLookupRequestMessage.TYPE:
			handleModuleLookupRequestMessage((ModuleLookupRequestMessage)msg);
			break;
		}
	}

	private void handleNodeRegisterMessage(final NodeRegisterRequestMessage msg) throws InterruptedException {
		final short nodeId = msg.getNodeId();
		if(nodeId <= 0) return;
		if(nodes.get(nodeId) != null) return;
		nodes.put(nodeId, new Node(nodeId, msg.getSource()));
		queue.put(new NodeRegisterResponseMessage(null, msg.getSource(), nodeId));
		queue.put(new NodeRegisteredMessage(null, null, nodeId, msg.getSource()));
		System.out.println("Node register: "+nodeId);
	}
	private void handleNodeHeartbeatMessage(final NodeHeartbeatMessage msg) {
		final short nodeId = msg.getNodeId();
		if(nodeId <= 0) return;
		final Node node = nodes.get(nodeId);
		if(node == null || !node.address.equals(msg.getSource())) return;
		node.heartbeatTimestamp = new Date().getTime();
		System.out.println("Node heartbeat: "+nodeId);
	}
	private void handleNodeTerminatingMessage(final NodeTerminatingMessage msg) throws InterruptedException {
		final short nodeId = msg.getNodeId();
		if(nodeId <= 0) return;
		final Node node = nodes.get(nodeId);
		if(node == null || !node.address.equals(msg.getSource())) return;
		// unregister all modules that belong to the node
		final List<String> moduleIds = new LinkedList<String>();
		for(final String moduleId : modules.keySet())
			if(modules.get(moduleId) == node)
				moduleIds.add(moduleId);
		for(final String moduleId : moduleIds)
			modules.remove(moduleId);
		// remove the node
		nodes.remove(nodeId);
		queue.put(new NodeTerminatingMessage(null, null, nodeId));
		System.out.println("Node terminated: "+nodeId);
	}
	private void handleNodeLookupRequestMessage(final NodeLookupRequestMessage msg) throws InterruptedException {
		final short nodeId = msg.getNodeId();
		final Node node = nodes.get(nodeId);
		if(node==null)
			queue.put(new NodeLookupResponseMessage(null, msg.getSource(), nodeId, null));
		else
			queue.put(new NodeLookupResponseMessage(null, msg.getSource(), nodeId, node.address));

	}
	private void handleModuleRegisterRequestMessage(final ModuleRegisterRequestMessage msg) throws InterruptedException {
		final short nodeId = msg.getNodeId();
		if(nodeId <= 0) return;
		final Node node = nodes.get(nodeId);
		if(node == null || !node.address.equals(msg.getSource())) return;
		final String moduleId = msg.getModuleId();
		if(modules.get(moduleId) != null) return;
		modules.put(moduleId, node);
		System.out.println("Module registered: "+moduleId);
		queue.put(new ModuleRegisterResponseMessage(null, msg.getSource(), nodeId, moduleId));
		queue.put(new ModuleRegisteredMessage(null, null, nodeId, moduleId));
	}
	private void handleModuleUnregisteringMessage(final ModuleUnregisteringMessage msg) throws InterruptedException {
		final short nodeId = msg.getNodeId();
		if(nodeId <= 0) return;
		final Node node = nodes.get(nodeId);
		if(node == null || !node.address.equals(msg.getSource())) return;
		final String moduleId = msg.getModuleId();
		if(modules.get(moduleId) != node) return;
		modules.remove(moduleId);
		System.out.println("Module unregistered: "+moduleId);
		queue.put(new ModuleUnregisteringMessage(null, null, nodeId, moduleId));
	}
	private void handleModuleQueryRequestMessage(final ModuleQueryRequestMessage msg) throws InterruptedException {
		final Pattern pattern = Pattern.compile(msg.getModuleId());
		final Map<String, Short> map = new LinkedHashMap<String, Short>();
		for(final String moduleId : modules.keySet())
			if(pattern.matcher(moduleId).matches())
				map.put(moduleId, modules.get(moduleId).id);
		queue.put(new ModuleQueryResponseMessage(null, msg.getSource(), msg.getModuleId(), map));
	}
	private void handleModuleLookupRequestMessage(final ModuleLookupRequestMessage msg) throws InterruptedException {
		final String moduleId = msg.getModuleId();
		final Node node = modules.get(moduleId);
		if(node == null)
			queue.put(new ModuleLookupResponseMessage(null, msg.getSource(), moduleId, (short)0));
		else
			queue.put(new ModuleLookupResponseMessage(null, msg.getSource(), moduleId, node.id));
	}
}

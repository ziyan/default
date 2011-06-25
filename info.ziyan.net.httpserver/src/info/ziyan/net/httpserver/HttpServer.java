package info.ziyan.net.httpserver;
import java.io.IOException;
import java.net.InetAddress;
import java.net.ServerSocket;
import java.net.UnknownHostException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.SynchronousQueue;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.TimeUnit;

public class HttpServer implements Runnable{
	public static final int DEFAULT_PORT = 80;
	public static final String DEFAULT_NAME = "info.ziyan.httpserver/1.0";
	public static final String[] DEFAULT_ALLOWED_METHODS = new String[] {"GET","HEAD","POST"};

	private final ExecutorService pool = new ThreadPoolExecutor(10, 1000, 60L, TimeUnit.SECONDS, new SynchronousQueue<Runnable>());
	private ServerSocket server;
	private final HttpHandler handler;
	private String name = DEFAULT_NAME;
	private String[] allowedMethods = DEFAULT_ALLOWED_METHODS;

	/**
	 * Construct default HttpServer with default port
	 * @throws UnknownHostException
	 * @throws IOException
	 */
	public HttpServer(final HttpHandler handler) throws IOException {
		this(DEFAULT_PORT, null, handler);
	}

	/**
	 * Construct a HttpServer to listen on a specific port at all addresses
	 * @param port
	 * @throws IOException
	 */
	public HttpServer(final int port, final HttpHandler handler) throws IOException {
		this(port, null, handler);
	}

	/**
	 * Construct a HttpServer with specific address and port
	 * @param port
	 * @param address
	 * @throws IOException
	 */
	public HttpServer(final int port, final InetAddress address, final HttpHandler handler) throws IOException {
		if(address != null)
			server = new ServerSocket(port, 0, address);
		else
			server = new ServerSocket(port);
		this.handler = handler;
	}

	public String[] getAllowedMethods() {
		return allowedMethods;
	}

	/**
	 * Return HttpHandler that handles HTTP request for this server
	 * @return
	 */
	public HttpHandler getHandler() {
		return handler;
	}

	public String getName() {
		return name;
	}

	/**
	 * Get the ServerSocket
	 * @return
	 */
	public ServerSocket getServerSocket() {
		return server;
	}

	/**
	 * Thread pool for connections
	 * @return
	 */
	public ExecutorService getThreadPool() {
		return pool;
	}

	public void run() {
		while(true) {
			try {
				pool.execute(new HttpConnection(server.accept(), this));
			} catch (final IOException e) {
				e.printStackTrace();
			}
		}
	}

	public void setAllowedMethods(final String[] allowedMethods) {
		this.allowedMethods = allowedMethods;
	}

	public void setName(final String name) {
		this.name = name;
	}

	public void start() {
		pool.execute(this);
	}
}

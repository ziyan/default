package info.ziyan.net.httpserver;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.TimeUnit;

class HttpConnection implements Runnable{
	private class HttpProcessor implements Runnable {
		private final HttpRequest request;
		private final HttpResponse response;
		private final HttpConnection connection;
		public HttpProcessor(final HttpRequest request, final HttpResponse response, final HttpConnection connection) {
			this.request = request;
			this.response = response;
			this.connection = connection;
		}
		public void run() {
			try {
				if (HttpError.handle(request, response, connection)) return;
				final HttpHandler handler = connection.getServer().getHandler();
				if(handler != null)
					handler.handle(new HttpContext(request, response, connection.getServer(), connection.getSocket().getRemoteSocketAddress()));
				request.end();
				response.end();
			} catch(final Exception e) {
				e.printStackTrace();
			}
		}
	}
	private static class HttpResponder implements Runnable {
		private final HttpConnection connection;
		public HttpResponder(final HttpConnection connection) {
			this.connection = connection;
		}
		public void run() {
			try {
				while(connection.isOpen()) {
					HttpResponse response = null;
					while(response == null && connection.isOpen())
						response = connection.pipeline.poll(200L, TimeUnit.MILLISECONDS);
					if (response == HttpResponse.END || response == null) break;
					HttpResponse.write(response);
					if (!response.isPersistedConnection()) break;
				}
			} catch(final Exception e) { e.printStackTrace(); }
			connection.close();
		}
	}
	private final Socket socket;
	private final InputStream in;
	private final OutputStream out;

	private BlockingQueue<HttpResponse> pipeline;

	private final HttpServer server;

	public HttpConnection(final Socket socket, final HttpServer server) throws IOException {
		this.socket = socket;
		this.in = new BufferedInputStream(socket.getInputStream());
		this.out = new BufferedOutputStream(socket.getOutputStream());
		this.server = server;
	}

	public void close() {
		try {
			this.in.close();
			this.out.close();
			this.socket.close();
		} catch(final Exception e) { }
	}
	public InputStream getInputStream() {
		return in;
	}

	public OutputStream getOutputStream() {
		return out;
	}

	public HttpServer getServer() {
		return server;
	}

	public Socket getSocket() {
		return socket;
	}

	public boolean isOpen() {
		return !socket.isClosed();
	}

	public void run() {
		HttpRequest request;
		HttpResponse response;
		// get the first request
		try {
			request = HttpRequest.parse(in);
		} catch(final Exception e) { 
			e.printStackTrace();
			return;
		}
		if(request == null) return;
		response = HttpResponse.create(request, this);
		if (!request.isPersistedConnection()) {
			// single request connection
			new HttpProcessor(request, response, this).run();
			close();
		} else {
			// persisted connection
			try {
				pipeline = new LinkedBlockingQueue<HttpResponse>(); 
				pipeline.put(response);
				server.getThreadPool().execute(new HttpResponder(this));
				server.getThreadPool().execute(new HttpProcessor(request, response, this));

				while (isOpen()) {
					HttpRequest.wait(request);
					request = HttpRequest.parse(in);
					if(request == null) break;
					response = HttpResponse.create(request, this);
					server.getThreadPool().execute(new HttpProcessor(request, response, this));
					pipeline.put(response);
					if (!response.isPersistedConnection()) break;
				}
			} catch(final Exception e) {
				e.printStackTrace();
			}
			try {
				pipeline.put(HttpResponse.END);
			} catch (final Exception e) {
				e.printStackTrace();
			}
		}
	}

}

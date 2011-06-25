package info.ziyan.net.httpserver;

import java.net.SocketAddress;

public class HttpContext {
	private final HttpRequest request;
	private final HttpResponse response;
	private final HttpServer server;
	private final SocketAddress remote;
	HttpContext(final HttpRequest request, final HttpResponse response, final HttpServer server, final SocketAddress remote) {
		this.request = request;
		this.response = response;
		this.server = server;
		this.remote = remote;
	}
	public SocketAddress getRemoteSocketAddress() {
		return remote;
	}
	public HttpRequest getRequest() {
		return request;
	}
	public HttpResponse getResponse() {
		return response;
	}
	public HttpServer getServer() {
		return server;
	}
}

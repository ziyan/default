package info.ziyan.net.httpserver;

import java.io.IOException;
import java.io.OutputStream;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.HashMap;
import java.util.concurrent.Semaphore;

public class HttpResponse {
	public static class HttpResponseCode {
		public static final HttpResponseCode CONTINUE = new HttpResponseCode(100, "Continue");
		public static final HttpResponseCode SWITCHING_PROTOCOLS = new HttpResponseCode(101, "Switching Protocols");

		public static final HttpResponseCode OK = new HttpResponseCode(200, "OK");
		public static final HttpResponseCode CREATED = new HttpResponseCode(201, "Created");
		public static final HttpResponseCode ACCEPTED = new HttpResponseCode(202, "Accepted");
		public static final HttpResponseCode NON_AUTHORITATIVE_INFORMATION = new HttpResponseCode(203, "Non-Authoritative Information");
		public static final HttpResponseCode NO_CONTENT = new HttpResponseCode(204, "No Content");
		public static final HttpResponseCode RESET_CONTENT = new HttpResponseCode(205, "Reset Content");
		public static final HttpResponseCode PARTIAL_CONTENT = new HttpResponseCode(206, "Partial Content");

		public static final HttpResponseCode MULTIPLE_CHOICES = new HttpResponseCode(300, "Multiple Choices");
		public static final HttpResponseCode MOVED_PERMANENTLY = new HttpResponseCode(301, "Moved Permanently");
		public static final HttpResponseCode FOUND = new HttpResponseCode(302, "Found");
		public static final HttpResponseCode SEE_OTHER =  new HttpResponseCode(303, "See Other");
		public static final HttpResponseCode NOT_MODIFIED = new HttpResponseCode(304, "Not Modified");
		public static final HttpResponseCode USE_PROXY = new HttpResponseCode(305, "Use Proxy");
		public static final HttpResponseCode TEMPORARY_REDIRECT = new HttpResponseCode(307, "Temporary Redirect");

		public static final HttpResponseCode BAD_REQUEST = new HttpResponseCode(400, "Bad Request");
		public static final HttpResponseCode UNAUTHORIZED = new HttpResponseCode(401, "Unauthorized");
		public static final HttpResponseCode FORBIDDEN = new HttpResponseCode(403, "Forbidden");
		public static final HttpResponseCode NOT_FOUND = new HttpResponseCode(404, "Not Found");
		public static final HttpResponseCode METHOD_NOT_ALLOWED = new HttpResponseCode(405, "Method Not Allowed");
		public static final HttpResponseCode NOT_ACCEPTABLE = new HttpResponseCode(406, "Not Acceptable");
		public static final HttpResponseCode PROXY_AUTHENTICATION_REQUIRED = new HttpResponseCode(407, "Proxy Authentication Required");
		public static final HttpResponseCode REQUEST_TIMEOUT = new HttpResponseCode(408, "Request Timeout");
		public static final HttpResponseCode CONFLICT = new HttpResponseCode(409, "Conflict");
		public static final HttpResponseCode GONE = new HttpResponseCode(410, "Gone");
		public static final HttpResponseCode LENGTH_REQUIRED = new HttpResponseCode(411, "Length Required");
		public static final HttpResponseCode PRECONDITION_FAILED = new HttpResponseCode(412, "Precondition Failed");
		public static final HttpResponseCode REQUEST_ENTITY_TOO_LARGE = new HttpResponseCode(413, "Request Entity Too Large");
		public static final HttpResponseCode REQUEST_URI_TOO_LONG = new HttpResponseCode(414, "Request-URI Too Long");
		public static final HttpResponseCode UNSUPPORTED_MEDIA_TYPE = new HttpResponseCode(415, "Unsupported Media Type");
		public static final HttpResponseCode REQUESTED_RANGE_NOT_SATISFIABLE = new HttpResponseCode(416, "Requested Range Not Satisfiable");
		public static final HttpResponseCode EXPECTATION_FAILED = new HttpResponseCode(417, "Expectation Failed");

		public static final HttpResponseCode INTERNAL_SERVER_ERROR = new HttpResponseCode(500, "Internal Server Error");
		public static final HttpResponseCode NOT_IMPLEMENTED = new HttpResponseCode(501, "Not Implemented");
		public static final HttpResponseCode BAD_GATEWAY = new HttpResponseCode(502, "Bad Gateway");
		public static final HttpResponseCode SERVICE_UNAVAILABLE = new HttpResponseCode(503, "Service Unavailable");
		public static final HttpResponseCode GATEWAY_TIMEOUT = new HttpResponseCode(504, "Gateway Timeout");
		public static final HttpResponseCode HTTP_VERSION_NOT_SUPPORTED = new HttpResponseCode(505, "HTTP Version Not Supported");

		private final int code;
		private final String name;
		private HttpResponseCode(final int code, final String name) {
			this.code = code;
			this.name = name;
		}
		public int getCode() {
			return code;
		}
		public String getName() {
			return name;
		}
		@Override
		public String toString() {
			return "" + code + " " + name;
		}
	}
	private static class HttpResponseOutputStream extends OutputStream {
		private final OutputStream out;
		private long length;

		public HttpResponseOutputStream(final OutputStream out, final long length) {
			this.out = out;
			this.length = length;
		}

		@Override
		public void close() throws IOException {
			while(length > 0)
				write('\n');
		}

		@Override
		public void flush() throws IOException {
			out.flush();
		}

		@Override
		public void write(final int b) throws IOException {
			if(length <= 0) return;
			length--;
			out.write(b);
		}
	}
	static final HttpResponse END = new HttpResponse(null, null, false, null, false);
	static HttpResponse create(final HttpRequest request, final HttpConnection connection) {
		final HttpResponse response;
		if(request == HttpRequest.INVALID) {
			response = new HttpResponse("HTTP/1.1", HttpResponseCode.BAD_REQUEST, false, connection.getOutputStream(), false);
			response.fields.put("Connection", "close");
		} else {
			String protocol = "HTTP/1.1";
			if(request.getProtocol().equals("HTTP/1.0"))
				protocol = "HTTP/1.0";
			response = new HttpResponse(protocol,
					HttpResponseCode.INTERNAL_SERVER_ERROR,
					request.getMethod().equals("HEAD"),
					connection.getOutputStream(),
					request.isPersistedConnection());
			if (protocol.equals("HTTP/1.1") && request.getField("Connection") != null)
					response.fields.put("Connection", request.getField("Connection"));
		}
		response.fields.put("Server", connection.getServer().getName());
		return response;
	}
	static void write(final HttpResponse response) throws InterruptedException, IOException {
		if (response.contentLength < 0)
			response.responder.acquire();
		final StringBuffer buffer = new StringBuffer();
		buffer.append(response.protocol + " " + response.code.toString() + "\r\n");
		response.fields.put("Date", new SimpleDateFormat("EEE, d MMM yyyy HH:mm:ss Z").format(new Date()));
		response.fields.put("Content-Length", ""+response.contentLength);
		for(final String key : response.fields.keySet())
			buffer.append(key + ": " + response.fields.get(key) + "\r\n");
		buffer.append("\r\n");
		response.out.write(buffer.toString().getBytes());
		if(response.headOnly)
			response.body = new HttpResponseOutputStream(response.out, 0);
		else
			response.body = new HttpResponseOutputStream(response.out, response.contentLength);
		response.out = null;
		response.processor.release();
		if (response.persistedConnection && response.body!=null)
			response.responder.acquire();
	}
	private final String protocol;
	private HttpResponseCode code;
	private final HashMap<String, String> fields;
	private long contentLength;

	private OutputStream body, out;

	private final Semaphore processor, responder;
	private final boolean headOnly, persistedConnection;
	private HttpResponse(final String protocol, final HttpResponseCode code, final boolean headOnly, final OutputStream out, final boolean persistedConnection) {
		this.protocol = protocol;
		this.code = code;
		this.headOnly = headOnly;
		this.fields = new HashMap<String, String>();
		this.contentLength = -1;
		this.out = out;
		this.processor = new Semaphore(0);
		this.responder = new Semaphore(0);
		this.persistedConnection = persistedConnection;
	}
	public void begin(final long contentLength) throws InterruptedException, IOException {
		if (contentLength < 0) return;
		if (this.contentLength >= 0) return;
		this.contentLength = contentLength;
		responder.release();
		if (!persistedConnection)
			write(this);
	}
	public void end() throws IOException, InterruptedException {
		if (contentLength < 0) {
			begin(0);
		}
		if (out != null && body == null)
			processor.acquire();
		if (body != null) {
			body.flush();
			body.close();
			body = null;
			responder.release();
		}
	}
	public OutputStream getBody() throws InterruptedException {
		if (body != null) return body;
		if (contentLength < 0) return null;
		if (body == null) processor.acquire();
		return body;
	}
	public HttpResponseCode getCode() {
		return code;
	}

	public String getField(final String key) {
		return fields.get(key);
	}

	public HashMap<String, String> getFields() {
		return fields;
	}

	public String getProtocol() {
		return protocol;
	}

	public boolean isHeadOnly() {
		return headOnly;
	}

	public boolean isPersistedConnection() {
		return persistedConnection;
	}

	public void setCode(final HttpResponseCode code) {
		this.code = code;
	}

	public void setField(final String key, final String value) {
		fields.put(key, value);
	}
}

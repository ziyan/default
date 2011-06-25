package info.ziyan.net.httpserver;

import java.io.IOException;
import java.io.InputStream;
import java.util.HashMap;
import java.util.concurrent.Semaphore;

/**
 * HttpRequest
 * @author ziyan
 *
 */
public class HttpRequest {
	private static class HttpRequestInputStream extends InputStream {

		private final InputStream in;
		private long length;

		public HttpRequestInputStream(final InputStream in, final long length) {
			this.in = in;
			this.length = length;
		}

		@Override
		public int available() {
			return (int)length;
		}

		@Override
		public void close() throws IOException {
			// consume all bytes upon close
			while(available() > 0)
				if(read() < 0) break;
		}

		@Override
		public int read() throws IOException {
			if(length <= 0) return -1;
			length--;
			return in.read();
		}
	}
	static final HttpRequest INVALID = new HttpRequest(null, null, null, null, null);
	/**
	 * Parse a HttpRequest from an InputStream
	 * @param in
	 * @return
	 * @throws IOException
	 */
	static HttpRequest parse(final InputStream in) throws IOException {
		final String line = readLine(in);
		if (line==null) return null;
		final String[] requestLine = line.split(" ");
		if (requestLine.length!=3) return HttpRequest.INVALID;
		final String method = requestLine[0];
		final String path = requestLine[1];
		final String protocol = requestLine[2];
		final HashMap<String, String> fields = new HashMap<String, String>();
		while(true) {
			final String headerFieldLine = readLine(in);
			if (headerFieldLine.length()==0) break;
			if (headerFieldLine.indexOf(": ") < 0) return HttpRequest.INVALID;
			fields.put(headerFieldLine.substring(0, headerFieldLine.indexOf(": ")),
					headerFieldLine.substring(headerFieldLine.indexOf(": ")+2));			
		}
		InputStream body = null;
		if(fields.containsKey("Content-Length"))
			body = new HttpRequestInputStream(in, Long.parseLong(fields.get("Content-Length")));
		return new HttpRequest(method, path, protocol, fields, body);
	}
	private static String readLine(final InputStream in) throws IOException {
		final StringBuffer string = new StringBuffer();
		while(true) {
			final int read = in.read();
			if(read < 0 || read > 255)
				return null;
			final char c = (char)read;
			if(c=='\r') continue;
			if(c=='\n') break;
			string.append(c);
		}
		return string.toString();
	}
	static void wait(final HttpRequest request) throws InterruptedException {
		if (request.body != null)
			request.semaphore.acquire();
	}
	private final String path;
	private final String protocol;

	private final String method;
	private final HashMap<String, String> fields;
	private InputStream body;
	private final Semaphore semaphore;
	private HttpRequest(final String method, final String path, final String protocol, final HashMap<String, String> fields, final InputStream body) {
		this.method = method;
		this.path = path;
		this.protocol = protocol;
		this.fields = fields;
		this.body = body;
		this.semaphore = new Semaphore(0);
	}
	public void end() throws IOException {
		if (body != null) {
			body.close();
			body = null;
			semaphore.release();
		}
	}
	public InputStream getBody() {
		return body;
	}
	public	String getField(final String key) {
		return fields.get(key);
	}
	public HashMap<String, String> getFields() {
		return fields;
	}

	public String getMethod() {
		return method;
	}

	public String getPath() {
		return path;
	}

	public String getProtocol() {
		return protocol;
	}

	public boolean isPersistedConnection () {
		if (this == HttpRequest.INVALID) return false;
		if (!this.getProtocol().equals("HTTP/1.1")) return false;
		return !"close".equals(fields.get("Connection"));
	}
}

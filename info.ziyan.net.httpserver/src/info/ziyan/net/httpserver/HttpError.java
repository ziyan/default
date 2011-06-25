package info.ziyan.net.httpserver;

import info.ziyan.net.httpserver.HttpResponse.HttpResponseCode;

import java.io.IOException;


public class HttpError {
	static boolean handle(final HttpRequest request, final HttpResponse response, final HttpConnection connection) throws IOException, InterruptedException {
		// check for invalid header
		if(request == HttpRequest.INVALID) {
			request.end();
			sendBadRequest(response);
			return true;
		}
		// check for protocol version
		if(!request.getProtocol().equals("HTTP/1.1") && !request.getProtocol().equals("HTTP/1.0")) {
			request.end();
			sendHTTPVersionNotSupported(response);
			return true;
		}
		// check for host field
		if(request.getField("Host") == null) {
			request.end();
			sendBadRequest(response);
			return true;
		}
		// check for invalid method
		boolean isMethodAllowed = false;
		final String[] allowedMethods = connection.getServer().getAllowedMethods();
		for(int i=0;i<allowedMethods.length;i++)
			if(request.getMethod().equals(allowedMethods[i])) {
				isMethodAllowed = true;
				break;
			}
		if(!isMethodAllowed) {
			request.end();
			sendMethodNotAllowed(response);
			return true;
		}
		// check for POST stream
		if(request.getMethod().equals("POST") && request.getBody() == null) {
			request.end();
			sendLengthRequired(response);
			return true;
		}

		return false;
	}
	public static void sendBadRequest(final HttpResponse response) throws IOException, InterruptedException {
		response.setCode(HttpResponseCode.BAD_REQUEST);
		response.end();
	}
	public static void sendHTTPVersionNotSupported(final HttpResponse response) throws IOException, InterruptedException {
		response.setCode(HttpResponseCode.HTTP_VERSION_NOT_SUPPORTED);
		response.end();
	}
	public static void sendLengthRequired(final HttpResponse response) throws IOException, InterruptedException {
		response.setCode(HttpResponseCode.LENGTH_REQUIRED);
		response.end();
	}
	public static void sendMethodNotAllowed(final HttpResponse response) throws IOException, InterruptedException {
		response.setCode(HttpResponseCode.METHOD_NOT_ALLOWED);
		response.end();
	}
}

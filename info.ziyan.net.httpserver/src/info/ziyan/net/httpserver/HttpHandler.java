package info.ziyan.net.httpserver;

/**
 * Impelemnt this interface to handle HTTP request
 * @author ziyan
 *
 */
public interface HttpHandler {
	/**
	 * Implement this method to handle a single HTTP request
	 * You must call context.getResponse().begin(contentLength)
	 * before context.getResponse().getBody().write();
	 * And after you are done with the body output stream,
	 * you must call context.getResponse().end() to signal finish.
	 * 
	 * Note that this method is used by multiple thread simultaneously.
	 * @param context
	 */
	void handle(HttpContext context);
}

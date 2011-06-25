package info.ziyan.net.httpserver.example;

import info.ziyan.net.httpserver.HttpContext;
import info.ziyan.net.httpserver.HttpHandler;
import info.ziyan.net.httpserver.HttpServer;
import info.ziyan.net.httpserver.HttpResponse.HttpResponseCode;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.OutputStream;
import java.text.DateFormat;
import java.text.ParseException;
import java.util.Date;

/**
 * A HTTP server that hosts a local directory
 * allows content retrieval and content list
 * of directories.
 * 
 * Usage: java info.ziyan.net.httpserver.example.FileServer <port> <path>
 * @author ziyan
 *
 */
public class FileServer implements HttpHandler {

	/**
	 * A HTTP server that hosts a local directory
	 * allows content retrieval and content list
	 * of directories.
	 * 
	 * Usage: java info.ziyan.net.httpserver.example.FileServer <port> <path>
	 * @param args
	 * @throws IOException 
	 */
	public static void main(String[] args) throws IOException {
		if(args.length != 2)
		{
			usage();
			return;
		}
		HttpServer server = new HttpServer(Integer.parseInt(args[0]), new FileServer(args[1]));
		server.setAllowedMethods(new String[] { "GET", "HEAD" });
		server.setName("info.ziyan.net.httpserver.example.FileServer/1.0");
		server.start();
	}
	
	/**
	 * Print usage of the program
	 */
	public static void usage() {
		System.err.println("Usage: java info.ziyan.net.httpserver.example.FileServer <port> <path>");
		System.err.println();
	}
	
	private String path;
	public FileServer(String path) {
		this.path = path;
	}
	
	public void handle(HttpContext context) {
		String path = context.getRequest().getPath();
		if (path.indexOf('?') > -1)
			path = path.substring(0, path.indexOf('?'));
		File file = new File(this.path + path);
		if (!file.exists()) {
			context.getResponse().setCode(HttpResponseCode.NOT_FOUND);
			return;
		}
		
		String ifModifiedSince = context.getRequest().getField("If-Modified-Since");
		if(ifModifiedSince!=null) {
			try {
				if(DateFormat.getDateTimeInstance().parse(ifModifiedSince).compareTo(new Date(file.lastModified())) >= 0) {
					context.getResponse().setCode(HttpResponseCode.NOT_MODIFIED);
					return;
				}
			} catch (ParseException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
		
		if (file.isDirectory()) {
			if(!path.endsWith("/")) {
				context.getResponse().setCode(HttpResponseCode.MOVED_PERMANENTLY);
				context.getResponse().setField("Location", path + "/");
				return;
			}
			String content = "<html><head><title>"+path+"</title></head><body><ul><li><a href=\"../\">../</a></li>";
			for(File child : file.listFiles()) {
				content += "<li><a href=\""+
				path+
				child.getName()+
				(child.isDirectory()?"/":"")+
				"\">"+
				child.getName()+
				(child.isDirectory()?"/":"")+
				"</a></li>";
			}
			content +="</ul></body></html>";
			context.getResponse().setCode(HttpResponseCode.OK);
			context.getResponse().setField("Content-Type", "text/html");
			try {
				context.getResponse().begin(content.length());
				context.getResponse().getBody().write(content.getBytes());
				context.getResponse().end();
			} catch (Exception e) {
				e.printStackTrace();
			}
			return;
		} else {
			FileInputStream in;
			try {
				in = new FileInputStream(file);
			} catch (FileNotFoundException e) {
				e.printStackTrace();
				context.getResponse().setCode(HttpResponseCode.NOT_FOUND);
				return;
			}
			context.getResponse().setCode(HttpResponseCode.OK);
			context.getResponse().setField("Content-Type", "application/octet-stream");
			try {
				context.getResponse().begin(file.length());
			} catch (InterruptedException e) {
				e.printStackTrace();
				context.getResponse().setCode(HttpResponseCode.INTERNAL_SERVER_ERROR);
				return;
			} catch (IOException e) {
				e.printStackTrace();
				context.getResponse().setCode(HttpResponseCode.FORBIDDEN);
				return;
			}
			try {
				byte[] buffer = new byte[1024 * 100];
				int read;
				OutputStream out = context.getResponse().getBody();
				while(true) {
					read = in.read(buffer, 0, buffer.length);
					if(read<0) break;
					out.write(buffer, 0, read);
				}
				context.getResponse().end();
				return;
			} catch (Exception e) {
				e.printStackTrace();
				return;
			}
		}
	}

}

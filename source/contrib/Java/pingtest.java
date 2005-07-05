import com.graphics3d.g3d.*;
import java.net.*;
import java.nio.*;
import java.io.*;

class pingtest {

    static final String clientGreeting = "hello, server";
    static final String serverResponse = "hello, client";

    public static void main(String[] args) {
        reliableClient("localhost");
    }
 
    public static void reliableClient(String server) {

        InetSocketAddress serverAddress = new InetSocketAddress(server, 1201);

        //printf("  Trying to connect to %s\n", serverAddress.toString().c_str());

        ReliableConduit conduit = null;
        
        try {
            conduit = new ReliableConduit(serverAddress);
        } catch (Exception e) {
            System.out.printf("Couldn't create ReliableConduit -- %s\n", e.toString());
            System.exit(-1);
        }

        // Send a hello message
        System.out.printf("  Sending \"%s\"... ", clientGreeting); 
        try {
            conduit.send(1008, new PingMessage(clientGreeting));
        } catch (Exception e) {
            System.out.printf("Couldn't send PingMessage -- %s\n", e.toString());
            System.exit(-1);
        }
        System.out.printf("sent.\n");

        // Wait for server response
        System.out.printf("  Waiting for server response");
        
        try {
            while (conduit.messageWaiting() == false) {
                System.out.printf(".");
                try {
                    Thread.sleep(1);
                } catch (InterruptedException e) {
                    System.out.printf("Couldn't sleep\n");
                }
            }
        } catch (Exception e) {
            System.out.printf("Error waiting for response -- %s\n", e.toString());
            System.exit(-1);
        }
        
        System.out.printf("\ngot it.\n");

        PingMessage response = new PingMessage();
        try {
            assert conduit.waitingMessageType() == 1008;
            conduit.receive(response);
        } catch (Exception e) {
            System.out.printf("Couldn't receive PingMessage -- %s\n", e.toString());
            System.exit(-1);
        }
        System.out.printf("  Server responded: \"%s\".\n", response.text);
        
        System.out.printf("  Dropping connection.\n");
    } 
}    
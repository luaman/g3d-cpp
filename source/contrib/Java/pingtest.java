import com.graphics3d.g3d.*;
import java.net.*;
import java.nio.*;

class pingtest {

    static final String clientGreeting = "hello, server";
    static final String serverResponse = "hello, client";

    public static void main(String[] args) {
        reliableClient("localhost");
    }
 
    public static void reliableClient(String server) {

        InetSocketAddress serverAddress = new InetSocketAddress(server, 1201);

        //printf("  Trying to connect to %s\n", serverAddress.toString().c_str());

        ReliableConduit conduit = new ReliableConduit(serverAddress);

//        if (conduit.isNull() || ! conduit->ok()) {
//            printf("  Unable to connect.\n");
//            return;
//        }

//        printf("  Connected successfully.\n\n");

            try {
                Thread.sleep(50);
            } catch (InterruptedException e) {
                System.out.printf("Couldn't sleep\n");
            }

        // Send a hello message
        System.out.printf("  Sending \"%s\"... ", clientGreeting); 
        conduit.send(1008, new PingMessage(clientGreeting));
        System.out.printf("sent.\n");

        // Wait for server response
        System.out.printf("  Waiting for server response");
        while (conduit.messageWaiting() == false) {
            System.out.printf(".");
            try {
                Thread.sleep(1);
            } catch (InterruptedException e) {
                System.out.printf("Couldn't sleep\n");
            }
        }
        System.out.printf("got it.\n");

        assert conduit.waitingMessageType() == 1008;
        PingMessage response = new PingMessage();
        conduit.receive(response);
        System.out.printf("  Server responded: \"%s\".\n", response.text);
        //debugAssert(response.text == serverResponse);
        //debugAssert(conduit->ok());
        System.out.printf("  Dropping connection.\n");
    }

 
    void printHelp() {
        System.out.printf("pingtest {machine|--server}\n");
        System.out.printf("\n");
        System.out.printf("  machine   Host name of the pingtest server.\n");
        System.out.printf("  --server  Run a server on this machine.\n");
        System.out.printf("\n");
    }
 
    
}    
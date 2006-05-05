package Test;

import com.graphics3d.g3d.*;
import java.math.*;
import java.io.*;

public class TestReliableConduit {
    public static void test() {
        System.out.print("ReliableConduit ");

        short port = 11112;

        NetListener serverListener = new NetListener(port);

        ReliableConduit serverConduit = serverListener.waitForConnection();

        System.out.println("passed");
    }

}

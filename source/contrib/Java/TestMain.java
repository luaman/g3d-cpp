import com.graphics3d.g3d.*;
import java.math.*;
import java.io.*;

/**
 <PRE>
  javac -cp . Test.java
  java -classpath . -ea Test Test
</PRE>
 */
class TestMain {
    public static void main(String[] args) {
        boolean assertionsOn = false;
        assert assertionsOn = true;
        if (! assertionsOn) {
            System.out.println("Must run Test with 'java -ea Test'.");
            System.exit(-1);
        }

        Test.BinaryIO.test();
        System.out.println("All tests passed.");
    }
    
}

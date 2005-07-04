import com.graphics3d.g3d.*;
import java.math.*;

/**
 <PRE>
  javac Test.java
  java -ea Test Test
</PRE>
 */
class Test {
    public static void main(String[] args) {
        boolean assertionsOn = false;
        assert assertionsOn = true;
        if (! assertionsOn) {
            System.out.println("Must run Test with 'java -ea Test'.");
            System.exit(-1);
        }

        testBinaryOutput();
        testBinaryInput();
        System.out.println("All tests passed.");
    }

    public static void testBinaryOutput() {
        System.out.print("BinaryOutput...");
	
        BinaryOutput b = null;
        try {
            b = new BinaryOutput("bin.dat", java.nio.ByteOrder.LITTLE_ENDIAN);
        } catch (Exception e) {
            System.out.println("Test file not found.");
            System.exit(-1);
        }
    
        b.writeUInt8(200);
        b.writeInt8(-5);
        b.writeInt8(101);
        b.writeBool8(true);
        b.writeInt32(12345);
        b.writeInt32(-12345);
        b.writeUInt32(50000);
        b.writeUInt64(BigInteger.valueOf(1234567));
//        b.writeInt64(-1235467);
        b.writeFloat32(50000);
        b.writeFloat64(50000);

        b.commit();
    }

    public static void testBinaryInput() {
        System.out.print("BinaryInput...");
	
        BinaryInput b = null;
        try {
            b = new BinaryInput("bin.dat", java.nio.ByteOrder.LITTLE_ENDIAN);
        } catch (Exception e) {
            System.out.println("Test file not found.");
            System.exit(-1);
        }

        int x;

        assert b.readUInt8() == 200;
        assert b.readInt8() == -5;
        assert b.readInt8() == 101;
        assert b.readBool8() == true;
        
        assert b.readInt32() == 12345;
        assert b.readInt32() == -12345;
        assert b.readUInt32() == 50000;

        assert b.readUInt64().longValue() == 1234567;
//        assert b.readInt64() == -1234567;

        assert b.readFloat32() == 50000;
        assert b.readFloat64() == 50000;

        /*
    b.writeUInt8(200);
    b.writeInt8(-5);
    b.writeInt8(101);
    b.writeBool8(true);
    b.writeInt32(12345);
    b.writeInt32(-12345);
    b.writeUInt32(50000);
    b.writeFloat32(50000);
    b.writeFloat64(50000);
        */
 
        System.out.println("   OK");
    }
}

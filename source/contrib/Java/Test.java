import com.graphics3d.g3d.*;

class Test {
    public static void main(String[] args) {
	testBinaryInput();
	System.out.println("All tests passed.");
    }

    public static void testBinaryInput() {
	System.out.print("BinaryInput...");
	
	BinaryInput b;
	try {
	    b = new BinaryInput("test.dat");
	} catch (Exception e) {
	    System.out.println("Threw exception");
	    System.exit(-1);
	}

	System.out.println("   OK");
    }
}

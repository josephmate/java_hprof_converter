import java.util.*;

public class Main {
	public static void main(String [] args) throws InterruptedException {
		// hprof expected output to have at least 1000 entries
		HashMap<Integer, String> map = new HashMap<>();
		for(int i = 0; i < 1000; i++) {
			map.put(10, Integer.toString(i));
		}
		for(int i = 0; i < 6*100; i++) { // 100 minutes
			Thread.sleep(10000); // 10 seconds
		}
		// so optimizer cannot conclude to remove map
		System.out.println(map.get(2)); 
	}
}

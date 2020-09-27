import java.io.File;
import java.io.FileNotFoundException;
import java.util.Scanner;

enum Color {WHITE, BLACK, BLUE, GREY}

//this acts like producer
class ReadFile implements Runnable {
    String name;
    ReadFile(String name){ this.name = name; }

    @Override
    public void run() {
        try{
            File file = new File(name);
            Scanner scanner = new Scanner(file);
            System.out.println("Reading the input file ...");
            while (scanner.hasNextLine()){
                String data = scanner.nextLine();
                System.out.println("data: " + data);
            }
        }catch (FileNotFoundException e){
            System.out.println("File: " + this.name+ " does not exist");
        }
    }
}

//need to make multiple consumers

public class Question1 {
    public static void main(String[] args) {
        if(args.length > 0) {
            ReadFile readFile = new ReadFile(args[0]);
            Thread inputReader = new Thread(readFile, "Input Reader");
            inputReader.start();
        }
        else System.out.println("Please give the input file");
    }
}

//import java.io.File;
//import java.io.FileNotFoundException;
//import java.util.*;
//
//enum Color {WHITE, BLACK, BLUE, GREY, NONE}
//
//class ReadFile implements Runnable {                                        //this acts like the producer
//    private final String name;                                              //stores the input file name
//    private Color color = Color.NONE;                                       //stores the color last read
//    private static Boolean colorSet = false;                                //stores whether next color available for arm to pick or is it being read at the moment
//
//    ReadFile(String name){ this.name = name; }                              //constructor setting the input filename
//    public synchronized static Boolean getColorSet() {                      //synchronized getColorSet
//        return colorSet;
//    }
//    public synchronized static void setColorSet(Boolean colorSet) {         //synchronized setColorSet so that multiple robotic arms(consumer)/setColor(producer) don't read the same current color
//        ReadFile.colorSet = colorSet;
//    }
//    public synchronized Color getColor() {                                  //thread safe function to get the current color read
//        while (!colorSet){                                                  //wait till the color is available
//            try { Thread.onSpinWait(); }
//            catch (Exception e) { System.out.println("Set Color Exception : " + e.getMessage()); }
//        }
//        setColorSet(false);
//        return color;
//    }
//    public void setColor(Color color) {                                     //set the current Color (no need for synchronized, as only used in run of Readfile)
//        while (colorSet){                                                   //wait till the last color set is read by some robotic arm (or the color will be overwritten)
//            try { Thread.onSpinWait(); }
//            catch (Exception e) { System.out.println("Set Color Exception : " + e.getMessage()); }
//        }
//        setColorSet(true);
//        this.color = color;
//    }
//    public Color getColorFromString(String colorName){                      //function to convert string color read from input to enum for ease of use
//        return switch (colorName.toLowerCase()) {
//            case "blue" -> Color.BLUE;
//            case "black" -> Color.BLACK;
//            case "grey" -> Color.GREY;
//            case "white" -> Color.WHITE;
//            default -> Color.NONE;
//        };
//    }
//
//    @Override
//    public void run() {
//        try{
//            File file = new File(name);
//            Scanner scanner = new Scanner(file);
//            System.out.println("Reading the input file ...");
//            while (scanner.hasNextLine()){
//                String data = scanner.nextLine();
//                System.out.println("data: " + data);
//                setColor(getColorFromString(data));
////                System.out.println(getColor());
//                RoboticArm roboticArm = new RoboticArm(0,getColor());
//                roboticArm.start();
//            }
//        }catch (FileNotFoundException e){
//            System.out.println("File: " + this.name+ " does not exist");
//        }
//    }
//}
//
////need to make multiple consumers
//class RoboticArm extends Thread{
//    Color color;
//    Integer id;
//    static Set<Integer> armId = new HashSet<>(Arrays.asList(0,1,2,3,4,5,6,7));
//    RoboticArm(Integer id, Color color){
//        this.id = id;
//        this.color = color;
//    }
//
//    @Override
//    public void run() {
//        System.out.println("Robotic Arm " + this.id + " picking up " + this.color);
//    }
//}
//
//class Matcher{
//
//}
//
//public class Question1 {
//    public static void main(String[] args) {
//        if(args.length > 0) {
//            ReadFile readFile = new ReadFile(args[0]);
//            Thread inputReader = new Thread(readFile, "Input Reader");
//            inputReader.start();
//        }
//        else System.out.println("Please give the input file");
//    }
//}

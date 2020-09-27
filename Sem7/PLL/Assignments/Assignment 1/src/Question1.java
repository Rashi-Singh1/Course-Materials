import java.io.File;
import java.io.FileNotFoundException;
import java.util.*;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.atomic.AtomicInteger;

enum Color {WHITE, BLACK, BLUE, GREY, NONE}

public class Question1{
    public Integer MAX_ARM_COUNT = 6;
    private static String inputFile;
    private Integer armCount = MAX_ARM_COUNT;
    private final LinkedBlockingQueue<Color> queue = new LinkedBlockingQueue<>();

    Question1(){}
    Question1(Integer armCount){
        this.armCount = Math.min(armCount, MAX_ARM_COUNT);
    }

    public static void main(String[] args) {
        if(args.length > 0) {
            inputFile = args[0];
            Question1 question1;
            if(args.length > 1) question1 = new Question1(Integer.parseInt(args[1]));
            else question1 = new Question1();
            try{ question1.run(); }
            catch (Exception e) { System.out.println("Question1 run() exception : " + e.getMessage());}
        }
        else System.out.println("Please give the input file");
    }
    public void run() {
        ThreadState[] threadStates = new ThreadState[armCount+1];
        threadStates[0] = new ReadFile(inputFile, queue);
        for (int armId = 1; armId <= armCount; armId++) threadStates[armId] = new RoboticArm(armId - 1, queue);
        try { startThreads(threadStates); }
        catch (Exception e) { System.out.println("startThreads() exception : " + e.getMessage());}
        try { join(threadStates); }
        catch (Exception e) { System.out.println("startThreads() exception : " + e.getMessage());}
    }

    static abstract private class ThreadState implements Runnable {
        volatile boolean terminate = false;
        public boolean isAlive() {
            return Thread.currentThread().isAlive();
        }
        public void join(long millis) throws InterruptedException {
            Thread.currentThread().join(millis);
        }
    }

    private static void join(ThreadState[] threadStates) throws InterruptedException {
        List<ThreadState> maybeNotTerminated = Arrays.asList(threadStates);

        while (!maybeNotTerminated.isEmpty()) {
            for (int threadId = maybeNotTerminated.size() - 1; threadId >= 0; --threadId) {
                ThreadState t = maybeNotTerminated.get(threadId);
                if (t.isAlive()) {
                    t.join(20_000);
                    t.terminate = true;             // force termination
                } else {
                    maybeNotTerminated.remove(threadId);
                }
            }
        }
    }
    private static void startThreads(Runnable[] runnables) {
        for (Runnable runnable : runnables) new Thread(runnable).start();
    }

    static class ReadFile extends ThreadState implements Runnable{
        private final String name;
        private final Queue<Color> queue;
        ReadFile(String name, Queue<Color> queue){
            this.name = name;
            this.queue = queue;
        }
        public Color getColorFromString(String colorName){                      //function to convert string color read from input to enum for ease of use
            return switch (colorName.toLowerCase()) {
                case "blue" -> Color.BLUE;
                case "black" -> Color.BLACK;
                case "grey" -> Color.GREY;
                case "white" -> Color.WHITE;
                default -> Color.NONE;
            };
        }
        @Override
        public void run() {
            try{
                File file = new File(name);
                Scanner scanner = new Scanner(file);
                System.out.println("Reading the input file ...");
                while (scanner.hasNextLine()){
                    String data = scanner.nextLine();
                    System.out.println("data: " + data);
                    queue.offer(getColorFromString(data));
                    synchronized (queue) { queue.notifyAll(); }
                }
            }catch (FileNotFoundException e){
                System.out.println("File: " + this.name+ " does not exist");
            }
        }
    }
    static class RoboticArm extends ThreadState implements Runnable{
        private final Queue<Color> queue;
        private final Integer armId, queueIdx;
        private static Integer consumerCounter = 0;
        private static Set<Color> count;
        private static final AtomicInteger shouldRun = new AtomicInteger(0);

        RoboticArm(Integer id, Queue<Color> queue){
            this.queue = queue;
            this.armId = id;
            count = new HashSet<>();
            queueIdx = consumerCounter;
            consumerCounter++;
        }
        @Override
        public void run() {
            while (!terminate) {
                try {
                    do {
                        if(queue.isEmpty()) synchronized (queue) { queue.wait(100); }
                    } while(shouldRun.get() != queueIdx);
                    Color color = queue.poll();
                    if (color != null && color != Color.NONE) {
                        System.out.println("Robotic Arm " + armId.toString() + " picked sock with color " + color);
                        if(count.contains(color)) {
                            System.out.println(color + " socks pair matched");
                            count.remove(color);
                        }
                        else count.add(color);
                    }
                } catch (InterruptedException ie) {
                    Thread.currentThread().interrupt();
                } catch (Exception e) { System.out.println("Robotic arm : run() exception : " + e.getMessage()); }
                shouldRun.set((queueIdx+1)%consumerCounter);
            }
        }
    }
}
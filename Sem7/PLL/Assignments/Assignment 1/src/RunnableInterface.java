public class RunnableInterface {
    public static void main(String[] args) {
        Runnable hello2 = new Hello2();
        Runnable hi2 = new Hi2();

        Thread t1 = new Thread(hello2);
        Thread t2 = new Thread(hi2);

        t1.start();
        t2.start();
    }
}

class Hello2 implements Runnable{
    @Override
    public void run() {
        for (int i = 0; i < 5; i++) {
            System.out.println("Hello");
            try {
                Thread.sleep(1000);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }
}

class Hi2 implements Runnable{
    @Override
    public void run() {
        for (int i = 0; i < 5; i++) {
            System.out.println("Hi");
            try {
                Thread.sleep(1000);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }
}
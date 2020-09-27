public class Synchronised{
    static class Counter{
        int count;
        Counter(){
            count = 0;
        }

        //to make this function thread safe, i.e. max one thread can work on this function at a time
        public synchronized void incr(){
            count++;
        }
    }
    public static void main(String[] args) throws Exception {
        Counter counter = new Counter();
        Thread thread = new Thread(() -> {
            for(int i = 0 ; i < 5 ; i++) counter.incr();
        });

        Thread thread1 = new Thread(new Runnable() {
            @Override
            public void run() {
                for(int i = 0 ; i < 5 ; i++) counter.incr();
            }
        });

        thread.start();
        thread1.start();
        thread.join();
        thread1.join();

        System.out.println(counter.count);
    }
}

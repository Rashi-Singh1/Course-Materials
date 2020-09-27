import java.util.Arrays;
import java.util.List;
import java.util.Queue;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.atomic.AtomicInteger;

public class ProducerConsumer {

    //done
    public static void main(String[] args) throws InterruptedException {
        ProducerConsumer cp = new ProducerConsumer();
        cp.run();
    }

    //done
    // Creates Threads and waits for them to finish till timeout
    private LinkedBlockingQueue<Elem> queue = new LinkedBlockingQueue<>();

    //done
    public void run() throws InterruptedException {
        Terminatable[] terminateables = new Terminatable[] {
                new Producer(queue), new Consumer(queue),
                new Consumer(queue), new Consumer(queue), };

        startThreads(terminateables);
        join(terminateables);
    }

    //done
    private static void join(Terminatable[] terminatables)
            throws InterruptedException {
        List<Terminatable> maybeNotTerminated = Arrays
                .asList(terminatables);

        while (!maybeNotTerminated.isEmpty()) {
            for (int idx = maybeNotTerminated.size() - 1; idx >= 0; --idx) {
                Terminatable t = maybeNotTerminated.get(idx);
                if (t.isAlive()) {
                    t.join(20_000);
                    // force termination
                    t.terminate = true;
                } else {
                    maybeNotTerminated.remove(idx);
                }
            }
        }
    }

    //done
    private static void startThreads(Runnable[] runnables) {
        for (int i = 0; i < runnables.length; i++) {
            new Thread(runnables[i]).start();
        }
    }

    //done
    static abstract private class Terminatable implements Runnable {
        volatile boolean terminate = false;

        public boolean isAlive() {
            return Thread.currentThread().isAlive();
        }

        public void join(long millis) throws InterruptedException {
            Thread.currentThread().join(millis);
        }
    }

    //done
    static private class Elem {
        public final int idx;
        public final String name;

        public Elem(int i, String name) {
            this.idx = i;
            this.name = name;
        }
    }

    //done
    // Producer class
    static private class Producer extends Terminatable implements Runnable {

        private Queue<Elem> queue;

        Producer(Queue<Elem> queue) {
            this.queue = queue;
        }

        @Override
        public void run() {
            for (int i = 0; i <= 100; ++i) {
                Elem e = new Elem(i, Thread.currentThread().getName());
                queue.offer(e);

                synchronized (queue) {
                    queue.notifyAll();
                }

            }
        }
    }

    // Consumer Class
    static private class Consumer extends Terminatable implements Runnable {

        private Queue<Elem> queue;
        private final int myIdx;
        private static int consumerCounter = 0;
        private static AtomicInteger shouldRun = new AtomicInteger(0);

        public Consumer(Queue<Elem> queue) {
            this.queue = queue;
            myIdx = consumerCounter;
            consumerCounter++;
        }


        @Override
        // very long with two nested loops
        // should be refactored.
        public void run() {
            while (!terminate) {
                for (int idx = 0; idx < 10; ++idx) {
                    try {
                        // wait until elements are produced
                        do {
                            if (queue.isEmpty()) {
                                synchronized (queue) {
                                    queue.wait(100);
                                }
                            }
                        } while (shouldRun.get() != myIdx);
                        // continue only if this consumer should execute
                        Elem elem = queue.poll();
                        if (elem != null) {
                            System.out.println("Thread "
                                    + Thread.currentThread().getName()
                                    + " fetch Element " + elem.idx);
                        }
                    } catch (InterruptedException ie) {
                        Thread.currentThread().interrupt();
                    }

                }

                // allow the next consumer to run
                shouldRun.set((myIdx + 1) % consumerCounter);
            }

        }

    }
}
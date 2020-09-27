public class Lambda {
    public static void main(String[] args) {
//        way1();
        way2();                             //running both at the same would give mixed result
    }

    //this contains join and isAlive methods
    static void way1(){                     //needed to make this static to be able to run this in main, which is static
        Runnable hello = () -> {
            for (int i = 0; i < 5; i++) {
                System.out.println("Hello");
                try{ Thread.sleep(1000);} catch(Exception e) { System.out.println("Error: " + e.getMessage());}
            }
        };

        Runnable hi = () -> {
            for (int i = 0; i < 5; i++) {
                System.out.println("Hi");
                try{ Thread.sleep(1000);} catch(Exception e) { System.out.println("Error: " + e.getMessage());}
            }
        };

        Thread t1 = new Thread(hello);
        Thread t2 = new Thread(hi);

        t1.start();
        try{ Thread.sleep(10);} catch(Exception e) { System.out.println("Error: " + e.getMessage());}
        t2.start();

        //this means that the main thread should here for t1 and t2 thread to join (i.e. finish their work)
        try{ t1.join();} catch(Exception e) { System.out.println("Error: " + e.getMessage());}

        //at this point t1 would def give false
        //however, we cannot say about t2, as the println might take place after the join completes for t2
        System.out.println("1 - t1 : " + t1.isAlive());
        System.out.println("1 - t2 : " + t2.isAlive());

        try{ t2.join();} catch(Exception e) { System.out.println("Error: " + e.getMessage());}

        //both will def give false
        System.out.println("2 - t1 : " + t1.isAlive());
        System.out.println("2 - t2 : " + t2.isAlive());

        System.out.println("Bye");
    }

    //this contains thread priority methods
    static void way2(){
        Thread t1 = new Thread(() -> {
            for (int i = 0; i < 5; i++) {
                System.out.println("Hello Way 2");
                try{ Thread.sleep(1000);} catch(Exception e) { System.out.println("Error: " + e.getMessage());}
            }
        }, "Hello");

        Thread t2 = new Thread(() -> {
            for (int i = 0; i < 5; i++) {
                System.out.println("Hi Way 2");
                try{ Thread.sleep(1000);} catch(Exception e) { System.out.println("Error: " + e.getMessage());}
            }
        });

        t2.setName("Hi");

        t1.setPriority(1);                      //1 is the highest priority
        t2.setPriority(Thread.MIN_PRIORITY);


        System.out.println("getName : " + t1.getName());
        System.out.println("getPriority : " + t2.getPriority());

        System.out.println("currentThread : " + Thread.currentThread().getName());

        t1.start();
        try{ Thread.sleep(10);} catch(Exception e) { System.out.println("Error: " + e.getMessage());}
        t2.start();
    }
}

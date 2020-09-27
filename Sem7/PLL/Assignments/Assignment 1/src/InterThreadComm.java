//here we are striving for one read then one write and so on (alternative behaviour)
class Q {
    int num;
    boolean valueSet = false;                       //this will act as lock since only one producer and consumer
    public synchronized void getNum() {             //need to add synchronized, ow gives error that current thread is not owner
        while(!valueSet){                           //if valueSet false, it means producer not done setting the value
            try { wait(); }                         //making this thread sleep/wait till condition satisfied
//            try { Thread.onSpinWait(); }
            catch(Exception e) { System.out.println(e.getMessage()); }
        }
        System.out.println("Get : " + num);
        valueSet = false;
        notify();                                   //wake up any one of the other sleeping threads
    }

    //without adding wait and notify and synchronized, (i.e. only keeping onSpinWait), still the same output...check out which is more efficient though
    public synchronized void setNum(int num) {
        while(valueSet) {                           //if valueSet true, it means consumer not done reading the value
//            try { Thread.onSpinWait(); }
            try { wait(); }
            catch(Exception e) { System.out.println(e.getMessage()); }
        }
        System.out.println("Set : " + num);
        this.num = num;
        valueSet = true;                            //this specifies value has been set
        notify();
    }
}

class Producer implements Runnable{
    Q q;

    public Producer(Q q) {
        this.q = q;
        Thread thread = new Thread(this, "Producer");
        thread.start();
    }

    //say producer is very fast..this method still works
    @Override
    public void run() {
        int i = 0;
        while (true){
            q.setNum(i++);
            try { Thread.sleep(10); } catch (Exception e) { System.out.println(e.getMessage());}
        }
    }
}

class Consumer implements Runnable{
    Q q;

    public Consumer(Q q) {
        this.q = q;
        Thread thread = new Thread(this, "Consumer");
        thread.start();
    }

    @Override
    public void run() {
        while (true){
            q.getNum();
            try { Thread.sleep(1000); } catch (Exception e) { System.out.println(e.getMessage());}
        }
    }
}

public class InterThreadComm {
    public static void main(String[] args) {
        Q q = new Q();
        Producer producer = new Producer(q);
        Consumer consumer = new Consumer(q);
    }
}

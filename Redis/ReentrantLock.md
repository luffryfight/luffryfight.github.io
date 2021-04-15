# ReentrantLock 

分析需求：一个锁被重复拿到？一个线程有多个任务需要去完成，两个任务都需要锁，为了保证两个任务都完成，所以需要可重入

[ReentrantLock流程图](https://www.processon.com/view/link/5fa53787e401fd45d10699e7)

## 内部类：

```java
abstract static class Sync extends AbstractQueuedSynchronizer
   | 
   |
static final class NonfairSync extends Sync  非公平锁
   | 
   |
static final class FairSync extends Sync  公平锁
```

## 构造函数：

默认创建非公平锁，也可以通过传参来确定，如果传入true就会创建公平锁

反之，则创建非公平锁。

```java
public ReentrantLock() {
    sync = new NonfairSync();
}

public ReentrantLock(boolean fair) {
    sync = fair ? new FairSync() : new NonfairSync();
}
```

## 

## **ReentrantLock.lock **

```
public void lock() {
    sync.lock();//具体看初始化时是哪种锁
}
```

## **NonfairSync.lock 方法**

1.  非公平锁和公平锁最大的区别在于，在非公平锁中我抢占锁的逻辑是，不管有没有线程排队，我先上来 cas 去抢占一下

CAS 成功，就表示成功获得了锁

CAS 失败，调用 acquire(1)走正常的锁竞争逻辑

```java
final void lock() {
    if (compareAndSetState(0, 1)) //CAS setExclusiveOwnerThread(Thread.currentThread();
    else
      acquire(1);
}
```

==CAS==：原子层面上实现功能

如果抢占锁失败：就执行AQS的 acquire 方法：

## AQS.acquire 

这个方法的主要逻辑是：

1.  通过tryAcquire尝试独占锁，如果成功就返回true，失败就返回false
2. 如果tryAcquire失败，就会通过addWaiter方法将当前线程封装为Node(后面补充Node的源码)添加到同步队列AQS的尾部
3. acquireQueued会将addWaiter返回的Node加入队尾(后面补充原理)

```java
public final void acquire(int arg) {
    if (!tryAcquire(arg) //Nonfair类tryAcquire
 &&acquireQueued(addWaiter(Node.EXCLUSIVE), arg))
        selfInterrupt();
}
```

## NonfairSync.tryAcquire 

再调用nonfairTryAcquire () 方法：获取成功就返回true，失败就返回false

`它是重写 AQS 类中的 tryAcquire 方法，并且大家仔细看一下 AQS中的tryAcquire方法的定义，并没有实现，而是抛出异常。按照一般的思维模式，既然是一个不实现的模版方法，那应该定义成 abstract，让子类来实现呀？大家想想为什么`

```java
protected final boolean tryAcquire(int acquires) {
    return nonfairTryAcquire(acquires);//非公平的抢占锁
}	
```

**AQS.tryAcquire**

```java
protected boolean tryAcquire(int arg) {
        throw new UnsupportedOperationException();
}
```



## **Sync.nonfairTryAcquire**

通过非公平的方法尝试获取锁，获取成功就返回true，失败就返回false

- 获取当前线程，判断当前线程的状态
- 如果state==0 表示当前表示当前线程是无锁状态，CAS抢占，然后更新state的状态。
- 如果是重入锁，则增加重入次数。

```java
final boolean nonfairTryAcquire(int acquires) {//1
    //获取当前的线程
	final Thread current = Thread.currentThread();
 	int c = getState();//获取当前线程的state值
 	if (c == 0) {//表示是无锁状态
    //通过CAS替换state的值，如果替换成功就代表抢锁成功
 	if (compareAndSetState(0, acquires)) {
    //设置当前线程为独占锁的线程，避免下次再次尝试竞争锁
		setExclusiveOwnerThread(current);
        return true;
 		}
 }
    //是一个线程来获取锁，则增加重入次数
 	else if (current == getExclusiveOwnerThread()) {
 		int nextc = c + acquires;
 		if (nextc < 0) // overflow
 			throw new Error("Maximum lock count exceeded");
 		setState(nextc);
 		return true;
 	}
    return false; 
}
```

再回到AQS.acquire 方法，获取锁失败后执行AQS.addWaiter()方法。

## AQS.addWaiter()

将当前线程封装成Node，并入参mode(表示当前节点的状态)-->Node.EXCLUSIVE-->表示独占状态。

- 判断链表的尾节点是否为空，，如果不为空，就通过CAS把当前线程的节点添加到队尾,然后返回当前节点
- 如果尾节点为空或者CAS失败，则调用enq()方法把该节点添加到AQS队列



```java
private Node addWaiter(Node mode) {
        Node node = new Node(Thread.currentThread(), mode);
        // Try the fast path of enq; backup to full enq on failure
    	//tail是AQS种同步队列的队尾，默认情况下是Null
        Node pred = tail;
        if (pred != null) {//不为空，说明队列中存在节点
            node.prev = pred;
            //通过CAS把节点设置为尾节点
            if (compareAndSetTail(pred, node)) {
                pred.next = node;//CAS成功则双向连接好
                return node;
            }
        }
        enq(node);//调用这个方法把Node加入队列
        return node;
    }
```



## AQS.enq()

通过自旋操作把当前节点加入到队列中。(至于为什么叫自旋，可能大家都这么叫)。

```java
private Node enq(final Node node) {
        for (;;) {//至少会循环两次
            Node t = tail;
            if (t == null) { // Must initialize
                if (compareAndSetHead(new Node()))
                    tail = head;
            } else {
                node.prev = t;
                if (compareAndSetTail(t, node)) {
                    t.next = node;
                    return t;
                }
            }
        }
    }
```

然后再过头看AQS.acquire 方法，addWaiter()调用结束后，把新加入的节点当作参数，传给AQS.acquireQueued(),其中arg==1

## AQS.acquireQueued()

通过addWaiter()方法把线程添加到链表后，然后会把返回的Node作为参数传递给acquireQueued()方法再次去竞争锁。这个函数还要决定是否要调用中断selfInterrupt();

- 先获取当前节点的前节点prev

- 如果prev节点为head节点，那么他有资格去竞争锁，通过tryAcquire()去竞争锁。

- 如果抢占成功就把当前节点设置为头节点，并且就移除原来初始化时创建的null节点head。

- 如果再次竞争锁失败，则根据waitStatus决定是否要挂起线程

  > (这里需要再次竞争的原因是因为第二个线程想调用锁时锁还被占用着(还没来得及释放)，所以为了成功调用锁，所以再尝试一次)

- finally会通过cancelAcquire取消尝试获取锁的操作

```java
final boolean acquireQueued(final Node node, int arg) {
        boolean failed = true;
        try {
            boolean interrupted = false;
            for (;;) {//注意这里是死循环
                final Node p = node.predecessor();
                if (p == head && tryAcquire(arg)) {
                    setHead(node);
                    p.next = null; // help GC
                    failed = false;
                    return interrupted;
                }
                //ThreadA 可能还没释放锁，使得 ThreadB 在执
//行 tryAcquire 时会返回 false
                if (shouldParkAfterFailedAcquire(p, node) &&parkAndCheckInterrupt())
                    //判断当前线程再等待过程中是否被中断过
                    interrupted = true;
            }
        } finally {
            if (failed)
                cancelAcquire(node);
        }
    }
```

这个方法中当 再次尝试失败后会调用两个函数shouldParkAfterFailedAcquire()和parkAndCheckInterrupt(),

## AQS.shouldParkAfterFailedAcquire()

看名字就是，失败后判断应不应该park

- 判断，如果当前节点的前节点pred是SIGNAL，就表示可以挂起线程
- 

```java
private static boolean shouldParkAfterFailedAcquire(Node pred, Node node) {
        int ws = pred.waitStatus;
        if (ws == Node.SIGNAL)
            //如果前节点为SIGNL，表示只需要其他前节点的线程被释放
            return true;
        if (ws > 0) {//只有CANCLE>0
            //表示前节点取消了排队，可以直接移除这个节点，并移除前面所有ws>0的节点
            do {
                node.prev = pred = pred.prev;
            } while (pred.waitStatus > 0);
            pred.next = node;
        } else {//通过CAS把前节点设置为SIGNAL
            compareAndSetWaitStatus(pred, ws, Node.SIGNAL);
        }
        return false;
    }
```

这个方法返回true后会执行parkAndCheckInterrupt

## AQS.parkAndCheckInterrupt()

说明：使用挂起当前线程，并设置为waiting状态。

Thread.interrupted(): 判断是否有其他线程发起过中断请求，也就是threadOther.interrupt();如果曾触发过，就返回true，然后会在AQS.acquire 方法执行      selfInterrupt();

```java
private final boolean parkAndCheckInterrupt() {
    LockSupport.park(this);
    return Thread.interrupted();
}
```

 AQS.selfInterrupt();

```java
static void selfInterrupt() {
    Thread.currentThread().interrupt();
}
```



## **NonfairSync.lock** 方法

非公平锁在获取锁的时候，会先通过 CAS 进行抢占，而公平锁则不会

```
final void lock() {
 	acquire(1);
}
```

## **FairSync.tryAcquire** 

```java
protected final boolean tryAcquire(int acquires) {
        final Thread current = Thread.currentThread();
        int c = getState();
        if (c == 0) {
            //和非公平锁唯一不同
            if (!hasQueuedPredecessors() &&
                compareAndSetState(0, acquires)) {
                setExclusiveOwnerThread(current);
                return true;
            }
        }
        else if (current == getExclusiveOwnerThread()) {
            int nextc = c + acquires;
            if (nextc < 0)
                throw new Error("Maximum lock count exceeded");
            setState(nextc);
            return true;
        }
        return false;
    }
}
```

不同点：hasQueuedPredecessors(),判断同步队列中是否有前节点，如果返回true，则表示有线程更早地请求了锁，因此需要等待。

## AQS.hasQueuedPredecessors            

返回true表示需要排队，false表示不需要排队。如果返回false就在acquire中取反，继续执行CAS.大概有三种情况

- 同步队列还没被初始化，头尾节点都为空，返回false，不排队
- 队列中只有一个节点，且thread为空，返回true,要排队
- 第三种情况是等待队列中有多个节点，如果队首节点的 thread 域指向当前线程，返回 true；其他情况返回 false。

```java
public final boolean hasQueuedPredecessors() {
    // The correctness of this depends on head being initialized
    // before tail and on head.next being accurate if the current
    // thread is first in queue.
    Node t = tail; // Read fields in reverse initialization order
    Node h = head;
    Node s;
    return h != t &&
        ((s = h.next) == null || s.thread != Thread.currentThread());
}
```

## 重要知识点补充：

**ReentrantLock 类的 lockInterruptibly 方法**

Sync 类是 AQS 的子类，这里 Sync 的实例调用的是 AQS.acquireInterruptibly 方法。

```java
public void lockInterruptibly() throws 					InterruptedException {
    sync.acquireInterruptibly(1);
}
```

先检查当前线程是否被中断，是则抛出异常，不是则调用 tryAcquire 方法尝试获取锁，成功则返回，不成功则调用 doAcquireInterruptibly 方法将当前线程放入等待队列，doAcquireInterruptibly 方法和 acquireQueued 方法相似。

```JAVA
public final void acquireInterruptibly(int arg)
        throws InterruptedException {
    if (Thread.interrupted())
        throw new InterruptedException();
    if (!tryAcquire(arg))
        doAcquireInterruptibly(arg);
}
```

对照 acquireQueued 方法来看，最大的区别就是 shouldParkAfterFailedAcquire 方法和 parkAndCheckInterrupt 方法返回 true 时，doAcquireInterruptibly 方法会抛出中断异常，而 acquireQueued 方法会将局部变量 interrupted 设置为 true。

```JAVA
private void doAcquireInterruptibly(int arg)
    throws InterruptedException {
    final Node node = addWaiter(Node.EXCLUSIVE);
    boolean failed = true;
    try {
        for (;;) {
            final Node p = node.predecessor();
            if (p == head && tryAcquire(arg)) {
                setHead(node);
                p.next = null; // help GC
                failed = false;
                return;
            }
            if (shouldParkAfterFailedAcquire(p, node) &&
                parkAndCheckInterrupt())
                throw new InterruptedException();
        }
    } finally {
        if (failed)
            cancelAcquire(node);
    }
}
```
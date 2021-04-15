

# Condition

主要方法：

**await（）**:把当前线程阻塞挂起

**signal（）**:唤醒阻塞的线程

## await()

```java
void await() throws InterruptedException;
```

## signal()

void signalAll();类似地唤醒全部线程

```java
void signal();
```



在AQS中的实现类为：

```java
public class ConditionObject implements Condition, java.io.Serializable
```

# ConditionObject

![流程图](C:\Users\86137\AppData\Roaming\Typora\typora-user-images\image-20201114222832246.png)

这个类中也是使用链表来保存线程节点，但是链表是单向表，有一个头尾指针，专业术语叫同步队列。其中只要有两个方法，等待和唤醒。运用情况见下图：

## 变量：

意义看下面的英文说明，很详细了。

```java
/** Mode meaning to reinterrupt on exit from wait */
private static final int REINTERRUPT =  1;
/** Mode meaning to throw InterruptedException on exit from wait */
private static final int THROW_IE    = -1;

-------------
    /** First node of condition queue. */
        private transient Node firstWaiter;
        /** Last node of condition queue. */
        private transient Node lastWaiter;
```





## await()

和AQS很类似，通过几个递进的方法来实现。需要特别注意的是下面有标注的几个方法，后面也会提出来。

await---->allowed_wait  允许被中断的等待方法，同时说明当前线程会被加入到等待队列！

```java
public final void await() throws InterruptedException {
    if (Thread.interrupted())//表示这个方法允许被中断
        throw new InterruptedException();
    Node node = ①addConditionWaiter();//在等待队列中创建一个新的节点，并设置状态为condition
    int savedState = ②fullyRelease(node);//完全释放当前的锁(因为要让给其他线程),并返回其状态，然后唤醒AQS同步队列中的下一个线程
    int interruptMode = 0;
 	//如果没在AQS中，则该节点线程没有被signal唤醒，会一直阻塞在等待队列
    //==然后回一直循环等待被唤醒，没有被中断过就返回0，被中断过就break
    while (③!isOnSyncQueue(node)) {//判断是否在AQS中，第一次判断一定是false，因为上面已经释放了锁   ---------其实就是判断有没有被唤醒,在AQS队列中就是被唤醒了，没有就挂起该线程this
        LockSupport.park(this);
        if ((interruptMode = ④checkInterruptWhileWaiting(node)) != 0)
            break;
    }//结束时说明该节点已经在AQS同步队列中，然后会尝试拿锁，或者说明要中断
    //当 acquireQueued 返回 false 就是拿到锁了
    if (acquireQueued(node, savedState) && interruptMode != THROW_IE)//执行还有可能是：在AQS中但是没有获得到锁，并且interruptMode != THROW_IE//应该是0
        interruptMode = REINTERRUPT;
    //做一些清理工作：如果 node 的下一个等待者不是 null, 则进行清理,清理 Condition 队列上的节点.  如果是 null ,就没有什么好清理的了.
    if (node.nextWaiter != null) // clean up if cancelled
        unlinkCancelledWaiters();
    //根据中断信息做出响应
    if (interruptMode != 0)
        reportInterruptAfterWait(interruptMode);
}
```

### addConditionWaiter()

这个方法的作用就是把当前线程封装成一个节点，然后添加到等待队列上。其实也就是尾插法连接节点，看看就能懂！！

​	------再说一下：这里的等待队列是普通的单向链表！

```java
private Node addConditionWaiter() {
    Node t = lastWaiter;
    // If lastWaiter is cancelled, clean out.
    if (t != null && t.waitStatus != Node.CONDITION) {
        unlinkCancelledWaiters();
        t = lastWaiter;
    }
    Node node = new Node(Thread.currentThread(), Node.CONDITION);//封装成节点
    if (t == null)//尾插法
        firstWaiter = node;
    else
        t.nextWaiter = node;
    lastWaiter = node;
    return node;
}
```

创建完成之后如图：

![等待队列](C:\Users\86137\AppData\Roaming\Typora\typora-user-images\image-20201114211251709.png)

### AQS:fullyRelease()

彻底释放当前节点获得的锁，彻底的意思就是：如果该节点获得的锁有多次重入，那么调用一次该方法就可以把重入次数归零。

```java
final int fullyRelease(Node node) {
    boolean failed = true;
    try {
        int savedState = getState();//重入次数
        if (release(savedState)) {//调用后就释放了锁，并会唤醒AQS的下一个线程
            failed = false;
            return savedState;
        } else {
            throw new IllegalMonitorStateException();
        }
    } finally {
        if (failed)
            node.waitStatus = Node.CANCELLED;
    }
}
```

### AQS:isOnSyncQueue()

判断当前节点是否在AQS同步队列中，如果在就返回true，不在就返回false。

如果在，说明该节点没有被唤醒，所以需要把该节点阻塞起来，直到被唤醒。

前两个if的判断原理：

1. node.waitStatus == Node.CONDITION说明在等待队列中，AQS 队列的状态没有 CONDITION。
2. node.prev == null：在AQS中只有一种可能，就是node==head,同时也意味着node就是获得锁的线程，实际node已经释放了锁，所以node不在AQS中
3. If has successor, it must be on queue。只有AQS中next和prev的关系。

```java
final boolean isOnSyncQueue(Node node) {
    if (node.waitStatus == Node.CONDITION || node.prev == null)
        return false;
    if (node.next != null) // If has successor, it must be on queue
        return true;
    /*
     * node.prev can be non-null, but not yet on queue because
     * the CAS to place it on queue can fail. So we have to
     * traverse from tail to make sure it actually made it.  It
     * will always be near the tail in calls to this method, and
     * unless the CAS failed (which is unlikely), it will be
     * there, so we hardly ever traverse much.
     */
    return findNodeFromTail(node);//从尾部向前找
}
```



被阻塞的线程被唤醒的过程

### checkInterruptWhileWaiting()

顾名思义：就是 Thread在 condition 队列被阻塞的过程中，有没有被其他线程触发过中断请求



```java
private int checkInterruptWhileWaiting(Node node) {
    return Thread.interrupted() ?//如果被中断过，则调用transferAfterCancelledWait
        (transferAfterCancelledWait(node) ? THROW_IE : REINTERRUPT) :
        0;
}
```

#### transferAfterCancelledWait()

/*该方法的返回值代表当前线程是否在 park 的时候被中断唤醒，如果为 true 表示中断在 signal 调用之前，signal 还未执行，那么这个时候会根据 await 的语义，在 await 时遇到中断需要抛出interruptedException，返回 true 就是告诉checkInterruptWhileWaiting 返回 THROW_IE(-1)。如果返回 false，否则表示 signal 已经执行过了，只需要重新响应中断即可*/

```java
final boolean transferAfterCancelledWait(Node node) {//被中断过才能进入这个方法！
    //如果CAS成功，说明node的状态不是0，也就是不在AQS队列中--说明线程被中断时，signal还没被调用
    if (compareAndSetWaitStatus(node, Node.CONDITION, 0)) {
        enq(node);//然后调用enq把node加入AQS
        return true;
    }
    /*
     * If we lost out to a signal(), then we can't proceed
     * until it finishes its enq().  Cancelling during an
     * incomplete transfer is both rare and transient, so just
     * spin.
     */
    //如果第一次 CAS 失败了，则不能判断当前线程是先进行了中断还是先进行了 signal 方法的调用，可能是先执行了 signal 然后中断，也可能是先执行了中断，后执行了 signal。
    while (!isOnSyncQueue(node))//没有CAS成功，说明在AQS中，就一直让其他线程执行
        Thread.yield();
    return false;
}
```

补充：也可能是调用了线程的 interrupt()方法，这个方法会更新一个中断标识，并且会唤醒处于阻塞状态下的线程

总流程



## signal()

```java
public final void signal() {
    if (!isHeldExclusively())//当前线程没有获得锁，还想唤醒就报错
        throw new IllegalMonitorStateException();
    Node first = firstWaiter;
    if (first != null)
        doSignal(first);
}
```

### AQS:isHeldExclusively()

```
protected boolean isHeldExclusively() {
    throw new UnsupportedOperationException();
}
```

```java
protected final boolean isHeldExclusively() {
    // While we must in general read state before owner,
    // we don't need to do so to check if current thread is owner
    return getExclusiveOwnerThread() == Thread.currentThread();
}
```

### doSignal()

```java
private void doSignal(Node first) {
    do {//从等待队列中依次移除头部节点,移除时判断能不能把该节点唤醒到AQS
        if ( (firstWaiter = first.nextWaiter) == null)
            lastWaiter = null;
        first.nextWaiter = null;
    } while (!transferForSignal(first) &&
             (first = firstWaiter) != null);
}
```

#### transferForSignal（）

设置等待队列中的节点状态为0，cas成功后继续执行，失败返回false

cas成功后加入AQS，判断如果上一个节点的状态被取消了, 或者尝试设置上一个节点的状态为 SIGNAL 失败了(SIGNAL 表示: 他的 next 节点需要停止阻塞)

```java
final boolean transferForSignal(Node node) {
    /*
     * If cannot change waitStatus, the node has been cancelled.
     */
    if (!compareAndSetWaitStatus(node, Node.CONDITION, 0))
        return false;//更新节点的状态为 0，如果更新失败，只有一种可能就是节点被 CANCELLED 了

    /*
     * Splice onto queue and try to set waitStatus of predecessor to
     * indicate that thread is (probably) waiting. If cancelled or
     * attempt to set waitStatus fails, wake up to resync (in which
     * case the waitStatus can be transiently and harmlessly wrong).
     */  
    Node p = enq(node);//返回的节点p是加入node节点后的前节点，也就是原tail节点
    int ws = p.waitStatus;
    if (ws > 0 || !compareAndSetWaitStatus(p, ws, Node.SIGNAL))
        LockSupport.unpark(node.thread);// 唤醒节点上的线程.
    return true;
     //如果 node 的 prev 节点已经是signal 状态，那么被阻塞的 ThreadA 的唤醒工作由 AQS 队列来完成
}
```

==该方法先是 CAS 修改了节点状态，如果成功，就将这个节点放到 AQS 队列中，然后唤醒这个节点上的线程。此时，那个节点就会在 await 方法中苏醒==



# condition应用：

# **CountDownLatch**

### 使用原理：

CountDownLatch是一个工具类，它允许一个或者多个线程一直等待，直到其他线程操作完成再执行。顾名思义：它是一个倒计时的概念。它提供了两个方法，一个是await，一个是countDown。创建CountDownLatch时需要传入一个整数，在这个整数减到0之前，调用了await的方法都必须等待。然后通过countDown来减少计数。看一下下面两个案例。![image-20201115083409074](C:\Users\86137\AppData\Roaming\Typora\typora-user-images\image-20201115083409074.png)

------------------------------------------------------------------------------------------------------------------------------------![image-20201115083255080](C:\Users\86137\AppData\Roaming\Typora\typora-user-images\image-20201115083255080.png)

### 源码分析：

#### await()

```java
public void await() throws InterruptedException {
    sync.acquireSharedInterruptibly(1);
}
```

##### acquireSharedInterruptibly(int arg)

```java
public final void acquireSharedInterruptibly(int arg)
        throws InterruptedException {
    if (Thread.interrupted())
        throw new InterruptedException();
    if (tryAcquireShared(arg) < 0)//state计数如果!=0，说明当前线程需要加入到共享锁队列中
        doAcquireSharedInterruptibly(arg);
}
```

##### tryAcquireShared()

计数为0返回1

```java
protected int tryAcquireShared(int acquires) {
    return (getState() == 0) ? 1 : -1;
}
```

##### doAcquireSharedInterruptibly()



```java
private void doAcquireSharedInterruptibly(int arg)
    throws InterruptedException {
    final Node node = ①addWaiter(Node.SHARED);//可以看出是共享锁
    boolean failed = true;
    try {
        for (;;) {//如果计数不为0就会一直阻塞在这
            final Node p = node.predecessor();
            if (p == head) {
                int r = ②tryAcquireShared(arg);//同上，计数为0返回1
                if (r >= 0) {//相当于head先后移动
                    ③setHeadAndPropagate(node, r);
                    p.next = null; // help GC
                    failed = false;
                    return;
                }
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

##### setHeadAndPropagate()

这个方法就是不断自旋，不断替换头节点，唤醒头节点。Propagate：表示传播的意思，就是依次唤醒

这个方法的主要作用是把被唤醒的节点，设置成 head 节 点。 然后继续唤醒队列中的其他线程。

```java
private void setHeadAndPropagate(Node node, int propagate) {
    Node h = head; // Record old head for check below
    setHead(node);//设置为头节点
    /*
     * Try to signal next queued node if:
     *   Propagation was indicated by caller,
     *     or was recorded (as h.waitStatus either before
     *     or after setHead) by a previous operation
     *     (note: this uses sign-check of waitStatus because
     *      PROPAGATE status may transition to SIGNAL.)
     * and
     *   The next node is waiting in shared mode,
     *     or we don't know, because it appears null
     *
     * The conservatism in both of these checks may cause
     * unnecessary wake-ups, but only when there are multiple
     * racing acquires/releases, so most need signals now or soon
     * anyway.
     */
    if (propagate > 0 || h == null || h.waitStatus < 0 ||
        (h = head) == null || h.waitStatus < 0) {
        Node s = node.next;
        if (s == null || s.isShared())
            doReleaseShared();
    }
}
```

#### **countDown（）**

```java
public void countDown() {
        sync.releaseShared(1);
    }
```

##### releaseShared()

```java
public final boolean releaseShared(int arg) {
    if (tryReleaseShared(arg)) {//state 减为 0就调用下面的方法
        doReleaseShared();
        return true;
    }
    return false;
}
```

##### tryReleaseShared()

只有当 state 减为 0 的时候，tryReleaseShared 才返回 true, 否则只是简单的 state = state - 1

```java
protected boolean tryReleaseShared(int releases) {
    // Decrement count; signal when transition to zero
    for (;;) {
        int c = getState();
        if (c == 0)
            return false;
        int nextc = c-1;
        if (compareAndSetState(c, nextc))
            return nextc == 0;
    }
}
```

##### doReleaseShared()

**PROPAGATE**： 标识为 **PROPAGATE** **状态的节**点，是共享锁模式下的节点状态，处于这个状态下的节点，会对线程的唤醒进行传播

```java
private void doReleaseShared() {
    /*
     * Ensure that a release propagates, even if there are other
     * in-progress acquires/releases.  This proceeds in the usual
     * way of trying to unparkSuccessor of head if it needs
     * signal. But if it does not, status is set to PROPAGATE to
     * ensure that upon release, propagation continues.
     * Additionally, we must loop in case a new node is added
     * while we are doing this. Also, unlike other uses of
     * unparkSuccessor, we need to know if CAS to reset status
     * fails, if so rechecking.
     */
    for (;;) {
        Node h = head;
        if (h != null && h != tail) {
            int ws = h.waitStatus;
            if (ws == Node.SIGNAL) {//先判断头结点是不是SIGNAL 状态，如果是，则修改为 0，并且唤醒头结点的下一个节点
                if (!compareAndSetWaitStatus(h, Node.SIGNAL, 0))
                    continue;            // loop to recheck cases
                unparkSuccessor(h);//cas成功就唤醒下一个节点，不成功continue
            }
            else if (ws == 0 &&
                     !compareAndSetWaitStatus(h, 0, Node.PROPAGATE))
                continue;                // loop on failed CAS
        }// 如果到这里的时候，前面唤醒的线程已经占领了 head，那么再循环
        if (h == head)                   // loop if head changed**
            break;//注意是如果改变了就继续循环
    }
}
```

## **Semaphore**

semaphore 也就是我们常说的信号灯，semaphore 可以控制同时访问的线程个数，通过 acquire 获取一个许可，如果没有就等待，通过 release 释放一个许可。有点类似限流的作用。叫信号灯的原因也和他的用处有关，比如某商场就 5 个停车位，每个停车位只能停一辆车，如果这个时候来了 10 辆车，必须要等前面有空的车位才能进入。

### **使用场景** 

Semaphore 比较常见的就是用来做限流操作了。

## **CyclicBarrier**

CyclicBarrier 的字面意思是可循环使用（Cyclic）的屏障（Barrier）。它要做的事情是，让一组线程到达一个屏障（也可以叫同步点）时被阻塞，直到最后一个线程到达屏障时，屏障才会开门，所有被屏障拦截的线程才会继续工作。CyclicBarrier 默认的构造方法是 CyclicBarrier(int parties)，其参数表示屏障拦截的线程数量，每个线程调用 await 方法告诉 CyclicBarrier 当前线程已经到达了屏障，然后当前线程被阻塞

### **使用场景** 

当存在需要所有的子任务都完成时，才执行主任务，这个时候就可以选择使用 CyclicBarrier

**注意点** 

1）对于指定计数值 parties，若由于某种原因，没有足够的线程调用 CyclicBarrier 的 await，则所有调用 await 的线程都会被阻塞；

2）同样的 CyclicBarrier 也可以调用 await(timeout, unit)，设置超时时间，在设定时间内，如果没有足够线程到达，则解除阻塞状态，继续工作；

3）通过 reset 重置计数，会使得进入 await 的线程出现BrokenBarrierException； 

4 ）如果采用是 CyclicBarrier(int parties, Runnable barrierAction) 构造方法，执行 barrierAction 操作的是最后一个到达的线程

### **实现原理** 

CyclicBarrier 相比 CountDownLatch 来说，要简单很多，源码实现是基于 ReentrantLock 和 Condition 的组合使用。看如下示意图，CyclicBarrier 和 CountDownLatch 是不是很像，只是 CyclicBarrier 可以有不止一个栅栏，因为它的栅栏（Barrier）可以重复使用（Cyclic）
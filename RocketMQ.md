# RocketMQ

![](D:\SpeedPanX\图灵学院\图灵\资料\四：分布式框架专题-更多it视频 IT视频学习网-www.itspxx.com\15.Rocketmq核心源码剖析-杨过-更多it视频 IT视频学习网-www.itspxx.com\Rocketmq物理存储空间结构.png)

## 组成结构：

![image-20210504093007234](C:\Users\86137\AppData\Roaming\Typora\typora-user-images\image-20210504093007234.png)

- nameserver：提供轻量级的服务发现和路由；每个NameServer上都记录有完整的路由信息，提供相应的读写服务，并支持快速的存储扩展。**多个NameServer实例组成集群，单独立互换，没线信息交流**
- broker：通过提供轻量级的Topic和Queue来存储消息；



**两种消费方式**

- 拉取式消费，应用主动调用Consumer的拉消息方法从Broker服务拉取消息，该过程由应用主动控制。
- 推动式消费，Broker主动给消费端推送消息，该模式下的实时性较高。其底层原理后面也可以看到就是拉取式消费；

**消息模式**

- 集群消费，相同的Consumer组的每个Consumer实例平均分摊消息；
- 广播消费，相同的Consumer组每个Consumer实例都接收全部消息；

**顺序模式**：

顺序消息的原理：默认情况下会采用轮训的方式把消息发送到不同的队列中；但是消费消息是从多个队列中拉取消息的，这样是不能保证有序性的。需要保证顺序的话，就要控制发送的消息有序同时保证是发送到同一个队列中了的，然后消费者拉取消息的时候就从该对队列中拉取，就保证了有序性。

- 普通顺序消息，通过同一个消费队列得到的消息是有序的，
- 严格顺序消息，消费者得到的所有消息都是有序的。





## 具体实现

![image-20210504095023919](C:\Users\86137\AppData\Roaming\Typora\typora-user-images\image-20210504095023919.png)

**延时消息**

可以通过指定一定时间，让消息延时发送。

```
Message message = new Message；
message.setDelayTimeLevel(3);
 
 * 1s 5s 10s 30s 1m 2m 3m 4m 5m 6m 7m 8m 9m 10m 20m 30m 1h 2h
 * 分别对应级别
 * 1 2 3....................
```

**批量消息**

批量发送消息能提供发送消息的性能。但是一次发送太多也会造成性能宕机。每次批量的消息总大小不能超过4mb，官方建议在1mb左右。

**过滤和查询消息**

RocketMQ支持过滤消息，通过消息的TAG来过滤消息，可以通过类似sql语句的语句来查询哈。通过MessageId和MessageKey来查询消息；



## 消息事务

分为事务消息和半事务消息，其中半事务消息是指暂时不能投递的消息，发送方已经成功地把消息发送到了消息队列，但是服务端未收到生产者对该消息的二次确认，此时该消息就被标记为半事务消息。

**消息回查**：由于网络问题或者生产者重启等原因，导致某条事务消息的二次确认丢失，服务端通过扫描发送该消息长期处于半事务状态，这时会主动向生产者询问该消息的最终状态。这个过程就是消息回查。



**事务流程**：

![image-20210504105438236](C:\Users\86137\AppData\Roaming\Typora\typora-user-images\image-20210504105438236.png)

需要注意：事务消息不支持延时发送或者批量发送；同时为了避免单个消息被检查多次而阻塞半队列消息，默认是在检查15次之后将该消息丢弃，然后在错误日志中打印相应信息；

事务消息将在 Broker 配置文件中的参数 transactionMsgTimeout 这样的特定时间
长度之后被检查。当发送事务消息时，用户还可以通过设置用户属性CHECK_IMMUNITY_TIME_I
N_SECONDS来改变这个限制，该参数优先于 transactionMsgTimeout 参数。

面对的情况：

1. 操作数据库成功，向MQ中投递消息也成功，皆大欢喜
2. 操作数据库失败，不会向MQ中投递消息了
3. 操作数据库成功，但是向MQ中投递消息时失败，向外抛出了异常，刚刚执行的更新数据库的操作将被回滚



## 原理分析

RocketMQ存储逻辑对象层：该层主要包含了RocketMQ数据文件存储直接相关的三个模型类IndexFile、ConsumerQueue和CommitLog。IndexFile为索引数据文件提供访问服务，ConsumerQueue为逻辑消息队列提供访问服务，CommitLog则为消息存储的日志数据文件提供访问服务。这三个模型类也是构成了RocketMQ存储层的整体结构

------------------------------------------------
<img src="C:\Users\86137\AppData\Roaming\Typora\typora-user-images\image-20210504122433477.png" alt="image-20210504122433477" style="zoom:80%;" />

![img](https://img-blog.csdnimg.cn/20191013120103801.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3Fmemhhbmd3ZWk=,size_16,color_FFFFFF,t_70)

CommitLog：消息主体以及元数据的存储主体，存储Producer端写入的消息主体内容,
消息内容不是定长的。单个文件大小默认1G ，文件名长度为20位，左边补零，剩余为起始
偏移量，比如00000000000000000000代表了第一个文件，起始偏移量为0，文件大小为
1G=1073741824；当第一个文件写满了，第二个文件为00000000001073741824，起始
偏移量为1073741824，以此类推。消息主要是顺序写入日志文件，当文件满了，写入下一
个文件；

ConsumeQueue：消息消费队列，引入的目的主要是提高消息消费的性能，由于
RocketMQ是基于主题topic的订阅模式，消息消费是针对主题进行的，如果要遍历
commitlog文件中根据topic检索消息是非常低效的。Consumer即可根据
ConsumeQueue来查找待消费的消息。其中，ConsumeQueue（逻辑消费队列）作为消
费消息的索引，保存了指定Topic下的队列消息在CommitLog中的起始物理偏移量offset，
消息大小size和消息Tag的HashCode值。consumequeue文件可以看成是基于topic的
commitlog索引文件，故consumequeue文件夹的组织方式如下：topic/queue/file三层
组织结构，具体存储路径为：

**$HOME/store/consumequeue/{topic}/{queueId}/{fileName}。**

同样consumequeue文件采取定长设计，每一个条目共20个字节，分别为8字节的commitlog物理偏移量、4字节的消息长度、8字节tag hashcode，单个文件由30W个条目组成，可以像数组一样随机访问每一个条目，每个ConsumeQueue文件大小约5.72M；



IndexFile：IndexFile（索引文件）提供了一种可以通过key或时间区间来查询消息的方
法。Index文件的存储位置是：$HOME \store\index${fileName}，文件名fileName是以
创建时的时间戳命名的，固定的单个IndexFile文件大小：40+500W*4+2000W*20=
420000040个字节大小，约为400M，一个IndexFile可以保存 2000W个索引，IndexFile
的底层存储设计为在文件系统中实现HashMap结构，故rocketmq的索引文件其底层实现
为hash索引。
索引文件由索引文件头（IndexHeader）+（ 槽位 Slot ）+（消息的索引内容）三部分构
成。

![image-20210504123523446](C:\Users\86137\AppData\Roaming\Typora\typora-user-images\image-20210504123523446.png)





## **消息刷盘**

![image-20210504112938299](C:\Users\86137\AppData\Roaming\Typora\typora-user-images\image-20210504112938299.png)

- 同步刷盘：只有当消息真正持久化到磁盘后RocketMQ的Broker端后才会真正返回给Producer端一个ACK的响应。这是一种不错的保证消息可靠的方式，但是性能会有较大影响，一般在金融业用得比较多。
- 异步刷盘：能够充分利用OS的PageCache的优势，只要消息写入PageCache（内存）即可将成功的ACK返回给Producer端。消息刷盘采用后台异步线程提交的方式进行，降低了读写延迟，提高了MQ的性能和吞吐量。



**零拷贝刷盘**

​		以文件下载为例，服务端的主要任务是：将服务端主机磁盘中的文件不做修改地从已连
接的socket发出去。操作系统底层I/O过程如下图所示：



<img src="C:\Users\86137\AppData\Roaming\Typora\typora-user-images\image-20210504114308786.png" alt="image-20210504114308786" style="zoom:50%;" />

上面的过程产生了四次数据拷贝，并且此过程中，我们没有对文件内容做任何修改，那么在内核空间和用户空间来回拷贝就是一种性能的浪费，而零拷贝就是为了解决这种低效性。

<img src="C:\Users\86137\AppData\Roaming\Typora\typora-user-images\image-20210504114802189.png" alt="image-20210504114802189" style="zoom:50%;" />

零拷贝主要就是减少数据的拷贝次数或者让别的组件来完成这类数据的传输任务，让CPU解放出来专注于别的任务，这样就可以让系统资源的利用更加有效。

原理就是磁盘文件会通过DMA方式将数据拷贝到内存（内核缓冲区），然后将其直接和应用程序的共享数据（这样就不需要把数据再拷贝一次到用户缓冲区），应用程序再直接调用write()操作时，操作系统会直接把内存空间的数据拷贝socket到缓冲区，这个过程全发生在内核态，最后socket缓冲区再把数据发到网卡去。
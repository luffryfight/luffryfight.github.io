# Kafak

## 简介：

Kafka是最初由Linkedin公司开发，是一个分布式、支持分区的（partition）、多副本的（replica），基于zookeeper协调的分布式消息系统，它的最大的特性就是可以实时的处理大量数据以满足各种需求场景：比如基于hadoop的批处理系统、低延迟的实时系统、Storm/Spark流式处理引擎，，，web/nginx日志、访问日志，消息服务等等，用scala语言编写，Linkedin于2010年贡献给了Apache基金会并成为顶级开源 项目。

**使用场景**

- 日志收集：可以用来收集各种服务的日志log。
- 消息系统：解耦和生产者，消费者、缓存消息；
- 用户活动追踪：也是日志收集的一种应用；
- 运营指标：同样也是收集日志来监控数据。

![image-20210506221908032](C:\Users\86137\AppData\Roaming\Typora\typora-user-images\image-20210506221908032.png)

服务端(brokers)和客户端(producer、consumer)之间通信通过TCP协议来完成。

## 主题和消息日志

一个主题下有多个分区，分区中是一个有序的消息序列，每一条消息都被按顺序添加到commit log（一个分区对应一个）文件中，每个partition中的
消息都有一个唯一的编号，称之为offset，用来唯一标示某个分区中message。

一个partition同一个时刻在一个consumer group中只有一个consumer instance在消费，（有一个consumer_offset文件，里面有每个分区的offset）所以根据每个分区的offset来定位commit log 中的消息来消费，一般情况是顺序消费的。从这里也可以看出来consumer对集群的影响很小，consumer自己维护各自的offset。所以说kafka集群是无状态的，性能不会因为consumer数量受太多影响。kafka还将很多关键信息记录在zookeeper里，保证自己的无状态，从而在水平扩容时非常方便。

**为什么要对Topic下数据进行分区存储？**



**Partition**
	每个Partition只会在一个Broker上，物理上每个Partition对应的是一个文件夹
		Kafka默认使用的是hash进行分区，所以会出现不同的分区数据不一样的情况，但是partitioner是可以override的

Partition包含多个Segment，每个Segment对应一个文件，Segment可以手动指定大小，当Segment达到阈值时，将不再写数据，每个Segment都是大小相同的(LogSegment对应磁盘上的一个日志文件和一个索引文件)

Segment由多个不可变的记录组成，记录只会被append到Segment中，不会被单独删除或者修改，每个Segment中的Message数量不一定相等![img](https://img-blog.csdn.net/20180926145703288?watermark/2/text/aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L2xyeGNtd3ky/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70)

日志文件：

里面存储的是n对key-value，其中key是Message在log文件中的编号，比如1，3，6，8.....，表示第1条、第3条、第6条、第8条消息等，但是因为Index文件中并没有为数据文件中的每条消息都建立索引，而是采用了稀疏存储的方式，从而避免索引文件占用过多空间。虽然有些消息不能一次被定位到，但是可以定位到一个范围，然后遍历，这个范围很小了，不会造成很大性能的影响。

![img](https://img-blog.csdn.net/20180926163828679?watermark/2/text/aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L2xyeGNtd3ky/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70)

## **Controller选举机制**

​	在kafka集群启动的时候，会自动选举一台broker作为controller来管理整个集群，选举的过程是集群中每个broker都会尝试在zookeeper上创建一个 **/controller 临时节点**，zookeeper会保证有且仅有一个broker能创建成功，这个broker就会成为集群的总控器controller。

  当这个controller角色的broker宕机了，此时zookeeper临时节点会消失，集群里其他broker会一直监听这个临时节点，发现临时节点消失了，就竞争再次创建临时节点，就是我们上面说的选举机制，zookeeper又会保证有一个broker成为新的controller。

作用：

- **监听broker相关的变化**。为Zookeeper中的/brokers/ids/节点添加BrokerChangeListener，用来处理broker增减的变化。
- 监听topic相关的变化。为Zookeeper中的/brokers/topics节点添加TopicChangeListener，用来处理topic增减的变化；为Zookeeper中的/admin/delete_topics节点添加TopicDeletionListener，用来处理删除topic的动作。
- 从Zookeeper中读取获取当前所有与topic、partition以及broker有关的信息并进行相应的管理。对于所有topic所对应的Zookeeper中的/brokers/topics/[topic]节点添加PartitionModificationsListener，用来监听topic中的分区分配变化。
- 更新集群的元数据信息，同步到其他普通的broker节点中。

**副本选举机制**

	controller感知到分区leader所在的broker挂了(controller监听了很多zk节点可以感知到broker存活)，controller会从每个parititon的 replicas 副本列表中取出第一个broker作为leader，当然这个broker需要也同时在ISR列表里。





## Rebalance机制

当消费组的消费者数量变化时：kafka会重新分配消费者和消费分区的关系。分配过程消费者无法从kafka读取消息，这会对性能有所影响；

有三种分区的分配机制：range、round-robin、sticky。

假设一个主题有10个分区(0-9)，现在有三个consumer消费：

**range策略**就是按照分区序号排序，假设 n＝分区数／消费者数量 = 3， m＝分区数%消费者数量 = 1，那么前 m 个消费者每个分配 n+1 个分区，后面的（消费者数量－m ）个消费者每个分配 n 个分区。

比如分区0~3给一个consumer，分区4~6给一个consumer，分区7~9给一个consumer。

**round-robin策略**就是轮询分配，比如分区0、3、6、9给一个consumer，分区1、4、7给一个consumer，分区2、5、8给一个consumer

**sticky策略**初始时分配策略与round-robin类似，但是在rebalance的时候，需要保证如下两个原则。

1）分区的分配要尽可能均匀 。

2）分区的分配尽可能与上次分配的保持相同。

当两者发生冲突时，第一个目标优先于第二个目标 。这样可以最大程度维持原来的分区分配的策略。

比如对于第一种range情况的分配，如果第三个consumer挂了，那么重新用sticky策略分配的结果如下：

consumer1除了原有的0~3，会再分配一个7

consumer2除了原有的4~6，会再分配8和9



当有消费者加入消费组时，消费者、消费组及组协调器之间会经历以下几个阶段。

​    ![0](https://note.youdao.com/yws/public/resource/d9fed88c81ff75e6c0e6364012d19fef/xmlnote/3A5970DAF50346389BA375660AD69D7A/105392)

1.选择组协调器；2.加入消费组；3. ；4.

- ​	每个consumer group都会选择一个broker作为自己的组协调器coordinator，负责监控这个消费组里的所有消费者的心跳，以及判断是否宕机，然后开启消费者rebalance。

  ​		consumer group中的每个consumer启动时会向kafka集群中的某个节点发送 FindCoordinatorRequest 请求来查找对应的组协调器GroupCoordinator，并跟其建立网络连接。

  **组协调器选择方式**：consumer消费的offset要提交到__consumer_offsets的哪个分区，这个分区leader对应的broker就是这个consumer group的coordinator

- 在成功找到消费组所对应的 GroupCoordinator 之后就进入加入消费组的阶段，在此阶段的消费者会向 GroupCoordinator 发送 JoinGroupRequest 请求，并处理响应。然后GroupCoordinator 从一个consumer group中选择第一个加入group的consumer作为leader(**消费组协调器**)，把consumer group情况发送给这个leader，接着这个leader会负责制定分区方案。

- consumer leader通过给GroupCoordinator发送SyncGroupRequest，接着GroupCoordinator就把分区方案下发给各个consumer，他们会根据指定分区的leader broker进行网络连接以及消息消费。

## 发送消息

**1、写入方式**

producer 采用 push 模式将消息发布到 broker，每条消息都被 append 到 patition 中，属于顺序写磁盘（顺序写磁盘效率比随机写内存要高，保障 kafka 吞吐率）。

**2、消息路由**

producer 发送消息到 broker 时，会根据分区算法选择将其存储到哪一个 partition。其路由机制为：

\1. 指定了 patition，则直接使用； 2. 未指定 patition 但指定 key，通过对 key 的 value 进行hash 选出一个 patition 3. patition 和 key 都未指定，使用轮询选出一个 patition。

**3、写入流程**

\1. producer 先从 zookeeper 的 "/brokers/.../state" 节点找到该 partition 的 leader 2. producer 将消息发送给该 leader 3. leader 将消息写入本地 log 4. followers 从 leader pull 消息，写入本地 log 后 向leader 发送 ACK 5. leader 收到**所有 ISR 中**的 replica 的 ACK 后，增加 HW（high watermark，最后 commit 的 offset） 并向 producer 发送 ACK



关于**Metadata**，这个之前没讲过，简单理解就是Topic/Partition和broker的映射关系，每一个topic的每一个partition，需要知道对应的broker列表是什么，leader是谁、follower是谁。这些信息都是存储在Metadata这个类里面。

## 补充

**消息丢失**

​	**发送端**：ack设置可以为0,1，-1/all ；

0为异步方式，不需要等待broker的收到确认。

1代表要等到leader将数据写入本地log再发送下一条消息。这时如果follower没有同步到消息，然后leader就挂掉了就会丢失消息；

-1/all意味着要所有接受者都保存了数据再发送下一条消息，虽然性能会有影响，但是不会丢失消息，一般用于金融级别；

 **消费端**

​	如果设置了自动提交就会有可能丢失消息，消费的数据还没处理完，consumer就宕机了，但是offset已经提交了。





**重复消费**

	消息发送端：
		发送消息如果配置了重试机制，比如网络抖动时间过长导致发送端发送超时，实际broker可能已经接收到消息，但发送方会重新发送消息
```
消息消费端：
	如果消费这边配置的是自动提交，刚拉取了一批数据处理了一部分，但还没来得及提交，服务挂了，下次重启又会拉取相同的一批数据重
复处理
```


一般消费端都是要做消费幂等处理的。



**消息乱序**

​	比如某几条消息的顺序是1 ， 2 ，3；但是发送消息1时由于网络波动，

1被超时重新发送了；然后broker接受到的消息是2 ，3 ，1；所以重试机制设置根据实际情况去设定；



**消息积压**

1. 线上有时因为发送方发送消息速度过快，或者消费方处理消息过慢，可能会导致broker积压大量未消费消息。
   此种情况如果积压了上百万未消费消息需要紧急处理，可以修改消费端程序，让其将收到的消息快速转发到其他topic(可以设置很多分
   区)，然后再启动多个消费者同时消费新主题的不同分区。
2. 由于消息数据格式变动或消费者程序有bug，导致消费者一直消费不成功，也可能导致broker积压大量未消费消息。
   此种情况可以将这些消费不成功的消息转发到其它队列里去(类似死信队列)，后面再慢慢分析死信队列里的消息处理问题。



**延时队列**

有些场景需要延时的功能；比如一个订单，下单后有一定时间来支付，如果相应分钟内没被支付，就将该订单异常处理。或者是订单完成后一小时通知用户来填写评价；

实现：设置几个主题，分别为topic_1s ,topic_5stopic_10s，...topic_2h，这个一般不能支持任意时间段的延时），然后在定时去消费不同队列。如果到期了就把消息发送到具体要处理的队列中，具体来说就是定时器在一次消费过程中，对消息的发送时间做判断，看下是否延迟到对应时间了，如果到了就转发，如果还没到这一次定时任务就可以提前结束了。



**消息回溯**

如果某段时间对已消费消息计算的结果觉得有问题，可能是由于程序bug导致的计算错误，当程序bug修复后，这时可能需要对之前已消费的消息重新消费，可以指定从多久之前的消息回溯消费，这种可以用consumer的   offsetsForTimes、seek等方法指定从某个offset偏移的消息开始消费，





kafka的高可用机制体现在那些方面？

 分区的高可用，一主多从；

 leo|hw的机制；
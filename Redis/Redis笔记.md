# Redis笔记

**Redis核心原理**

为什么Redis是单线程还这么快？

```txt
因为Redis的所有数据都在内存中，所有运算都是内存级别的，同时还避免了多线程之间切换而造成的性能损耗。正是因为Redis是单线程，所以使用命令时更应注意，很容易造成Redis卡顿。
比如keys（获取所有键！）；
```

Redis单线程如何处理高并发？

Redis的IO多路复用，然后依次放入文件事件派发器上，派发器再分发非时间处理器；（Nginx也是采用IO复用的原理来解决C10k问题）；

![image-20210327080351589](C:\Users\86137\AppData\Roaming\Typora\typora-user-images\image-20210327080351589.png)







## 五大基础数据类型

相关命令自己多用就记住了，还有相关的*应用场景*；





## Redis持久化

**RDB**快照

Redis将内存数据库快照保存在名为dump.rdb的二进制文件中；你可以设置“sava N M” |“N秒内数据库至少有M个改动”时就会自动保存一次数据库；

同时也可以手动执行命令来生成RDB快照；save/bgsave生成dump.rdb文件，每次执行命令都会重写生成快照然后覆盖原来的文件；

save与bgsave对比：save是同步命令，bgsave是异步命令，bgsave会从redis主进fork（fork()是linux函数）出一个子进程专门用来生成rdb快照文件；默认是使用bgsave；

![image-20210327154901178](C:\Users\86137\AppData\Roaming\Typora\typora-user-images\image-20210327154901178.png)



<img src="https://img-blog.csdnimg.cn/20200513215141519.jpg?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3dlaXhpbl80Mzg3MzIyNw==,size_16,color_FFFFFF,t_70" alt="bgsave" style="zoom: 50%;" /><img src="https://img-blog.csdnimg.cn/20200513215150892.jpg?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3dlaXhpbl80Mzg3MzIyNw==,size_16,color_FFFFFF,t_70" alt="img" style="zoom:50%;" />

触发RDB的机制：

> 1. `save规则满足时，会自动触发；`
> 2. `执行flushall命令，也会触发；`
> 3. `退出redis时，也会自动产生`



**AOF方式**

rdb的快照式功能并不是很耐久：如果Redis因为某些原因而宕机，那么服务器就会丢失最近写入的一些数据。从1.1版本开始，Redis增加了一种完全耐久的持久化方式，<u>AOF持久化</u>：将修改的每一条语句都记录到appendonly.aof中；

当然可以如果每条语句都记录会使性能有所降低，但是可以通过设置更新的条件；

> - appendfsync always：每次都把新命令追加到aof文件，虽然慢，但是很安全。
>
> - appendfsync everysec：每经过一秒钟保存一次命令；该条件下最多丢失一秒钟的数据；
>
> - appendfsync no：将数据交给系统来处理，这种方式执行最快，但最不安全；
>

Redis官方推荐使用每秒更新一次appendfsync everysec；

AOF重写：AOF中可能会出现重复语句，然后会定期根据内存的最新数据生产aof文件；

```
    1 127.0.0.1:6379> incr readcount
    2 (integer) 1
    3 127.0.0.1:6379> incr readcount
    4 (integer) 2
    5 127.0.0.1:6379> incr readcount
    6 (integer) 3
    7 127.0.0.1:6379> incr readcount
    8 (integer) 4
    9 127.0.0.1:6379> incr readcount
    10 (integer) 5
```

重写后：	

```
    1 *3
    2 $3
    3 SET
    4 $2
    5 readcount
    6 $1
    7 5
```

同样的，我们可以通过命令来设置重写的频率；

```
    # auto-aof-rewrite-min-size 64mb   //aof文件至少要达到64M才会自动重写，文件太小恢复速度本
    来就很快，重写的意义不大
    # auto-aof-rewrite-percentage 100  //aof文件自上一次重写后文件大小增长了100%则再次触发重
    写
```

RDB 和 AOF ，我应该用哪一个？

（RDB经过二进制压缩，文件会较小一些）

![image-20210327161702432](C:\Users\86137\AppData\Roaming\Typora\typora-user-images\image-20210327161702432.png)

**Redis 4.0 混合持久化**
重启 Redis 时，我们很少使用 RDB来恢复内存状态，因为会丢失大量数据。我们通常使用 AOF 日志重
放，但是重放 AOF 日志性能相对 RDB来说要慢很多，这样在 Redis 实例很大的情况下，启动需要花费很
长的时间。 Redis 4.0 为了解决这个问题，带来了一个新的持久化选项——混合持久化。
通过如下配置可以开启混合持久化：

''#'' **aof-use-rdb-preamble yes**  

如果开启了混合持久化，AOF在重写时，不再是单纯将内存数据转换为RESP命令写入AOF文件，而是将重写这一刻之前的内存做RDB快照处理，并且将RDB快照内容和增量的AOF修改内存数据的命令存在一起，都写入新的AOF文件，新的文件一开始不叫appendonly.aof，等到重写完新的AOF文件才会进行改名，原子的覆盖原有的AOF文件，完成新旧两个AOF文件的替换。
	于是在 Redis 重启的时候，可以先加载 RDB 的内容，然后再重放增量 AOF 日志就可以完全替代之前的
AOF 全量文件重放，因此重启效率大幅得到提升。
混合持久化AOF文件结构

![image-20210327162100996](C:\Users\86137\AppData\Roaming\Typora\typora-user-images\image-20210327162100996.png)

**Redis主从架构**

---------------------

原理如图：细节看看笔记；

```
    Redis主从工作原理
    如果你为master配置了一个slave，不管这个slave是否是第一次连接上Master，它都会发送一个SYNC命
    令(redis2.8版本之前的命令)给master请求复制数据。
    master收到SYNC命令后，会在后台进行数据持久化通过bgsave生成最新的rdb快照文件，持久化期间，
    master会继续接收客户端的请求，它会把这些可能修改数据集的请求缓存在内存中。当持久化进行完毕以
    后，master会把这份rdb文件数据集发送给slave，slave会把接收到的数据进行持久化生成rdb，然后再加
    载到内存中。然后，master再将之前缓存在内存中的命令发送给slave。
    当master与slave之间的连接由于某些原因而断开时，slave能够自动重连Master，如果master收到了多
    个slave并发连接请求，它只会进行一次持久化，而不是一个连接一次，然后再把这一份持久化的数据发送
    给多个并发连接的slave。
    当master和slave断开重连后，一般都会对整份数据进行复制。但从redis2.8版本开始，master和slave断
    开重连后支持部分复制。
    
    数据部分复制
    从2.8版本开始，slave与master能够在网络连接断开重连后只进行部分数据复制。
    master会在其内存中创建一个复制数据用的缓存队列，缓存最近一段时间的数据，master和它所有的
    slave都维护了复制的数据下标offset和master的进程id，因此，当网络连接断开后，slave会请求master
    继续进行未完成的复制，从所记录的数据下标开始。如果master进程id变化了，或者从节点数据下标
    offset太旧，已经不在master的缓存队列里了，那么将会进行一次全量数据的复制。
    从2.8版本开始，redis改用可以支持部分数据复制的命令PSYNC去master同步数据
```

![image-20210327162743060](C:\Users\86137\AppData\Roaming\Typora\typora-user-images\image-20210327162743060.png)

主从复制(部分复制)流程图：


![image-20210327170323509](C:\Users\86137\AppData\Roaming\Typora\typora-user-images\image-20210327170323509.png)

*主从架构搭建得自己实现一次：具体步骤查看资料；还有springboot的整合*

```
    1 <dependency>
    2  <groupId>org.springframework.boot</groupId>
    3  <artifactId>spring‐boot‐starter‐data‐redis</artifactId>
    4 </dependency>
    5
    6 <dependency>
    7  <groupId>org.apache.commons</groupId>
    8  <artifactId>commons‐pool2</artifactId>
    9 </dependency>
    
    
    1 server:
    2  port: 8080
    3
    4 spring:
    5  redis:
    6  database: 0
    7  timeout: 3000
    8  lettuce:
    9  pool:
    10  max‐idle: 50
    11  min‐idle: 10
    12  max‐active: 100
    13  max‐wait: 1000
    14  sentinel: #哨兵模式
    15  master: mymaster #主服务器所在集群名称
    16  nodes: 192.168.0.60:26379,192.168.0.60:26380,192.168.0.60:26381
```

作用

> 数据冗余：主从复制实现了数据的热备份，是持久化之外的一种数据冗余的方式。
> 		故障恢复：当主节点故障时，从节点可以暂时替代主节点提供服务，是一种服务冗余的方式
> 		负载均衡：在主从复制的基础上，配合读写分离，由主节点进行写操作，从节点进行读操作，分担服器的负载；尤其是在多读少写的场景下，通过多个从节点分担负载，提高并发量。
> 		高可用基石：主从复制还是哨兵和集群能够实施的基础。

**Redis Lua脚本**
		Redis在2.6推出了脚本功能，允许开发者使用Lua语言编写脚本传到Redis中执行。使用脚本的好处如下:
1、减少网络开销：本来5次网络请求的操作，可以用一个请求完成，原先5次请求的逻辑放在redis服务器
上完成。使用脚本，减少了网络往返时延。这点跟管道类似。
2、原子操作：Redis会将整个脚本作为一个整体执行，中间不会被其他命令插入。管道不是原子的，不过
redis的批量操作命令(类似mset)是原子的。
3、替代redis的事务功能：redis自带的事务功能很鸡肋，报错不支持回滚，而redis的lua脚本几乎实现了
常规的事务功能，支持报错回滚操作，官方推荐如果要使用redis的事务功能可以用redis lua替代。
官网文档上有这样一段话：

`A Redis script is transactional by definition, so everything you can do with a Redis t
ransaction, you can also do with a script,and usually the script will be both simpler and faster.`

从Redis2.6.0版本开始，通过内置的Lua解释器，可以使用EVAL命令对Lua脚本进行求值。EVAL命令的格
式如下：
1 EVAL script numkeys key [key ...] arg [arg ...]
script参数是一段Lua脚本程序，它会被运行在Redis服务器上下文中，这段脚本不必(也不应该)定义为一
个Lua函数。numkeys参数用于指定键名参数的个数。键名参数 key [key ...] 从EVAL的第三个参数开始算
起，表示在脚本中所用到的那些Redis键(key)，这些键名参数可以在 Lua中通过全局变量KEYS数组，用1
为基址的形式访问( KEYS[1] ， KEYS[2] ，以此类推)。
在命令的最后，那些不是键名参数的附加参数 arg [arg ...] ，可以在Lua中通过全局变量ARGV数组访问，
访问的形式和KEYS变量类似( ARGV[1] 、 ARGV[2] ，诸如此类)。例如

127.0.0.1:6379> eval "return {KEYS[1],KEYS[2],ARGV[1],ARGV[2]}" 2 key1 key2 first seco
nd
1) "key1"
2) "key2"
3) "first"
4) "second"

注意，不要在Lua脚本中出现死循环和耗时的运算，否则redis会阻塞，将不接受其他的命令， 所以使用
时要注意不能出现死循环、耗时的运算。redis是单进程、单线程执行脚本。管道不会阻塞redis。



---------------

**Reids哨兵模式**

![image-20210327170559491](C:\Users\86137\AppData\Roaming\Typora\typora-user-images\image-20210327170559491.png)

sentinel哨兵是特殊的redis服务，不提供读写服务，主要用来监控redis实例节点。哨兵架构下client端第一次从哨兵找出redis的主节点，后续就直接访问redis的主节点，不会每次都通过sentinel代理访问redis的主节点，当redis的主节点发生变化，哨兵会第一时间感知到，并且将新的redis主节点通知给client端(这里面redis的client端一般都实现了订阅功能，订阅sentinel发布的节点变动消息)



**StringRedisTemplate与RedisTemplate **

前者使用采用的String序列化策略，保存的key和value都是此方法；

后者默认采用JDK的序列策略；

![image-20210327172842681](C:\Users\86137\AppData\Roaming\Typora\typora-user-images\image-20210327172842681.png)

![image-20210327172910077](C:\Users\86137\AppData\Roaming\Typora\typora-user-images\image-20210327172910077.png)

![image-20210327172945242](C:\Users\86137\AppData\Roaming\Typora\typora-user-images\image-20210327172945242.png)



## Redis的高可用集群部署

<img src="C:\Users\86137\AppData\Roaming\Typora\typora-user-images\image-20210330193849495.png" alt="image-20210330193849495" style="zoom:50%;" />

先谈谈集群的优点（哨兵模式分析在下面）：Redis集群是高可用，可拓展（水平拓展），能复制能分片的；不需要sentinel哨兵也能完成节点移除和故障转移功能；官方不推荐扩展超过1000个节点，

**原理分析**：

​	Reids cluster 将所有数据分为16384个槽位（slots），每个master/slave负责部分槽位，并且槽位的信息存储在每个节点中；当客户端每传来一个key，通过槽定位算法找到其槽位，然后直接定位；算法：`HASH_SLOT = CRC16(key) mod 16384`;

**选举原理**

<img src="C:\Users\86137\AppData\Roaming\Typora\typora-user-images\image-20210331100303472.png" alt="image-20210331100303472" style="zoom: 80%;" />

（偷懒了，这里自己重新总结一下更好。）



**重定位**

当客户端发出的指令中的key不属于自己管理，这时就会根据key重新定位其应该的槽位；

![image-20210331093249435](C:\Users\86137\AppData\Roaming\Typora\typora-user-images\image-20210331093249435.png)

**通信机制：gossip**

Redis集群中每个节点之间都存在通信，不仅是master和master、从节点之间、主从之间都有通信；

- 维护集群和元数据的方式有两种：集中式和gossip

**集中式**

顾名思义：集中处理；数据的更新和读取，时效性很好；一旦元数据改变就会更新到集中的储存中，其他节点能立即获取到最新的数据；但是问题在于：当所有的元数据的更新压力都集中在同一个地方，容易增加储存压力；

**gossip**

<img src="C:\Users\86137\AppData\Roaming\Typora\typora-user-images\image-20210331095214030.png" alt="image-20210331095214030" style="zoom:50%;" />

gossip协议包含多种消息，包括ping，pong，meet，fail等等。
		ping：每个节点都会频繁给其他节点发送ping，其中包含自己的状态还有自己维护的集群元数据，互相通过ping交换元
数据；
		pong: 返回ping和meet，包含自己的状态和其他信息，也可以用于信息广播和更新；
		fail: 某个节点判断另一个节点fail之后，就发送fail给其他节点，通知其他节点，指定的节点宕机了。
		meet：某个节点发送meet给新加入的节点，让新节点加入集群中，然后新节点就会开始与其他节点进行通信，不需要
发送形成网络的所需的所有CLUSTER MEET命令。发送CLUSTER MEET消息以便每个节点能够达到其他每个节点只需通过一条已知的节点链就够了。由于在心跳包中会交换gossip信息，将会创建节点间缺失的链接。gossip协议的优点在于元数据的更新比较分散，不是集中在一个地方，更新请求会陆陆续续，打到所有节点上去更新，有一定的延时，降低了压力；缺点在于元数据更新有延时可能导致集群的一些操作会有一些滞后。

`10000端口`
每个节点都有一个专门用于节点间通信的端口，就是自己提供服务的端口号+10000，比如7001，那么用于节点间通信的就是17001端口。 每个节点每隔一段时间都会往另外几个节点发送ping消息，同时其他几点接收到ping消息之后返回pong消息。如果想要部署集群，要么关闭防火墙，要么就在服务器管理上打开相应端口；

**cluster-­node-­timeout**

这个参数用于减少主从节点的切换，如果没有这个参数，那么每当一个主节点出现失连（可能是网络抖动等其他正常情况导致）就切换主从，那么会十分影响性能（主从切换包括数据的复制等操作）

**cluster-require-full-coverage**

当一个主节点及其从节点都宕机后，整个集群会处于不可用状态，除非设置了该参数为no；

---------------------------

6个示例，3主3从；最好由6台服务器部署；（详细过程课程的pdf中其实说得很详细了，就是对外ip的设置会有一点不一样，博客和笔记结合起来就完整了，-。-。弄了好久，真的气死）

```
修改每个配置文件：
1. 注释掉 bind
2. 修改protect--mode no
3. port 700x
4. cluster的 -enabled yes  /nodex.xx
5.不注释的顶头，注释的后面加空格；
```

中间的步骤按照这个来就可以了我补充一点容易错的：

```
https://blog.csdn.net/hellowrjun/article/details/103061773
```

![image-20210330194236301](C:\Users\86137\AppData\Roaming\Typora\typora-user-images\image-20210330194236301.png)

```
1.每个文件都要配置，配置成相应节点的文件；
2.redis-3.0.0.gem要放在redis.1和reids.2的同文件下
3.要进入redis/src目录下统一多个集群；
4../redis-cli --cluster create --cluster-replicas 1 服务器IP(公):7001 服务器IP(公):7002 服务器   IP(公):7003 服务器IP(公):7004 服务器IP(公):7005 服务器IP(公):7006
其中的 1 必不可少
5.回到 src下 redis-cli -c -h yourhost -p yourpost 这个命令来启动客户端；
6.可以在idea使用jedis测试；
7.阿里云服务器上的700x h和1700x都要开放
```

部署成功后：后面可以分别在xzq和rjw的服务器分开部署；这样的部署只能称为伪集群；

<img src="C:\Users\86137\AppData\Roaming\Typora\typora-user-images\image-20210330124214699.png" alt="image-20210330124214699" style="zoom:50%;" />



再回来看哨兵模式：

哨兵模式存在着一些问题，通常情况下是使用哨兵sentinel工具来监控master节点的状态，如果master节点异常，则会切换主从，将另外一台slave作为master；但是哨兵的配置略显复杂并且性能和高可用性等各方面都变现一般，在主从切换的过程中也会存在访问断失的情况；而且哨兵模式只要一个主节点向外提供服务，不能支持很高的并发。且单个主节点内存也不宜设置得过大，否则会导致持久化文件过大，影响数据恢复或主从同步的效率







## **Redis缓存优化**

几种关于缓存的问题：

**缓存穿透**

----------

缓存穿透是指查询一个不存在的数据，缓存层和存储层都没有被命中；通常是因为容错的考虑，如果存储层没有查询到数据，那么缓存层也不会写入数据；

缓存穿透将导致每次查询数据都去存储层获取，从而导致缓存失效：造成缓存穿透的原因主要有两个：

1. 本身业务逻辑或者数据出现问题；
2. 一些黑客攻击或者脚本 恶意进攻，造成大量空命中；

解决办法：缓存空对象（看代码即知其意）

```java
String get(String key) {
  // 从缓存中获取数据
  String cacheValue = cache.get(key);
  // 缓存为空
  if (StringUtils.isBlank(cacheValue)) {
  // 从存储中获取
  String storageValue = storage.get(key);
  cache.set(key, storageValue);
  // 如果存储数据为空， 需要设置一个过期时间(300秒)
      10  if (storageValue == null) {
  cache.expire(key, 60 * 5);
  }
  return storageValue;
  } else {
  // 缓存非空
  return cacheValue;
  }
}
```



**缓存失效**

大批量缓存同时失效，导致数据库压力骤增甚至宕机；这种情况的解决办法通常是：将这批key的过期时间设置为不同；

```java
1 String get(String key) {
2  // 从缓存中获取数据
3  String cacheValue = cache.get(key);
4  // 缓存为空
5  if (StringUtils.isBlank(cacheValue)) {
6  // 从存储中获取
7  String storageValue = storage.get(key);
8  cache.set(key, storageValue);
    9  //设置一个过期时间(300到600之间的一个随机数)
10  int expireTime = new Random().nextInt(300) + 300;
11  if (storageValue == null) {
12  cache.expire(key, expireTime);
13  }
14  return storageValue;
15  } else {
16  // 缓存非空
17  return cacheValue;
18  }
19 }
```

**缓存雪崩**

---------------

指的是缓存层支撑不住或者宕机后，请求像学崩一样打向后端。

缓存层承载了大量请求，有效地保护了存储层。但是一旦缓存层崩掉，大量流量透过来访问存储层，就会造成存储层的级联崩溃。主要从三个方面着手

- 提高缓存层的抗打击能力，比如使用redis集群或者哨兵；
- 使用一些组件来限流；
- 提前演练；项目上线前演练崩溃的情况，从而提前设置一些预备方案；



**热点key重建优化**

开发人员使用“缓存+过期时间”的策略既可以加速数据读写， 又保证数据的定期更新， 这种模式基本能够满
足绝大部分需求。 但是有两个问题如果同时出现， 可能就会对应用造成致命的危害：
当前key是一个热点key（例如一个热门的娱乐新闻），并发量非常大。
重建缓存不能在短时间完成， 可能是一个复杂计算， 例如复杂的SQL、 多次IO、 多个依赖等。
在缓存失效的瞬间， 有大量线程来重建缓存， 造成后端负载加大， 甚至可能会让应用崩溃。
要解决这个问题主要就是要避免大量线程同时重建缓存。
我们可以利用互斥锁来解决，此方法只允许一个线程重建缓存， 其他线程等待重建缓存的线程执行完， 重新从
缓存获取数据即可。

```java
1 String get(String key) {
2  // 从Redis中获取数据
3  String value = redis.get(key);
4  // 如果value为空， 则开始重构缓存
5  if (value == null) {
6  // 只允许一个线程重建缓存， 使用nx， 并设置过期时间ex
7  String mutexKey = "mutext:key:" + key;
8  if (redis.set(mutexKey, "1", "ex 180", "nx")) {
9  // 从数据源获取数据
10  value = db.get(key);
11  // 回写Redis， 并设置过期时间
12  redis.setex(key, timeout, value);
13  // 删除key_mutex
14  redis.delete(mutexKey);
15  }// 其他线程休息50毫秒后重试
16  else {
17  Thread.sleep(50);
18  get(key);
19  }
20  }
21  return value;
22 }
```

bigkey的危害：

1. 导致redis阻塞：
2. 网络拥塞：bigkey也就意味着每次获取要产生的网络流量较大，假设一个bigkey为1MB，客户端每秒问
   量为1000，那么每秒产生1000MB的流量，对于普通的千兆网卡(按照字节算是128MB/s)的服务
   器来说简直是灭顶之灾，而且一般服务器会采用单机多实例的方式来部署，也就是说一个bigkey
   可能会对其他实例也造成影响，其后果不堪设想。
3. 过期删除：当一个bigkey的过期时间到了后，redis会主动来删除该key（单线程），key过大就可能会阻塞redis（Redis4.0可以通过lazyfree-lazy-expire yes设置为异步删除）

bigkey的产生：

​	一般来说，bigkey的产生都是由于程序设计不当，或者对于数据规模预料不清楚造成的；

(1) 社交类：粉丝列表，如果某些明星或者大v不精心设计下，必是bigkey。
		(2) 统计类：例如按天存储某项功能或者网站的用户集合，除非没几个人用，否则必是bigkey。
		(3) 缓存类：将数据从数据库load出来序列化放到Redis里，这个方式非常常用，但有两个地方需要注意，第一，是不是有必要把所有字段都缓存；第二，有没有相关关联的数据，有的同学为了图方便把相关数据都存一个key下，产生bigkey；

**优化**

- 把一个key拆成几个小key
- 使用合适的数据类型
- 不要使用批量命令
- 给变量设置生命周期
- 推荐使用带有连接池的数据库，有效控制多个应用同时使用一个Redis
                   maxTotal(maxActive)资源池中最大连接数 ；maxIdle资源池允许最大空闲
  的连接数；minIdle资源池确保最少空闲的连接数；
- 连接池的最佳性能是maxTotal = maxIdle
- 高并发情况下建议添加 熔断功能（netflix  hystrix）



**Redis对于过期键有三种清除策略：**
被动删除：当读/写一个已经过期的key时，会触发惰性删除策略，直接删除掉这个过
期key
主动删除：由于惰性删除策略无法保证冷数据被及时删掉，所以Redis会定期主动淘汰
一批已过期的key
当前已用内存超过maxmemory限定时，触发主动清理策略
当REDIS运行在主从模式时，只有主结点才会执行被动和主动这两种过期删除策略，然后把删除
操作”del key”同步到从结点。







**布隆过滤器**

​	存在恶意攻击，向服务器大量请求不存的数据，造成缓存穿透时，布隆过滤器可以用来先做一次过滤，对于不存在的数据，布隆过滤器能很大程度过滤掉；布隆过滤器实质就是一个大型的位数组+几个不同的无偏hash函数（无偏指能把数据hash得比较均匀）；

每当一个key来到布隆过滤器，每个hash函数都会对其散列，然后在对应位置赋值为1；每个函数都会得到一个不同得位置；注意当布隆过滤器说某个值存在时，这个值可能不存在；当它说不存在时，那就肯定不存在。

当通过布隆过滤器判断key是否存在时，同样也会通过hash把几个位置都算出来，只要有一个位置为0，那么该key就是不存在；就算都是1，也不能说明该key一定存在；

==这种方法适用于数据命中不高、 数据相对固定、 实时性低（通常是数据集较大） 的应用场景， 代码维护较为复杂， 但是缓存空间占用很少。==



<img src="C:\Users\86137\AppData\Roaming\Typora\typora-user-images\image-20210403195523577.png" alt="image-20210403195523577" style="zoom: 67%;" />

## 补充的命令：

scan：渐进式遍历键

命令：scan cursor  match xpattern count xcount;

```
cursor第一次遍历时为0，然后将结果的第一个整数值坐下一次的cursor；
xpattern:key的正则表达式；
xcount:一次遍历的key的数量；
```

e.g.

![image-20210327081740731](C:\Users\86137\AppData\Roaming\Typora\typora-user-images\image-20210327081740731.png)
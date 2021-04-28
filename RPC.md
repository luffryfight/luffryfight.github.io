# Rpc

![基础架构](C:\Users\86137\Desktop\Git\luffryfight.github.io\中间\基础架构.png)

## Rpc实现原理升入分析

RPC(Remote Procedure Call):远程服务调用，他是一个计算机通信协议。它允许像本地方法一样调用远程服务。由于不在同一个内存中，不能直接调用，需要通过网络来表表达调用的语义和传达调用的数据。

![定义](C:\Users\86137\Desktop\Git\luffryfight.github.io\中间\定义.png)

![调用过程](C:\Users\86137\Desktop\Git\luffryfight.github.io\中间\调用过程.png)

## 精简版Rpc调用代码实现

​	**序列化协议**：

远程调用涉及数据的传输，就会涉及组包和解包，即需要调用方和服务方约定数据格式----序列化协议。

![协议](C:\Users\86137\Desktop\Git\luffryfight.github.io\中间\协议.png)

解释：

- 版本号的作用：可能会有多个调用方，每个调用方的协议版本都可能不同。故需要将调用方使用的协议版本标明。并且服务方必须同时支持多个协议的版本；
- cmd 表示是调用的方法；
- magic 约定的一个相同的值；
- bodylen  表示后面的可变数组的长度；用来判断数据的完整性；



![流程图](C:\Users\86137\Desktop\Git\luffryfight.github.io\中间\流程图.png)





## Rpc服务消费方核心功能设计实现

​								Rpc**产品是什么样的**？

仅是实现远程调用是不够的，离产品化还有很长一段距离。

数据传输 		序列化/反序列化		客户端代理类			请求映射分发

![RPC产品需要的功能](C:\Users\86137\Desktop\Git\luffryfight.github.io\中间\RPC产品需要的功能.png)

**Consumer**

### 连接管理

保持和服务方的长连接，用于传输请求的数据，和返回结果

- 初始化时机
- 连接数维护
- 心跳/重连



![客户端线程模型](C:\Users\86137\Desktop\Git\luffryfight.github.io\中间\客户端线程模型.png)

调用方法组装数据包之后，把数组包放入队列中，让专门的线程去发生该数据;

接收时也直接放入另一个队列中，然后去唤醒工作线程。

问题：

- 如何唤醒工作线程
- 如何确定接受的数据包对应的哪个请求；

### 负载均衡

确保多个服务提供方的节点流量均匀合理，支持节点扩容与灰度发布；

![负载均衡](C:\Users\86137\Desktop\Git\luffryfight.github.io\中间\负载均衡.png)

几种方式：

- 轮训  遍历访问
- 随机  需要保证足够随机
- 取模  根据uid取模n,计算其对应机器；有些错误会使其每次都是同一个机器；
- 带权重   最常用的方式
- 一致性Hash ：该方式存在一个问题，当一个节点承受不住流量的冲击而崩溃后，其他节点也很大可能会随之崩溃（因为挂掉一个节点后，下一个节点接受的流量会雪崩式地增加！）

**均衡设计**

有个问题，是选择随机生成一个值，还是选择使用（轮训访问）；

其实是选择后者，如果每次均衡的时候都随机生成一个数，当量很大的时候，开销就会很大。

![底层原理](C:\Users\86137\Desktop\Git\luffryfight.github.io\中间\底层原理.png)

**轮训数组**

![轮训数组](C:\Users\86137\Desktop\Git\luffryfight.github.io\中间\轮训数组.png)

**轮训+权重数组**

![轮训_权重](C:\Users\86137\Desktop\Git\luffryfight.github.io\中间\轮训_权重.png)

![请求路由](C:\Users\86137\Desktop\Git\luffryfight.github.io\中间\请求路由.png)

![路由表功能设计](C:\Users\86137\Desktop\Git\luffryfight.github.io\中间\路由表功能设计.png)

![灰度流量](C:\Users\86137\Desktop\Git\luffryfight.github.io\中间\灰度流量.png)



**超时处理**

对于长时间没有返回的请求，需要做异常处理，及时释放资源。

![超时处理](C:\Users\86137\Desktop\Git\luffryfight.github.io\中间\超时处理.png)

![超时处理的实现](C:\Users\86137\Desktop\Git\luffryfight.github.io\中间\调用方具体代码.png)

![invoke](C:\Users\86137\Desktop\Git\luffryfight.github.io\中间\invoke.png)

![image-20210423051745373](C:\Users\86137\AppData\Roaming\Typora\typora-user-images\image-20210423051745373.png)

一次性发生几个，提供I/O效率；

## Rpc服务提供方核心功能设计实现



**线程池和队列**

将不同类型的请求，放入各自的队列，每个队列分配独立的线程池，资源隔离。

Req Queue可能发生故障，从而发生等待，造成后续队列不能被访问。所以需要多个线程，来隔离资源。

如何分配线程池资源？

- 单队列多线程 1*64: 对于满请求的适应性强。但是在等待的时候，因为锁的缘故，性能就会降低。
- 多队列单线程 64*1 ：一个队列中如果存在多个超时任务，容易使该队列崩溃。
- 多队列多线程 n*m

最终选择前者：

![提供方](C:\Users\86137\Desktop\Git\luffryfight.github.io\中间\提供方.png)



-----------------

序列化的作用于好处？？？
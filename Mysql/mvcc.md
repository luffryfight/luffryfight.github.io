## MVCC机制

Mysql默认的隔离级别是RR 可重复读。但是在多并发的情况下,如何保证每次查询到的数据都是满足RR的隔离级别嘞？

比如，下面这个情况下：

![image-20210704231450229](C:\Users\86137\AppData\Roaming\Typora\typora-user-images\image-20210704231450229.png)

其中表结构为：

```
mysql> CREATE TABLE `t` (
    `id` int(11) NOT NULL,
    `k` int(11) DEFAULT NULL,
    PRIMARY KEY (`id`)
) ENGINE=InnoDB;
insert into t(id, k) values(1,1),(2,2);
```



首先阐述一些概念：

```
事务：
事务的启动：
	begin/start transaction：一致性视图在执行第一个快照读的时候创建。
	start transaction with consistent snapshot:一致性试图是在执行这个语句的时候就创建。
	同时autocommit 默认为 1;
	update语句也会隐式的使用第一种方式创建事务，语句完成后会自动提交。

	事务在开始的时候会向InnoDB事务系统申请一个transcation id ，这个id是严格按照顺序递增的，也是就每个事务都有自己唯一的事务id。
```

```
视图：
	一个是 view。它是一个用查询语句定义的虚拟表，在调用的时候执行查询语句并生成结
果。创建视图的语法是 create view … ，而它的查询方法与表一样
	另一个是InnoDB在实现MVCC时使用的一致性视图，即consistent read view，这个一致性视图可以用来支持RC和RR。

	对于可重复读的隔离级别，需要理解的是“以我启动的时刻为准，如果一个数据版本是在我启动之前生成的，就认；如果是我启动以后才生成的，我就不认，我必须要找到它的上一个版本”。对于这个实现：InnoDB为每个事务构造了一个**数组**，用来保存这个事务启动瞬间当前**活跃**的事务id。活跃是指启动了但是还没有提交。 对于这个视图数组,有高水位和低水位之分的概念。
```

```
更新数据都是先读后写的，而这个读，只能读当前的值，称为“当前读”（current read）。

mysql> select k from t where id=1 lock in share mode; 读锁	 S锁|共享锁
mysql> select k from t where id=1 for update;		写锁
```



![image-20210705093205158](C:\Users\86137\AppData\Roaming\Typora\typora-user-images\image-20210705093205158.png)

```
一致性读，一致性读会根据 row trx_id 和一致性视图
确定数据版本的可见性。
而当前读，总是读取已经提交完成的最新版本
```

判断数据版本可见性的规则：所有数据有效性都是下面的两个规则来判断的。

![image-20210705093356233](C:\Users\86137\AppData\Roaming\Typora\typora-user-images\image-20210705093356233.png)

1. 版本未提交，不可见；
2. 版本已提交，但是是在视图创建后提交的，不可见；
3. 版本已提交，而且是在视图创建前提交的，可见 ***  所以事务B在更新的时候C事务的操作是可见的。

用第一张图来举例：

- 事务 A 开始前，系统里面只有一个活跃事务 ID 是 99；
- 事务 A、B、C 的版本号分别是 100、101、102，且当前系统里只有这四个事务；
- 三个事务开始前，(1,1）这一行数据的 row trx_id 是 90。

这样，事务 A 的视图数组就是 [99,100], 事务 B 的视图数组是 [99,100,101], 事务 C 的视
图数组是 [99,100,101,102]。

![image-20210705093703901](C:\Users\86137\AppData\Roaming\Typora\typora-user-images\image-20210705093703901.png)

现在事务 A 要来读数据了，它的视图数组是 [99,100]。当然了，读数据都是从当前版
本读起的。所以，事务 A 查询语句的读数据流程是这样的：

找到 (1,3) 的时候，判断出 row trx_id=101，比高水位大，处于红色区域，不可见；接着，找到上一个历史版本，一看 row trx_id=102，比高水位大，处于红色区域，不可见；再往前找，终于找到了（1,1)，它的 row trx_id=90，比低水位小，处于绿色区域，可见。



如果修改一下：

![image-20210705094725486](C:\Users\86137\AppData\Roaming\Typora\typora-user-images\image-20210705094725486.png)

事务 C’的不同是，更新后并没有马上提交，在它提交前，事务 B 的更新语句先发起了。
前面说过了，虽然事务 C’还没提交，但是 (1,2) 这个版本也已经生成了，并且是当前的
最新版本。那么，事务 B 的更新语句会怎么处理呢？

这里就是用到了两阶段锁协议了。事务C还没提交，也就是C中(1,2)这个结果还没释放，而事务B是当前读，必须读取最新的数据，而且必须要加锁，因为C拿到了锁，所以B被锁住了，必须等C释放锁。



还有RC隔离级别：与RR的区别就是，RR的视图数组是在事务启动的时候创建的，然后一直使用这个视图数组，直到事务提交。RC是在执行一个语句的时候创建的；用RC分析第一张图：

	(1,3) 还没提交，属于情况 1，不可见；
	(1,2) 提交了，属于情况 3，可见。
	所以，这时候事务 A 查询语句返回的是 k=2





补充一个情况：更新丢失的情况：

```
mysql> CREATE TABLE `t` (
`id` int(11) NOT NULL,
`c` int(11) DEFAULT NULL,
PRIMARY KEY (`id`)
) ENGINE=InnoDB;
insert into t(id, c) values(1,1),(2,2),(3,3),(4,4);
//依次执行以下语句
begin;
select * from t;
update t set c=0 where id=c;
select * from t;
发现数据没有改变、
```

![image-20210705104153176](C:\Users\86137\AppData\Roaming\Typora\typora-user-images\image-20210705104153176.png)

![image-20210705104157671](C:\Users\86137\AppData\Roaming\Typora\typora-user-images\image-20210705104157671.png)

可能出现这俩中情况就会数据修改失败。

最后检验下：判断一下的查询是什么结果？

![img](file:///D:\下载的视频\猴博士《高等数学》上下册\1161074235\Image\C2C\5N7B%B9KR22$[LL36VFPAD6.png)
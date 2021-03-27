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

相关命令自己多用就记住了，还有相关的应用场景；













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
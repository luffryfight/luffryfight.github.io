# 《疯狂的JAVA讲义》复习笔记：

## 内部类：

- 非静态内部类:不能拥有静态成员，可以有四个作用域！
- 静态内部类：是属于外部类的，可以在外部类的静态代码块中初始化
- 局部内部类：实际并不怎么运用，作用域只在这个方法中；
- 匿名内部类：当一个类只需使用一次时，可以使用来简化代码

创建方法：

`OutClass.InnerClass innerClass = `

`new OutClass().new InnerClass();`

注意：

- 外部类的静态方法和静态代码块不能访问非静态内部类
- 接口中的类只能时静态内部类，但由于接口就是暴露后用来调用的，所以这样的内部类实际意义不大；

--------------------------------分割线-----------------------------------------

## Lambda与匿名内部类表达式的区别和关系：

1. 无论接口中有多少抽象方法，匿名内部类都可以用来创建实例；而Lambda只能创建函数式接口的实例；
2. 无论类是否是抽象类，匿名内部类都可以用来创建实例；而Lambda只能创建函数式接口的实例；
3. 匿名内部类实现的抽象方法可以调用接口中的默认方法；而Lambda不允许调用；

补：

`接口定义`：

- 使用 interface 关键字 。
- 接口中的 所有 成员变量 都默认是由 public static final 修饰的。
- 接口中的 所有方法 都默认是由 public abstract 修饰的。也可以使用 protected ，但不能用 private 。
- 接口中的 所有方法 都没有方法体。
- 接口没有构造方法。因为构造方法用于创建对象。
- 实现接口的类  必须提供接口中所有方法的具体实现内容。

`JDK7`及之前： 静态常量、属性和 抽象方法

`JDK8`：静态方法 和 默认方法（必须写方法体）

`JDK9`：私有方法，包括静态私有和普通私有（解决函数间重复代码的问题）

-----    接口中 `default` 方法属于实现接口类的对象方法，可以重写

---更新：看了一眼别人说的,觉得将来最大的好处是可以改变接口,扩展接口功能.比如现在的接口如果添加新的抽象方法,会因为老版本程序中没有覆盖新增方法,造成代码报错.也就是为保证兼容性,无法升级接口.现在给你个默认的不用覆盖,老版本代码和程序应该可以更容易迁移到jdk8上面来. 其他lambda表达式之类的就不了解了.

-----    多个父接口当中的默认方法如果重复，子接口必须进行默认方法的覆盖重写，而且必须带着`default`关键字。

----------------------------分割线-----------------------------------------

## 垃圾回收机制：

参考连接http://www.linuxidc.com/Linux/2015-06/118829.htm
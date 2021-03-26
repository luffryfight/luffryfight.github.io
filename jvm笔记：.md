#   JVM笔记：



## 1.类加载过程 

多个java文件经过编译打包生成可运行的jar包，最终由命令运行某个主类的main函数启动程序，这里首先通过类加载器把主类加载到JVM。主类在运行过程中如果使用到其他类，会逐步加载这些类。需要注意的是：jar包里的类不是一次性全部加载的，是使用时才加载。

类加载整个过程有如下几步，

加载->验证->准备->解析->初始化->使用->卸载

```
加载： 在硬盘上加载并通过IO读入字节码文件，使用到该类时才会加载，比如调用该类的main()，new对象等方法。

验证： 校验字节码文件的正确性。
 
准备： 给类的静态变量分配内存，并赋值默认值。

解析： 将引用替换为直接引用，该阶段会把一些静态方法（符号应用，比如main方法）替换为指向数据所在内存的指针或者句柄（静态连接）动态链接，在程序运行期间将符号替换为直接引用（类加载时把math.compute()放入方法区，当解析compute()符号时，会通过math头部的指针指向的区域获得方法==指令码==，换成直接引用）。
```

![类加载过程](https://raw.githubusercontent.com/luffryfight/picgo/master/data/20210305144534.png)



##  2.类加载器和双亲委派机制

类加载器的种类：

- 启动类扩展器： 负责加载支撑JVM运行的位于JRE的lib目录下核心类库，比如jar,charsets.jar等
- 扩展类加载器： 负责加载支撑JVM运行的位于JRE的lib目录下的ext扩展目录中的jar类包；
- 应用程序加载器： 负责加载classpath路径下的类包，也就是自己写的类。
- 自定义加载器： 负责加载自定义路径下的类包。

类加载实例：

```java
public class TestJDKClassLoader {
    public static void main(String[] args) {
        System.out.println(String.class.getClassLoader());
        System.out.println(com.sun.crypto.provider.DESKeyFactory.class.getClassLoader());
        System.out.println(TestJDKClassLoader.class.getClassLoader());
        System.out.println(ClassLoader.getSystemClassLoader().getClass().getName());
    }
    /*
        null//启动类加载器是c++实现的，无法找到java类
        sun.misc.Launcher$ExtClassLoader@4b67cf4d
        sun.misc.Launcher$AppClassLoader@18b4aac2
        sun.misc.Launcher$AppClassLoader//系统应用程序加载器
    */
}
```

**自定义一个类加载器**

自定义类加载器只需要继承 java.lang.ClassLoader 类，该类有两个核心方法，一个是loadClass（String boolean）实现双亲委派机制。双亲委派机制大致是:

1. 首先检查一下指定的类是否已经被加载过了，如果加载过了，就不需要再加载了，直接返回。
2. 如果没有加载过此类，那么再判断一下是否有父加载器；如果有父加载器，就调用父加载器，或者调用bootstrap类加载器来加载；
3. 如果父加载器及bootstrap类加载器都没有找到指定类，那么调用当前的类加载器的findClass方法来完成；

还有一个方法是findClass，默认实现是抛出异常，所以我们自定义类加载器就算重写findClass方法；



```java
 public class MyClassLoaderTest {
    static class MyClassLoader extends ClassLoader{
        private String classPath;

        public MyClassLoader(String classPath) {
            this.classPath = classPath;
        }
        private byte[] loadByte(String name) throws Exception{
            name=name.replaceAll("\\.","/");
            FileInputStream fis=new FileInputStream(classPath+"/"+name+".class");
            int len=fis.available();
            byte[] data=new byte[len];

            fis.read(data);
            fis.close();
            return data;
        }
        protected Class<?> findClass(String name) throws ClassNotFoundException{
            try {
                byte[] data = loadByte(name);
                return defineClass(name,data,0,data.length);
            } catch (Exception e) {
                e.printStackTrace();
                throw new ClassNotFoundException();
            }
        }
    }

    public static void main(String[] args) throws Exception {
        MyClassLoader classLoader=new MyClassLoader("D:/jvm/test");
        Class clazz=classLoader.loadClass("cn.kobe.User1");
        Object object=clazz.newInstance();
        Method method = clazz.getDeclaredMethod("test", null);
        method.invoke(object,null);
        System.out.println(clazz.getClassLoader().getClass().getName());
    }
     /* 输出:
     	调用了User1的test			//反射调用了User1的test
		sun.misc.Launcher$AppClassLoader	//使用的类加载器是AppClassLoader
		//解释：因为会向上调用父加载器，返回到应用程序加载器的时候发现classpath下有这个类，所以就调		用AppClassLoader。如果classpath下没有，会调用自定义的加载器。
     */
     
     
     public static void main(String[] args) throws Exception {
        MyClassLoader classLoader=new MyClassLoader("D:/jvm/test");
        Class clazz=classLoader.loadClass("cn.kobe.User1");
        Object object=clazz.newInstance();
        Method method = clazz.getDeclaredMethod("test", null);
        method.invoke(object,null);
        System.out.println(clazz.getClassLoader().getClass().getName());
    }
     /* 输出:该输出验证了猜想：即验证了双亲委派机制
     	调用了User1的test
		cn.kobe.MyClassLoaderTest$MyClassLoader
     */
}

```



**双亲委派机制**

​	类加载中的双亲委派机制就是：加载某个类时会优先委托父加载器（关系如下图），找不到就再向下返回；如果所有加载器都找不到不到目标，则再自己的类加载器中查找并载入目标类。

比如Math类，一开始会找应用程序类加载器，它将委托扩展类加载器，然后扩展类加载器又会委托启动类加载器；顶层类加载器在自己的类加载路径中没有找到对应类（Math类），就会乡下回退。然后在扩展类加载器的路径里也没有找到，最终回退到应用程序类加载器，然后在自己的路径找到了Math类，就加载了。

==双亲委派机制的作用==：

- 安全机制：自己写的java.lang.String.class类不会被加载，这样可以防止核心库API被随意修改。
- 避免类的重复加载，当父类加载已经加载了该类的时候，就没必要让子类加载器再加载一次，保证了被加载类的唯一性！

![image-20210305164008005](https://raw.githubusercontent.com/luffryfight/picgo/master/data/20210305164016.png)![双亲委派机制](

```java
package java.lang;

public class String {
    public static void main(String[] args) {
        System.out.println("can this main method run?");
    }
  	/*输出：
  		错误: 在类 java.lang.String 中找不到 main 方法, 请将 main 方法定义为:
   		public static void main(String[] args)
		否则 JavaFX 应用程序类必须扩展javafx.application.Application
  	*/
    //解释：因为当类加载时，会调用类加载器，通过双亲委派机制，就到了调用启动类加载器，会加载核心类库在核心类库发现了 java.lang.String 类 然后就调用main方法，结果jdk的String没有main方法，就报错了
    
    
    public static void main(String[] args) throws Exception {
        MyClassLoader classLoader=new MyClassLoader("D:/jvm/test");
        Class clazz=classLoader.loadClass("java.lang.String");
        Object object=clazz.newInstance();
        Method method = clazz.getDeclaredMethod("test", null);
        method.invoke(object,null);
        System.out.println(clazz.getClassLoader().getClass().getName());
    }
    //沙箱安全:再自定义的打破双亲委派机制中调用这个mian，并且指定路径下有String这个类，就会报错
        //java.lang.SecurityException: Prohibited package name : java.lang
}



```



**打破双亲委派机制**

以Tomcat为例：

![打破双亲委派](https://raw.githubusercontent.com/luffryfight/picgo/master/data/20210305172759.png)

![image-20210305172706777](https://raw.githubusercontent.com/luffryfight/picgo/master/data/20210305172709.png)

![image-20210305173114926](https://raw.githubusercontent.com/luffryfight/picgo/master/data/20210305173226.png)

## 3.内存模型



笔记好像在诸葛老师的git上。记得下载jvm的指令大全

 举例代码：分析流程。

 ![image-20210308213614573](https://raw.githubusercontent.com/luffryfight/picgo/master/data/20210311151024.png)![image-20210308202043821](C:\Users\86137\AppData\Roaming\Typora\typora-user-images\image-20210308202043821.png)

![线程区](C:\Users\86137\Pictures\笔记需要的图\jvm\线程区.png)

程序计数器：jvm正在执行到jvm指令码的行数；

方法出口：方法执行结束后返回主方法的位置；

方法区：常量+静态变量+类元信息 

动态链接：  见上！

本地方法栈:native 方法，保存调用的本地方法；  

对象类型都分配在堆上；栈的默认大小为1M,这个值越小，可分配的线程就越多；元空间使用的直接内存；

总结：栈中有很多引用变量，指向堆中的具体变量；堆中有很多对象，每个对象头部有一个指针，都指向这个类的类元信息（方法区） ；特别地，当一个类有一个静态的对象，那这个静态对象 有一个指针指向这个类在堆中的地址；



![image-20210308202351880](https://raw.githubusercontent.com/luffryfight/picgo/master/data/20210311151034.png)![image-20210311153701069](C:\Users\86137\AppData\Roaming\Typora\typora-user-images\image-20210311153701069.png)

堆： new出来的对象一般会放在Eden区， 年轻代占堆的内存的大概1/3； 当Eden快满了的时候，会触发 minor GC 把无引用的对象回收，其他的放入From;下次再minor GC会把Eden区和From区放入To区,循环15次之后；这些对象会被放在老年代；老年代有：@Serverice 和Bean 对象；当老年代也满了之后会触发full GC，对整个堆回收；full GC会停止线程，

![image-20210308213439461](https://raw.githubusercontent.com/luffryfight/picgo/master/data/20210311151041.png)

![堆](C:\Users\86137\Pictures\笔记需要的图\jvm\堆.png)

## 4.jvm内存参数设置



栈内存溢出实例：

`spring Boot 程序的JVM参数设置格式（Tomcat启动直接再bin目录下的catalina.sh文件里）` 

```boot
java -Xms2048M -Xmx2048M -Xmn1024M -Xss512k -XX:metaspace=356M-XX:MaxMetaspaceSize=256M -jar microserv
```



```java
public class StackOverflowError {
    static int count=0;
	//JVM设置
        // -Xss128k -Xss默认1M
    static void redo(){
        count++;
        redo();
    }

    public static void main(String[] args) {
        try {
            redo();
        } catch (Exception e) {
            System.out.println(count);
            e.printStackTrace();
        }
    }
    /*
    * 运行结果：
    * Exception in thread "main" java.lang.StackOverflowError
	at cn.kobe.StackOverflowError.redo(StackOverflowError.java:14)
	at cn.kobe.StackOverflowError.redo(StackOverflowError.java:14)
	at cn.kobe.StackOverflowError.redo(StackOverflowError.java:14)
	at cn.kobe.StackOverflowError.redo(StackOverflowError.java:14)
    * */
}
```



## 5.逃逸分析

内存预估：存货几十秒的对象在minor gc触发时大部分已经变成了垃圾对象，都可以被及时回收，基本不会被挪到老年代，这样可以大大减少老年代的full gc的次数；

![image-20210311195503962](C:\Users\86137\AppData\Roaming\Typora\typora-user-images\image-20210311195503962.png) 

JVM的运行模型：

- 解释模式：只使用解释器，执行一行代码就编译一一行为机器码
- 编译模式：之使用编译器，先将所有JVM字节码一次编译为机器码，然后一次性执行所有diamagnetic；
- 混合模式：依然使用解释模式执行代码，但是对于一些热点代码采用编译模式，混合执行；

解释模式执行快，对于只需执行一次的情况比较合适；编译模式要将所有编译为机器码，所以启动很慢，但是后期执行快，而且比较占用内存，这种模式适合需要反复执行的代码。混合模式是JVM默认采用的执行代码方式，一开始是解释模式，但是对于少部分热点代码采用编译模式，，这些热点代码会被缓存起来，再次执行将无需编译，也称之为JIT计时编译技术。（这个过程中JVM可能会对代码做一些优化，比如逃逸分析，指令重排和去除未被使用的变量）；

编译期间做的事：

https://blog.csdn.net/fuzhongmin05/article/details/54880257

```
1）词法分析：读取源代码，一个字节一个字节的读进来，找出这些词法中我们定义的语言关键词如：if、else、while等，识别哪些if是合法的哪些是不合法的。这个步骤就是词法分析过程。
    词法分析的结果：就是从源代码中找出了一些规范化的token流，就像人类语言中，给你一句话你要分辨出哪些是一个词语，哪些是标点符号，哪些是动词，哪些是名词。

  2）语法分析：就是对词法分析中得到的token流进行语法分析，这一步就是检查这些关键词组合在一起是不是符合Java语言规范。如if的后面是不是紧跟着一个布尔型判断表达式。
    语法分析的结果：就是形成一个符合Java语言规定的抽象语法树，抽象语法树是一个结构化的语法表达形式，它的作用是把语言的主要词法用一个结构化的形式组织在一起。                  这棵语法树可以被后面按照新的规则再重新组织。

  3）语义分析：语法分析完成之后也就不存在语法问题了，语义分析的主要工作就是把一些难懂的，复杂的语法转化成更简单的语法。              就如难懂的文言文转化为大家都懂的百话文，或者是注释一下一些不懂的成语。
     语义分析结果：就是将复杂的语法转化为简单的语法，对应到Java就是将foreach转化为for循环，还有一些注释等。                 最后生成一棵抽象的语法树，这棵语法树也就更接近目标语言的语法规则。

  4）字节码生成：将会根据经过注释的抽象语法树生成字节码，也就是将一个数据结构转化为另外一个数据结构。                就像将所有的中文词语翻译成英文单词后按照英文语法组装文英文语句。代码生成器的结果就是生成符合java虚拟机规范的字节码。
```

··`**语法分析就是检查源码是否符合java语法规范并将词组成语句。语义分析就是简化复杂的添加缺少的，检查变量类型是否合法。代码生成器就是遍历这棵树生成符合JVM规范的代码**。`

**对象逃逸分析**：就算分析对象动态作用域，当一个对象在方法中被定义后，它可能被外部方法引用，例如作为参数传递到其他地方去；

```java
public User test1(){
    User user=new User();
    user.setAge(12);
    user.setName("jjl");
    //TODO 保存到数据库
    return  user;
}
public void test2(){
    User user=new User();
    user.setAge(12);
    user.setName("jjl");
    //TODO 保存到数据库
}
/*分析：
    test1方法中创建的user可能会被其他地方引用（保存在堆内存中）；
    test2方法中的user不返回，对于这种对象JVM的逃逸优化就会将其内存分配在栈内存中，让其跟随方法一起被回收；JVM通过参数(-XX：+DoEscapeAnalysis)来优化对象内存分配，；到那时JDK7后默认开启逃逸分析，如果想关闭逃逸分析，使用参数（-XX：-DoEscapeAnalysis）
*/
```



## 6.JVM内存分配和回收

### 6.1对象分配机制

6.1.1对象优先在Eden区分配

​	大多数情况下，对象在新生代中Eden去分配。当其没有足够空间时，虚拟机将会发起一次Minor GC.

- ​	**Minor FC/Young GC** :指发生在新生代的垃圾收集动作，Minor GC 非常频繁，但是回收速度很快；
- ​    **Major GC/Full GC**: 全部回收（老年代，年轻代，方法区的垃圾），其速度比Minor GC会慢10倍以上；

6.1.2大对象直接放入老年代

  大对象即指大量需要连续内存的对象。JVM参数 -XX：PretenureSizeThreshold 可以设置大对象的判断值；注意：这个参数只有在Serial 和ParNew两个收集器下有效；

使用例子：-XX: PretenureSizeThreshold=1000000 -XX:+UsrSerialGC

  为什么要把大对象放入老年代呢？：为了避免在年轻代的多次交换分区时，为大对象分配内存的复制操作时而降低效率；

6.1.3 长期存活的对象将进入老年代

​	在年轻代中交换15次后依然存活的对象； 

6.1.4 动态年龄判断机制

​	当放对象的Survivor区中的一批对象的总大小大于这块s区内存大小的50%，那么后面大于等于这批对象中年龄的最大值的其他剩下对象都会被放入老年代。比如：s区中有一批对象，年龄1+年龄2+年龄3+...+年龄n+年龄n1+年龄n2...，其中1到n的对象的内存大于了s区的50%,那么就会把年龄大于n的对象放入老年代；这个规则的目的是希望：可以把那些可能长期存活的对象提前进入老年代。换个容易记忆的例子：提前退休；

6.1.5 老年代的担保机制

 嗯。。如图吧

![image-20210311214631196](C:\Users\86137\AppData\Roaming\Typora\typora-user-images\image-20210311214631196.png)

6.1.6 Eden区和Survivor区默认比例 8：1：1

​	JVM有参数：-XX+UserAdaptiveSizePolicy 可以让这个比例自动改变。如果不想让默认比例改变，可以使用参数：-XX: UserAdaptiveSizePolicy

### 6.2如何判断对象可以被回收



1.引用计数法：

​	给对象增加一个引用计数器，每当有一个地方引用它，计数器就加1；当引用失效就减一；任何一个对象的计数器为0就代表改对象可以被回收；

​	这个方法效率高，实现简单，但是存在一个问题：当两个对象互相引用时，会判断不能回收；比如：A指向B，B指向A。他们的计数器都不为0，故引用计数算法无法告知GC回收器来回收他们；

2.可达性分析法：

​	`一般以线程栈的本地变量，静态变量，本地方法栈为根节点`

​	以一个对象作为GC Roots 根，从这个节点向下搜索，找到的对象都被标记为非垃圾对象，其他的未标记对象都是垃圾对象；

3.引用类型：

​	java的引用一般氛围四种：强引用，软引用，弱引用，虚引用；

- ​	强引用：普通变量的引用；

  ```java
  public static User user=new User();
  ```

- 软引用：将对象通过SoftReference软引用类型对象报过起来；一般情况下不会被回收，但是GC做完后如果发生存放不下新的对象，则会把这些软引用对象回收掉。用途：可以用来实现内存敏感的高速缓存;

  ```java
  public static SoftReference<User> user=new SoftReference<>(new User());
  ```

  实际例子：（1）:网页浏览时的回退功能；通过软引用来缓存，而不是立即删除浏览过的页面；

- 弱引用：将对象通过WeakReference软引用类型对象报过起来；弱引用相当于没引用，GC会直接回收掉，很少用；

  ```
  public static WeakReference<User> user=new WeakReference<>(new User());
  ```

- 虚引用：虚引用也称为幽灵引用或者幻影引用，这是一种引用关系最弱，几乎不使用；

4.finalize()方法最终判断对象是否存活

​	这个方法可以在对象即将被回收前给它一个自我救赎的机会。

​	如果对象没有重写该方法会被直接回收;如下使用可以自我救赎（再次把自己复制给某个量引用）

![image-20210311221800108](C:\Users\86137\AppData\Roaming\Typora\typora-user-images\image-20210311221800108.png)

### 6.3标记清除算法

首先需要标记出所有需要回收的对象，在标记完成后统一回收所有被标记的对象。他是最基础的收集算法，效率也很高，但是存在两个问题：

- 效率问题
- 空间问题（标记清楚后会留下很多不连续的碎片）



![image-20210313104335287](C:\Users\86137\AppData\Roaming\Typora\typora-user-images\image-20210313104335287.png)

**6.3.2复制算法**

为了解决上诉问题：出现了复制收集算法，它将内存分为两个相同大小的部分，每次只使用其中一块。当其中一块使用完之后，就将还存活的对象移到另一块内存区域，然后把使用的空间一次性清理掉，这样就可以处理掉空间碎片。

![image-20210313104825958](C:\Users\86137\AppData\Roaming\Typora\typora-user-images\image-20210313104825958.png)

**6.3.3标记整理算法**

 该算法在后续的步骤中不是直接对可回收对象的 回收，而是让存活的对象统一移动到一起，占用一段连续的内存，然后清理其他碎片。

![image-20210313105254793](C:\Users\86137\AppData\Roaming\Typora\typora-user-images\image-20210313105254793.png)

**6.3.4分代收集算法**

把不同存活周期的对象分为几块，一般将java兑分为新生代和老年代，然后根据不同代区选择不同算法；

比如，在新生代中，每次收集都会有大量对象死去，所以选择复制算法。而老年代存活几率比较高，选择标记清除或者标记复制算法；（标记清楚比标记整理慢10倍以上）

## 7.垃圾收集器

垃圾收集器救赎垃圾回收算法的具体实现；包含以下几种：

![垃圾收集器 ](C:\Users\86137\AppData\Roaming\Typora\typora-user-images\image-20210313105955916.png)



**7.1Serial收集器**（-XX：+UseSerialGC -XX:+UseSerialOldGC）

最基本的收集器：单线程收集；它在进行垃圾收集的时候必须暂停其他所有的线程；（stw: stop the  world）,直到它的工作结束；

`新生代采用复制算法，老年代采用标记整理算法；`

特点：简单高效。

问题：单线程处理，如果系统是多核的，将不能充分利用系统资源；

SerialOldGC：是Serial的老版本；他有两个用途，一种是将jdk1.5以前的版本和Parallel Scavenge收集器搭配使用;还用来作为CMS收集器的后备方案；

![image-20210313120941202](C:\Users\86137\AppData\Roaming\Typora\typora-user-images\image-20210313120941202.png)

**7.2ParNew收集器**（-XX：+UseParNewGC）

该收集器是Serial的多线程版本。默认收集器线程数和cpu核数相同，也可以通过参数修改，但是不建议修改；

```
-XX：ParallelGCThreads
```

`新生代采用复制算法，老年代采用标记整理算法；`

![ParNew收集器](C:\Users\86137\AppData\Roaming\Typora\typora-user-images\image-20210313121920456.png)

**7.3Parallel Scavenge收集器**（-XX:+UseParallelGC -XX:+UseParallelOldGC）

和ParNew收集器类似，但是更注重于用户体验，也就是能通过参数修改用户线程停顿的时间（在内存大于2G，2个cpu时）



`新生代采用复制算法，老年代采用标记整理算法；`



ParallelOldGC是Parallel Scavenge收集器的老版本；在注重吞吐量和cpu资源的情况下，都可以可以这两个收集器！

![Parallel Scavenge收集器](C:\Users\86137\AppData\Roaming\Typora\typora-user-images\image-20210313121920456.png)

**7.3Parallel Scavenge收集器**（-XX:+UseConcMarkSweepGC(old) ）

cms(concurrent mark sweep) 收集器是一种以获取最短停顿时间为目标的收集器，很适合实际使用。是HotSpot虚拟机第一款真正意义上的收集器，第一次是实现了垃圾收集器和用户线程用时同坐。

`cms采用标记清除算法`，他的运行过程比前面几种略显复杂。整个过程分为如下几步：

- 初始标记：stw并记录下gc roots能直接引用的对象（速度很快）；
- 并发标记：开启gc线程和用户线程，并用一个闭包结果来记录所有可达的对象；但是这个过程不能保证实时性，所以会记录不完整，但是能通过记录修改了的地方追踪这些引用更新了的地方。（速度最慢）
- 重新标记：stw(这很重要),修正上一步中没有实时更新的引用;(比初始标记略微慢一点)
- 并发清理：同时开启用户线程和gc，同时工作。
- 并发重置：

特点：并发运行，低停顿。

缺点：和服务线程抢占cpu资源；无法处理浮动垃圾；标记清除算法会产生内存碎片；

问题：执行过程中不确定性，上一次垃圾回收还没执行结束，下一次的gc又来了；尤其在并发标记和并发清理阶段出现（这两个地方耗时长）；此时会静茹stw 用serial old 垃圾收集器来回收（单线程）；

cms的参数百度。

![image-20210313125831163](C:\Users\86137\AppData\Roaming\Typora\typora-user-images\image-20210313125831163.png)



7.4 G1收集器（-XX:+UseG1GC）

是一款面向服务器的收集器，主要针对多颗处理器及大容 量内存的机器，以极高概率满足GC停顿时间要求的同时，还具备高吞吐量的性能特征； 

G1将java对分为多个大小相等的区域，JVM最多有2048个Region;单个区域大小为堆内存大小除2048，也可也用参数：-XX：G1HeapRegionSize指定，一般推荐使用默认；

默认年轻代占用5%堆内存，在系统运行的过程中，系统会不断给年轻代增加内存，但是最多不会超过60%；年轻代中Eden和Survivor的大小默认也是8:1:1;一个Region可能当前是年轻代，但是经过一个GC后可能会变成老年代，也就是说，Region区域功能是动态变化的。

G1收集器唯一不同的是对大对象的处理，有专门的Region叫做Humongous区（大对象：超过当前Region大小的50

%） ，这样做的目的是让小对象有更多空间去存放，

![image-20210313170501842](C:\Users\86137\AppData\Roaming\Typora\typora-user-images\image-20210313170501842.png)

G1收集器的一次GC的运行过程：

- 初始标记：同CMS  stw
- 并发标记：同CMS    
- 最终标记：同CMS的重新标记    stw
- 筛选回收：(stw)筛选回收阶段首先将各个Region的回收价值和成本进行排序，根据用户设置的预期停顿时间来指定回收计划，从小到大回收，直到时间超过设定值，然后停止gc；整个期间是停顿用户线程，来大幅度提高收集效率；年轻代和老年代使用的都主要是复制算法：将一个region中存活的对象转移到另一个region，不会像cms一样再回收结束后产生很多内存碎片还需要再整理一次；（补充：当一个region内存被回收一部分，另一个也被回收一部分，G1收集器就会把这两个剩余的内存对象转移到一个region下，然后清除原来那两个region）

![image-20210313174235906](C:\Users\86137\AppData\Roaming\Typora\typora-user-images\image-20210313174235906.png)

![image-20210313174331377](C:\Users\86137\AppData\Roaming\Typora\typora-user-images\image-20210313174331377.png)

**G1收集器的分类:**

`YoungGC`

​		YoungGC并不是说Eden区放满了就会立即被触发，G1会计算Eden区回收大概需要多长时间，如果远远小于参数设定值，就i增加年轻代region，继续给空间存放对象；直到下一次Eden区放满了，会再次比较；

`MixedGC`

​		和Full GC 有区别，当老年代的对占有率达到参数设定值就会触发，会回收所有的Young和部分Old以及大对象区；正常情况下G1会先执行MixedGC，期间使用`复制算法`,需要把各个regio n中存活的对象转移到别的rgion中去，拷转移过程中如果发生没有足够的region就会触发一次Full GC；

`Full GC`

​		停止系统程序，然后采用单线程进行标记，清理和整理，腾出一些region来给下一次MidedGC提供空间，注意：这个过程十分耗时；

`·优化参考·`

![image-20210313180539867](C:\Users\86137\AppData\Roaming\Typora\typora-user-images\image-20210313180539867.png)

## 案例分析

PreNew和





编写一个程序，打印输入中的单词长度的直方图，需要输出水平直方图和垂直直方图；

比如 hello welcome to 3g

长度：5		7			2   2



<img src="C:\Users\86137\AppData\Roaming\Typora\typora-user-images\image-20210314122718672.png" alt="image-20210314122718672" style="zoom:50%;" /><img src="C:\Users\86137\AppData\Roaming\Typora\typora-user-images\image-20210314122436481.png" alt="image-20210314122436481" style="zoom: 50%;" />




































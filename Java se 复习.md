#  Java se 复习

## 1.基本类型：

## 包装类型

基本类型都有对应的包装类型，基本类型与其对应的包装类型之间的赋值使用自动装箱与拆箱完成。

```
Integer x = 2;     // 装箱 调用了 Integer.valueOf(2)
int y = x;         // 拆箱 调用了 X.intValue()
```

## 缓存池

new Integer(123) 与 Integer.valueOf(123) 的区别在于：

- new Integer(123) 每次都会新建一个对象；
- Integer.valueOf(123) 会使用缓存池中的对象，多次调用会取得同一个对象的引用。
- java8中Integer缓存池的大小默认是-128~127
- 并且缓冲池的大小可调，-XX:AutoBoxCacheMax=<size> 来指定

## 2.String :

[参考链接](https://www.cnblogs.com/scywkl/p/12489672.html)

```
java8之前 使用char[]数组存储数据
private final char value[];
java9之后使用 ，同时用coder来标识使用了那种编码
private final byte[] value;
private final byte coder;
```

String 的构造方法:

```
public String() {
        this.value = "".value;
    }
public String(char value[]) {}
public String(char/int/byte value[], int offset, int count) 
public String(StringBuilder builder)
public String(StringBuffer buffer)
```

equlas:基本类型比较值，引用类型比较地址，可以重写

compareTo():同样是比较两个字符串，返回整数（但是不能接受Object类型的变量）

重写了equals方法把引用改成了对字符串值的比较。

常用方法：

```
indexOf():返回字符串首次出现的下标
lastIndexOf():返回~最后出现的下标
contians():查询字符串中是否包含另一个字符串
toLowerCase():把字符串转换成小写
length():返回长度
trim():去掉首位空格
replace():替换字符串中的某些字符
split(): 把字符串分割并返回字符串数组
join(): 把字符串数组转为字符串
```

常问问题：

1. 为什么用final修饰String？

   - 高效：如果可变的话，当传参数的时候就要考虑谁会修改它的值！有可能需要拷贝一个值去传参。
   - 安全：会改变字符串常量池中的数据，造成不可预估的错误！

2. ==和equals的区别?

   String中的equals()就是重写了Object中的equals()，前者的实现基本靠==实现！

3. String，StringBuilder,StringBuffer的区别？

   1 					2					3

   1因为安全问题，在拼接字符串时会照成性能损耗。2，3都可以拼接字符串，但是3内部使用了synchronized来保证线程安全，性能不如2;2在jdk1.5中推出；

4. String 中的intern()方法有什么含义？

   ​	intern()把字符串放入常量池

   ```
    String s1 = new String("java");
              String s2 = s1.intern();
              String s3 = "java";
              System.out.println(s1==s2); //false
              System.out.println(s2==s3); //true
   ```

   

5. String 类型再JVM中是如何存储的？编译器对String 类型做了哪些优化？

   ```
   String s1="java";//先在常量池获取，没有创建之后再放入常量池
           String s2=new String("java");//在堆中创建对象
   ```

   优化如下：

   ```
   String s1="ja"+"va";
   String s2="java";
   Sout(s1==s2);//true
   ```


## @Mapper 与 @MapperScan 的区别

前者是对单个接口的注解，编译之后会生成其相应的接口实现类！后者是相当于多个@Mapper的操作，可以指定单个或多个包!

![image-20210118114041446](C:\Users\86137\AppData\Roaming\Typora\typora-user-images\image-20210118114041446.png)

## Model类型

```
Model 只有寥寥几个方法只适合用于储存数据，简化了新手对于Model对象的操作和理解；

ModelMap 继承了 LinkedMap ，除了实现了自身的一些方法，同样的继承 LinkedMap 的方法和特性；

ModelAndView 可以在储存数据的同时，可以进行设置返回的逻辑视图，进行控制展示层的跳转。
```

# 遇到的问题

- make (redis)时编译出错时，清除编译生成的文件     不执行也可以

  make distclean

- xshell解决linux安装软件：/lib/ld-linux.so.2: bad ELF interpreter问题需要使用yum install glibc.i686命令，把32位的换成64位

- xshell中连普通命令都找不到：可以执行：export PATH=/bin:/usr/bin:$PATH

- **export PATH=/usr/local/sbin:/usr/local/bin:/sbin:/bin:/usr/sbin:/usr/bin:/root/bin**

- -----直接/css就可以，不用回退

- 连接数据库时：Server returns invalid timezone. Go to 'Advanced' tab and set 'serverTimezone' property manually.

  -------------------set global time_zone = '+8:00'; 

- ```
  <!--错误提示信息！-->
  <p style="color: red" th:text="${msg}"  th:if="${not #strings.isEmpty(msg)}"></p>
  ```

- **重定向和转发的区别**：

- log4j的使用：一个依赖：

  ```
  <dependency>
      <groupId>log4j</groupId>
      <artifactId>log4j</artifactId>
      <version>1.2.17</version>
  </dependency>
  
  ---必须加在当项目里
  ```

  一个配置文件log4j.properties

  一个配置属性：

  ```
  <settings>
              <setting name="logImpl" value="LOG4J"/>
  </settings>
  ```
  
- Thymeleaf整合Spring Security，解析sec标签没有效果:

  ```java
  导入依赖
  <dependency>
              <groupId>org.thymeleaf.extras</groupId>
              <artifactId>thymeleaf-extras-springsecurity5</artifactId>
   </dependency>
   修改规范  
  <html xmlns:th="http://www.thymeleaf.org"
     xmlns:sec="http://www.thymeleaf.org/extras/spring-security">
   
  ```

- Data truncated for column 'signed' at row 1：字段类型与数据类型不同，或者类型长度不够储存数据！

- select
      count(*)  ----> 不能写成 count  (*) 即不能有空格
  
- ###### 数据库插入数据返回当前主键ID值方法

  原理：最新创建的id最大，使用两次查询，但是这样不保证原子性；

  - 使用MyBatis框架xml文件中：

  ```xml
  <insert id="addUser" parameterType="User">
      insert into candidate values(null, #{uname},#{mobile},#{sex},#{address},#{imgurl})
      <selectKey keyProperty="uid" order="AFTER" resultType="int">
         select LAST_INSERT_ID()
      </selectKey>
  
  </insert>
  属性含义：
  
      keyProperty：java对象中的属性名
  
      order：有两个值，AFTER 和 BEFORE。（注意，必须是大写）
  
      resultType：返回值得类型
  
  插入数据后，id就储存在传入的参数中了(传入一个对象)
  ```

  - 直接的sql语句

    ```sql
    insert into 
    sms_message (phone,sms_code) 
    values 
    ('12312','test') ;select @@Identity	
    ```


  所有的地址栏请求默认都会是 HTTP GET 类型的。所以直接访问接口是不行的！

  **访问提交的是一个对象**

  要求提交的表单域和对象的属性名一致  , 参数使用对象即可

  1、实体类

  ```
  public class User {
     private int id;
     private String name;
     private int age;
     //构造
     //get/set
     //tostring()
  }
  ```

  2、提交数据 : http://localhost:8080/mvc04/user?name=kuangshen&id=1&age=15

  3、处理方法 :

  ```
  @RequestMapping("/user")
  public String user(User user){
     System.out.println(user);
     return "hello";
  }
  ```

  后台输出 : User { id=1, name='kuangshen', age=15 }

  说明：如果使用对象的话，前端传递的参数名和对象名必须一致，否则就是null。

- 获取最新创建的id:   select @@identity; （前面记过的。。。）
- 创建表时注意的字符集：Engine=innodb  auto_increment=2;



- (不同jar有相同的类)qq的sdk和Jwt中的base64会冲突！！！！类的加载和jar导入的顺序有关，所以我把pom里的qqsdk换到最后，然后就用的commons包的base64

unsafe类的访问限制去掉箭头下的信息

![image-20210414220555488](C:\Users\86137\AppData\Roaming\Typora\typora-user-images\image-20210414220555488.png)



# 参考：

## [mybatis官方文档](https://mybatis.org/mybatis-3/zh/index.html)

## [spring官方文档](https://spring.io/projects)

## thymeleaf依赖

```java
<dependency>
    <groupId>org.thymeleaf</groupId>
    <artifactId>thymeleaf-spring5</artifactId>
</dependency>
<dependency>
    <groupId>org.thymeleaf.extras</groupId>
    <artifactId>thymeleaf-extras-java8time</artifactId>
</dependency>
    
 ------------
    命名规范
    <html xmlns:th="http://www.thymeleaf.org"
      xmlns:sec="http://www.thymeleaf.org/extras/spring-security" >
```

## shiro依赖

```html
<!--shiro-->
<dependency>
    <groupId>org.apache.shiro</groupId>
    <artifactId>shiro-spring</artifactId>
    <version>1.6.0</version>
</dependency>
```

远程连接Redis

```
1.把redis.conf中的bind注释掉
2.protected-mode no
3.添加 requirepass 123456
4.防火墙firewall-cmd --zone=public --add-port=6379/tcp --permanent
5.重启systemctl restart firewalld
6.退出redis的server，然后重新登录（密码123456）
7.在阿里云开放6379端口，服务器实例的安全组中添加；
8.jedis连接
```

```java
public static void main(String[] args) {
        Jedis jedis = new Jedis("xxx服务器ipxxx", 6379);
        jedis.auth("123456");
        String response = jedis.ping();
        System.out.println(response); // PONG
 }
```


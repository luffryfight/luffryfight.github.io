# JWT笔记

## 1.JWT是什么

JWT(jsonwebtoken)是一个开放标准（rfc7518），它定义了一种紧凑自包含的方式，用于在各方之间以json形式传输信息。此信息可以通过验证来确定其正确性。jwt可以使用密钥（HMAC算法）或者RSA或者ECDSA的公钥/私钥进行签名；

--jwt检查json web token ，也就是通过json形式作为web应用的令牌，用于在各方之间安全传输对象。在传输过程中可以完成数据的加密和签名等相关操作。

## 2.JWT能做什么

1.授权：

这是jwt常见的方案。一旦用户登录后，后续的每个请求都要在传递jwt的token，从而允许用户访问该令牌允许的路由，服务和资源。单点登录是现在最常用的jwt的功能，因为jwt的开销小并且可以在不同域中轻松使用；

2.信息交换

jwt是在各方之间安全交换信息的一种很好的方式。因为jwt可以通过签名进行加密，可以确保发件人是他们说的人。此外，由于签名是使用表头和有效负载计算的，还可以验证内容是否被修改过。



注意：jwt是以token形式保存在客户端的，所以当服务器断电重启后，通过jwt仍然可以调用接口（当然token没有过期），而session是断电即失；

## 3.为什么是JWT

### 传统的认证方式

传统的认证方式是通过session的；

`1.认证方式`：

http协议是一种无状态的协议，这意味着当用户带着用户名和密码来进行验证后，他的下次请求仍然需要进行验证，因为http不能保存上次的登录信息。所以为了避免这个麻烦，所以我们在服务器存储一份用户登录信息，保存为cookie并返回，下次请求带着这个cookie就可以识别是哪个用户了。这就是传统的session认证；

`2.认证流程`

<img src="https://img-blog.csdnimg.cn/20201022130410229.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3VuaXF1ZV9wZXJmZWN0,size_16,color_FFFFFF,t_70#pic_center" style="zoom: 150%;" />

`3.暴露的问题`

1.每个用户经过我们的应用认证之后，我们的应用都要在服务端做一次记录，以方便用户下次请求的鉴别，通常而言session都是保存在内存中，而随着认证用户的增多，服务端的开销会明显增大

2.用户认证之后，服务端做认证记录，如果认证的记录被保存在内存中的话，这意味着用户下次请求还必须要请求在这台服务器上,这样才能拿到授权的资源，这样在分布式的应用上，相应的限制了负载均衡器的能力。这也意味着限制了应用的扩展能力。

3.因为是基于cookie来进行用户识别的, cookie如果被截获，用户就会很容易受到跨站请求伪造的攻击。

4.在前后端分离系统中就更加痛苦:如下图所示
也就是说前后端分离在应用解耦后增加了部署的复杂性。通常用户一次请求就要转发多次。如果用session 每次携带sessionid 到服务	器，服务器还要查询用户信息。同时如果用户很多。这些信息存储在服务器内存中，给服务器增加负担。还有就是CSRF（跨站伪造请求攻	击）攻击，session是基于cookie进行用户识别的, cookie如果被截获，用户就会很容易受到跨站请求伪造的攻击。还有就是	     sessionid就是一个特征值，表达的信息不够丰富。不容易扩展。而且如果你后端应用是多节点部署。那么就需要实现session共享机制。	不方便集群应用。

### 基于JWT的认证方式

### ![JWT](https://img-blog.csdnimg.cn/20201022130449432.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3VuaXF1ZV9wZXJmZWN0,size_16,color_FFFFFF,t_70#pic_center)

`1.认证方式`：

​	首先，前端提交用户的账号和密码发送到后端的接口。这个过程是一个http 的post请求；这里建议使用http+ssl（https）的方式，从而避免敏感信息被探测；

后端核对账号和密码后，然后一个以id等信息作为负载，将其与头部进行base64编码后签名，形成一个jwt的token返回给前段；前端可以将其保存在localStorage或者sessionStroage上；退出时前段删除保存的jwt即可；

前端在请求时将jwt放入http header 中的Authorization中可以避免XSS和XSRF问题；

后端可以验证该jwt是否正确，然后利用其中的用户信息进行其他操作；



`2.jwt的优势`：

​		其实也就是相对于session的缺点；

简洁：可以通过URL，post请求的参数或者在http header发送，因为数据量小，传输速度也很快；

自包含：负载（）中包含了用户的信息，可以减少数据库的查询；

跨域：token是json形式的，原则上任何web形式都支持；

支持分布式：因为token是保存在客户端的，所以特别使用分布式微服务；



## 4.JWT的结构

```
格式：
header.payload.signature
举例：
eyJhbGciOiJIUzI1NiIs.eyJzdWIiOiIxMjM0NTY3ODvaW4iOnRydWV9.TJVA95OrM7E2cB
```

### 1.第一串：header

头部通常由两部分组成：令牌的类型和使用的签名算法；然后通过Base64编码组成JWT的header。

注意：Base64只是一种编码；

```
{
	"alg": "HS256"，
	"typ": "JWT"
}
```

### 2.第二串：payload

其中包含:相关声明，有关实体（一般为用户）和其他一些数据。然后通过Base64编码成第二部分；

注意：其中不要包含敏感信息，如果含有密码等信息，容易被别人登录，造成不必要的损失；

### 3.第三串：signature

signature=header+payload ，然后再结合secret（秘钥)和加密算法（header中包含）加密，加密之后可以判断jwt是否被修改过；
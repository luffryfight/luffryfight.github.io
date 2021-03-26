# Mysql笔记：

预读：

​	`MySql根据B+Tree的特性和局部性原理，将B+Tree节点的大小设为等于一个页，每次新建节点直接申请一个页的空间，这样就保证一个节点物理上也存储在一个页里，就实现了一个节点的载入只需一次I/O，大大减少磁盘I/O次数，提高获取数据的效率。同时B+Tree的度d一般会超过100，因此h非常小(一般为3到5之间)，只需要几次I/O操作就能把索引检索完，效率非常高。`

------------------------------------------------
版权声明：本文为CSDN博主「街灯下的小草」的原创文章，遵循CC 4.0 BY-SA版权协议，转载请附上原文出处链接及本声明。
原文链接：https://blog.csdn.net/yhl_jxy/article/details/88392411

### 索引的本质：

索引就是帮助Mysql高效获取数据的排好序的数据结构。

索引数据结构：

**判断那种结构的好坏，通过磁盘判读读取IO的次数来判断**

- 二叉树
- 红黑树
- Hash表
- B-树

<img src="C:\Users\86137\AppData\Roaming\Typora\typora-user-images\image-20210318215038446.png" alt="image-20210318215038446" style="zoom:50%;" />



**二叉树**：

二叉树可以通过logn复杂度查询到数据，但是存在一个问题：当插入的数据是已经排好序的，比如1 2 3 4 5 ，那么二叉树就会退化为线性结构，从而降低效率；

**红黑树**：

也是优化的二叉树，通过自旋避免退化成线性结构。红黑树同样存在一些问题：当数据量特别大时： 树的高度就会很高2^n=500w ;n为高度，也就是读取io的次数；这种情况下的操作读取IO的次数也是比较多的；



**Hash表**

Hash表在查询某个数据时可以有很高的效率；但是同样也存在问题：Hash无法范围查找和模糊查询；

### B-树的结构

- 叶子节点的高度都相同，且叶子节点的指针为空。
- 所有索引不重复
- 节点中的数据索引从左到右依次递增；

![image-20210319131128817](C:\Users\86137\AppData\Roaming\Typora\typora-user-images\image-20210319131128817.png)

```
问题：
如果使用的是InnoDB的搜索引擎(聚集)，那么每个节点（页）存储的数据就会是包含完整的数据记录。当data的大小变化时，B-树的高度也会随之变化，也就是不可控了。但是仍然比红黑树更具有优势；
```



### B+树的结构

- 非叶子节点不储存数据，只存索引，从而可以存放更多索引
- 叶子节点包含所有索引字段
- 叶子节点间相互连接，可以提高访问性能；（是一个双向循环链表）

<img src="C:\Users\86137\AppData\Roaming\Typora\typora-user-images\image-20210318221550243.png" alt="image-20210318221550243" style="zoom:50%;" />



```
sql  查看mysql文件分页（也就是一个叶子）的大小： show global status like 'innodb_page_size';
问题：
为什么mysql的分页大小默认为16k？
这是经过官方测试后，性能较高的值；假如一行数据为1k那么一个页子节点数据就能存16条数据；再看非叶子节点，假如主键id是bigint类型（长度为8b），指针大小在InnoDB中为6b，那么一页就可以存 16kb/14b =1170 个（指针+主键）；然后一棵高度为2的B+树可以存放1170*16=18720条数据，高度为3的B+树可以存放1170*1170*16=21 902 400 （2kw）；
如果默认大小为8k的话，有什么影响呢？：查看网上的资料：16k对cpu的压力影响要小很多，但是8k的select的吞吐量高一些；
```



### MyISAM存储引擎实现

MyISAM的索引文件和数据文件是分离的，也就是非聚集的。

<img src="C:\Users\86137\AppData\Roaming\Typora\typora-user-images\image-20210318221431881.png" alt="image-20210318221431881" style="zoom:67%;" />



### InnoDB存储引擎索引实现

InnoDB索引实现（聚集），InnoDB是修饰表结构的，也就是说一个库的搜索引擎是Innodb，但是这个库里的表不一定都是Innodb的；

- 表数据文件本身就是按B+Tree构成的一个索引结构的文件
- 聚集：叶 子节点索引包含完整的数据记录

![image-20210319145208131](C:\Users\86137\AppData\Roaming\Typora\typora-user-images\image-20210319145208131.png)



<img src="C:\Users\86137\AppData\Roaming\Typora\typora-user-images\image-20210318221307296.png" alt="image-20210318221307296" style="zoom: 50%;" /><img src="C:\Users\86137\AppData\Roaming\Typora\typora-user-images\image-20210318221324408.png" alt="image-20210318221324408" style="zoom:50%;" />

##### ==问题==：

```
为什么InnoDB 表必须有主键，并且推荐使用整型的自增主键？
	因为InnoDB是按照B+树组织的索引结构，索引不能重复，不为空。而主键刚好符合这个要求，所以必须要有主键。补充：如果设置了主键，InnoDB会选择主键作为聚集索引；如果没有设置主键，InnoDB会选择一个不包含null且值唯一的字段作为索引；如果这样的字段也没有，那么InnoDB就会内置一个6字节rowid作为隐私的聚集索引（该索引也是自增的。）
	不是整型的主键比如uuid 字符串,存在两个问题，其一是字符串比较大小会比整型比较大小麻烦很多；其二是字符串更占用空间，会减少一个分页存储索引数目；使用自增的原因是：如果主键自增的话，新增的节点就会假如到叶子节点的最后；如果随便假如一个大小的索引可能造成B+树的节点的分裂和增加树的高度（即平衡效率）；
	
为什么非主键索引结构的叶子节点存储的是主键值？
	一致性：如果非主键索引存储的所有数据，当数据发生变化，在需要修改主键索引上的数据的时候，也需要维护非主键索引上的数据，这个过程必须保证一致性，（也就是事务的过程），这样就降低了性能。如果存储主键，那么二级索引就不需要修改什么（二级索引会先查找到主键值，然后通过主键索引查找数据）；
	节省空间：
如果每个索引的叶子节点都存储索引数据，那么有多少个索引就创建多少个这样的结构，大大地浪费了空间。

```















### 索引最前缀原理

联合索引的底层存储结构是什么样的？

一个字段字段地比较，从左往右是从大到小；

<img src="C:\Users\86137\AppData\Roaming\Typora\typora-user-images\image-20210318220522532.png" alt="image-20210318220522532" style="zoom:50%;" />

比如有通过三个字段id，name, date构建联合索引，如下：

最左前缀原理就是多个字段构建的联合索引，最左边的字段优先比较。







## 第二节：Explain分析

一个explain语句会出现的字段

![image-20210326092111807](C:\Users\86137\AppData\Roaming\Typora\typora-user-images\image-20210326092111807.png)

每个字段代表什么意思自行查看资料，这里不再重复补充（或者以后有需要再说）。

其中Extra字段中有一个是 Using index：表示查询的字段都在索引树中，e.g：explain select * from film left join film_actor on film.id=film_actor.film_id;

**索引使用总结** 

​	看了之后能回忆起大部分！like KK%相当于=常量，%KK和%KK% 相当于范围

![image-20210326094407448](https://raw.githubusercontent.com/luffryfight/picgo/master/data/20210326094424.png)

![image-20210326094608305](C:\Users\86137\AppData\Roaming\Typora\typora-user-images\image-20210326094608305.png)



## 第三节：Mysql优化实战（一）

实际中可能会出现下面这种情况:

```
EXPLAIN select * from employees where name > 'a';
第一行中查询字段为所有，通过联合索引树后还需要去主键索引中获取数据，扫描时间成本就会增加，此时可以通过覆盖索引，让查询字段都在索引中（在本来就只需要这几个字段的情况下）；
EXPLAIN select name,age,position from employees where name > 'a' ;-- 这就是覆盖索引
EXPLAIN select * from employees where name > 'zzz' ;
现在出现的问题是：name > 'a' 和 name > 'zzz' 现在如何判断mysql怎么选择索引?mysql内部会有自己的性能计算，智能通过trace工具临时分析；
```

`trace分析`:

> select * from employees where name >'a' order by position;
> 		select * from information_schema.OPTIMIZER_TRACE;

```json
{
  "steps": [
    {
      "join_preparation": {
        "select#": 1,
        "steps": [
          {
            "expanded_query": "/* select#1 */ select `employees`.`id` AS `id`,`employees`.`name` AS `name`,`employees`.`age` AS `age`,`employees`.`position` AS `position`,`employees`.`hire_time` AS `hire_time` from `employees` where (`employees`.`name` > 'a') order by `employees`.`position`"
          }
        ] /* steps */
      } /* join_preparation */
    },
    {
      "join_optimization": {
        "select#": 1,
        "steps": [
          {
            "condition_processing": {
              "condition": "WHERE",
              "original_condition": "(`employees`.`name` > 'a')",
              "steps": [
                {
                  "transformation": "equality_propagation",
                  "resulting_condition": "(`employees`.`name` > 'a')"
                },
                {
                  "transformation": "constant_propagation",
                  "resulting_condition": "(`employees`.`name` > 'a')"
                },
                {
                  "transformation": "trivial_condition_removal",
                  "resulting_condition": "(`employees`.`name` > 'a')"
                }
              ] /* steps */
            } /* condition_processing */
          },
          {
            "substitute_generated_columns": {
            } /* substitute_generated_columns */
          },
          {
            "table_dependencies": [
              {
                "table": "`employees`",
                "row_may_be_null": false,
                "map_bit": 0,
                "depends_on_map_bits": [
                ] /* depends_on_map_bits */
              }
            ] /* table_dependencies */
          },
          {
            "ref_optimizer_key_uses": [
            ] /* ref_optimizer_key_uses */
          },
          {
            "rows_estimation": [
              {
                "table": "`employees`",
                "range_analysis": {
                  "table_scan": {
                    "rows": 3,
                    "cost": 3.7
                  } /* table_scan */,
                  "potential_range_indexes": [
                    {
                      "index": "PRIMARY",
                      "usable": false,
                      "cause": "not_applicable"
                    },
                    {
                      "index": "idx_name_age_position",
                      "usable": true,
                      "key_parts": [
                        "name",
                        "age",
                        "position",
                        "id"
                      ] /* key_parts */
                    },
                    {
                      "index": "idx_hire_time",
                      "usable": false,
                      "cause": "not_applicable"
                    }
                  ] /* potential_range_indexes */,
                  "setup_range_conditions": [
                  ] /* setup_range_conditions */,
                  "group_index_range": {
                    "chosen": false,
                    "cause": "not_group_by_or_distinct"
                  } /* group_index_range */,
                  "analyzing_range_alternatives": {
                    "range_scan_alternatives": [
                      {
                        "index": "idx_name_age_position",
                        "ranges": [
                          "a < name"
                        ] /* ranges */,
                        "index_dives_for_eq_ranges": true,
                        "rowid_ordered": false,
                        "using_mrr": false,
                        "index_only": false,
                        "rows": 3,
                        "cost": 4.61,
                        "chosen": false,
                        "cause": "cost"
                      }
                    ] /* range_scan_alternatives */,
                    "analyzing_roworder_intersect": {
                      "usable": false,
                      "cause": "too_few_roworder_scans"
                    } /* analyzing_roworder_intersect */
                  } /* analyzing_range_alternatives */
                } /* range_analysis */
              }
            ] /* rows_estimation */
          },
          {
            "considered_execution_plans": [
              {
                "plan_prefix": [
                ] /* plan_prefix */,
                "table": "`employees`",
                "best_access_path": {
                  "considered_access_paths": [
                    {
                      "rows_to_scan": 3,
                      "access_type": "scan",
                      "resulting_rows": 3,
                      "cost": 1.6,
                      "chosen": true,
                      "use_tmp_table": true
                    }
                  ] /* considered_access_paths */
                } /* best_access_path */,
                "condition_filtering_pct": 100,
                "rows_for_plan": 3,
                "cost_for_plan": 1.6,
                "sort_cost": 3,
                "new_cost_for_plan": 4.6,
                "chosen": true
              }
            ] /* considered_execution_plans */
          },
          {
            "attaching_conditions_to_tables": {
              "original_condition": "(`employees`.`name` > 'a')",
              "attached_conditions_computation": [
              ] /* attached_conditions_computation */,
              "attached_conditions_summary": [
                {
                  "table": "`employees`",
                  "attached": "(`employees`.`name` > 'a')"
                }
              ] /* attached_conditions_summary */
            } /* attaching_conditions_to_tables */
          },
          {
            "clause_processing": {
              "clause": "ORDER BY",
              "original_clause": "`employees`.`position`",
              "items": [
                {
                  "item": "`employees`.`position`"
                }
              ] /* items */,
              "resulting_clause_is_simple": true,
              "resulting_clause": "`employees`.`position`"
            } /* clause_processing */
          },
          {
            "reconsidering_access_paths_for_index_ordering": {
              "clause": "ORDER BY",
              "steps": [
              ] /* steps */,
              "index_order_summary": {
                "table": "`employees`",
                "index_provides_order": false,
                "order_direction": "undefined",
                "index": "unknown",
                "plan_changed": false
              } /* index_order_summary */
            } /* reconsidering_access_paths_for_index_ordering */
          },
          {
            "refine_plan": [
              {
                "table": "`employees`"
              }
            ] /* refine_plan */
          }
        ] /* steps */
      } /* join_optimization */
    },
    {
      "join_execution": {
        "select#": 1,
        "steps": [
          {
            "filesort_information": [
              {
                "direction": "asc",
                "table": "`employees`",
                "field": "position"
              }
            ] /* filesort_information */,
            "filesort_priority_queue_optimization": {
              "usable": false,
              "cause": "not applicable (no LIMIT)"
            } /* filesort_priority_queue_optimization */,
            "filesort_execution": [
            ] /* filesort_execution */,
            "filesort_summary": {
              "rows": 3,
              "examined_rows": 3,
              "number_of_tmp_files": 0,
              "sort_buffer_size": 200704,
              "sort_mode": "<sort_key, packed_additional_fields>"
            } /* filesort_summary */
          }
        ] /* steps */
      } /* join_execution */
    }
  ] /* steps */
}
```

#### **Order by /Group by的优化**

- Mysql支持两种排序：filesort和index；Using index是指Mysql**扫描索引**本身来完成排序。index相比于filesort效率高很多。
- 如果order by和where 的条件组合语句满足索引最左前缀条件，就会通过index排序。
- 尽量在索引上来完成排序，条件顺序满足建立时的顺序；
- 尽量使用覆盖索引
- group by是先排序后分组，同样也遵循索引最左前缀原则。如果使用group by 并且不需要排序的话，可以用order by null禁止排序。注意，where 优先于having ，能写在where中的条件就不要写在having中了；

**Using filesort文件排序原理：**

`select * from employees where name = 'zhuge' order by position;`

- 单路排序	一次性取出所有满足条件的数据，放入sort buffer ,然后再进行排序；在trace中可以看到以下信息：

  ​				sort_mode`<sort_key,packed_additional_fields>`

  - 从索引中找到  name = 'zhuge' 的id
  - 然后通过聚集索引和id读出该id对应的所有数据，放入sort buffer中
  - 重复上面两步，直到所有满足条件的数据被读取完毕
  - 把sort buffer中的所有数据按照position字段进行排序；
  - 返回结果

- 双路排序  首先从条件中取出相应排序字段和可以直接定位的id，在sort buffer中排序。在trace中有信息：

  ​				`sort_mode<sort_key,rowid>`

  - 从索引中找到   name = 'zhuge'  的主键id
  - 通过聚集索引和id读取出排序字段 position ，然后把id和position放入sort buffer
  - 继续读取下一个 满足条件 的数据 放入
  - 读取完数据后 再进行排序；
  - 最后按照排好序的position和id,通过id再去读取完整的数据，把全部数据返回；

注意：如果全部使用sort_buffer内存排序，一般情况下效率回高于磁盘文件的排序；但是不能随意修改其大小，因为这已经是优化过了的，调整了可能反而效率不高；

==补充==：

where 和 having 的区别，where是分组前进行限定，如果不满足条件，则不参与分组；having在分组之后进行限定，如果不满足条件，就不会被查询出来；where后面不能跟聚合函数；

#### **Limit by的优化**

复习limit语法：

> limit a(开始的索引),b（每页查询的页数）;
>
> a=(页数-1)*b;   limit是Mysql的一个方言；

`select * from employees limit 10000,10;`



该语句将从表employees 中取出从10001行开始的10行记录，实际上进行的操作是先读取了前10000条数据，然后将其抛弃，再读取后面的10条，所以在一个比较大的表中查询较后的数据时效率会很低；



索引：

```
 KEY `idx_name_age_position` (`name`,`age`,`position`) USING BTREE
```

优化情况：

1.根据自增且连续的主键排序的分页查询

> select * from employees limit 90000,5;
>
> select * from employees where id > 90000 limit 5;

直接限定查找id > 9000  ，这样就使用了索引树了

但是很明显其中存在着问题：中间的id出现中断或者主键id不是连续自增的就会出错；

2.根据非主键字段排序的分页查询

> select * from employees ORDER BY name limit 90000,5;
>
> select * from employees e inner join (select id from employees order by name limit by 10000,5) ed on ed.id=e.id;

第一条语句的问题在于：扫描整个索引并查找到没索引的行（可能会遍历多个索引树）的成本可能比扫描全表的成本更高，索引优化器放弃使用索引；

优化的关键在于：让排序时的字段尽量少，这样可能可以少遍历几个索引树（从联合索引树的知识联想）；做法是让排序和分页操作先查出主键（使用最少的字段），然后根据主键查到对应的记录；





#### 表关联算法

> 两种关联算法：
>
> - Nested-Loop Join 算法 ---NLJ
> - Block Nested-Loop Join 算法  ---BNJ

首先明确一个概念，存在两个表，一个叫做驱动表，一个叫做被驱动表。一般来说先执行的表叫做驱动表，优化器会优先选择较小一点的表；所以排在前面的表不一定就是驱动表；

使用了NLJ算法。在一般的join语句中，不会在执行计划的Extra中看到Using join buffer字段；如果出现该字段则表示是使用的BNL算法；可以看到如下示例：

mysql> EXPLAIN select*from t1 inner join t2 on t1.a= t2.a;

![image-20210326154036434](C:\Users\86137\AppData\Roaming\Typora\typora-user-images\image-20210326154036434.png)

上面的sql执行流程如下：

1. 从t2表中取出一行数据
2. 从第一步中取出的数据中拿出字段a，然后到表t1中去寻找；
3. 取出t1中满足条件的数据，跟t2的数据合并，然后返回给客户端；
4. 重复步骤3；

整个过程会读取 t2 表的所有数据(扫描100行)，然后遍历这每行数据中字段 a 的值，根据 t2 表中 a 的值索引扫描 t1 表
中的对应行(扫描100次 t1 表的索引，1次扫描可以认为最终只扫描 t1 表一行完整数据，也就是总共 t1 表也扫描了100
行)。因此整个过程扫描了 200 行。
如果被驱动表的关联字段没索引，使用NLJ算法性能会比较低(下面有详细解释)，mysql会选择Block Nested-Loop Join
算法。

-------------------------------------------------------

把驱动表中读取到的数据放到join buffer中，然后扫描被驱动表，把被驱动表每一行取出来的数据跟join buffer中的数据作对比；

mysql>EXPLAIN select*from t1 inner join t2 on t1.b= t2.b;

![image-20210326155007256](C:\Users\86137\AppData\Roaming\Typora\typora-user-images\image-20210326155007256.png)

上面的sql大致流程如下：

1. 把t2表中的所有数据放入join buffer
2. 从t1表中取出一行数据，跟join_buffer中的数据做对比，返回满足条件的结果；
3. 重复第三步，直到读取完t1中的所有数据；

整个过程对表 t1 和 t2 都做了一次全表扫描，因此扫描的总行数为10000(表 t1 的数据总量) + 100(表 t2 的数据总量) =
10100。并且 join_buffer 里的数据是无序的，因此对表 t1 中的每一行，都要做 100 次判断，所以==内存中==的判断次数是100 * 10000= 100 万次。



straight_join解释：straight_join功能同join类似，但能让左边的表来驱动右边的表，能改表优化器对于联表查询的执行顺序。
比如：select * from t2 straight_join t1 on t2.a = t1.a; 代表制定mysql选着 t2 表作为驱动表。

- straight_join只适用于inner join，并不适用于left join，right join。（因为left join，right join已经代表指定了表的执行顺序）
- 尽可能让优化器去判断，因为大部分情况下mysql优化器是比人要聪明的。使用straight_join一定要慎重，因为部分情况下人为指定的执行顺序并不一定会比优化引擎要靠谱。count(*)查询优化



常见优化方法
		1、查询mysql自己维护的总行数

对于myisam存储引擎的表做不带where条件的count查询性能是很高的，因为myisam存储引擎的表的总行数会被mysql存储在磁盘上，查询不需要计算

2、show table status

​		如果只需要知道表总行数的估计值可以用如下sql查询，性能很高

3、将总数维护到Redis里
插入或删除表数据行的时候同时维护redis里的表总行数key的计数值(用incr或decr命令)，但是这种方式可能不准，很难保证表操作和redis操作的事务一致性

4、增加计数表
插入或删除表数据行的时候同时维护计数表，让他们在同一个事务里操作

## 第四节：Mysql事务和锁分析

### 锁分类:

- 乐观锁（通过比较版本实现？？）/悲观锁
- 读锁和写锁：

读锁（共享锁）：同一份数据，多个读操作可以同时进行；

写锁（排它锁）：当前写操作完成前，他会阻断其他写锁；

- 对数据的操作来说的话，可以分为表锁和读锁；

表锁：每次操作都锁住整张表。开销小，加锁快，并且不会出现死锁；锁定范围大，锁发生冲突的可能性小，并发度较低（能并发的量少）；


























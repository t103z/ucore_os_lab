## Lec9 exercise
#### 2014011561 张欣阳
---

物理页帧数量为3，且初始时没有对应的虚拟页。虚拟页访问序列为 0,1,2,0,1,3,0,3,1,0,3，请问采用最优置换算法的缺页次数为（**4**）

缺页情况为：

|访问|0|1|2|0|1|3|0|3|1|0|3|
|---|-|-|-|-|-|-|-|-|-|-|-|
|缺页|x|x|x|o|o|x|o|o|o|o|o|
|换出|-|-|-|-|-|2|-|-|-|-|-|




---
物理页帧数量为3，且初始时没有对应的虚拟页。虚拟页访问序列为 0,1,2,0,1,3,0,3,1,0,3，请问采用FIFO置换算法的缺页次数为（**6**）

缺页情况为：

|访问|0|1|2|0|1|3|0|3|1|0|3|
|---|-|-|-|-|-|-|-|-|-|-|-|
|缺页|x|x|x|o|o|x|x|o|x|o|o|
|换出|-|-|-|-|-|0|1|-|2|-|-|

---
物理页帧数量为4，且初始时没有对应的虚拟页。虚拟页访问序列为 0,3,2,0,1,3,4,3,1,0,3,2,1,3,4 ，请问采用CLOCK置换算法（用1个bit表示存在时间）的缺页次数为（**9**）

缺页情况为：

|访问|0|3|2|0|1|3|4|3|1|0|3|2|1|3|4|
|---|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|
|缺页|x|x|x|o|x|o|x|o|o|x|o|x|x|o|x|
|换出|-|-|-|-|-|-|0|-|-|2|-|1|4|-|0|

---
物理页帧数量为4，且初始时没有对应的虚拟页。虚拟页访问序列为 0,3,2,0,1,3,4,3,1,0,3,2,1,3,4 ，请问采用CLOCK置换算法（用2个关联，bit表示存在时间,可以表示4,）的缺页次数为（**7**）

|访问|0|3|2|0|1|3|4|3|1|0|3|2|1|3|4|
|---|-|-|-|-|-|-|-|-|-|-|-|-|-|-|-|
|缺页|x|x|x|o|x|o|x|o|o|o|o|x|x|o|o|
|换出|-|-|-|-|-|-|2|-|-|-|-|1|0|-|-|


---


（spoc）根据你的学号 mod 4的结果值，确定选择四种页面置换算法（0：LRU置换算法，1:改进的clock 页置换算法，2：工作集页置换算法，3：缺页率置换算法）中的一种来设计一个应用程序（可基于python, ruby, C, C++，LISP等）模拟实现，并给出测试用例和测试结果。请参考如python代码或独自实现。

[页置换算法实现的参考实例]( https://github.com/chyyuu/ucore_lab/blob/master/related_info/lab3/page-replacement-policy.py)

我的学号是2014011561，实现改进的clock算法，程序见`page-replacement-clock.py`，给出课件中的样例输入：
```shell
python page-replacement-policy.py -p CLOCK-e -a a,b,c,d,c,A,d,B,e,b,A,b,c,d -f 4 -c
```
程序输出为：
```
ARG addresses a,b,c,d,c,A,d,B,e,b,A,b,c,d
ARG policy CLOCK-e
ARG clockbits 1
ARG pageframesize 4
ARG seed 0
ARG notrace False

Solving...

Read:    a  MISS Left -> [['a', 0, 1]] <- Right  Replaced:- [Hits:0 Misses:1]
Read:    b  MISS Left -> [['a', 0, 1], ['b', 0, 1]] <- Right  Replaced:- [Hits:0 Misses:2]

Read:    c  MISS Left -> [['a', 0, 1], ['b', 0, 1], ['c', 0, 1]] <- Right  Replaced:- [Hits:0 Misses:3]
Read:    d  MISS Left -> [['a', 0, 1], ['b', 0, 1], ['c', 0, 1], ['d', 0, 1]] <- Right  Replaced:- [Hits:0 Misses:4]
Read:    c  HIT  Left -> [['a', 0, 1], ['b', 0, 1], ['c', 0, 1], ['d', 0, 1]] <- Right  Replaced:- [Hits:1 Misses:4]
Write:   a  HIT  Left -> [['a', 1, 1], ['b', 0, 1], ['c', 0, 1], ['d', 0, 1]] <- Right  Replaced:- [Hits:2 Misses:4]
Read:    d  HIT  Left -> [['a', 1, 1], ['b', 0, 1], ['c', 0, 1], ['d', 0, 1]] <- Right  Replaced:- [Hits:3 Misses:4]
Write:   b  HIT  Left -> [['a', 1, 1], ['b', 1, 1], ['c', 0, 1], ['d', 0, 1]] <- Right  Replaced:- [Hits:4 Misses:4]
Read:    e  MISS Left -> [['a', 0, 0], ['b', 0, 0], ['e', 0, 1], ['d', 0, 0]] <- Right  Replaced:c [Hits:4 Misses:5]
Read:    b  HIT  Left -> [['a', 0, 0], ['b', 0, 1], ['e', 0, 1], ['d', 0, 0]] <- Right  Replaced:- [Hits:5 Misses:5]
Write:   a  HIT  Left -> [['a', 1, 1], ['b', 0, 1], ['e', 0, 1], ['d', 0, 0]] <- Right  Replaced:- [Hits:6 Misses:5]
Read:    b  HIT  Left -> [['a', 1, 1], ['b', 0, 1], ['e', 0, 1], ['d', 0, 0]] <- Right  Replaced:- [Hits:7 Misses:5]
Read:    c  MISS Left -> [['a', 1, 1], ['b', 0, 1], ['e', 0, 1], ['c', 0, 1]] <- Right  Replaced:d [Hits:7 Misses:6]
Read:    d  MISS Left -> [['a', 0, 0], ['d', 0, 1], ['e', 0, 0], ['c', 0, 0]] <- Right  Replaced:b [Hits:7 Misses:7]

FINALSTATS hits 7   misses 7   hitrate 50.00
```

---

请判断OPT、LRU、FIFO、Clock和LFU等各页面置换算法是否存在Belady现象？如果存在，给出实例；如果不存在，给出证明。

#### OPT算法
不存在。作为“最优”算法，我们首先应当证明其最优性，即对于给定输入序列，OPT不存在算法比OPT算法的缺页次数少。在这方面[(Roy 2007)](http://web.stanford.edu/~bvr/pubs/paging.pdf)给出了一个证明。有了最优性保证，我们不难证明OPT算法不存在Belady现象。设想对于一个指定序列，在具有 $k$ 个物理页时缺页次数为 $n$ ；那么如果我们有 $k'>k$ 个物理页的话，一种策略是不使用多于 $k$ 个页的部分，运用OPT算法，这时缺页次数一定也是 $n$ ，又由算法最优性，对于 $k'$ 个页，其最优策略缺页次数一定不多于上面给出的策略，所以运行OPT算法一定不存在Belady现象。

#### LRU算法
不存在。假设在增加物理页面之前，有n个物理页，内存中物理页的集合为A。A中页的含义为最近访问过的n个物理页面。当增加物理页的数目之后，有n+1个物理页，新的物理页集合为B。B中页的含义为最近访问过的n+1个物理页面。所以对于同一访问序列，在同一时间，集合A必然是集合B的子集合。所以不存在Belady现象。

详细的证明用归纳法给出：

题设：对于任意访问序列 $S=a_1, a_2, ..., a_n$ ，记 $\mathbf{LRU}_i(S)$ 为物理页数为 $i$ 的LRU算法的缺页次数，则要证明对 $\forall i<j$ 有 $\mathbf{LRU}_i(S) \geq \mathbf{LRU}_{i+1}(S) \geq ... \geq \mathbf{LRU}_j(S)$

证明：我们先定义一个有 $i$ 个元素的队列能嵌入另一个有 $i+1$ 个元素的队列，如果除了后一个队列的队尾元素以外，两个队列的元素完全相同。然后我们再证明命题：在任何一个时刻， $\mathbf{LRU}_i$ 的队列一定能够嵌入 $\mathbf{LRU}_{i+1}$ 的队列。

1. 首先当 $n=1$ 时，两个队列中都只有一个元素，命题成立
2. 假设前 $n$ 步命题都成立，下面证明 $n+1$ 步命题也成立。
    1. 如果 $a_{n+1}$ 在 $\mathbf{LRU}_i$ 的队列中，即命中，那么由归纳假设，它也一定在 $\mathbf{LRU}_{i+1}$ 的队列中。所以 $n+1$ 时刻， $a_{n+1}$ 都被挪到队首，命题成立。
    2. 如果 $a_{n+1}$ 不在 $\mathbf{LRU}_i$ 的队列中，即出现缺页，那么又有两种情况
        1. $a_{n+1}$ 也不在 $\mathbf{LRU}_{i+1}$ 的队列中，那么它在两种物理页大小的算法中都被加到队首，队尾元素都被弹出，所以嵌入关系仍然满足
        2. $a_{n+1}$ 在 $\mathbf{LRU}_{i+1}$ 的队列中，那么对于 $\mathbf{LRU}_{i+1}$ 来说， 由归纳假设，$a_{n+1}$ 只可能在队尾，它将被挪到队首；而对 $\mathbf{LRU}_i$ 它会被加到队首，队尾元素被抛弃。这种情况下，嵌入关系仍然满足。

综上我们的命题成立，所以一定有 $\mathbf{LRU}_i(S) < \mathbf{LRU}_{i+1}(S)$ 我们不难把这个结论推广到 $\forall i < j$的情况，所以不存在Belady现象。

#### FIFO算法
存在。例如课件中给出的123412512345序列，物理页数为3缺页次数为9；物理页数为4缺页次数为10。

#### Clock算法
存在。我们可以用上一题中的程序测试，当全部输入都是读时，改进的clock算法与clock算法表现相同。输入序列为a,b,c,d,a,b,e,a,b,c,d,e
物理页数为3时的输出为
```
Read:	 a  MISS Left -> [['a', 0, 1]] <- Right  Replaced:- [Hits:0 Misses:1]
Read:	 b  MISS Left -> [['a', 0, 1], ['b', 0, 1]] <- Right  Replaced:- [Hits:0 Misses:2]
Read:	 c  MISS Left -> [['a', 0, 1], ['b', 0, 1], ['c', 0, 1]] <- Right  Replaced:- [Hits:0 Misses:3]
Read:	 d  MISS Left -> [['d', 0, 1], ['b', 0, 0], ['c', 0, 0]] <- Right  Replaced:a [Hits:0 Misses:4]
Read:	 a  MISS Left -> [['d', 0, 1], ['a', 0, 1], ['c', 0, 0]] <- Right  Replaced:b [Hits:0 Misses:5]
Read:	 b  MISS Left -> [['d', 0, 1], ['a', 0, 1], ['b', 0, 1]] <- Right  Replaced:c [Hits:0 Misses:6]
Read:	 e  MISS Left -> [['e', 0, 1], ['a', 0, 0], ['b', 0, 0]] <- Right  Replaced:d [Hits:0 Misses:7]
Read:	 a  HIT  Left -> [['e', 0, 1], ['a', 0, 1], ['b', 0, 0]] <- Right  Replaced:- [Hits:1 Misses:7]
Read:	 b  HIT  Left -> [['e', 0, 1], ['a', 0, 1], ['b', 0, 1]] <- Right  Replaced:- [Hits:2 Misses:7]
Read:	 c  MISS Left -> [['e', 0, 0], ['c', 0, 1], ['b', 0, 0]] <- Right  Replaced:a [Hits:2 Misses:8]
Read:	 d  MISS Left -> [['e', 0, 0], ['c', 0, 1], ['d', 0, 1]] <- Right  Replaced:b [Hits:2 Misses:9]
Read:	 e  HIT  Left -> [['e', 0, 1], ['c', 0, 1], ['d', 0, 1]] <- Right  Replaced:- [Hits:3 Misses:9]

FINALSTATS hits 3   misses 9   hitrate 25.00
```
物理页数为4的输出为
```
Read:	 a  MISS Left -> [['a', 0, 1]] <- Right  Replaced:- [Hits:0 Misses:1]
Read:	 b  MISS Left -> [['a', 0, 1], ['b', 0, 1]] <- Right  Replaced:- [Hits:0 Misses:2]
Read:	 c  MISS Left -> [['a', 0, 1], ['b', 0, 1], ['c', 0, 1]] <- Right  Replaced:- [Hits:0 Misses:3]
Read:	 d  MISS Left -> [['a', 0, 1], ['b', 0, 1], ['c', 0, 1], ['d', 0, 1]] <- Right  Replaced:- [Hits:0 Misses:4]
Read:	 a  HIT  Left -> [['a', 0, 1], ['b', 0, 1], ['c', 0, 1], ['d', 0, 1]] <- Right  Replaced:- [Hits:1 Misses:4]
Read:	 b  HIT  Left -> [['a', 0, 1], ['b', 0, 1], ['c', 0, 1], ['d', 0, 1]] <- Right  Replaced:- [Hits:2 Misses:4]
Read:	 e  MISS Left -> [['e', 0, 1], ['b', 0, 0], ['c', 0, 0], ['d', 0, 0]] <- Right  Replaced:a [Hits:2 Misses:5]
Read:	 a  MISS Left -> [['e', 0, 1], ['a', 0, 1], ['c', 0, 0], ['d', 0, 0]] <- Right  Replaced:b [Hits:2 Misses:6]
Read:	 b  MISS Left -> [['e', 0, 1], ['a', 0, 1], ['b', 0, 1], ['d', 0, 0]] <- Right  Replaced:c [Hits:2 Misses:7]
Read:	 c  MISS Left -> [['e', 0, 1], ['a', 0, 1], ['b', 0, 1], ['c', 0, 1]] <- Right  Replaced:d [Hits:2 Misses:8]
Read:	 d  MISS Left -> [['d', 0, 1], ['a', 0, 0], ['b', 0, 0], ['c', 0, 0]] <- Right  Replaced:e [Hits:2 Misses:9]
Read:	 e  MISS Left -> [['d', 0, 1], ['e', 0, 1], ['b', 0, 0], ['c', 0, 0]] <- Right  Replaced:a [Hits:2 Misses:10]

FINALSTATS hits 2   misses 10   hitrate 16.67
```
缺页由9次增加到了10次

#### LFU算法
不存在。证明与LRU相似对于相同访问序列，具有n个物理页的最少访问集合必然是n+1个物理页最少访问集合的子集，所以不存在Belady现象。

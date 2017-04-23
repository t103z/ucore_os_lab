# Lec 16 Exercise
#### 2014011561 张欣阳

## stride 算法目标
stride调度算法旨在解决按权分配的进程调度问题。进程的优先级按数量（原论文中称为ticket）量化，stride算法希望保证，如果一个进程的ticket是另一个进程的2倍，则其获得的资源（一般情况下是CPU时间）也是另一个进程的2倍，而等待时间则是另一个进程的二分之一。同时stride算法希望实现确定性的，可预测的调度。

定量来讲，对于一个系统，总ticket数为T，一个进程ticket数为t，则在连续的n次分配中，该进程理论上被选中的次数是：n * t / T。stride算法希望控制该进程实际被分配次数和这个理论值之间的误差。实验表明，最终设计出的stride算法可以将这个误差控制在O(1)。

## 算法思路
stride算法的基本思路是，给每个进程定义三个量，ticket，stride，pass。其中ticket与前文定义相同，stride是步长，为ticket的倒数，pass是进度，表示当前进程的位置，用于调度。

每次算法选择pass最小的进程执行，执行后该进程pass增加stride大小，如此往复。如果有pass相同的进程，则按照某种策略，比如按进程号，来选择应当执行的进程。

## 算法设计

算法实现时，有两点细节。首先，为了高效选择pass最小的进程，需要一种数据结构来维护进程队列。一般可以选择优先队列（堆），或跳跃表来实现，复杂度都为O(log n)。如果进程数非常少的话，则一般用普通的队列来实现效率最高。

其次，stride在经过ticket取倒数以后应当是一个浮点数，为了避免浮点数运算，最简单的方式就是将这个数乘一个较大的常数stride1，来将stride转换成一个整型数。

以下是算法代码：

```c
/* per-client state */
typedef struct {
    //...
    int tickets, stride, pass;
} *client t;

/* large integer stride constant (e.g. 1M) */
const int stride1 = (1 << 20);

/* current resource owner */
client t current;

/* initialize client with speciﬁed allocation */
void client_init(client t c, queue t q, int tickets) {

    /* stride is inverse of tickets */
    c->tickets = tickets;
    c->stride = stride1 / tickets;
    c->pass = c->stride;

    /* join competition for resource */
    queue_insert(q, c);
}

/* proportional-share resource allocation */
void allocate(queue t q) {
    /* select client with minimum pass value */
    current = queue_remove_min(q);

    /* use resource for quantum */
    use resource(current);

    /* compute next pass using stride */
    current->pass += current->stride;
    queue_insert(q, current);
}
```

## 整数溢出问题
使用整数处理stride后，需要考虑整数的溢出问题。因为stride1是一个较大的整数，所以分配的ticket数较小的话，该进程的stride也会是一个比较大的值，就会存在溢出问题。整数溢出与整数的位数有关。对于64位整数，溢出问题并不显著。以stride1 = 2^20为例，最小分配的ticket数是1，如果以每毫秒分配一次计，发生溢出需要的时间是世纪的数量级。

对于32位整数，就需要考虑溢出问题。可行的做法是，当发生溢出时，将该进程的pass值减去当前所有进程中最小的pass值。除了在发生溢出时运行这种方法，我们也可以周期性地运行此方法。

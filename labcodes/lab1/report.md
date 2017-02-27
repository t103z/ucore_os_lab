# Lab 1 实验报告

## 2014011561 张欣阳

--------------------------------------------------------------------------------

## Ex1

### Ex1.1

#### 1.1

先来看一下Makefile中指定的一些gcc编译选项和ld连接选项

```makefile
# define compiler and flags
ifndef  USELLVM
HOSTCC        := gcc
HOSTCFLAGS    := -g -Wall -O2
CC        := $(GCCPREFIX)gcc
CFLAGS    := -fno-builtin -Wall -ggdb -m32 -gstabs -nostdinc $(DEFS)
CFLAGS    += $(shell $(CC) -fno-stack-protector -E -x c /dev/null >/dev/null 2>&1 && echo -fno-stack-protector)
else
HOSTCC        := clang
HOSTCFLAGS    := -g -Wall -O2
CC        := clang
CFLAGS    := -fno-builtin -Wall -g -m32 -mno-sse -nostdinc $(DEFS)
CFLAGS    += $(shell $(CC) -fno-stack-protector -E -x c /dev/null >/dev/null 2>&1 && echo -fno-stack-protector)
endif

CTYPE    := c S

LD      := $(GCCPREFIX)ld
LDFLAGS    := -m $(shell $(LD) -V | grep elf_i386 2>/dev/null)
LDFLAGS    += -nostdlib
```

前面一段代码指定了编译器，在我们的Makefile中`USELLVM`是未定义的，也就是说会使用gcc编译。相关的编译器选项有：

- `-fno-builtin`，不承认不以__builtin_开头的函数为内建（built-in）函数。关闭这个选项gcc会关闭一些对内建函数的优化，方便设置断点调试。
- `-Wall`打开警告。
- `-ggdb`产生gdb调试信息。
- `-m32`产生32位代码。
- `-gstabs`使用stab格式调试信息。
- `-nostdinc`不在系统标准路径中寻找头文件，所有头文件路径必须由`-I`指出。
- `-fno-stack-protector`关掉栈保护，栈保护是防止缓冲区溢出攻击用的，会使得栈地址不确定。

相关的连接器选项有：

- `-m`，指定连接器模拟选项，这里的参数由`$(shell $(LD) -V | grep elf_i386 2>/dev/null)`选定，首先通过`ld -V`列出支持的模拟器选项，再从中用`grep`选出含`elf_i386`的。
- `-nostdlib`不搜索标准路径，与上面编译器选项里对应的项功能相似，用户需要指明要连接的路径。

#### 1.2

生成`ucore.img`的代码是

```makefile
UCOREIMG    := $(call totarget,ucore.img)

$(UCOREIMG): $(kernel) $(bootblock)
    $(V)dd if=/dev/zero of=$@ count=10000
    $(V)dd if=$(bootblock) of=$@ conv=notrunc
    $(V)dd if=$(kernel) of=$@ seek=1 conv=notrunc

$(call create_target,ucore.img)
```

第一行使用call函数调用了`totarget`，其在`tools/function.mk`中定义如下：

```makefile
totarget = $(addprefix $(BINDIR)$(SLASH),$(1))
```

这行里的`BINDIR`在Makefile前文被定义为`bin`，所以这个函数将接受的参数前加上`bin/`并返回。

回到生成ucore.img的代码，执行完第一句以后，`UCOREIMG`被赋值为bin/ucore.img

第二段设置了`UCOREIMG`的依赖项为`kernel`和`bootblock`。如果依赖项满足，则通过`dd`命令，先清空`UCOREIMG`，再将`bootblock`和`kernel`拷贝到`UCOREIMG`文件。最后调用`create_target`创建目标。

#### 1.3

创建`kernel`的代码如下

```makefile
kernel = $(call totarget,kernel)

$(kernel): tools/kernel.ld

$(kernel): $(KOBJS)
    @echo + ld $@
    $(V)$(LD) $(LDFLAGS) -T tools/kernel.ld -o $@ $(KOBJS)
    @$(OBJDUMP) -S $@ > $(call asmfile,kernel)
    @$(OBJDUMP) -t $@ | $(SED) '1,/SYMBOL TABLE/d; s/ .* / /; /^$$/d' > $(call symfile,kernel)

$(call create_target,kernel)
```

可以看到`kernel`的依赖项为tools/kernel.ld, `KOBJS`，如果依赖项满足，则执行连接。`$(V)$(LD) $(LDFLAGS) -T tools/kernel.ld -o $@ $(KOBJS)`这条命令将与kernel相关的obj文件连接起来，`-T`选项指示了连接代码文件为`tools/kernel.ld`。其后两条指令使用objdump将汇编代码和符号表输出到了`kernel.asm`和`kernel.sym`。

#### 1.3.1

`KBOJS`的定义在前文代码段给出

```makefile
KINCLUDE    += kern/debug/ \
               kern/driver/ \
               kern/trap/ \
               kern/mm/

KSRCDIR        += kern/init \
               kern/libs \
               kern/debug \
               kern/driver \
               kern/trap \
               kern/mm

KCFLAGS        += $(addprefix -I,$(KINCLUDE))

$(call add_files_cc,$(call listf_cc,$(KSRCDIR)),kernel,$(KCFLAGS))

KOBJS    = $(call read_packet,kernel libs)
```

这一段代码首先定义了include和src的路径，接着调用`addprefix`函数定义`KCFLAGS`。该函数是GNU make的自带函数，调用后将`KINCLUDE`加上了前缀`-I`。定义完源代码和include文件后，这段代码调用了`add_files_cc`函数。为了说明这个函数的功能，我们首先看`Makefile`文件前文的定义。`add_files_cc`在前文被定义为`add_files_cc = $(call add_files,$(1),$(CC),$(CFLAGS) $(3),$(2),$(4))`，到这里还是不知道它的功能，但可以看到它调用了`add_files`函数。为了进一步找到`add_files`函数的功能，我们需要查看`function.mk`中的定义。

来到`function.mk`，我们先考虑这部分代码：

```makefile
# add files to packet: (#files, cc[, flags, packet, dir])
add_files = $(eval $(call do_add_files_to_packet,$(1),$(2),$(3),$(4),$(5)))
```

我们发现`add_files`用`eval`函数展开了`do_add_files_to_packet`宏，所以我们要进一步查看这个宏的定义：

```makefile
# add files to packet: (#files, cc[, flags, packet, dir])
define do_add_files_to_packet
__temp_packet__ := $(call packetname,$(4))
ifeq ($$(origin $$(__temp_packet__)),undefined)
$$(__temp_packet__) :=
endif
__temp_objs__ := $(call toobj,$(1),$(5))
$$(foreach f,$(1),$$(eval $$(call cc_template,$$(f),$(2),$(3),$(5))))
$$(__temp_packet__) += $$(__temp_objs__)
endef
```

我们看到这个宏接受五个输入，根据注释，它们分别是files, cc, flags, packet, dir。而在这个宏中，又调用了两个函数`packetname`，`toobj`和一个宏`cc_template`。所以我们还需要再向下走一步查看它们的定义：

```makefile
OBJPREFIX    := __objs_

# change $(name) to $(OBJPREFIX)$(name): (#names)
packetname = $(if $(1),$(addprefix $(OBJPREFIX),$(1)),$(OBJPREFIX))

# get .o obj files: (#files[, packet])
toobj = $(addprefix $(OBJDIR)$(SLASH)$(if $(2),$(2)$(SLASH)),\
        $(addsuffix .o,$(basename $(1))))

# cc compile template, generate rule for dep, obj: (file, cc[, flags, dir])
define cc_template
$$(call todep,$(1),$(4)): $(1) | $$$$(dir $$$$@)
    @$(2) -I$$(dir $(1)) $(3) -MM $$< -MT "$$(patsubst %.d,%.o,$$@) $$@"> $$@
$$(call toobj,$(1),$(4)): $(1) | $$$$(dir $$$$@)
    @echo + cc $$<
    $(V)$(2) -I$$(dir $(1)) $(3) -c $$< -o $$@
ALLOBJS += $$(call toobj,$(1),$(4))
endef
```

从这一段代码中，我们看到`packetname`这个函数判断输入，并给输入加上`__objs_`头。`toobj`这个函数将输入的文件名转换成`objdir/[packet]/file.o`的形式。而宏`cc_template`则接受file，cc，flags和dir输入，并生成编译文件的代码。

回到`do_add_files_to_packet`宏，它先判断输入的参数4，即packet名，并尝试用`packetname`函数将其转换为带有前缀的形式；再将输入的文件用`toobj`函数转换成带路径的形式。最后将整个packet中的文件编译。所以`add_files`函数将输入的文件、包编译到相对应的`objdir/[packet]/*`路径。

现在我们终于可以回到`Makefile`，

```makefile
$(call add_files_cc,$(call listf_cc,$(KSRCDIR)),kernel,$(KCFLAGS))
```

这行代码的作用就是编译处理好的`KOBJS`。至此，我们就梳理清楚了生成`kernel`所有依赖项的过程。其核心思想就是将各种依赖项，包含上它们的include，编译到对应的obj以下的路径。

#### 1.4

现在再来看生成`bootblock`的代码

```makefile
bootfiles = $(call listf_cc,boot)
$(foreach f,$(bootfiles),$(call cc_compile,$(f),$(CC),$(CFLAGS) -Os -nostdinc))

bootblock = $(call totarget,bootblock)

$(bootblock): $(call toobj,$(bootfiles)) | $(call totarget,sign)
    @echo + ld $@
    $(V)$(LD) $(LDFLAGS) -N -e start -Ttext 0x7C00 $^ -o $(call toobj,bootblock)
    @$(OBJDUMP) -S $(call objfile,bootblock) > $(call asmfile,bootblock)
    @$(OBJCOPY) -S -O binary $(call objfile,bootblock) $(call outfile,bootblock)
    @$(call totarget,sign) $(call outfile,bootblock) $(bootblock)

$(call create_target,bootblock)
```

首先通过`listf_cc`函数列出boot路径下的代码文件赋值给`bootfiles`，然后用cc_compile函数编译上面得到的所有的文件。再编译`bootblock`，其依赖项是编译`bootfiles`产生的目标文件，以及`sign`的目标文件。

如果依赖项得到满足，则使用连接器连接文件。除了1.1节中列出的连接器选项，这里还有几个附加选项：

- `-N`设置代码段和数据段均可读写。
- `-e start`指定程序入口。
- `-Ttext`指定代码段开始位置。

连接后，使用objdump输出反汇编代码，再用objcopy将bootblock.o输出到bootblock.out。

#### 1.4.1

我们再来看一下`bootblock`的依赖项`sign`是怎么生成的。相关代码是：

```makefile
# create 'sign' tools
$(call add_files_host,tools/sign.c,sign,sign)
$(call create_target_host,sign,sign)
```

首先通过`add_files_host`函数添加源代码文件。这里的编译选项不太相同，使用的是`-g -Wall -O2`。添加文件后，调用`create_target_host`函数生成obj/sign/tools/sign.o以及bin/sign

### Ex1.2

510位置为0x55，511位置为0xFF

--------------------------------------------------------------------------------

## Ex2

### Ex2.1

修改gdbinit文件，设置内容为：

```
file bin/kernel
set architecture i8086
target remote :1234
```

运行`make debug`后，gdb启动，PC停在了0xffff0的位置。使用`x\10i 0xffff0`可以看到如下反汇编代码：

```assembly
0xffff0:     ljmp   $0x3630,$0xf000e05b
0xffff7:     das
0xffff8:     xor    (%ebx),%dh
0xffffa:     das
0xffffb:     cmp    %edi,(%ecx)
0xffffd:     add    %bh,%ah
0xfffff:     add    %al,(%eax)
...
```

### Ex2.2

gdb启动后，输入命令`b *0x7c00`，在0x7c00处设置断点，键入`c`继续运行代码，gdb在所设置的断点处停止。

### Ex2.3

运行`make debug`前，先修改Makefile代码，在运行qemu一行加入`-d in_asm -D $(BINDIR)/q.log`，以便将运行的代码反汇编保存起来。

截止到`call bootmain`前的反汇编代码如下：

```assembly
0x7c00:      cli
0x7c01:      cld
0x7c02:      xor    %ax,%ax
0x7c04:      mov    %ax,%ds
0x7c06:      mov    %ax,%es
0x7c08:      mov    %ax,%ss
0x7c0a:      in     $0x64,%al
0x7c0c:      test   $0x2,%al
0x7c0e:      jne    0x7c0a
0x7c10:      mov    $0xd1,%al
0x7c0a:      in     $0x64,%al
0x7c0c:      test   $0x2,%al
0x7c0e:      jne    0x7c0a
0x7c10:      mov    $0xd1,%al
0x7c12:      out    %al,$0x64
0x7c14:      in     $0x64,%al
0x7c16:      test   $0x2,%al
0x7c18:      jne    0x7c14
0x7c1a:      mov    $0xdf,%al
0x7c1c:      out    %al,$0x60
0x7c1a:      mov    $0xdf,%al
0x7c1c:      out    %al,$0x60
0x7c1e:      lgdtw  0x7c6c
0x7c23:      mov    %cr0,%eax
0x7c26:      or     $0x1,%eax
0x7c2a:      mov    %eax,%cr0
0x7c2d:      ljmp   $0x8,$0x7c32
0x7c32:      mov    $0xd88e0010,%eax
0x7c38:      mov    %ax,%es
0x7c3a:      mov    %ax,%fs
0x7c3c:      mov    %ax,%gs
0x7c3e:      mov    %ax,%ss
0x7c40:      mov    $0x0,%bp
0x7c43:      add    %al,(%bx,%si)
0x7c45:      mov    $0x7c00,%sp
0x7c48:      add    %al,(%bx,%si)
0x7c4a:      call   0x7ccf
```

这一段代码与bootasm.S和bootblock.asm是完全对应的

### Ex2.4

我们尝试调试一下`kern_init`函数，修改gdbinit文件如下：

```
file bin/kernel
set architecture i8086
target remote :1234
break kernel_init
continue
```

再运行`make debug`，gdb停在0x100000

<kern_init>处。输入<code>x/10i $pc</code>可以查看PC处向后10条指令的反汇编，如下：</kern_init>

```assembly
0x100000 <kern_init>:        push   %bp
0x100001 <kern_init+1>:      mov    %sp,%bp
0x100003 <kern_init+3>:      sub    $0x18,%sp
0x100006 <kern_init+6>:      mov    $0xed20,%dx
0x100009 <kern_init+9>:      adc    %al,(%bx,%si)
0x10000b <kern_init+11>:     mov    $0xda18,%ax
0x10000e <kern_init+14>:     adc    %al,(%bx,%si)
0x100010 <kern_init+16>:     sub    %ax,%dx
0x100012 <kern_init+18>:     mov    %dx,%ax
0x100014 <kern_init+20>:     sub    $0x4,%sp
```

对比kernel.asm，它们是对应的。在断点处，继续输入`si`可以进行指令级单步调试，由于指定了源码文件，输入`s`可以进行源码级单步调试。

--------------------------------------------------------------------------------

## Ex3

### Ex3.1

A20 Gate是Intel为了兼容8086计算机，加入的硬件逻辑，实现在实模式下访存地址超过20位时地址回绕的行为。在切换到保护模式时，需要将A20 Gate置1，从而保证保护模式下对更大地址空间的访问是正确的。

打开A20 Gate要通过8042键盘控制芯片，至于为什么使用这个芯片，是历史问题。打开A20 Gate的具体步骤是：

1. 等待8042 Input buffer为空；
2. 发送Write 8042 Output Port （P2）命令到8042 Input buffer；
3. 等待8042 Input buffer为空；
4. 将8042 Output Port（P2）得到字节的第2位置1，然后写入8042 Input buffer；

我们来看bootasm.S中对应的代码

第一步，等待8042 Input buffer为空，对应代码为：

```assembly
seta20.1:
    inb $0x64, %al                                  # Wait for not busy(8042 input buffer empty).
    testb $0x2, %al
    jnz seta20.1
```

首先用`in`指令读取64h地址的8位数据，其中低第1位（0起始）表示input是否有数据，所以用`test`指令和0x2判断该位是否为1，若不为空则跳转。

若为空，则进行第二步

```assembly
movb $0xd1, %al                                 # 0xd1 -> port 0x64
    outb %al, $0x64                                 # 0xd1 means: write data to 8042's P2 port
```

先写命令0xd1到al寄存器，再用`out`指令将寄存器中的值发送到0x64。

第三步，等待Input buffer为空，与第一步相同，代码为：

```assembly
seta20.2:
    inb $0x64, %al                                  # Wait for not busy(8042 input buffer empty).
    testb $0x2, %al
    jnz seta20.2
```

第四步，向output port写数据

```assembly
movb $0xdf, %al                                 # 0xdf -> port 0x60
    outb %al, $0x60                                 # 0xdf = 11011111, means set P2's A20 bit(the 1 bit) to 1
```

将数据0xdf写入al寄存器，再用`out`指令发送。

### Ex3.2

读取GDT是通过`lgdt`一条指令实现的：

```assembly
lgdt gdtdesc
```

在`gdtdesc`处定义了GDT的具体项

```assembly
gdt:
    SEG_NULLASM                                     # null seg
    SEG_ASM(STA_X|STA_R, 0x0, 0xffffffff)           # code seg for bootloader and kernel
    SEG_ASM(STA_W, 0x0, 0xffffffff)                 # data seg for bootloader and kernel

gdtdesc:
    .word 0x17                                      # sizeof(gdt) - 1
    .long gdt                                       # address gdt
```

其中的一些宏是在`asm.h`中定义的。

### Ex3.3

进入保护模式通过设置CR0实现：

```assembly
movl %cr0, %eax
    orl $CR0_PE_ON, %eax
    movl %eax, %cr0
```

设置完成后，执行长跳转，处理器进入32位地址的保护模式

```assembly
# Jump to next instruction, but in 32-bit code segment.
    # Switches processor into 32-bit mode.
    ljmp $PROT_MODE_CSEG, $protcseg
```

然后再初始化段寄存器DS，ES，FS，GS，SS，并初始化栈

```assembly
.code32                                             # Assemble for 32-bit mode
protcseg:
    # Set up the protected-mode data segment registers
    movw $PROT_MODE_DSEG, %ax                       # Our data segment selector
    movw %ax, %ds                                   # -> DS: Data Segment
    movw %ax, %es                                   # -> ES: Extra Segment
    movw %ax, %fs                                   # -> FS
    movw %ax, %gs                                   # -> GS
    movw %ax, %ss                                   # -> SS: Stack Segment

    # Set up the stack pointer and call into C. The stack region is from 0--start(0x7c00)
    movl $0x0, %ebp
    movl $start, %esp
    call bootmain
```

--------------------------------------------------------------------------------

## Ex4

### Ex4.1

bootloader通过`readsect`函数读取磁盘扇区，其定义如下：

```c
/* waitdisk - wait for disk ready */
static void
waitdisk(void) {
    while ((inb(0x1F7) & 0xC0) != 0x40)
        /* do nothing */;
}

/* readsect - read a single sector at @secno into @dst */
static void
readsect(void *dst, uint32_t secno) {
    // wait for disk to be ready
    waitdisk();

    outb(0x1F2, 1);                         // count = 1
    outb(0x1F3, secno & 0xFF);
    outb(0x1F4, (secno >> 8) & 0xFF);
    outb(0x1F5, (secno >> 16) & 0xFF);
    outb(0x1F6, ((secno >> 24) & 0xF) | 0xE0);
    outb(0x1F7, 0x20);                      // cmd 0x20 - read sectors

    // wait for disk to be ready
    waitdisk();

    // read a sector
    insl(0x1F0, dst, SECTSIZE / 4);
}
```

可以看到该函数首先等待磁盘就绪，判断方式是读取IO地址寄存器0x1F7地址。就绪后，将读取扇区数，扇区号等信息写入0x1F2-0x1F7，再等待磁盘就绪，然后将数据读到内存里。这里每次只读取一个扇区。

### Ex4.2

bootmain.c中的bootmain函数描述了读取操作系统ELF文件的过程，该函数调用了`readseg`函数来读取磁盘段，`readseg`函数在`readsect`函数的基础上做了一个封装：

```c
/* *
 * readseg - read @count bytes at @offset from kernel into virtual address @va,
 * might copy more than asked.
 * */
static void
readseg(uintptr_t va, uint32_t count, uint32_t offset) {
    uintptr_t end_va = va + count;

    // round down to sector boundary
    va -= offset % SECTSIZE;

    // translate from bytes to sectors; kernel starts at sector 1
    // kernel从扇区1开始
    uint32_t secno = (offset / SECTSIZE) + 1;

    // If this is too slow, we could read lots of sectors at a time.
    // We'd write more to memory than asked, but it doesn't matter --
    // we load in increasing order.
    for (; va < end_va; va += SECTSIZE, secno ++) {
        readsect((void *)va, secno);
    }
}
```

可以看到它将offset转换成扇区号，并处理虚地址对齐，然后调用`readsect`读取磁盘扇区。

现在再来看`bootmain`函数

```c
/* bootmain - the entry of bootloader */
void
bootmain(void) {
    // 读取ELF header，位于文件头
    readseg((uintptr_t)ELFHDR, SECTSIZE * 8, 0);

    // 判断ELF header是否有效
    if (ELFHDR->e_magic != ELF_MAGIC) {
        goto bad;
    }

    struct proghdr *ph, *eph;

    // 从文件头中获取program header表偏移，然后读取所有program header表内容
    ph = (struct proghdr *)((uintptr_t)ELFHDR + ELFHDR->e_phoff);
    eph = ph + ELFHDR->e_phnum;
    // 根据program header表，将磁盘中的可执行文件部分读到内存中
    for (; ph < eph; ph ++) {
        readseg(ph->p_va & 0xFFFFFF, ph->p_memsz, ph->p_offset);
    }

    // 读取完毕后，ucore已经被加载到了内存中，现在只需要跳转到操作系统入口执行，就可以运行起
    // 操作系统
    ((void (*)(void))(ELFHDR->e_entry & 0xFFFFFF))();

bad:
    outw(0x8A00, 0x8A00);
    outw(0x8A00, 0x8E00);

    /* do nothing */
    while (1);
}
```

具体步骤已经在注释中标出。

--------------------------------------------------------------------------------

## Ex5

函数调用栈的每个栈帧内容，根据该栈帧的保存ebp地址就可以获得。该ebp+4处为返回值地址，+8起为参数列表地址。而ebp所指地址则保存了上一个栈帧的帧底地址。具体实现请参考源代码。

--------------------------------------------------------------------------------

## Ex6

### Ex6.1

每个表项占8个字节，表项详细定义如下：

```c
struct gatedesc {
    unsigned gd_off_15_0 : 16;        // low 16 bits of offset in segment
    unsigned gd_ss : 16;            // segment selector
    unsigned gd_args : 5;            // # args, 0 for interrupt/trap gates
    unsigned gd_rsv1 : 3;            // reserved(should be zero I guess)
    unsigned gd_type : 4;            // type(STS_{TG,IG32,TG32})
    unsigned gd_s : 1;                // must be 0 (system)
    unsigned gd_dpl : 2;            // descriptor(meaning new) privilege level
    unsigned gd_p : 1;                // Present
    unsigned gd_off_31_16 : 16;        // high bits of offset in segment
};
```

低0...15位和48...63位为偏移，16...31位为段选择符。

### Ex6.2 & Ex6.3

见代码

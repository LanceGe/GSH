# 说明

## 介绍

这是大一下学期的时候的一份程序设计实验作业。内容是，实现一个类似于bash的shell。本shell命名为gsh，意思是G's Shell（我姓氏首字母为G）。  
由于本人之前并未接触过系统编程，所以参考了一份别人造的轮子。此处感谢[vvy](https://github.com/vvy/wshell)。主要参考部分为：输入命令的解析，内置指令cd的实现。

## 编译

本项目文件夹结构如下：  
.  
├── [`CMakeLists.txt`](CMakeLists.txt)  
├── [`compile.sh`](compile.sh)  
├── executable  
│   └── [`gsh`](executable/gsh)  
├── include  
│   ├── [`builtin.h`](include/builtin.h)  
│   ├── [`color.h`](include/color.h)  
│   ├── [`gsh.h`](include/gsh.h)  
│   ├── [`prompt.h`](include/prompt.h)  
│   ├── [`read.h`](include/read.h)  
│   └── [`run.h`](include/run.h)  
├── [`README.md`](README.md)  
└── source  
    ├── [`builtin.c`](source/builtin.c)  
    ├── [`gsh.c`](source/gsh.c)  
    ├── [`prompt.c`](source/prompt.c)  
    ├── [`read.c`](source/read.c)  
    └── [`run.c`](source/run.c)  

其中，[`gsh`](executable/gsh)为在Ubuntu 16.04 64bit下使用gcc 5.3.1编译成的可执行文件。若要在不同的环境中编译相应版本，可以在项目主目录下运行
```bash
sh compile.sh
```

## 使用

在终端中运行可执行文件后，很快会见到一个哲♂学符号（当然，如果是以root用户的身份执行的，则会见到一个♀），如
```bash
~♂ 
```

这是gsh的特色哲♂学命令提示符。

**警告：如果不是在Linux环境使用，那么有可能会造成出现一堆奇怪的字符。这是[`color.h`](include/color.h)的锅。我做的时候，为了好看，给文字加上了Linux特有的字体颜色前缀。为了保留最佳风味，请在Linux环境下食用。**

### 操作

本shell的基本操作与bash类似，即
```
可执行文件名 [参数]...
```
本shell支持Tab键补全功能，但是能补全的仅仅是文件名且仅限于当前目录中的（-_-b）。

### 内置指令
本shell实现了以下几条内置指令：
* cd
* cp
* cmp
* wc
* quit
* philosophy

其中，`cd -`并没有实现，且cp和cmp都不支持参数。不过，cp的三种文件名组合顺序都有支持。wc命令支持且仅支持`-l`参数。  
quit命令的功能是退出这个shell程序，返回它的父进程。至于philosophy命令，嘿嘿，其作用为打印一张[比利王](https://zh.moegirl.org/zh/%E6%AF%94%E5%88%A9%E5%A8%98)的帅照（ASCII码阵列）。这条指令，体现了本shell所代表的哲♂学思想。  

当然，限于本人写代码的过程是学习的过程，因此对于非法输入、执行错误的报错会比较奇怪，很惭愧。

### 外部调用

外部调用可以的东西和bash相同。实际上，如果你用我实现的`cp`, `cmp`, `wc`不爽，大可在代码里删掉它们，然后用系统自带的。

# 多AGV物流分拣系统的设计
## 论文摘要
[知网论文地址](https://kns.cnki.net/KCMS/detail/detail.aspx?dbcode=CMFD&dbname=CMFD201802&filename=1018872311.nh&v=MTg3MTR6Z1Vyek9WRjI2RnJ1L0hOTE5ycEViUElSOGVYMUx1eFlTN0RoMVQzcVRyV00xRnJDVVI3cWZadWR0Rnk= "知网论文地址")

我国物流行业的分拣环节基本还处于人工分拣阶段，存在分拣效率低下、人工成本高、差错率高等问题。自动化物流分拣正朝着智能化方向发展，AGV（自动引导车）具有智能化程度高和灵活性高的特点，应用AGV来分拣快递包裹可以极大地提高分拣效率、降低人工成本和减小差错率。	本文研究的对象是一种多AGV物流分拣系统，在对多AGV物流分拣系统进行设计的基础上，对视觉导航及定位、多AGV路径规划两个关键技术进行了研究。论文主要研究工作包括三个部分：

1. AGV视觉导航及定位技术的研究与实现。	本文分析了现有视觉导航及定位技术的不足之处，如摄像头昂贵、定位不精确，并针对这些不足提出了一种基于编码标志的视觉导航及定位方法，采用这种方法的AGV即使利用普通摄像头也能够精准导航和精确定位。
2. 多AGV路径规划方法的研究。多AGV物流分拣系统具有路径网络复杂、AGV数量庞大等特点，基于静态确定网络的传统多AGV路径规划方法不适用于多AGV物流分拣系统。本文针对多AGV物流分拣系统的特点，建立基于动态随机网络的路径规划模型，并对A*算法进行改进，如引入时间变量、考虑转弯耗费、优化OPEN表。最后通过实验对两种路径规划方法进行了分析和对比。
3. 多AGV物流分拣系统的设计与实现。本文重点阐述了多AGV物流分拣系统的总体设计，根据设计要求制作了快递分拣机器人，并基于Java开发了上位机系统软件，采用C++结合OpenCv编程实现了基于编码标志的视觉导航及定位方法和基于动态随机网络的路径规划方法。

## 模块介绍
本论文的研究对应工程主要分为三块，express（调度系统）、smartcar-vision（分拣机器人）、simulator（大规模机器人模拟系统）。
### express 
[github地址](https://github.com/xxxtai/express "github地址")

调度系统是整个多AGV物流分拣系统的大脑，负责信息统筹、调度和控制快递分拣机器人。该工程基于java、swing、spring boot、neety等技术，实现了论文中所述三种路径规划算法：改进A*算法、dijkstra算法、贪婪算法，以及多机器人之间防冲突算法，结合simulator工程可以模拟大规模机器人的调度。[大规模机器人调度模拟](https://v.qq.com/x/page/c3050fw4ria.html)


### smartcar-vision
[github地址](https://github.com/xxxtai/smartcar-vision "github地址")

[demo机器人视频](https://v.qq.com/x/page/v30506h0k6r.html "demo机器人视频")

该工程是分拣机器人的大脑，实现了视觉导航、机器控制、调度，该工程基于C++、openCV、PID控制等技术，运行在Jeson Tk1嵌入式板（其实就是一个ubuntu系统），底层电机控制使用了Arduino。该工程依赖于硬件，有了这份代码也只有助于研究视觉导航的实现，很难实际运行起来。

### simulator
[github地址](https://github.com/xxxtai/simulator "github地址")

[大规模机器人调度模拟](https://v.qq.com/x/page/c3050fw4ria.html)

一台分拣机器人造价不便宜，所以没有大规模机器人分拣实验。为了验证论文提出的算法的有效性，开发了本模拟软件，本工程模拟了机器人运行的物理特性，并完全适配于调度系统，调度系统没有特殊定制。

### 如何运行起来
smartcar-vision工程依赖于物理硬件，所以很难运行起来，里面代码可以用来学习机器视觉。这里我们主要讲“大规模机器人调度模拟”如何运行起来。

express和simulator都是基于spring开发的swing工程，之间采用netty通信，找到main函数就能运行起来了，先运行express，再运行simulaotr。两个工程运行都需要提供一分地图元数据Excel文件，仓库也为大家准备了一份Graph.xls，大家运行的时候需要在ComGraph文件中修改文件路径。





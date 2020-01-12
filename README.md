# 多AGV物流分拣系统的设计
## 论文摘要
[知网论文地址](https://kns.cnki.net/KCMS/detail/detail.aspx?dbcode=CMFD&dbname=CMFD201802&filename=1018872311.nh&v=MTg3MTR6Z1Vyek9WRjI2RnJ1L0hOTE5ycEViUElSOGVYMUx1eFlTN0RoMVQzcVRyV00xRnJDVVI3cWZadWR0Rnk= "知网论文地址")  如果不方便下载，在issues留下邮箱，有空我会发给你

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

一台分拣机器人造价不便宜，所以没有使用大规模真实机器人进行分拣实验。为了验证论文提出的算法的有效性，开发了本模拟软件，本工程模拟了机器人运行的物理特性，以及送料系统，使随机生成的快递包裹模拟放置在机器人托盘上，并完全适配于调度系统，调度系统没有特殊定制就可以像调度真实机器人一样调度模拟机器人。

### 如何运行起来
smartcar-vision工程依赖于物理硬件，所以很难运行起来，里面代码可以用来学习机器视觉。这里我们主要讲“大规模机器人调度模拟”如何运行起来。

express和simulator都是基于spring开发的swing工程，之间采用netty通信，找到main函数就能运行起来了，先运行express，再运行simulaotr。两个工程运行都需要提供一分地图元数据Excel文件，仓库也为大家准备了一份Graph.xls，大家运行的时候需要在ComGraph文件中修改文件路径。

## 如果对你有帮助的话就打赏一杯咖啡吧
![](https://user-gold-cdn.xitu.io/2020/1/12/16f9916c9960ef7e?w=1282&h=716&f=jpeg&s=64952)

# Design and Key Technologies of Multi AGV Logistics Sorting System
[paper link](https://kns.cnki.net/KCMS/detail/detail.aspx?dbcode=CMFD&dbname=CMFD201802&filename=1018872311.nh&v=MTg3MTR6Z1Vyek9WRjI2RnJ1L0hOTE5ycEViUElSOGVYMUx1eFlTN0RoMVQzcVRyV00xRnJDVVI3cWZadWR0Rnk= "知网论文地址")    If it's not convenient to download, leave an email in issues, and I'll send it to you when I have time
# Design and Key Technologies of Multi AGV Logistics Sorting System 
## ABSTRACT
The sorting process of China's logistics industry is still at the stage of manual sorting, which has the problems of low sorting efficiency, high labor cost and high error rate. Automated logistics sorting is developing in the direction of intellectualization. AGV (automatic guide car) has the characteristics of high intelligence and high flexibility. Using a large number of AGV to pick up the express package can greatly improve the sorting efficiency, reduce the labor cost and reduce the error rate. The object of this paper is a multi AGV logistics sorting system. On the basis of the design of the multi AGV logistics sorting system, two key technologies of visual navigation, positioning and multi AGV path planning are studied. The main research work of this paper consists of three parts:
1. research and implementation of AGV vision navigation and positioning technology. This paper analyzes the shortcomings of the existing visual navigation and positioning techniques, such as the expensive and inaccurate location of the camera, and proposes a visual navigation and positioning method based on the coded logo for these shortcomings. By this method, AGV can be used to navigate accurately even when the ordinary camera is running at a high speed. And precise positioning.
2. the research of multi AGV path planning method. The multi AGV logistics sorting system has the characteristics of complex path network and large number of AGV. The traditional multi AGV path planning method based on static determination of network is not suitable for multi AGV logistics sorting system. In this paper, based on the characteristics of multi AGV logistics sorting system, a path planning model based on dynamic random network is established, and the A* algorithm is improved, such as introducing time variables, considering the cost of turning and optimizing the OPEN table. Finally, the two path planning methods are analyzed and compared through experiments.
3. design and implementation of multi AGV logistics sorting system. This paper focuses on the overall design of the multi AGV logistics sorting system. According to the design requirements, the express sorting robot is made, and the upper computer system software is developed based on the Java. The visual navigation and positioning method based on the coded symbol and the path planning method based on the dynamic random network are realized by using the C++ and OpenCv programming.



## Module introduction
The research of this paper is divided into three parts，express、smartcar-vision、simulator。
### express 
[github地址](https://github.com/xxxtai/express "github地址")

The scheduling system is the brain of the whole multi AGV logistics sorting system, which is responsible for information co-ordination, scheduling and control of the express sorting robot. Based on Java, swing, spring boot, neety and other technologies, this project implements three path planning algorithms: improved a * algorithm, Dijkstra algorithm, greedy algorithm, and conflict prevention algorithm among multiple robots. Combined with the simulator project, it can simulate the scheduling of large-scale robots。[Large scale robot scheduling simulation](https://v.qq.com/x/page/c3050fw4ria.html)


### smartcar-vision
[github地址](https://github.com/xxxtai/smartcar-vision "github地址")

[Demo robot video](https://v.qq.com/x/page/v30506h0k6r.html "demo机器人视频")

The project is the brain of sorting robot, which realizes visual navigation, machine control and scheduling. Based on C + +, opencv, PID control and other technologies, the project runs on the embedded board of Jason TK1 (in fact, it is a Ubuntu system). Arduino is used for the bottom motor control. The project depends on hardware, with this code only helps to study the implementation of visual navigation, it is difficult to actually run.

### simulator
[github地址](https://github.com/xxxtai/simulator "github地址")

[Large scale robot scheduling simulation](https://v.qq.com/x/page/c3050fw4ria.html)

A sorting robot is not cheap, so there is no large-scale robot sorting experiment. In order to verify the effectiveness of the algorithm proposed in this paper, the simulation software is developed. This project simulates the physical characteristics of the robot operation, and fully adapts to the scheduling system, which has no special customization.

### How to run
smartcar-vision Engineering relies on physical hardware, so it's hard to run. The code inside can be used to learn machine vision. Here we mainly talk about how "large scale robot scheduling simulation" works.

Express and simulator are based on spring development of the swing project. Netty communication is used between them. Find the main function and run it. First run express, then run simulator. Both projects need to provide a map metadata excel file, and the warehouse has also prepared a graph.xls for you. When you run, you need to modify the file path in the comgraph file.

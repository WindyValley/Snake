## 项目介绍

使用SDL库实现的贪吃蛇游戏

## 经验

1. 图像渲染与交互事件的处理分置于不同线程，可以简化程序开发，便于稳定帧率，也可以借助阻塞式事件机制，节省CPU资源。

2. 参考[Embedding-binary-resources-with-CMake-and-C-11](https://beesbuzz.biz/code/4399-Embedding-binary-resources-with-CMake-and-C-11)，实现资源文件批量转换为对象文件，再封装成Resource对象编译为静态库，提供给主程序加载。cmake脚本为[cmake/resources.cmake](https://github.com/WindyValley/Snake/blob/main/cmake/resources.cmake)，封装资源文件数据接口的类在[res/Resource.h](https://github.com/WindyValley/Snake/blob/main/res/Resource.h)

3. 由二进制资源文件转换所得对象文件中的符号，在使用时可分别以char和char[]的方式引用，通过思考二者的区别，进一步加深了对指针与数组区别的认识。

4. 本次开发使用了goto，在资源初始化过程中完成初始化失败的统一处理。
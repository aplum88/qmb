#本项目主要用来研究mb在qt下的使用，尝试实现基于HTML+CSS+JAVASCript精美的界面和Qt C++基础库的强大功能实现桌面软件开发

## 目前实现的功能

* 基于离屏渲染实现了一个QMiniBlink的类，只需要继承该类，就可以拥有miniblink的功能

## 使用注意

* 需要先把res目录下的文件拷贝到debug目录
* 需要把node.dll拷贝到debug目录，或者调用wkeSetWkeDllPath设置node.dll的路径
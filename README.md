muek 是一个由KBE服务器引擎和虚幻结合的MMOPRPG的游戏框架。
怪物逻辑控制是用虚幻的服务器通过KBE的controlledBy进行控制。
KBE的定义通过程序转换为虚幻的结构。这样虚幻和KBE交互的函数参数
直接可以在KBE的def定义。

GenActor    是用来把KBE的定义转化为虚幻的结构体和UObject。
kbengine    KBE服务器内容
Plugins\KBE 和KBE服务器通信相关的虚幻插件

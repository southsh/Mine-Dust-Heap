Name:

    HoneyPot


注意事项: 

    1、蜜罐需要与主服务器进行TCP连接，籍此反馈信息。主服务器端口号和主服务器IP需要先在config.h里定义
    
    2、SSH，RDP，TELNET，服务器以多线程的方式独立运行，记录每次攻击者的IP和端口号
    
    3、在config.h里定义DEBUG的值，定义是否输出一些调试信息
    
    4、此蜜罐不涉及任何内部信息，目前不存在安全泄露问题



Installation:

    Linux版本 ：使用g++工具，
    g++ -o HoneyPot Telnet_HoneyPot.cpp SSH_HoneyPot.cpp RDP_HoneyPot.cpp main.cpp AttackHandle.cpp -I. -std=c++11 -pthread 
    
    Windows版本：启动Release文件夹下启动HoneyPot.exe文件


Usage:

    Linux版本 ： HoneyPot [-h]  [-p <port>]
                -h       -- 显示蜜罐使用方式
                
                [-p]     --port <port>   Port to listen on; defaults to 22.
                -s       --port <port>   表示开启ssh蜜罐，后面接端口号
                -t       --port <port>   表示开启telnet蜜罐，后面接端口号
                -r       --port <port>   表示开启rdp蜜罐，后面接端口号
                
            ./HoneyPot -s 22 -t 23 -r 3389   
                
             
                
    Windows版本：我是在vs2022里直接设置命令行参数


​    



Dependencies:

    Windows下：
        1、ws2_32.lib
        2、Winsock2.h
        3、WS2tcpip.h
        
    Linux下：
        1、pthread库


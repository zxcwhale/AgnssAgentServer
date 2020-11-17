该工程演示如何构架AGNSS代理服务器.

开发环境: QT5.6.1
编译器: MinGW4.9.2 32bit

代码文件:
agnssagentserver.h agnssagentserver.cpp: 代理服务器类.
casicAgnssAidIni.h casicAgnssAidIni.c: Casic AID INI 消息打包函数
ubloxAgnssAidIni.h ubloxAgnssAidIni.c: Ublox AID INI 消息打包函数
main.cpp: 入口函数.

程序功能:
AgnssAgentServer:
1. 每半个小时向服务器(agps.u-blox.com或www.gnss-aide.com)请求更新星历数据.
2. 侦听端口313, 当有客户端连接时, 对形如"cmd=full;lat=34.1;lon=114.5;"的消息做出解析, 并根据解析结果, 打包AGNSS辅助数据, 最后反馈给客户端.

请求辅助数据消息构成:
由一系列的key=value键值对构成, 用分号(;)分隔每个键值对.
key的说明:
cmd: 值为eph, aid或full. 其中eph表示只发送星历辅助数据, aid表示只发送时间和位置辅助数据, full表示两者皆发送.
lon,lat: 粗略的经纬度, 应该通过基站ID,蓝牙或wifi获取粗略的经纬度.

注意事项:
1. 在agnssagentserver.h中, 如果宏定义"#define CASIC", 表示使用www.gnss-aide.com为星历数据源, 并使用CASIC格式的辅助数据.
   而如果未宏定义"#define CASIC", 则表示使用agps.u-blox.com为星历数据源, 并使用UBLOX格式的辅助数据.
2. 在agnssagentserver.h中, 应当宏定义正确的"#define USERNAME xxx"以及"#define PASSWORD xxx", 否则无法正确获取星历辅助数据.

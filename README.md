## 说明
该工程演示如何构架AGNSS代理服务器.

## 开发环境: 
QT5

## 代码文件:
agnssagentserver.h agnssagentserver.cpp: 代理服务器类.
casicAgnssAidIni.h casicAgnssAidIni.c: Casic AID INI 消息打包函数
main.cpp: 入口函数.

## 程序功能:
AgnssAgentServer:
1. 每半个小时向服务器(www.gnss-aide.com)请求更新星历数据.
2. 侦听端口3131, 当有客户端连接时, 对形如"cmd=full;lat=34.1;lon=114.5;gnss=gps+bds"的消息做出解析, 并根据解析结果, 打包AGNSS辅助数据, 最后反馈给客户端.

## 请求辅助数据消息构成:
由一系列的key=value键值对构成, 用分号(;)分隔每个键值对.
key的说明:
cmd: 值为eph, aid或full. 其中eph表示只发送星历辅助数据, aid表示只发送时间和位置辅助数据, full表示两者皆发送.
lon,lat: 粗略的经纬度, 应该通过基站ID,蓝牙或wifi获取粗略的经纬度.
gnss: 辅助数据的卫星系统.  gps, bds或gps+bds.

## 注意事项:
AgnssAgentServer程序运行时需要以下参数:
用户名 - 该参数为www.gnss-aide.com服务器的注册用户名，必填。
密码 - 该参数为用户名参数对应的密码，必填。
端口 - 代理服务器的服务端口，选填，默认为3131.

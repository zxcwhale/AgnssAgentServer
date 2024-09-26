## 说明
该工程演示如何构架AGNSS代理服务器.

## 开发环境: 
QT6

## 代码文件:
- agnssagentserver: 代理服务器.
- casicAgnssAidIni: Casic AID INI 消息打包函数


## 程序功能:
- 定时从星历服务器获取星历数据.
- 当有客户端请求时, 发送AGNSS辅助数据.

## 请求消息说明:
- cmd: 请求数据的类型
  * eph: 星历数据
  * aid: 时间和位置数据
  * full: eph+aid
- lon: 粗略经度
- lat: 粗略纬度
- gnss: 辅助数据的卫星系统
  * gps, bds或gps+bds.

## 运行参数:
- --user, -u: 星历服务器用户名
- --password, -s: 星历服务器密码
- --port, -p: 代理端口

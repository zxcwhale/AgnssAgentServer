�ù�����ʾ��ι���AGNSS���������.

��������: QT5.6.1
������: MinGW4.9.2 32bit

�����ļ�:
agnssagentserver.h agnssagentserver.cpp: �����������.
casicAgnssAidIni.h casicAgnssAidIni.c: Casic AID INI ��Ϣ�������
ubloxAgnssAidIni.h ubloxAgnssAidIni.c: Ublox AID INI ��Ϣ�������
main.cpp: ��ں���.

������:
AgnssAgentServer:
1. ÿ���Сʱ�������(agps.u-blox.com��www.gnss-aide.com)���������������.
2. �����˿�313, ���пͻ�������ʱ, ������"cmd=full;lat=34.1;lon=114.5;"����Ϣ��������, �����ݽ������, ���AGNSS��������, ��������ͻ���.

������������Ϣ����:
��һϵ�е�key=value��ֵ�Թ���, �÷ֺ�(;)�ָ�ÿ����ֵ��.
key��˵��:
cmd: ֵΪeph, aid��full. ����eph��ʾֻ����������������, aid��ʾֻ����ʱ���λ�ø�������, full��ʾ���߽Է���.
lon,lat: ���Եľ�γ��, Ӧ��ͨ����վID,������wifi��ȡ���Եľ�γ��.

ע������:
1. ��agnssagentserver.h��, ����궨��"#define CASIC", ��ʾʹ��www.gnss-aide.comΪ��������Դ, ��ʹ��CASIC��ʽ�ĸ�������.
   �����δ�궨��"#define CASIC", ���ʾʹ��agps.u-blox.comΪ��������Դ, ��ʹ��UBLOX��ʽ�ĸ�������.
2. ��agnssagentserver.h��, Ӧ���궨����ȷ��"#define USERNAME xxx"�Լ�"#define PASSWORD xxx", �����޷���ȷ��ȡ������������.

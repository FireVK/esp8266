# esp8266
arduino esp8266 library

//恢复出厂
AT+RESTORE

//设置波特率
AT+CIOBAUD=57600

//软复位
AT+RST






//作为AP模式并建立服务器

AT+CWMODE=2

AT+RST

AT+CIPMUX=1

AT+CIPSERVER=1,80

AT+CIPAP="192.168.4.2"

AT+CWSAP="AP_01","88888888",1,3







//作为Station发送数据&服务器接受数据，有建立服务器的情况下须采用多路连接
AT+CWMODE=1

AT+RST

AT+CWJAP="SOGWORKS_AP","wwwsogworkscn"


//多路连接或单连接AT+CIPMUX=0
AT+CIPMUX=1


//建立服务器
AT+CIPSERVER=1,88


//多路连接
AT+CIPSTART=0,"TCP","192.168.1.112",81


//或单连接
AT+CIPSTART="TCP","192.168.1.112",82


//多路连接
AT+CIPSEND=0,5


//或单连接
AT+CIPSEND=5
Hello


//关闭连接，id为5时关闭所有
AT+CIPCLOSE=0
0


//查看IP
AT+CIFSR


//查看状态
AT+CIPSTATUS

1、
2、获得IP
3、建立连接
4、失去连接
5、接入AP

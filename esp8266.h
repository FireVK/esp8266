#ifndef ESP8266_h
#define ESP8266_h

#define ATTEMPTS 10 //错误尝试次数
#define MULT_CONNECT 1
#define SINGLE_CONNECT 0
#define DEBUG 0 
#include "Arduino.h"
#include <SoftwareSerial.h>

class ESP8266
{
  public:
    ESP8266(uint8_t rx, uint8_t tx, unsigned baudrate);
    bool connectWifi();
    //尝试连接WIFI，最多{ATTEMPTS}次
    bool tryConnectWifi();
    //复位
    bool resetModule();
    //设置WIFI模式，AP或Station
    bool setWiFiMode(uint8_t mdoe);
    //设置连接方式，单连接或多连接
    bool connectionMode(uint8_t mode);
    //获取所有的IP
    void getIP();
    bool connectServer(String type);
    //尝试与远端服务器建立连接，最多{ATTEMPTS}次
    bool tryConnectServer(String type);
    //http报文形式发送数据
    unsigned uploadPacket(String url, String *packet, unsigned n, unsigned delaytime);
    //读取或清空串口缓冲区
    String read_esp_buffer(bool rs);
    //设置WIFI参数及远端服务器参数
    void initWIFI(String _ssid, String _passwd , String _ip, uint8_t _port);
    //建立服务器
    bool setupServer(uint8_t port);
    //关闭服务器
    bool closeServer();
    //设置透传或一般模式
    bool setupCIP(uint8_t mode);
    //（软）串口数据长度
    int available();
    //接受网络数据
    String getMessage();
    //检测WIFI状态
    unsigned checkStatus();
    //根据WIFI状态让WIFI在线
    bool keepOnline();
    //以普通文本方式传输数据
    bool sendPacket(String data);
	bool setGateway(String ip, String gateway , String netsubMask);
  private:
    SoftwareSerial *_serial;
    unsigned baudrate;
    String cmd;
    String wifi[2];         //存放WIFI连接信息
    String server[2];       //存放远端服务器相关信息
    //String host[2] = {"MyAP", "8888888", 1, 3};     //当作AP时的AP信息
    byte buffer[1];         //临时缓冲
    unsigned pausetime;     //等待串口数据填充缓冲区的时间
};

#endif

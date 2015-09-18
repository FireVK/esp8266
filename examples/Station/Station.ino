/**
 * 基于esp8266的CS模式聊天室，可以作为client和server
 */

#include "DHT.h"      //添加DHT库
#include <SoftwareSerial.h>
#include "esp8266.h"

#define NEXTTIME 10                   // 发送时间最小间隔（单位秒）

#define SSID "SOGWORKS_AP"                   //配置成自己的WIFI名称
#define PASS "wwwsogworkscn"              //配置成自己的WIFI密码

#define OSERVER "192.168.1.113"           //此处为对方IP地址
#define OPORT 88						  //对方端口
#define NATIVESERVER "192.168.1.112"           //此处为本地IP地址

ESP8266 esp8266 = ESP8266(8, 9, 57600); // RX, TX

void setup()
{
  Serial.begin(57600);
  esp8266.initWIFI(SSID, PASS, OSERVER, OPORT); //初始化WIFI参数，连接参数
  while (!esp8266.setWiFiMode(1));       //设置WIFI模式为Station
  while (!esp8266.resetModule());        //WIFI复位
  while (!esp8266.setGateway(NATIVESERVER, "192.168.1.254", "255.255.255.0"));
  esp8266.keepOnline();        //连接WIFI
  delay(1000);
  esp8266.setupCIP(0);                    //非透传模式
  delay(1000);
  esp8266.getIP();                        //打印出IP
  esp8266.connectionMode(MULT_CONNECT);
  while (!esp8266.setupServer(88));       //建立端口为88的服务器
  delay(1000);
  esp8266.getIP();                        //打印出IP
  Serial.println("Now,let's chat...");
}
void loop()
{
  //esp8266.keepOnline();        //保持在线
  if (Serial.available() > 1) {
    String input = "";
    while (Serial.available() > 0) {
      input += (char)Serial.read();
      delay(5);
    }
    if (input.length() > 1) {
      Serial.println(">>>Send:" + input);
      sendData(input);
    }
  }

  if (esp8266.available() > 0) {
    String msg = esp8266.getMessage();
    if (msg.length() > 0) {
      //如果是自动回复的，就不打印
      if (msg.indexOf("auto_OK") == -1) {
        Serial.print("###Recived:");
        Serial.println(msg);
      }
    }
  }
}
void sendData(String data) {
  esp8266.tryConnectServer("TCP");
  esp8266.sendPacket(data);
}



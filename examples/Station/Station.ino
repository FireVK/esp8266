#include "DHT.h"      //添加DHT库
#include <SoftwareSerial.h>
#include "esp8266.h"

#define NEXTTIME 10                   // 发送时间最小间隔（单位秒）

#define SSID "SOGWORKS_AP"                   //配置成自己的WIFI名称
#define PASS "wwwsogworkscn"              //配置成自己的WIFI密码

#define OSERVER "192.168.1.113"           //此处为对方IP地址
#define OPORT 88						  //对方端口
ESP8266 esp8266 = ESP8266(8, 9, 57600); // RX, TX

void setup()
{
  Serial.begin(57600);
  esp8266.initWIFI(SSID, PASS, OSERVER, OPORT); //初始化WIFI参数，连接参数
  while (!esp8266.setWiFiMode(1));       //设置WIFI模式为Station
  while (!esp8266.resetModule());        //WIFI复位
  esp8266.keepOnline();        //连接WIFI
  delay(3000);
  esp8266.setupCIP(0);                    //非透传模式
  delay(3000);
  esp8266.getIP();                        //打印出IP
  esp8266.connectionMode(1);
  while (!esp8266.setupServer(81));       //建立端口为88的服务器
  delay(3000);
  esp8266.getIP();                        //打印出IP
  Serial.println("Now,let's chat...");
}
void loop()
{
  //esp8266.keepOnline();        //保持在线
  if (Serial.available() > 0) {
    String input = "";
    while (Serial.available() > 0) {
      input += (char)Serial.read();
      delay(5);
    }
    if (input.length() > 0) {
      Serial.println("Me:" + input);
      sendData(input);
    }
  }

  if (esp8266.available() > 0) {
    String msg = esp8266.getMessage();
    if (msg.length() > 0) {
      Serial.println("###Message###################################");
      Serial.println(msg);
      Serial.println("#######################################End###");
    }
  }
}
void sendData(String data) {
  esp8266.tryConnectServer("TCP");
  esp8266.sendPacket(data);
}



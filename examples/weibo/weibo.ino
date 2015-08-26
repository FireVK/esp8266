#include "DHT.h"      //添加DHT库
#include <SoftwareSerial.h>
#include "esp8266.h"

#define DHTPIN 7                        // DHT11的数据通讯引脚连接数字D7引脚
#define DHTTYPE DHT11                   // 设置器件类型为DHT11，可选择其他DHT22、DHT21
#define POSTSIZE 5                      // 发送的数据分割成的数组大小
#define NEXTTIME 600                    // 发送时间最小间隔（单位秒）

#define SSID "SOGWORKS_AP"                   //配置成自己的WIFI名称
#define PASS "wwwsogworkscn"              //配置成自己的WIFI密码
#define TOKEN "2.00zNDovBSteQtB42659e05c3ZQ_gTE"                         //从http://www.sogworks.cn/openbay/weibo.html获取
#define SERVER "www.sogworks.cn"         //中转服务器地址（勿修改）
#define PORT 80                        //中转服务器端口（勿修改）
#define PURL "proxy.html"                //中转服务器请求地址（勿修改）
#define RURL "https://api.weibo.com/2/statuses/update.json"          //真实请求地址，如果后面加参数，请使用URLENCODE转码后的地址

boolean gonext = false;    //gonext,是否可以进行下一步

ESP8266 esp8266 = ESP8266(8, 9, 57600); // RX, TX
DHT dht(DHTPIN, DHTTYPE);       //定义温湿度传感器对象，设置引脚和类型

void setup()
{
  Serial.begin(57600);
  esp8266.initWIFI(SSID,PASS,SERVER,PORT);//初始化WIFI参数
  while(!esp8266.setWiFiMode(1));        //设置WIFI模式为Station
  while(!esp8266.resetModule());         //WIFI复位
  esp8266.connectionMode(1);        //设置为多连接模式
  esp8266.setupCIP(0);                  //非透传模式
}
void loop()
{
  Serial.println("Starting ... ");
  if (!gonext) {
    gonext = esp8266.keepOnline();      //连接WIFI
  }
  if (gonext) {
    int t = dht.readTemperature();  //读取温度
    int h = dht.readHumidity();   //读取湿度
    String content[POSTSIZE];    //需要发送的数据，以数组形式给出防止被截断
    content[0] = "access_token=";
    content[0] += TOKEN;
    content[1] = "&url=";
    content[1] += RURL;
    content[2] = "&status=";              //以上为发微博所需的参数，以下为微博实际内容（不能超过140个汉字）
    content[2] += "深圳，位置(22°32′41.65″N ,113°55′11.71″E)，";
    content[3] = "湿度：";
    content[3] += h;
    content[3] += "%，温度：";
    content[3] += t;
    content[3] += "°；";
    content[4] = "播报信息来自开源硬件平台Arduino（开发者：Chuck）";
    
    gonext = esp8266.tryConnectServer("TCP");          //与服务器建立连接（创建TCP）

    if (gonext) {
      unsigned delayTime = esp8266.uploadPacket(PURL, content, POSTSIZE, NEXTTIME);  //发送数据
      Serial.print("Delay ");
      Serial.print(delayTime);
      Serial.println(" seconds ...");
      unsigned long _delayTime = (unsigned long)delayTime * 1000;  //注意数据类型，这里需要转换一下
      delay(_delayTime);
    }
    else {          //服务器连接10次失败
      delay(3000);
    }
  }
  else {                        //Wifi连接10次失败
    delay(3000);
  }
}

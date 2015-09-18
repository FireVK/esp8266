/*
ESP8266.cpp - Library for uploading to ukhas.net from ESP8266.
Created by Chris Stubbs, November 1, 2014.
Released into the public domain.
*/

#include "Arduino.h"
#include "ESP8266.h"


ESP8266::ESP8266(uint8_t rx, uint8_t tx, unsigned _baudrate) {
  baudrate = _baudrate;
  pausetime = 64 * 1000 / (_baudrate / 8);
  _serial = new SoftwareSerial(rx, tx);
  _serial->begin(_baudrate);
}

void ESP8266::initWIFI(String _ssid, String _passwd , String _ip, uint8_t _port) {
  wifi[0] = _ssid;
  wifi[1] = _passwd;
  server[0] = _ip;
  server[1] = (String)_port;
}

bool ESP8266::setWiFiMode(uint8_t mode) {
  read_esp_buffer(false);
  cmd = "AT+CWMODE=";
  cmd += mode;
  _serial->println(cmd);
  unsigned timer = 0;
  while (_serial->available() < 20 && timer < 100) {
    timer++;
    delay(pausetime);
  }
  String response = read_esp_buffer(true);
  if (response.indexOf("OK") > -1 || response.indexOf("change") > -1) {
    switch (mode) {
      case 1: Serial.println("Station mode..."); break;
      case 2: Serial.println("AP mode ..."); break;
      case 3: Serial.println("AP & Station mode..."); break;
      default: Serial.println("Error..."); break;
    }
    return true;
  }
  else {
    Serial.println("Error:" + response);
    delay(pausetime * 2);
    return false;
  }
}

bool ESP8266::resetModule() {
  read_esp_buffer(false);
  Serial.println("Reset module ...");
  cmd = "AT+RST";
  _serial->println(cmd);
  //_serial->readBytes(buffer,cmd.length());
  unsigned timer = 0;
  while (_serial->available() < 15 && timer < 100) {
    timer++;
    delay(pausetime);
  }
  String response = read_esp_buffer(true);
  if (response.indexOf("OK") > -1 || response.indexOf("ready") > -1 || response.indexOf("invalid") > -1) {
    return true;
  }
  else {
    Serial.println("Module did not respond ... ");
    delay(pausetime * 2);
    return false;
  }
}

int ESP8266::available() {
  return _serial->available();
}

String ESP8266::getMessage() {
  if (_serial->find("+IPD,"))
  {
    delay(pausetime);
    _serial->readBytesUntil(',', buffer , 5);
    byte index = buffer[0] - 48;
    String response = read_esp_buffer(true);
    {
        String restr = "AT+CIPSEND=";//回复“OK”
        restr += index;
        restr += ",";
        restr += 7;
        _serial->println(restr);
        delay(pausetime * 2);
        restr = read_esp_buffer(true);
        if (restr.indexOf(">") > -1) {
          _serial->print("auto_OK");
        }
    }
    String close = "AT+CIPCLOSE=";
    close += index;
    _serial->println(close);
    return response;
  } else {
    return "";
  }
}

unsigned ESP8266::checkStatus() {
  read_esp_buffer(false);
  cmd = "AT+CIPSTATUS";
  _serial->println(cmd);
  delay(pausetime * 2);
  String response = read_esp_buffer(true);
  if (response.indexOf("STATUS:2") > -1) {
    return 2;
  } else if (response.indexOf("STATUS:3") > -1) {
    return 3;
  } else if (response.indexOf("STATUS:4") > -1) {
    return 4;
  } else if (response.indexOf("STATUS:5") > -1) {
    return 5;
  } else {
    return 0;
  }
}

bool ESP8266::keepOnline() {
  unsigned status = checkStatus();
  if (status == 0) {
    return tryConnectWifi();
  } else {
    return true;
  }
}

bool ESP8266::connectWifi() {
  read_esp_buffer(false);
  cmd = "AT+CWJAP=\"";
  cmd += wifi[0];
  cmd += "\",\"";
  cmd += wifi[1];
  cmd += "\"";
  _serial->println(cmd);
  delay(pausetime * 2);
  read_esp_buffer(false);//wifi连接耗时长，秒级，此处直接清理缓冲区
  unsigned timer = 0;
  while (_serial->available() < 6 && timer < 1000) { //此处最多等待8秒
    timer++;
    delay(pausetime);
  }
  String response = read_esp_buffer(true);
  if (response.indexOf("OK") > -1) {
    connectionMode(1);
    return true;
  }
  else {
    return false;
  }
}

bool ESP8266::tryConnectWifi() {
  Serial.print("Connecting wifi...>>");
  unsigned status = checkStatus();
  bool connected = false;
  if (status == 2 || status == 3 || status == 4 || status == 5) {
    connected = true;
  }
  if (!connected) {
    for (int i = 0; i < ATTEMPTS; i++) {
      Serial.print(i + 1);
      if (connectWifi()) {
        connected = true;
        break;
      }
      Serial.print("  >>");
    }
    Serial.println();
  }
  if (!connected) {//连接失败尝试复位
    Serial.println("Can not connect to the WiFi ...");
    delay(pausetime * 2);
    for (int i = 0; i < ATTEMPTS; i++) {
      if (resetModule()) {
        break;
      }
    }
  }
  return connected;
}

bool ESP8266::connectionMode(uint8_t mode) {
  read_esp_buffer(false);
  Serial.print("connection mode ...");
  cmd = "AT+CIPMUX=";
  cmd += mode;
  _serial->println(cmd);
  delay(pausetime * 2);
  String response = read_esp_buffer(true);
  if (response.indexOf("OK") > -1 || response.indexOf("builded") > 1 ) {
    Serial.print(mode);
    Serial.println("...OK!");
    return true;
  } else {
    Serial.println("Error");
    return false;
  }
}

void ESP8266::getIP() { //this is broken, I will fix it when I actually need it...
  read_esp_buffer(false);
  cmd = "AT+CIFSR";
  _serial->println(cmd);
  delay(pausetime * 2);
  String ipString = read_esp_buffer(true);
  Serial.println("IP addr is " + ipString);
}

bool ESP8266::connectServer(String type) {
  read_esp_buffer(false);
  String cmd = "AT+CIPSTART=";
  cmd += 0;
  cmd += ",";
  cmd += "\"";
  cmd += type;
  cmd += "\",";
  cmd += "\"";
  cmd += server[0];
  cmd += "\"";
  cmd += ",";
  cmd += server[1];
  _serial->println(cmd);
  unsigned timer = 0;
  while (_serial->available() < 58 && timer < 1000) {
    timer++;
    delay(pausetime);
  }
  String response = read_esp_buffer(true);
  if (response.indexOf("OK") > -1 || response.indexOf("ALREADY") > -1) {
    return true;
  } else {
    if(DEBUG) Serial.println("@connectServer():can not connected Server");
    return false;
  }
}

bool ESP8266::tryConnectServer(String type) {
  if(DEBUG) Serial.print("Connecting server...>>");
  bool connected = false;
  for (int i = 0; i < ATTEMPTS; i++) {
    if (connectServer(type)) {
      connected = true;
      break;
    }
    Serial.print("Send again");
    Serial.println(i + 1);
  }
  if (!connected) {//连接失败尝试复位
    Serial.println("Can not connect to this server ... Send fail");
    delay(pausetime * 2);
    for (int i = 0; i < ATTEMPTS; i++) {
      if (resetModule()) {
        break;
      }
    }
  }
  return connected;
}

unsigned ESP8266::uploadPacket(String url, String *packet, unsigned len, unsigned delaytime) {
  Serial.print("Upload packet ...>>  ");
  unsigned packetLen = 0;
  for (int i = 0; i < len; i++) {
    packetLen += packet[i].length();
  }
  //组装POST请求数据
  String sendstr = "POST /";
  sendstr += url;
  sendstr += " HTTP/1.0\n";
  sendstr += "Host:sogworks.cn\n";
  sendstr += "Content-Type:application/x-www-form-urlencoded\n";
  sendstr += "Content-Length:";
  sendstr += packetLen;
  sendstr += "\nConnection: close\n\n";

  String sendstr2 = "\r\n\r\n";
  unsigned sendLen = packetLen + sendstr.length() + sendstr2.length();
  read_esp_buffer(false);
  /****方案一：普通模式****/
  cmd = "AT+CIPSEND=0,";
  cmd += sendLen;

  _serial->println(cmd);
  //_serial->readBytes(buffer,cmd.length());
  delay(pausetime * 2);
  /***普通模式结束****/
  /****方案二：透传模式****/
  /****透传模式结束****/
  String response = read_esp_buffer(true);
  if (response.indexOf(">") == -1) { //Did we find the ">" (prompt for request)?
    Serial.println("Not find the \">\"");
    delay(pausetime * 2);
    _serial->println("AT+CIPCLOSE=0");
    return 5; //Delay 5 seconds
  } else {
    _serial->print(sendstr);
    for (int i = 0; i < len; i++) {
      _serial->print(packet[i]);
    }
    _serial->print(sendstr2);
    delay(pausetime * 2);
    Serial.println("ALL done ...");
    _serial->println("AT+CIPCLOSE=0");
    return delaytime;   //Delay [delaytime] seconds
  }
}


bool ESP8266::sendPacket(String data) {
  read_esp_buffer(false);

  if (DEBUG) Serial.println("Send packet ...>>  ");
  unsigned sendLen = data.length();
  cmd = "AT+CIPSEND=0,";
  cmd += sendLen;
  _serial->println(cmd);
  unsigned timer = 0;
  while (_serial->available() < 25 && timer < 100) {
    timer++;
    delay(pausetime);
  }
  String response = read_esp_buffer(true);
  if (response.indexOf(">") == -1) {
    Serial.println("Send fail,please send again!");
    _serial->println("AT+CIPCLOSE=1");
    return false;
  } else {
    _serial->print(data);
    delay(pausetime * 2);
    if (DEBUG) Serial.println("ALL done ...");
    _serial->println("AT+CIPCLOSE=0");
    return true;
  }
}


bool ESP8266::setupServer(uint8_t _port) {
  read_esp_buffer(false);
  Serial.println("Setup server ... ");
  String cmd = "AT+CIPSERVER=1,";
  cmd += _port;
  _serial->println(cmd);
  //_serial->readBytes(buffer,cmd.length());
  unsigned timer = 0;
  while (_serial->available() < 15 && timer < 100) {
    timer++;
    delay(5);
  }
  String response = read_esp_buffer(true);
  if (response.indexOf("OK") > -1) {
    _serial->println("AT+CIPSTO=10");//设置超时为10秒
    return true;
  }
  else {
    return false;
  }
}

bool ESP8266::closeServer() {
  read_esp_buffer(false);
  Serial.println("Close server ... ");
  cmd = "AT+CIPSERVER=0";
  _serial->println(cmd);
  //_serial->readBytes(buffer,cmd.length());
  unsigned timer = 0;
  while (_serial->available() < 15 && timer < 100) {
    timer++;
    delay(5);
  }
  String response = read_esp_buffer(true);
  if (response.indexOf("OK") > -1) {
    for (int i = 0; i < ATTEMPTS; i++) {
      if (resetModule()) {
        break;
      }
    }
    return true;
  }
  else {
    return false;
  }
}

bool ESP8266::setupCIP(uint8_t mode) {
  read_esp_buffer(false);
  Serial.println("Setup CIP mode ... ");
  String cmd = "AT+CIPMODE=";
  cmd += mode;
  _serial->println(cmd);
  unsigned timer = 0;
  while (_serial->available() < 15 && timer < 100) {
    timer++;
    delay(5);
  }
  String response = read_esp_buffer(true);
  if (response.indexOf("OK") > -1 || response.indexOf("builded") > -1) {
    return true;
  }
  else {
    return false;
  }
}

bool ESP8266::setGateway(String ip, String gateway , String netsubMask){
	String cmd = "AT+CIPSTA_CUR=\"";
	cmd += ip;
	cmd += "\",\"";
	cmd += gateway;
	cmd += "\",\"";
	cmd += netsubMask;
	cmd += "\"";
	_serial->println(cmd);
	unsigned timer = 0;
  	while (_serial->available() < 15 && timer < 10) {
    	timer++;
    	delay(5);
  	}
	String response = read_esp_buffer(true);
 	if (response.indexOf("OK") > -1 || response.indexOf("builded") > -1) {
    Serial.println("set gateway ok!");
 		return true;
 	}
	else {
    Serial.println("set gateway fail!");
		return false;
	}
}

/**
* read esp serial 里面的buffer内容，同时具有清空缓冲区的作用
* 参数意为是否返回缓冲区内容
*/
String ESP8266::read_esp_buffer(bool rs) {
  String response = "";
  if (rs) {
    while (_serial->available()) {
      char buf[65];
      int index = _serial->readBytes(buf, 64);
      buf[index] = '\0';
      response += buf;
      delay(pausetime);
    }
  } else {
    while (_serial->available()) {
      _serial->read(); //clear the incoming buffer
      delay(pausetime);
    }
  }
  return response;
}
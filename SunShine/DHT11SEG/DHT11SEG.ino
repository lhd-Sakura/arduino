#include <Wire.h>
//#include <OneWire.h>
#include "TM1650.h"
#include <ESP8266WiFi.h>
#include <dht.h>
#include<math.h>
int BH1750address = 0x23;
byte buff[2];


//加速度传感器头文件
// I2Cdev and MPU6050 must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
#include "I2Cdev.h"
#include "MPU6050.h"
MPU6050 accelgyro;

dht DHT;

#define DHT11_PIN D2

TM1650 d;
bool blinkState = false;

char line[] = "0000";
float TEMP,SPX,SPY,SPZ;
byte addr[8];
byte data[12];

const char* ssid     = "长路漫漫";                    //wifi的名称
const char* password = "l1285446775";                 //wifi的密码

const char* host ="api.heclouds.com";//访问的网址
const int httpPort = 80;//http的端口号



void setup() 
{
  Wire.begin(); 
  Serial.begin(9600); 


  d.init();                                       //初始化
  d.displayOff();

  
  d.setBrightness(TM1650_MIN_BRIGHT);             //设定亮度
  d.displayOn();
  delay(2000);
  d.displayString(line);
  
 WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    d.displayString("----");
  }

}
int value = 0;




void loop()
{


  int i;
  uint16_t val = 0;
  BH1750_Init(BH1750address);
  delay(200);
  if (2 == BH1750_Read(BH1750address))
  {
    val = ((buff[0] << 8) | buff[1]) / 1.2;
    Serial.print(val, DEC);
    Serial.println("[lx]");
  }
    char temp[]="";
    dtostrf(val,4,0,temp);   //数码管的温度和湿度显示
    d.displayString(temp);
 //   d.setDot(1,1);
   delay(2000);



   int chk = DHT.read11(DHT11_PIN);
  Serial.print("temperature:");                  //串口打印温度  
  Serial.println(DHT.temperature, 1);  
  
  Serial.print("\n");Serial.print("\n");

  pinMode(DHT11_PIN, OUTPUT);
    digitalWrite(DHT11_PIN, LOW);
    Wire.begin(); 

  d.init();                                       //初始化
  d.displayOn();
 
    TEMP = DHT.temperature;            //温度赋值
    
  //  char temp[]="";
    dtostrf(TEMP*100,4,0,temp);   //数码管的温度和湿度显示
    d.displayString(temp);
    d.setDot(1,1);




//云平台设置内容
    
     String postdata = String("")+"{\"Temperature_1\":"+TEMP+",\"SunShine_1\":"+val+"}";        //根据云平台数据流进行修改
      WiFiClient client;
  
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }
  String url = "/devices/573496351/datapoints?type=3";                  //设备id修改
   

  // This will send the request to the server
  //云平台上传数据，不可被注释
  client.print(String("POST ") + url + " HTTP/1.1\r\n" +
               "api-key:QhJZKb0Bd4N9037egPaq1v1u7hA=\r\n"+          //api根据云平台进行修改
               "Host:" + host + "\r\n" + 
               "Content-Length:"+postdata.length()+"\r\n\r\n"+
               //"Content-Type: application/x-www-form-urlencoded\r\n\r\n"+
               postdata+"\r\n");
              
  digitalWrite(LED_BUILTIN, LOW); 
  unsigned long timeout = millis();
  
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {

      client.stop();
      return;
    }
  }

 
  // Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    String line = client.readStringUntil('\r');

  }

}



int BH1750_Read(int address) //
{
  int i = 0;
  Wire.beginTransmission(address);
  Wire.requestFrom(address, 2);
  while (Wire.available()) //
  {
    buff[i] = Wire.read();  // receive one byte
    i++;
  }
  Wire.endTransmission();
  return i;
}

void BH1750_Init(int address)
{
  Wire.beginTransmission(address);
  Wire.write(0x10);//1lx reolution 120ms
  Wire.endTransmission();
}

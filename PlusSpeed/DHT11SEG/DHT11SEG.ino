#include <Wire.h>
//#include <OneWire.h>
#include "TM1650.h"
#include <ESP8266WiFi.h>
#include <dht.h>
#include<math.h>

//加速度传感器头文件
// I2Cdev and MPU6050 must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
#include "I2Cdev.h"
#include "MPU6050.h"
MPU6050 accelgyro;

dht DHT;

#define DHT11_PIN D2

TM1650 d;
int16_t ax, ay, az;
int16_t gx, gy, gz;
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

//加速度传感器设置
    accelgyro.initialize();

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
    accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

//************************************************************************************************
//角度计算
  //范围为2g时，换算关系：16384 LSB/g
  //deg = rad*180/3.14
  float x=0, y=0, z=0;
  float Accel_x,Accel_y,Accel_z,Gyro_x,Gyro_y,Gyro_z,Angle_x_temp,Angle_y_temp,Angle_z_temp;
  float Pi = 3.14;
 
  Accel_x = ax; //x轴加速度值暂存
  Accel_y = ay; //y轴加速度值暂存
  Accel_z = az; //z轴加速度值暂存
  Gyro_x = gx;  //x轴陀螺仪值暂存
  Gyro_y = gy;  //y轴陀螺仪值暂存
  Gyro_z = gz;  //z轴陀螺仪值暂存



//加速度 
  //处理x轴加速度
  if (Accel_x<32764) x = Accel_x / 16384;
  else              x = 1 - (Accel_x - 49152) / 16384;
  //处理y轴加速度
  if (Accel_y<32764) y = Accel_y / 16384;
  else              y = 1 - (Accel_y - 49152) / 16384;
  //处理z轴加速度
  if (Accel_z<32764) z = Accel_z / 16384;
  else              z = (Accel_z - 49152) / 16384;


 
  //用加速度计算三个轴和水平面坐标系之间的夹角
  Angle_x_temp = (atan2(z , y)) * 180 / Pi;
  Angle_y_temp = (atan2(x , z)) * 180 / Pi;
  Angle_z_temp = (atan2(y , x)) * 180 / Pi;
  //角度的正负号
  if (Accel_y<32764) Angle_y_temp = +Angle_y_temp;
  if (Accel_y>32764) Angle_y_temp = -Angle_y_temp;
  if (Accel_x<32764) Angle_x_temp = +Angle_x_temp;
  if (Accel_x>32764) Angle_x_temp = -Angle_x_temp;
  if (Accel_z<32764) Angle_z_temp = +Angle_z_temp;
  if (Accel_z>32764) Angle_z_temp = -Angle_z_temp;
  //角速度
  //向前运动
  if (Gyro_x<32764) Gyro_x = -(Gyro_x / 16.4);//范围为2000deg/s时，换算关系：16.4 LSB/(deg/s)
  //向后运动
  if (Gyro_x>32764) Gyro_x = +(65535 - Gyro_x) / 16.4;
  //向前运动
  if (Gyro_y<32764) Gyro_y = -(Gyro_y / 16.4);//范围为2000deg/s时，换算关系：16.4 LSB/(deg/s)
  //向后运动
  if (Gyro_y>32764) Gyro_y = +(65535 - Gyro_y) / 16.4;
  //向前运动
  if (Gyro_z<32764) Gyro_z = -(Gyro_z / 16.4);//范围为2000deg/s时，换算关系：16.4 LSB/(deg/s)
  //向后运动
  if (Gyro_z>32764) Gyro_z = +(65535 - Gyro_z) / 16.4;

  
//加速度传感器串口打印
    Serial.print("Speed_x:");
    Serial.print(x); Serial.print("\n");
    Serial.print("Speed_y:");
    Serial.print(y); Serial.print("\n");
    Serial.print("Speed_z:");
    Serial.print(z); Serial.print("\n");
    Serial.print("Gyro_x:");
    Serial.print(Gyro_x);Serial.print("\n");
    Serial.print("Gyro_y:");
    Serial.print(Gyro_y);Serial.print("\n");
    Serial.print("Gyro_z:");
    Serial.print(Gyro_z);Serial.print("\n");


   int chk = DHT.read11(DHT11_PIN);
  Serial.print("temperature:");                  //串口打印温度  
  Serial.println(DHT.temperature, 1);  
  
  Serial.print("\n");Serial.print("\n");Serial.print("\n");

  pinMode(DHT11_PIN, OUTPUT);
    digitalWrite(DHT11_PIN, LOW);
    Wire.begin(); 

  d.init();                                       //初始化
  d.displayOn();
 
    TEMP = DHT.temperature;            //温度赋值
    
    char temp[]="";
    dtostrf(TEMP*100,4,0,temp);   //数码管的温度和湿度显示
    d.displayString(temp);
    d.setDot(1,1);




//云平台设置内容
    
     String postdata = String("")+"{\"Temperature_1\":"+TEMP+",\"PlusSpeed_x\":"+x+",\"PlusSpeed_y\":"+y+",\"PlusSpeed_z\":"+z+"}";        //根据云平台数据流进行修改
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

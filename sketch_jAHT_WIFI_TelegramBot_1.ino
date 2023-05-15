#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
// #include <DHT.h>
#include <Wire.h>
#include <Adafruit_AHTX0.h>
#define BOT_TOKEN "5847787737:AAFG37PXtsNPQV3EgA6NvBMDigyeA6_qTjc"
String temp_1;
String vlaj_1;
#include <FastBot.h>
FastBot bot(BOT_TOKEN);
bool alarm_temp;

ESP8266WebServer server(80);

Adafruit_AHTX0 aht;

Adafruit_Sensor *aht_humidity, *aht_temp;

// #define DHTPIN D4      // к этому контакту подключен датчик DHT11
// #define DHTTYPE DHT11  // указываем тип используемого dht датчика: DHT11 или DHT22
// DHT dht(DHTPIN, DHTTYPE);


void headroot(void)  // Процедура обработки запроса GET , "/"
{
  String web = "";
  web += "Html UAU-UAU       </br>";
  web += "Temperatura :      </br>";
  web += temp_1;
  web += "</br> Vlajnost :      </br>";
  web += vlaj_1;
  web += "</br>   my local IP    </br>";
  web += WiFi.localIP().toString();
  server.send(200, "text/html", web);
}

// обработчик сообщений Telegram
void newMsg(FB_msg& msg) {
  // выводим всю информацию о сообщении
  Serial.println(msg.toString());
  String tel_mes = "";
  tel_mes = "Temperatura home: " + temp_1 + "\n Vlajnost home: " + vlaj_1;

  bot.setChatID(msg.chatID);
  // отправить сообщение обратно
  bot.sendMessage(tel_mes, msg.chatID);
}

void setup() {
  Serial.begin(115200);
  alarm_temp=0;


  const char* ssid = "DIR-300NRU";
  const char* password = "15101979";
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);                   // Задаем режим работы WIFI_STA (клиент)
  WiFi.begin(ssid, password);            // Подключаемся
  while (WiFi.status() != WL_CONNECTED)  // Ждем пока статус не станет WL_CONNECTED
  {
    delay(500);
  }

  Serial.println("WiFi connected ");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  // показывает наше IP
  Serial.print("Subnet mask: ");

  server.on("/", headroot);  // Ответ сервера на запрос главной страницы
  server.begin();            //Запуск сервера
  Wire.begin();
  bot.attach(newMsg);
  // bot.chatIDs(2113497626);
  if (!aht.begin()) {
    Serial.println("Failed to find AHT10/AHT20 chip");
    while (1) {
      delay(10);
    }
  }

  //   Serial.println("AHT10/AHT20 Found!");
  // aht_temp = aht.getTemperatureSensor();
  // aht_temp->printSensorDetails();

  // aht_humidity = aht.getHumiditySensor();
  // aht_humidity->printSensorDetails();
}




void loop() {
  sensors_event_t humidity, temp;
  char buffer[15];
  char buffer2[15];
  float tempf;
  float vlajf;
  aht.getEvent(&humidity, &temp);  // populate temp and humidity objects with fresh data
  tempf = temp.temperature;
  vlajf = humidity.relative_humidity;
// переведем в текст значение времени и влажности

  sprintf(buffer, "%d.%02d", (int)tempf, (int)(tempf * 100) % 100);
  temp_1 = String(buffer);
  sprintf(buffer2, "%d.%02d", (int)vlajf, (int)(vlajf * 100) % 100);
  vlaj_1 = String(buffer2);
// При привышении температуры в чат идут сообщения
if(tempf>float(29)&&(!alarm_temp)){
  alarm_temp=1;
  String tel_mes = "";
  tel_mes = "!!!!Temperatura home: " + temp_1 + "\n Vlajnost home: " + vlaj_1;
  bot.sendSticker("CAACAgEAAxkBAAEHdJdj0nTEOJHmmiCcJ1cfUPQtNfWcRAACEgEAAjgOghHo9UX5qETZii0E");
  // отправить сообщение обратно
  bot.sendMessage(tel_mes);
}

  Serial.print("Temperature: ");
  Serial.print(temp_1);
  Serial.println(" degrees C");
  Serial.print("Humidity: ");
  Serial.print(vlaj_1);
  Serial.println("% rH");

  delay(900);
  bot.tick();



  server.handleClient();  //обработка текущих входящих HTTP-запросов
  delay(100);
}

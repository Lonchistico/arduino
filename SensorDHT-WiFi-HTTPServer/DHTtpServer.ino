/* El script inicial, HelloServer, Sencillo y util para agregar diferentes modulos a la placa
 *  05-23-2018
 *  ponkbrown
 */
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include "DHT.h"
#define DHTTYPE DHT11
#define DHTPIN D1

const char* ssid = "";
const char* password = "";

ESP8266WebServer server(80);

DHT dht(DHTPIN, DHTTYPE);
// Led, este es para reportar estatus
const int led = D0;

void handleRoot() {
  /* Este es la funcion para el requrso raiz '/' */
  // TODO Aqui deberia de encapsular la salida de los diferentes modulos, y hacerlo un solo script
  digitalWrite(led, 0);
  delay(500);
  String dht11 = jsonDHT();
  server.send(200, "text/plain", dht11);
  digitalWrite(led, 1);
}

void handleNotFound(){
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}

String jsonDHT(){
  String json;
  // -- obtiene datos de el sensor  
  // Leemos la (h)Humedad relativa y la temperatura en (t)Centigrados y (f)Farenheit
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float f = dht.readTemperature(true);
  //Comprobamos si hay error en la lectura
  if(isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Error obteniendo datos!!!!");
    json = "{\"DHT\":{\"ERROR\": \"Error Obteniendo Datos de el Sensor\"}}";
  }
  else{
  //Calcular el indice de calor en Fahrenheit
  float hif = dht.computeHeatIndex(f,h);
  //Calcula el indice de calor en Centigrados
  float hic = dht.computeHeatIndex(t,h, false);
  json = "{\"DHT\":{\"h\": "+ String(h) +" , \"t\" : "+ String(t) +", \"f\" :"+ String(f) +", \"hif\" :"+  String(hif) +", \"hic\" :"+  String(hic) +"}}";
  }
  
  return json;
}

void setup(void){
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Conectado a: ");
  Serial.println(ssid);
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
  for(int x = 0; x < 10; x++){
    // parpadea el led cuando se conecta al wifi
    digitalWrite(led, 1);
    delay(200);
    digitalWrite(led, 0);
    delay(100);
    }

// mDNS para localizarlo mas facil en la red con zeroconfig o bonjour  
  if (MDNS.begin("steamhood")) {
    Serial.println("MDNS Inicializado");
  }

  server.on("/", handleRoot);

  server.on("/ip",[](){
    IPAddress ip = WiFi.localIP();
    Serial.print(ip);
    String message= String(ip[0]) + String(".") + \
        String(ip[1]) + String(".") + \
        String(ip[2]) + String(".") + \
        String(ip[3]);
    server.send(200, "text/plain",message);
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP Inicializado");
}

void loop(void){
  server.handleClient();
}

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <DHT.h>
//-------------------------------------------------------------
#define LEDROJO D7
#define TRIGGER D6
#define ECHO D5
// Constantes...
// Velocidad del sonido en cm/s; distancia de accion
const float sonido = 34300.0; 
const float umbral3 = 10.0;
//coneccionWifi
const char* ssid = "INFINITUM3479";
const char* password = "sY21veYvHn";
// Led, este es para reportar estatus
const int led = D0;
//-------------------------------------------------------------
ESP8266WebServer server(80);
//-------------------------------------------------------------
void setup() {
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(115200);
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
//---------------------------------------------------------------
  // Modo entrada/salida de los pines
  pinMode(LEDROJO, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(TRIGGER, OUTPUT); 
  // Apagamos todos los LEDs
  apagarLEDs();
}

void loop() {
  String json; 
  //Comprobamos si hay error en la lectura
  if(isnan(TRIGGER)|| isnan(ECHO) || isnan(led)) {
  Serial.println("Error obteniendo datos!!!!");
  json = "{\"DHT\":{\"ERROR\": \"Error Obteniendo Datos de el Sensor\"}}";
  }
  else
  {    
  //Error en conección
  handleNotFound();
  //Preparamos el sensor de ultrasonidos
  iniciarTrigger(); 
  // Obtenemos la distancia
  float distancia = calcularDistancia();
  
  // Apagamos todos los LEDs
  apagarLEDs(); 
  // Lanzamos alerta si estamos dentro del rango de peligro
  if (distancia <= umbral3) 
  {
    // Lanzamos alertas
    alertas(distancia);
    delay(500);
  } 
  else
  {
  apagarLEDs(); 
  }
  json = "{\"Sensor\":{\"US\": "+ String(distancia) +"}}"; 
  }
  

}//loop...
//Error de conección...
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
//Apaga todos los LEDs...
void apagarLEDs()
{
  // Apagamos todos los LEDs
  digitalWrite(LEDROJO, LOW);
}
// Función que comprueba si hay que lanzar alguna alerta visual o sonora
void alertas(float distancia)
{  
  if (distancia <= umbral3)
  {
    // Encendemos el LED rojo
    digitalWrite(LEDROJO, HIGH);
  }
}
//Método que calcula la distancia a la que se encuentra un objeto.
//Devuelve una variable tipo float que contiene la distancia
float calcularDistancia()
{
  // La función pulseIn obtiene el tiempo que tarda en cambiar entre estados, en este caso a HIGH
  unsigned long tiempo = pulseIn(ECHO, HIGH);
 
  // Obtenemos la distancia en cm, hay que convertir el tiempo en segudos ya que está en microsegundos
  // por eso se multiplica por 0.000001
  float distancia = tiempo * 0.000001 * sonido / 2.0;
  Serial.print(distancia);
  Serial.print("cm");
  Serial.println();
  delay(500); 
  return distancia;
}
//Método que inicia la secuencia del Trigger para comenzar a medir
void iniciarTrigger()
{
  // Ponemos el Triiger en estado bajo y esperamos 2 ms
  digitalWrite(TRIGGER, LOW);
  delayMicroseconds(2);
 
  // Ponemos el pin Trigger a estado alto y esperamos 10 ms
  digitalWrite(TRIGGER, HIGH);
  delayMicroseconds(10);
 
  // Comenzamos poniendo el pin Trigger en estado bajo
  digitalWrite(TRIGGER, LOW);
}



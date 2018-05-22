#include <ESP8266WiFi.h> 
const char* ssid = "INFINITUM3479";
const char* password = "sY21veYvHn"; 
int ledPin = D3;
WiFiServer server(80);
//---------------------fotoresistor-----------------------------
// Pin analogico de entrada para el LDR
int pinLDR = A0; 
// Variable donde se almacena el valor del LDR
float valorLDR = 0; 
float fotoRes (float valorLDR){
// Guardamos el valor leido del ADC en una variable
// El valor leido por el ADC (voltaje) aumenta de manera directamente proporcional
// con respecto a la luz percibida por el LDR
valorLDR= analogRead(pinLDR);  
// Devolver el valor leido a nuestro monitor serial en el IDE de Arduino
Serial.println(valorLDR); 
// Imprime el valor de ADC en pantalla...
return valorLDR;      
}
//-------------------ultrasonido---------------------------------
// DECLARACION DE VARIABLES PARA PINES
const int pinecho = D5;
const int pintrigger = D6;
const int pinled = D7; 
// VARIABLES PARA CALCULOS
unsigned int tiempo, distancia;
int ultraSon(int distancia){
// ENVIAR PULSO DE DISPARO EN EL PIN "TRIGGER"
digitalWrite(pintrigger, LOW);
delayMicroseconds(2);
digitalWrite(pintrigger, HIGH);
// EL PULSO DURA AL MENOS 10 uS EN ESTADO ALTO
delayMicroseconds(10);
digitalWrite(pintrigger, LOW); 
// MEDIR EL TIEMPO EN ESTADO ALTO DEL PIN "ECHO" EL PULSO ES PROPORCIONAL A LA DISTANCIA MEDIDA
tiempo = pulseIn(pinecho, HIGH); 
// LA VELOCIDAD DEL SONIDO ES DE 340 M/S O 29 MICROSEGUNDOS POR CENTIMETRO
// DIVIDIMOS EL TIEMPO DEL PULSO ENTRE 58, TIEMPO QUE TARDA RECORRER IDA Y VUELTA UN CENTIMETRO LA ONDA SONORA
distancia = tiempo / 58;
return distancia; 
}
//---------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  delay(10);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW); 
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid); 
  WiFi.begin(ssid, password); 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected"); 
  // Start the server
  server.begin();
  Serial.println("Server started"); 
  // Print the IP address
  Serial.print("Use this URL : ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
  //--------------------ultraSon--------------------------------------------------
   // CONFIGURAR PINES DE ENTRADA Y SALIDA
  pinMode(pinecho, INPUT);
  pinMode(pintrigger, OUTPUT);
  pinMode(pinled, OUTPUT);
}
void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  } 
  // Wait until the client sends some data
  Serial.println("new client");
  while(!client.available()){
    delay(1);
  } 
  // Read the first line of the request
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush(); 
  // Match the request 
  int value = LOW;
  if (request.indexOf("/LED=ON") != -1) {
    digitalWrite(ledPin, HIGH);
    value = HIGH;
  } 
  if (request.indexOf("/LED=OFF") != -1){
    digitalWrite(ledPin, LOW);
    value = LOW;
  }   
  // Return the response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); //  do not forget this one
  client.println("<!DOCTYPE HTML>");
  client.println("<html>"); 
  client.print("Led pin is now: "); 
  if(value == HIGH) {
    client.print("On");  
  } else {
    client.print("Off");
  }
  client.println("<br><br>");
  client.println("Click <a href=\"/LED=ON\">here</a> turn the LED on pin 5 ON<br>");
  client.println("Click <a href=\"/LED=OFF\">here</a> turn the LED on pin 5 OFF<br>");
  client.println("</html>"); 
  delay(1);
  Serial.println("Client disconnected");
  Serial.println("");  
  if(fotoRes > 0)
  {
   client.println("<br><br>");
   client.println("I N T E N C I D A D. "+String(valorLDR)+"LDR's");
  }
   // ENVIAR EL RESULTADO AL MONITOR SERIAL
  Serial.print(distancia);
  Serial.println(" cm");
  delay(200); 
  // ENCENDER EL LED CUANDO SE CUMPLA CON CIERTA DISTANCIA
  if (distancia <= 15) {
    digitalWrite(pinled, HIGH);
    delay(500);
  } else {
    digitalWrite(pinled, LOW);
  }  
  }

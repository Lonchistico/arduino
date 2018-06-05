#define LEDROJO D7
#define TRIGGER D6
#define ECHO D5
 
// Constantes
const float sonido = 34300.0; // Velocidad del sonido en cm/s
const float umbral3 = 10.0;
//--------------------------------------------------------------
void setup() {
  // Iniciamos el monitor serie
  Serial.begin(115200);
  // Modo entrada/salida de los pines
  pinMode(LEDROJO, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(TRIGGER, OUTPUT); 
  // Apagamos todos los LEDs
  apagarLEDs();
}

void loop() {
   // Preparamos el sensor de ultrasonidos
  iniciarTrigger(); 
  // Obtenemos la distancia
  float distancia = calcularDistancia(); 
  // Apagamos todos los LEDs
  apagarLEDs(); 
  // Lanzamos alerta si estamos dentro del rango de peligro
  if (distancia < umbral3)
  {
    // Lanzamos alertas
    alertas(distancia);
  }
}
//---------------------Funciones----------------------------------
// Apaga todos los LEDs
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
// Método que calcula la distancia a la que se encuentra un objeto.
// Devuelve una variable tipo float que contiene la distancia
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
// Método que inicia la secuencia del Trigger para comenzar a medir
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



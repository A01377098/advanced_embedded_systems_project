// A01379571 | Juan Daniel Aranda Morales     
// A01377098 | Eva Denisse Vargas Sosa    
// A01750164 | Paul Martín García Morfín  
// ITESM CEM  

// TRANSMISOR Y RECEPTOR
// NodeMCU ID: 0x038

// Librerías para la comunicación CAN
#include <SPI.h>
#include <mcp2515.h>

// Librerías para usar la placa de desarollo
#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>
#include <addons/RTDBHelper.h>

//Conexión a wifi
#define WIFI_SSID "Galaxys10"
#define WIFI_PASSWORD "hola12345"

//Base de datos
#define DATABASE_URL "https://reto-jd-default-rtdb.firebaseio.com"
#define DATABASE_SECRET "BABN7pP4hO8HULEVBDhGMUlXGa5XHZOof3xVUVGd"

//Firebase data object
FirebaseAuth auth;
FirebaseData fbdo;

//Firebase data config
FirebaseConfig config;

unsigned long dataMillis = 0;
int contador = 0;

String dato = "";

// variable de comando del usuario 
String datof = "";

int datoInt;

// Estructura de los mensajes a mandar y recibir 
struct can_frame canMsgenvio;
struct can_frame canMsgrecibo;
MCP2515 mcp2515(15);


void setup() {
  Serial.begin(115200);
  
  // Inicializar Wifi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("Buscando wifi...");
  
  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Conexión Exitosa IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
    
  // Inicializar Base de datos FB
  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);  
  config.database_url = DATABASE_URL;
  config.signer.tokens.legacy_token = DATABASE_SECRET;
  
  Firebase.reconnectWiFi(true);
  
  Firebase.begin(&config, &auth);
  
  // Inicializar CAN
  mcp2515.reset();
  mcp2515.setBitrate(CAN_125KBPS);
  mcp2515.setNormalMode();
  canMsgenvio.can_id = 0x038; // ID del NodeMCU 
  canMsgenvio.can_dlc = 1; // Número de espacios a utilizar
  Serial.println("--CAN inicializado con éxtio--");
}

void loop() {

  //Enviando datos a la BD
  recibiendo_datos();
    
  // Leyendo los mensajes entrantes durante 3s 
  while(contador < 150){
    
    //Enviando el dato (el numero 1 es como prueba, mientras se crea la variable en la GUI)        
    if(Firebase.RTDB.getString(&fbdo, "Movimiento")){
      datof = fbdo.stringData();
      Serial.print("Dato de la FireBase: "); 
      Serial.println(datof);
      Serial.println("");
      
      if(datof == "\"1\""){
        datoInt = 1;
      }
      else if(datof == "\"0\""){
        datoInt = 0;
      }
      else if(datof == "\"2\""){
        datoInt = 2;
      }
      else if(datof == "\"3\""){
        datoInt = 3;
      }
      else if(datof == "\"4\""){
        datoInt = 4;
      }   
      
      canMsgenvio.data[0] = datoInt; // Actualizar el valor de la base de datos
      mcp2515.sendMessage(&canMsgenvio); 
      Serial.print("Dato enviado: ");
      Serial.println(datoInt);
      Serial.println("");
    
      contador+=1;
    }
  }
    
  // Reset de la variable de tiempo
  contador = 0;
}

// Función de recibir datos del Arduino Mega y Uno. 
void recibiendo_datos() {
  Serial.println("Recibiendo temperatura...");
  if (mcp2515.readMessage(&canMsgrecibo) == MCP2515::ERROR_OK) {
    // Identificación del dispositivo que manda el mensaje 
    // Id del arduino Uno = 0x036  -> Motores
    // Id del arduino Mega = 0x037 -> Temperatura 
    if (canMsgrecibo.can_id == 0x037){ // Leyendo del Arduino Mega
      Firebase.RTDB.setInt(&fbdo, "Temperatura",canMsgrecibo.data[0]);
      Serial.println("Dato recibido y enviado");
    }      
  }
}

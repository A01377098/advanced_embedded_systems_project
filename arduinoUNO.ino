  // A01379571 | Juan Daniel Aranda Morales     
// A01377098 | Eva Denisse Vargas Sosa    
// A01750164 | Paul Martín García Morfín  
// ITESM CEM           

// TRANSMISOR Y RECEPTOR
// ADUINO UNO ID: 0x036

// Librerías para la comunicación CAN
#include <SPI.h> 
#include <mcp2515.h>

// Estructura de los mensajes a mandar y recibir 
struct can_frame canMsgrecibo; // Recibe dato
struct can_frame canMsgenvio;  // Envió dato
MCP2515 mcp2515(10); // Pin CS del Arduino UNO 

// Variable de movimiento 
int movimiento;

// Pines de llantas 
// Derecha
int entrada1 = 6;
int entrada2 = 7;
// Izquierda 
int entrada3 = 8;
int entrada4 = 9;

void setup() {
  Serial.begin(115200);   
              
  // Inicializar CAN
  mcp2515.reset();
  mcp2515.setBitrate(CAN_125KBPS); 
  mcp2515.setNormalMode();
  canMsgenvio.can_id = 0x036; // ID del Arduino Uno
  canMsgenvio.can_dlc = 1; // Número de espacios a utilizar
  Serial.println("--CAN inicializado con éxtio--");
}

void loop(){

  // Verificar la comunicación CAN
  if (mcp2515.readMessage(&canMsgrecibo) == MCP2515::ERROR_OK) {
  
    // Identificación del dispositivo que manda el mensaje 
    if(canMsgrecibo.can_id == 0x038){ // Leyendo del NodeMCU
      
      // Leyendo el mensaje
      movimiento  = canMsgrecibo.data[0];
      // Enviando mensaje
      canMsgenvio.data[0] = movimiento;
      mcp2515.sendMessage(&canMsgenvio); 
      Serial.println("Dato enviado");

      // Control de los motores
      if(movimiento == 1){
        Serial.println("Adelante");
        digitalWrite(entrada1, HIGH);
        digitalWrite(entrada2, LOW);
        digitalWrite(entrada3, HIGH);
        digitalWrite(entrada4, LOW);
      } else if(movimiento == 2){
        Serial.println("Atrás");
        digitalWrite(entrada1, LOW);
        digitalWrite(entrada2, HIGH);
        digitalWrite(entrada3, LOW);
        digitalWrite(entrada4, HIGH);
      } else if(movimiento == 3){
        Serial.println("Giro izquierda");
        digitalWrite(entrada1, HIGH);
        digitalWrite(entrada2, LOW);
        digitalWrite(entrada3, LOW);
        digitalWrite(entrada4, HIGH);
      } else if(movimiento == 4){
        Serial.println("Giro Derecha");
        digitalWrite(entrada1, LOW);
        digitalWrite(entrada2, HIGH);
        digitalWrite(entrada3, HIGH);
        digitalWrite(entrada4, LOW);
      } else {
        Serial.println("STOP");
        digitalWrite(entrada1, LOW);
        digitalWrite(entrada2, LOW);
        digitalWrite(entrada3, LOW);
        digitalWrite(entrada4, LOW);
      }
    }
  }
}

// A01379571 | Juan Daniel Aranda Morales     
// A01377098 | Eva Denisse Vargas Sosa    
// A01750164 | Paul Martín García Morfín  
// ITESM CEM

// TRANSMISOR Y RECEPTOR
// ADUINO MEGA 

// Librerías para la comunicación CAN
#include <SPI.h> 
#include <mcp2515.h>

//Librería para el sensor de temperatura 
#include <DHT.h>

//Librería para la comunicación I2C con LCD 
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

// Declaración de la variable para el LCD 
LiquidCrystal_I2C lcd(0x27,16,2);
 
// Estructura de los mensajes a mandar y recibir 
struct can_frame canMsgrecibo; // Recibe dato
struct can_frame canMsgenvio;  // Envió dato
MCP2515 mcp2515(53); // Pin CS del Arduino Mega 

// Declaración de variables para el sensor de temperatura 
#define DHTPIN 3
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// Declaración de la variable de temperatura 
float temp;

//Límites de la temperatura
float limitInf = 19;
float limitSup = 24;

// Ventilador 
int ventilador = 7;

void setup() {
  Serial.begin(115200);
  
  //Ventilador
  pinMode(ventilador, OUTPUT);

  //Inicializar LCD
  lcd.init();    
  lcd.backlight();               
  lcd.setCursor (0,0);     
  lcd.print ("Iniciando...");
  delay(1000);
  lcd.clear();               
  
  //Inicializar CAN
 // SPI.begin();
  mcp2515.reset();
  mcp2515.setBitrate(CAN_125KBPS); 
  mcp2515.setNormalMode();
  canMsgenvio.can_id = 0x037; // ID del Arduino Mega 
  canMsgenvio.can_dlc = 1;

  // Inicializador del sensor 
  dht.begin();
}

void loop(){
  
  //Leyendo temperatura
  temp = dht.readTemperature();
  // Posición del cursor en el LCD
  lcd.setCursor (0,0);
  lcd.print ("Temperatura");
  lcd.setCursor (0,1); 
  lcd.print(temp);

  //Comparando Temp
  if(temp > limitSup){
    //Enciende ventilador
    digitalWrite(ventilador, HIGH);
  }else{
    //Apaga ventilador
    digitalWrite(ventilador, LOW);
  }

  //Enviando el dato            
  canMsgenvio.data[0] = temp; // Actualizar el valor de la distancia en [0] 
  mcp2515.sendMessage(&canMsgenvio); 
  Serial.println("Dato enviado");
  Serial.println("");
  Serial.println("");
  
  // Leyendo los mensajes entrantes durante 3s 
    receptor();
}

// Función de receptor de mensajes 
void receptor()
{
  Serial.println("Recibiendo...");
  if (mcp2515.readMessage(&canMsgrecibo) == MCP2515::ERROR_OK) {
    Serial.println("Dentro del if");
    // Identificación del dispositivo que manda el mensaje 
    if(canMsgrecibo.can_id == 0x036){
      // Leyenndo el mensaje
        int movimiento  = canMsgrecibo.data[0];
        Serial.println(movimiento);
        // Imprime STOP/MOVE dependiendo de lo que mensaje diga
        if (movimiento == 1 || movimiento == 2 || movimiento == 3 || movimiento == 4){
          Serial.println("MOVE");
          lcd.setCursor(12,1);
          lcd.print("MOVE");
      }else{
        Serial.println("STOP");
        lcd.setCursor(12,1);
        lcd.print("STOP");
      }
         
  }
}

//delay(200);
}

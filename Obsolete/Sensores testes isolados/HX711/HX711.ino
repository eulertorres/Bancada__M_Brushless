// --- Configuração de pinos e parâmetros ---
//#include "HX711.hpp"
#include <Arduino.h>
#include <Servo.h>
#include <SD.h>
#include <SPI.h>

//=============================================================================
// 						Definição dos pinos
//=============================================================================
const uint8_t servo_pin {7};
const uint8_t CS_SDcard {4};
const uint8_t LED 		{8};
const uint8_t HX711_DOUT{10};
const uint8_t HX711_SCK	{9};

//=============================================================================
// 							Sensores
//=============================================================================

//HX711 Balanca(HX711_DOUT, HX711_SCK, "Empuxo");  //Dt, CSK

//Sensor* sensores[] = {
	//&Balanca
//};

//const uint8_t N_sensores = sizeof(sensores)/sizeof(sensores[0]);

int16_t thrust_g = 0;

volatile bool botao = false;



//=============================================================================
// 						Variaveis globais
//=============================================================================
#define GAIN_PULSES     1   // 1 pulso = canal A ganho 128


// Controle de Tempo ------------------------------------------------
unsigned long lastRise, now, HIGH_time, lastSample, lastInterrupt, lastLog;

// ---------------------  Controle do ESC   -------------------------
uint16_t sample_dt = 1000, min_PWM = 1500, max_PWM = 2000;
Servo esc;
bool Inverte_PWM = false;

// ---------------------     Cartão SD	------------------------------------------------
File logfile;

//bool lock=false;
char data;

// ---------------------     Funções	------------------------------------------------
bool testRunning = false;
uint16_t testPWM = min_PWM;

float offset = 1974;      // “tare” zero
float scale  = 114.6;   // fator de calibração (raw/gramas)

//=============================================================================
// 						Funções auxiliares
//=============================================================================

void ProcessSerial(){
	if (Serial.available() > 0){
		
		if(isDigit(Serial.peek())){
			setPWM(Serial.parseInt());
			Serial.read();
			return;
		}
		
		data = Serial.read();	
		if(data == 'd'){	
			openNewLog();
			testRunning = true;
			Serial.read();
		}
		else if (data == 's'){
			testRunning = false;
			closeCurrentLog();
			Serial.read();
		}
		
	}
}

void setPWM(uint16_t currentPWM){
	if (currentPWM > max_PWM) currentPWM = max_PWM;
	if (currentPWM > min_PWM) currentPWM = min_PWM;
	currentPWM = Inverte_PWM ? (min_PWM + max_PWM - currentPWM) : currentPWM;
	esc.writeMicroseconds(currentPWM);
}

void showError(){
	for(int i = 0; i<8; i++){
		digitalWrite(LED, !digitalRead(LED));
		delay(200);
	}
}

void openNewLog(){
	closeCurrentLog();
	digitalWrite(LED, HIGH);
	int idx = 1;
	String path;
	do{
		path = "LOGs/LOG" + String(idx) + ".CSV";
		idx++;
	} while(SD.exists(path));
	
	logfile = SD.open(path, FILE_WRITE);
	if (!logfile){
		showError();
		return;
	}
	
	String header = "t[ms], PWM[us]";
	//for (auto sensor : sensores){
	//	//sensor -> calibra();
	//	header += "," + sensor -> getLabel();
	//}
	header += "\n";
	logfile.print(header);
	logfile.flush();
	
	lastLog = millis();
}

void closeCurrentLog(){
	if (logfile){
		logfile.close();
		digitalWrite(LED, LOW);
	}
}

void amostra() {
	now = millis(); 
	
	unsigned long dt = now - lastSample;
	if (dt < sample_dt || !logfile) return;
	lastSample = now;
	String logline = String(dt);
	
	//for (auto sensor : sensores){
	//	logline += ";" + String(sensor->read()); 
	//	Serial.println(logline);
	//}
	logline += "\n";
	Serial.println(logline);
}

void HandleInt(){
	if (botao){
		
		botao = false;
	}
}


// --- Função de inicialização ---
void setup() {
  
  Serial.begin(115200);
	SPI.begin();
  
  pinMode(HX711_SCK, OUTPUT);
  pinMode(HX711_DOUT, INPUT);
  digitalWrite(HX711_SCK, HIGH);

  // calibra offset com célula vazia
  Serial.println("Tare: calibrando zero sem carga...");
  //offset = readRawAverage(10);
  Serial.print("Offset ajustado para: ");
  Serial.println(offset);
  Serial.println("Pronto para medições.");
  
	while(Serial.available()) Serial.read(); 
	
	//
	//if(!SD.begin(CS_SDcard)){
	//	Serial.println("Bota o cartaoo");
	//	while(1){
	//		showError();
	//	}
	//}
	//
	//if(!SD.exists("LOGs")){
	//	SD.mkdir("LOGs/");
	//}
	
	EICRA |= (1 << ISC01) | (1 << ISC00);
	EIFR  |= (1 << INTF0);
	EIMSK |= (1 << INT0	);
	
	sei();
    
	//PCMSK2 |= (1 << PCINT19);      // Bit 19 do Registrador PCINT0 em alto (entrada digital 3)  para causar uma interrupção em qualquer mudança.
	Serial.println("prontinhoo");
	lastSample = millis();
}

// --- Loop principal ---
void loop() {
  long raw  = readRawAverage(3);
  float w   = float(raw)/scale - offset;

  Serial.print(raw);      // valor bruto
  Serial.print('\t');     // separador de colunas
  Serial.println(w, 2);   // peso com 2 casas decimais

  delay(50);
}


// --- Lê uma única conversão de 24 bits e aplica seleção de ganho ---
long readRaw() {
  // aguarda DOUT ficar LOW (dado pronto)
  while (digitalRead(HX711_DOUT) == HIGH);

  unsigned long value = 0;
  // lê 24 bits
  for (uint8_t i = 0; i < 24; i++) {
    digitalWrite(HX711_SCK, HIGH);
    value <<= 1;
    if (digitalRead(HX711_DOUT)) {
      value |= 1;
    }
    digitalWrite(HX711_SCK, LOW);
  }
  // pulsos adicionais para ganho
  for (uint8_t i = 0; i < GAIN_PULSES; i++) {
    digitalWrite(HX711_SCK, HIGH);
    digitalWrite(HX711_SCK, LOW);
  }
  // converte de 24 bits two’s-complement para signed long
  if (value & 0x800000) {
    value |= 0xFF000000;
  }
  return (long)value;
}

// --- Lê várias vezes e retorna a média ---
long readRawAverage(uint8_t times) {
  long sum = 0;
  digitalWrite(HX711_SCK, LOW);
  for (uint8_t i = 0; i < times; i++) {
    sum += readRaw();
  }
  digitalWrite(HX711_SCK, HIGH) ;
  return sum / times;
}

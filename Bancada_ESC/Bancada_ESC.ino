/*
 * CÓDIGO PARA BANCADA DE CARACTERIZAÇÃO DE GRUPO MOTOPROPULSOR DE DRONES
 * Versão: 2.0
 * Data: 07/2025
 * Autor: Euler Torres
 * * Esse código salva um csv com leitura de RPM, corrente (ESC), tração hélice, temperatura motor, temperatura ambiente e tensão da bateria.
 * O teste consiste em subir o PWM em 10% e capturar diversas amostras, fazendo uma mapa completo do motor/bateria/hélice
 */
#include <Arduino.h>
#include <Servo.h>
#include <SD.h>
#include <SPI.h>

#include "HX711.hpp"
#include "LM35.hpp"
#include "VoltDiv.hpp"
#include "ACS712.hpp"

//=============================================================================
// 						Definição dos pinos
//=============================================================================
const uint8_t RPM_PIN	{2};
const uint8_t buttPin	{3};
const uint8_t HX711_DOUT{4};
const uint8_t HX711_SCK	{5};
const uint8_t servo_pin	{7};
const uint8_t LED		{8};
const uint8_t CS_SDcard	{10};
const uint8_t TAMB_PIN	{A0};
const uint8_t TMOT_PIN	{A1};
const uint8_t Curr_PIN	{A2};
const uint8_t Volt_PIN	{A3};


//=============================================================================
// 							Sensores
//=============================================================================

HX711 	 Balanca(HX711_DOUT, HX711_SCK, "Empuxo");
LM35 	 Temp_amb(TAMB_PIN, "TempAmbiente");
LM35	 Temp_Mot(TMOT_PIN, "TempMotor");
VoltDiv  Bateria(Volt_PIN,  "Tensao");
ACS712   Corrente(Curr_PIN, "Corrente");

Sensor* sensores[] = {
	&Balanca,
	&Temp_amb,
	&Temp_Mot,
	&Bateria,
	&Corrente
};

const uint8_t N_sensores = sizeof(sensores)/sizeof(sensores[0]);

int16_t thrust_g = 0;

volatile bool botao = false;

//=============================================================================
// 						Variaveis globais
//=============================================================================

// Controle de Tempo ------------------------------------------------
unsigned long now;
volatile unsigned long lastInterrupt;

// ---------------------  Controle do ESC   -------------------------
uint16_t sample_dt = 500, min_PWM = 1500, max_PWM = 2000;
uint8_t	 step_PWM = 1;
Servo esc;
bool Inverte_PWM = false;

// ---------------------     Cartão SD	------------------------------------------------
unsigned long lastSample, lastLog;
File logfile;
char data;
// ---------------------        RPM		------------------------------------------------
const int N_pas =	2;
volatile unsigned long contadorPulsos = 0;
// ---------------------       Debug	------------------------------------------------
bool debugging = false;
// ---------------------       Teste    ------------------------------------------------
bool testRunning = false;
uint16_t  target_PWM = 1500, ms_increasse = 80, targetPWMsample= 1500;
unsigned long lastPWMUpdate, sampleTime = 6000;

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
			beginTest();
			Serial.read();
		}
		else if (data == 'a'){
			showError();
			openNewLog();
			debugging = true;
		}
		else if (data == 's'){
			debugging = false;
			endTest();
			Serial.read();
		}
	}
}

void beginTest(){
	target_PWM = min_PWM;
	setPWM(min_PWM);
	openNewLog();
	testRunning = true;
	//Serial.println("comecou");
	lastPWMUpdate = millis();	
}

void runTest(){
	if(target_PWM < max_PWM){
	// Enquanto nao chegar no máximo
		if (target_PWM >= targetPWMsample){
			amostra();
			if (now - lastPWMUpdate >= sampleTime) targetPWMsample += 50;
		}			
		else if (now - lastPWMUpdate >= ms_increasse){
			target_PWM++;
			lastPWMUpdate = now;
		}
	} else{
		amostra();
		if (now - lastPWMUpdate >= sampleTime) endTest();
	}
	//Serial.println(target_PWM);
	setPWM(target_PWM);	
}

void endTest(){
	target_PWM = min_PWM;	
	setPWM(min_PWM);
	closeCurrentLog();
	testRunning = false;
}

void debug(){
	//Serial.println("Debugando");
	amostra();
}

void setPWM(uint16_t currentPWM){
	if (currentPWM > max_PWM) currentPWM = max_PWM;
	if (currentPWM < min_PWM) currentPWM = min_PWM;
	currentPWM = Inverte_PWM ? (min_PWM + max_PWM - currentPWM) : currentPWM;
	esc.writeMicroseconds(currentPWM);
}

void showError(){
	//Serial.println("tentando rodar issaqui");
	for(int i = 0; i<16; i++){
		digitalWrite(LED, !digitalRead(LED));
		delay(100);
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
	for (auto sensor : sensores){
		sensor -> calibra();
		header += "," + sensor -> getLabel();
	}
	header += "\n";
	logfile.print(header);
	
	digitalWrite(HX711_SCK, LOW);
	Serial.println(path);
	contadorPulsos = 0;
	lastLog = millis();
}

void closeCurrentLog(){
	if (logfile){
		logfile.close();
		digitalWrite(LED, LOW);
	}
	digitalWrite(HX711_SCK, HIGH);
	//digitalWrite(LED, LOW);
}

void amostra() {
	unsigned long dt = now - lastSample;
	unsigned long elapsed = now - lastLog;
	if (dt < sample_dt) return;
	lastSample = now;
	String logline = String(elapsed) + "," + String(target_PWM);
	
	for (auto sensor : sensores){
		logline += "," + String(sensor->read()); 
	}
	
	noInterrupts();
	float rpm = ((60000.0 / dt) * contadorPulsos) / N_pas;
	contadorPulsos = 0;
	interrupts();
	
	if (debugging) logline += "," + String(contadorPulsos);
	logline += "," + String(rpm) + "\n";
	
	if (debugging) Serial.print(logline);
	logfile.print(logline);
	logfile.flush();
}

void HandleInt(){
	if (botao){
		unsigned long dt = now - lastInterrupt;
		
		if (dt < 200) return;	//Minimo para estabilizar nivel lógico
		//lastInterrupt = now;
		if(dt > 5000 && !testRunning && digitalRead(buttPin)){
			botao = false;
			showError();
			for (auto sensor : sensores){
				sensor -> calibra();
				digitalWrite(LED, !digitalRead(LED));
			}
			digitalWrite(LED, LOW);
			lastInterrupt = millis();
		}
		else if(!digitalRead(buttPin)){
			botao = false;
			lastInterrupt = millis();
			if(!testRunning)	beginTest();
			else endTest();
		}
	}
}

void setup() {
	
	esc.attach(servo_pin, min_PWM, max_PWM);
	setPWM(1500);
	
	Serial.begin(115200);              //Inicia comunicação serial Bluetooth
	
	delay(100);
	
	while(Serial.available()) Serial.read(); 

	pinMode(buttPin, INPUT);
	pinMode(RPM_PIN, INPUT);
	pinMode(LED, OUTPUT);
	
	if(!SD.begin(CS_SDcard)){
		Serial.println("Bota o cartaoo");
		while(1){
			showError();
		}
	}
	
	if(!SD.exists("LOGs")){
		SD.mkdir("LOGs/");
	}
	
	attachInterrupt(digitalPinToInterrupt(buttPin), button_ISR, RISING);
	attachInterrupt(digitalPinToInterrupt(RPM_PIN), pulso_ISR, FALLING);
	Serial.println("prontinhoo");
	lastSample = millis();
}

void loop() {
	now = millis(); 

	ProcessSerial();

	HandleInt();

	
	if (testRunning){
		runTest();
	}
	else if (debugging){
		debug();
	}
}

void button_ISR(){
	botao = true;
	lastInterrupt = millis();
}

void pulso_ISR() {
	contadorPulsos++;
}
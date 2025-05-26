// Programinha feito por Euler Torres 25_05_25
// Bancada de teste propulsão
#include <Arduino.h>
#include "HX711.hpp"

unsigned long loop_timer, esc_loop_timer, timer_channel, button_timer;
int esc=1000;
//bool lock=false;
byte data, trash;

HX711 Balanca(3, 2);  //Dt, CSK

Sensor* sensores[] = {
	&Balanca
};

const uint8_t N_sensores = sizeof(sensores)/sizeof(sensores[0]);

void amostra() {
int16_t thrust;
	
	for (auto sensor : sensores){
		
	}
}

void setup() {
  Serial.begin(115200);              //Inicia comunicação serial Bluetooth
    DDRD |= B10000000;             // Porta 7 output
    //DDRB |= B00101111;             // Porta 8, 9, 10, 11, e 13 output

  Serial.println("Envie: 'd' para aumentar 10% | 'a' para diminuir 10% | '0' para parar o motor");

  while(Serial.available())data = Serial.read(); //Limpa o buffer serial
  data = 0;                                      //Zera os dados

    //PCICR |= (1 << PCIE2);         // Registeador PCIE0  em alto para habilitar scaneamento da interrupção através do PCMSK0
    //PCMSK2 |= (1 << PCINT18);      // Bit 18 do Registrador PCINT0 em alto (entrada digital 2)  para causar uma interrupção em qualquer mudança.
    //PCMSK2 |= (1 << PCINT19);      // Bit 19 do Registrador PCINT0 em alto (entrada digital 3)  para causar uma interrupção em qualquer mudança.
}

void loop() {
  if(Serial.available() > 0){
    data = Serial.read();                               // Faz a leitura da entrada do teclado
    delay(100);                                         // Espera a chegada dos demais bytes
    while(Serial.available() > 0)trash = Serial.read(); // Limpa buffer serial (descarta bytes extras)
    trash = 0;											                    // Variável para lixo da cominicação serial
    if(data == 'd'){
      if(esc<1500)esc += 50;
      Serial.print("Aumenta em 10%. Atualmente está em: "); Serial.println((esc-1000)/5);
    }
    if(data == 'a'){
      if(esc>1000)esc -= 50;
      Serial.print("Diminui em 10%. Atualmente está em: "); Serial.println((esc-1000)/5);
    }
    if(data == '0'){
      esc = 1000;
      Serial.print("Esc em zerado. Atualmente está em: "); Serial.println((esc-1000)/5);
    }
  }

  while(micros() - loop_timer < 20000);                  // Garante que teremos 250hz de atualização
  loop_timer = micros();                                // Zera o contador do loop

  Serial.print("Leitura Balanca: "); Serial.println(Balanca.read());

  PORTD |= B10000000;                                   // Pulso alto pro ESC
  timer_channel = esc + loop_timer;                     // Define o tempo de alto

  while(PORTD >= 64){                                   // Enquanto tiver em alto
	esc_loop_timer = micros();                            //Lê o tempo atual
	if(timer_channel <= esc_loop_timer)PORTD &= B01111111;// Quando o atual for maior que estabelecido, coloca em baixo
  }
}

/* ISR(PCINT2_vect){
  if(PIND & B00000100){
    if(esc<1500 & (millis()>button_timer)){
      esc += 50;
      Serial.print("Aumenta em 10%. Atualmente está em: "); Serial.println((esc-1000)/5);
      button_timer = millis()+2000;
    }
  }
  if(PIND & B00001000){
    if (millis()>button_timer){
      esc = 1000;
      Serial.print("Esc em zerado. Atualmente está em: "); Serial.println((esc-1000)/5);
      button_timer = millis()+2000;
    }
  }
}
 */
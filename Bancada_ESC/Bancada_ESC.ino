// Programinha feito por Euler Torres 11_07_22 EDRA
// Bancada de teste aeropropulsão

unsigned long loop_timer, esc_loop_timer, timer_channel, button_timer;
int esc=1000;
//bool lock=false;
byte data, trash;

void setup() {
  Serial.begin(57600);             //Inicia comunicação serial
    DDRD |= B10000000;             // Porta 7 output
    DDRB |= B00101111;             // Porta 13 output

  Serial.println("Envie: 'd' para aumentar 10% | 'a' para diminuir 10% | '0' para parar o motor");

  while(Serial.available())data = Serial.read(); //Limpa o buffer serial
  data = 0;                                      //Zera os dados

    PCICR |= (1 << PCIE2);         // Registeador PCIE0  em alto para habilitar scaneamento da interrupção através do PCMSK0
    PCMSK2 |= (1 << PCINT18);      // Bit 18 do Registrador PCINT0 em alto (entrada digital 2)  para causar uma interrupção em qualquer mudança.
    PCMSK2 |= (1 << PCINT19);      // Bit 18 do Registrador PCINT0 em alto (entrada digital 2)  para causar uma interrupção em qualquer mudança.
}

void loop() {
  if(Serial.available() > 0){
    data = Serial.read();                               //Faz a leitura da entrada do teclado
    delay(100);                                         //Espera a chegada dos demais bytes
    while(Serial.available() > 0)trash = Serial.read(); //Limpa buffer serial (descarta bytes extras)
    trash = 0;
    if(data == 'd'){
      if(esc<2000)esc += 100;
      Serial.print("Aumenta em 10%. Atualmente está em: "); Serial.println((esc-1000)/10);
    }
    if(data == 'a'){
      if(esc>1000)esc -= 100;
      Serial.print("Diminui em 10%. Atualmente está em: "); Serial.println((esc-1000)/10);
    }
    if(data == '0'){
      esc = 1000;
      Serial.print("Esc em zerado. Atualmente está em: "); Serial.println((esc-1000)/10);
    }
  }

  if(micros() - loop_timer > 4050)PORTB |= B00100000;   // Se o loop for maior que 4050 us, ligamos  LED de alerta
  while(micros() - loop_timer < 4000);                  // Garante que teremos 250hz de atualização
  loop_timer = micros();                                // Zera o contador do loop
  PORTD |= B10000000;                                   // Pulso alto pro ESC
  timer_channel = esc + loop_timer;                     // Define o tempo de alto

  while(PORTD >= 64){                                   // Enquanto tiver em alto
  esc_loop_timer = micros();                            //Lê o tempo atual
  if(timer_channel <= esc_loop_timer)PORTD &= B01111111;  // Quando o atual for maior que estabelecido, coloca em baixo
  }

  switch (esc) {
    case 1000:
      PORTB = B00000000;
      break;
    case 1100:
      PORTB = B00000001;
      break;
    case 1200:
      PORTB = B00000010;
      break;
    case 1300:
      PORTB = B00000011;
      break;
    case 1400:
      PORTB = B00000100;
      break;
    case 1500:
      PORTB = B00000101;
      break;
    case 1600:
      PORTB = B00000110;
      break;
    case 1700:
      PORTB = B00000111;
      break;
    case 1800:
      PORTB = B00001000;
      break;
    case 1900:
      PORTB = B00001001;
      break;
    case 2000:
      PORTB = B00001010;
      break;
    default:
      PORTB |= B00000000;
      break;
  }
}

ISR(PCINT2_vect){
  if(PIND & B00000100){
    if(esc<2000 & (millis()>button_timer)){
      esc += 100;
      Serial.print("Aumenta em 10%. Atualmente está em: "); Serial.println((esc-1000)/10);
      button_timer = millis()+2000;
    }
  }
  if(PIND & B00001000){
    if (millis()>button_timer){
      esc = 1000;
      Serial.print("Esc em zerado. Atualmente está em: "); Serial.println((esc-1000)/10);
      button_timer = millis()+2000;
    }
  }
}

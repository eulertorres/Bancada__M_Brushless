/*
 * CÓDIGO PARA MEDIR RPM COM SENSOR ÓPTICO TCRT5000
 * Versão: 1.0
 * Data: 13/07/2025
 * Autor: Euler
 * * Este código mede a velocidade de rotação (RPM) de um objeto
 * usando um sensor óptico e interrupção do Arduino.
 */

// --- CONFIGURAÇÕES ---

// Pino onde a saída DIGITAL (DO) do TCRT5000 está conectada.
// Deve ser um pino de interrupção (no Arduino Uno/Nano, são os pinos 2 e 3).
const int pinoSensor = 2; 

// Quantidade de furos, ranhuras ou marcas no seu disco giratório.
// Se seu disco tem apenas UM furo, cada pulso é uma volta completa.
// Se for uma hélice com 3 pás bloqueando o sensor, use 3.
const int pulsosPorVolta = 2;

// --- VARIÁVEIS GLOBAIS ---

// Variável para armazenar a contagem de pulsos.
// 'volatile' é essencial, pois esta variável é alterada por uma interrupção.
volatile unsigned long contadorPulsos = 0;

// Variável para o cálculo de tempo.
unsigned long tempoAnterior = 0;

// Intervalo de tempo para calcular e exibir o RPM (em milissegundos).
const unsigned int intervaloMedicao = 1000; // 1 segundo

void setup() {
  // Inicia a comunicação com o computador para exibir os resultados.
  Serial.begin(115200);
  Serial.println("Iniciando medidor de RPM...");

  // Configura o pino do sensor como entrada.
  pinMode(pinoSensor, INPUT);

  // Anexa a interrupção ao pino do sensor.
  // A função 'pulsoDetectado' será chamada toda vez que o pino for de HIGH para LOW (FALLING).
  // FALLING ocorre quando o sensor, que via a luz, é bloqueado.
  attachInterrupt(digitalPinToInterrupt(pinoSensor), pulsoDetectado, FALLING);
}

void loop() {
  // Verifica se já passou o tempo definido para o intervalo de medição.
  if (millis() - tempoAnterior >= intervaloMedicao) {
    
    // Para a contagem de pulsos temporariamente para fazer o cálculo com segurança.
    noInterrupts();
    
    // --- CÁLCULO DO RPM ---
    // A fórmula converte os pulsos contados no intervalo de tempo para rotações por minuto.
    // (contadorPulsos / pulsosPorVolta) -> Número de rotações.
    // (60000.0 / intervaloMedicao) -> Fator de conversão para minutos.
    float rpm = ((60000.0 / intervaloMedicao) * contadorPulsos) / pulsosPorVolta;
    
    // Exibe o resultado no Monitor Serial.
    Serial.print("Pulsos no ultimo segundo: ");
    Serial.print(contadorPulsos);
    Serial.print("  |  RPM: ");
    Serial.println(rpm);
    
    // Zera o contador para a próxima medição.
    contadorPulsos = 0;
    
    // Atualiza o tempo da última medição.
    tempoAnterior = millis();
    
    // Reativa as interrupções para começar uma nova contagem.
    interrupts();
  }
}

// --- FUNÇÃO DE INTERRUPÇÃO (ISR) ---
// Esta função é executada AUTOMATICAMENTE a cada pulso do sensor.
// Mantenha-a o mais curta e rápida possível!
void pulsoDetectado() {
  contadorPulsos++;
}
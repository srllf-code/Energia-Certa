#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int pinoSensor = A0; 
const int pinoRele = 10;   
const int ledVerde = 8;
const int ledAmarelo = 9;
const int ledVermelho = 11;

const int RELE_LIGADO = LOW;
const int RELE_DESLIGADO = HIGH;

// === CONFIGURAÇÕES DE CALIBRAÇÃO ===
const float SENSIBILIDADE = 0.185; 
const int VCC_ARDUINO = 5000;       

// Valor bruto corrigido conforme os testes 
const float ZERO_DO_SENSOR = 202.2; 

// Tensão da rede para o cálculo de kW 
const float TENSAO_REDE = 127.0; 

float limiteCorrente = 3.0; // Limite padrão (alterável pelo Python)
int contadorSobrecarga = 0; 
int statusDisjuntor = 0; 
unsigned long tempoUltimoPrint = 0;

void setup() {
  pinMode(pinoRele, OUTPUT);
  pinMode(ledVerde, OUTPUT);
  pinMode(ledAmarelo, OUTPUT);
  pinMode(ledVermelho, OUTPUT);
  
  digitalWrite(ledVerde, LOW);
  digitalWrite(ledAmarelo, LOW);
  digitalWrite(ledVermelho, LOW);
  digitalWrite(pinoRele, RELE_LIGADO); 
  
  Serial.begin(9600); 
  lcd.init();
  lcd.backlight();
  
  // Animação de inicialização do sistema
  digitalWrite(ledAmarelo, HIGH);
  lcd.setCursor(0, 0);
  lcd.print(" DISJUNTOR ELETR");
  lcd.setCursor(0, 1);
  lcd.print("LIMITE: ");
  lcd.print(limiteCorrente, 2);
  lcd.print("A  ");
  
  delay(2000); 
  digitalWrite(ledAmarelo, LOW); 
  lcd.clear();
}

void loop() {
  // 1. LEITURA MÉDIA DO SENSOR (Filtro de ruído)
  unsigned int somaLeituras = 0;
  const int numeroAmostras = 200; 

  for (int i = 0; i < numeroAmostras; i++) {
    somaLeituras += analogRead(pinoSensor);
    delay(1);
  }
  float mediaLeitura = (float)somaLeituras / numeroAmostras;

  // 2. CÁLCULO DA CORRENTE EM AMPÈRES
  float diferencaLeitura = mediaLeitura - ZERO_DO_SENSOR;
  float voltagemDelta = (diferencaLeitura * VCC_ARDUINO) / 1023.0;
  float correnteAmperes = abs(voltagemDelta / (SENSIBILIDADE * 1000.0));

  // Pequena trava de segurança para zerar variações mínimas em repouso
  if (correnteAmperes < 0.26) {
    correnteAmperes = 0.0;
  }

  // 3. CÁLCULO DA POTÊNCIA EM kW
  float potenciaKW = (correnteAmperes * TENSAO_REDE) / 1000.0;

  // 4. PRINT DE MONITORAMENTO DE CALIBRAÇÃO (Visível na Arduino IDE)
  if (millis() - tempoUltimoPrint > 5000) {
    tempoUltimoPrint = millis();
    Serial.print("[BANCADA] Bruto: ");
    Serial.print(mediaLeitura);
    Serial.print(" | Corrente: ");
    Serial.print(correnteAmperes);
    Serial.println(" A");
  }

  // 5. ATUALIZAÇÃO DO DISPLAY LCD FÍSICO
  if (statusDisjuntor == 0) {
    digitalWrite(ledVerde, HIGH);
    lcd.setCursor(0, 0);
    lcd.print("I:");
    lcd.print(correnteAmperes, 2);
    lcd.print("A Lim:");
    lcd.print(limiteCorrente, 2);
    lcd.print("A   "); 
    lcd.setCursor(0, 1);
    lcd.print("STATUS: NORMAL  ");
  }

  // 6. ENVIO DE DADOS EM kW PARA O SEU DASHBOARD PYTHON
  // Envia no formato exato esperado: POTENCIA_KW,STATUS_DISJUNTOR
  Serial.print(potenciaKW, 3); // Envia com 3 casas decimais para manter a precisão do gráfico
  Serial.print(",");
  Serial.println(statusDisjuntor);

  // 7. LÓGICA DE PROTEÇÃO DO DISJUNTOR
  if (correnteAmperes > limiteCorrente && statusDisjuntor == 0) {
    contadorSobrecarga++; 
    if (contadorSobrecarga >= 4) { // Se persistir por 4 leituras, desarmar
      digitalWrite(pinoRele, RELE_DESLIGADO); 
      digitalWrite(ledVerde, LOW);
      statusDisjuntor = 1; 
      
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("! SOBRECARGA !");
      lcd.setCursor(0, 1);
      lcd.print("DISJUNTOR CAIU! ");
    }
  } else if (correnteAmperes <= limiteCorrente && statusDisjuntor == 0) {
    contadorSobrecarga = 0; 
  }

  // 8. RECEBE NOVOS LIMITES DE CORRENTE ENVIADOS PELO PYTHON
  if (Serial.available() > 0) {
    float novoLimite = Serial.parseFloat();
    if (novoLimite > 0.05) { 
      limiteCorrente = novoLimite;
    }
  }

  // Pisca o LED vermelho se o disjuntor cair por segurança
  if (statusDisjuntor == 1) {
    digitalWrite(ledVermelho, HIGH);
    delay(150);
    digitalWrite(ledVermelho, LOW);
    delay(150);
  } else {
    delay(100); 
  }
}
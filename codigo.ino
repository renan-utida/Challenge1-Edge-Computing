#include <LiquidCrystal_I2C.h>
#include <RTClib.h>
#include <Wire.h>
#include <EEPROM.h>
#include "DHT.h" 

// Definindo o fuso horário (UTC -3)
#define UTC_OFFSET -3

// Definições de constantes e pinos
#define buzzer_Pin 12
#define pir_Pin 8
#define pot_Pin A0
#define orangeLED_Pin 5 // Defina o pino do LED laranja

#define DHTPIN 10
#define DHTTYPE DHT22

// Instanciando os objetos para os sensores e o display LCD
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);
RTC_DS1307 RTC;

// Definindo os limites de armazenamento de registros na EEPROM
const int maxRecords = 100;
const int recordSize = sizeof(uint32_t) + 2 * sizeof(float);
int startAddress = 0;
int endAddress = maxRecords * recordSize;
int currentAddress = 0;

int lastLoggedMinute = -1;
int displayMode = 0;
unsigned long lastSwitchTime = 0;
unsigned long lastPIRTime = 0;
unsigned long lastPIRTriggerTime = 0;
unsigned long totalElapsedTime = 0;
bool pirFirstTime = true;

// Definindo caracteres personalizados para o display LCD (LOGO, CARRO DE FORMULA E, IMG TEMPERATURA, UMIDADE, SETA PARA CIMA, PARA BAIXO e SORRISO)
byte logo_cima1[8] = { B00000, B01000, B01111, B01011, B01011, B00011, B00011, B00011 };
byte logo_cima2[8] = { B00000, B01011, B11011, B01011, B01101, B00101, B00001, B00001 };
byte logo_cima3[8] = { B00000, B00000, B10000, B11000, B11101, B10101, B10101, B10111 };
byte logo_cima4[8] = { B00000, B00110, B01110, B11110, B11100, B01100, B01100, B01100 };
byte logo_baixo1[8] = { B00011, B00011, B00011, B01011, B01011, B01111, B01000, B00000 };
byte logo_baixo2[8] = { B00001, B00001, B00101, B01101, B01001, B11011, B00110, B00000 };
byte logo_baixo3[8] = { B10111, B10010, B10010, B10000, B10000, B11000, B00000, B00000 };
byte logo_baixo4[8] = { B01100, B01100, B01100, B01100, B01100, B11110, B00011, B00000 };

byte formula_e1[8] = { B00000, B00010, B00100, B01010, B00001, B00001, B00000, B00000 };
byte formula_e2[8] = { B00000, B00000, B00000, B11100, B11111, B11111, B11100, B00000 };
byte formula_e3[8] = { B00000, B11100, B10000, B11100, B11111, B11111, B00000, B00000 };
byte formula_e4[8] = { B00000, B00000, B00000, B01110, B11111, B11111, B01110, B00000 };
byte formula_e5[8] = { B00000, B00000, B00000, B00000, B11000, B11110, B00000, B00000 };

byte temp[8] = { B00000, B00100, B01110, B01110, B01110, B11111, B11011, B01110 };
byte umid[8] = { B00000, B00100, B01110, B11111, B11111, B11101, B01110, B00000 };
byte alta[8] = { B00000, B00100, B01110, B10101, B00100, B00100, B00100, B00000 };
byte media[8] = { B00000, B00000, B01010, B00000, B10001, B01110, B00000, B00000 };
byte baixa[8] = { B00000, B00100, B00100, B00100, B10101, B01110, B00100, B00000 };


// Função para mostrar o logotipo inicial no display LCD
void aparecerLogo() {
  lcd.createChar(1, logo_cima1);
  lcd.setCursor(6, 0);
  lcd.write(byte(1));
  lcd.createChar(2, logo_cima2);
  lcd.setCursor(7, 0);
  lcd.write(byte(2));
  lcd.createChar(3, logo_cima3);
  lcd.setCursor(8, 0);
  lcd.write(byte(3));
  lcd.createChar(4, logo_cima4);
  lcd.setCursor(9, 0);
  lcd.write(byte(4));
  lcd.createChar(5, logo_baixo1);
  lcd.setCursor(6, 1);
  lcd.write(byte(5));
  lcd.createChar(6, logo_baixo2);
  lcd.setCursor(7, 1);
  lcd.write(byte(6));
  lcd.createChar(7, logo_baixo3);
  lcd.setCursor(8, 1);
  lcd.write(byte(7));
  lcd.createChar(0, logo_baixo4);
  lcd.setCursor(9, 1);
  lcd.write(byte(0)); 
  delay(4000); // Espera 4 segundos
  lcd.clear();
}

// Função para mostrar uma mensagem de aguarde com animação
void aguarde() {
  lcd.createChar(1, formula_e1);
  lcd.createChar(2, formula_e2);
  lcd.createChar(3, formula_e3);
  lcd.createChar(4, formula_e4);
  lcd.createChar(5, formula_e5);
  for (int i = -5; i < 19; i++) {
    lcd.clear();
    lcd.setCursor(4, 0);
    lcd.print("Aguarde");
    if (i < 18) {
      lcd.setCursor(i, 1);
      lcd.write(byte(1));
      lcd.setCursor(i + 1, 1);
      lcd.write(byte(2));
      lcd.setCursor(i + 2, 1);
      lcd.write(byte(3));
      lcd.setCursor(i + 3, 1);
      lcd.write(byte(4));
      lcd.setCursor(i + 4, 1);
      lcd.write(byte(5));
    }
    delay(250); // Espera 250 milissegundos entre cada passo da animação
  }
  delay(2000); // Espera 2 segundos
  lcd.clear();
}

// Função de introdução com animação e som
void introducao() {
  int musica[] = {100, 200, 150, 250, 300};
  for (int i = 0; i < 7; i++) {
    tone(buzzer_Pin, musica[i]);
    delay(200); // Espera 200 milissegundos entre cada nota
    noTone(buzzer_Pin);
  }
  aparecerLogo();
  for (int i = 0; i < 10; i++) {
    lcd.setCursor(3 + i, 0);
    lcd.print("Innovation"[i]);
    delay(200); // Espera 200 milissegundos entre cada caractere
  }
  for (int i = 0; i < 7; i++) {
    lcd.setCursor(4 + i, 1);
    lcd.print("Masters"[i]);
    delay(200); // Espera 200 milissegundos entre cada caractere
  }
  delay(1000); // Espera 1 segundo
  lcd.clear();
  aguarde();
}

// Configuração inicial do sistema
void setup() {
  lcd.begin(16, 2);
  lcd.backlight();
  dht.begin();
  RTC.begin();
  pinMode(buzzer_Pin, OUTPUT);
  pinMode(orangeLED_Pin, OUTPUT); // Inicialize o pino do LED laranja
  pinMode(pir_Pin, INPUT);
  Serial.begin(9600);
  EEPROM.begin();
  introducao(); // Executa a introdução ao iniciar
  lcd.createChar(1, temp);
  lcd.createChar(2, umid);
  lcd.createChar(3, alta);
  lcd.createChar(4, baixa);
  lcd.createChar(5, media);
}

// Loop principal do sistema
void loop() {
  unsigned long currentMillis = millis();
  float tempValue = dht.readTemperature();
  float humidityValue = dht.readHumidity();
  DateTime now = RTC.now();
  int offsetSeconds = UTC_OFFSET * 3600;
  now = now.unixtime() + offsetSeconds;
  now = now + 3 * 3600;
  DateTime adjustedTime = DateTime(now);

  // Alterna o modo de exibição a cada 10 segundos
  if (currentMillis - lastSwitchTime >= 10000) {
    lastSwitchTime = currentMillis;
    displayMode = (displayMode + 1) % 2;
    lcd.clear();
  }

  // Leitura do sensor PIR e cálculo do tempo decorrido
  if (digitalRead(pir_Pin) == HIGH && (currentMillis - lastPIRTime > 5000)) {
    lastPIRTime = currentMillis;

    unsigned long diffTime;
    unsigned long hours;
    unsigned long minutes;
    unsigned long seconds;
    float velocidade;

    if (pirFirstTime) {
      diffTime = 0;
      hours = 0;
      minutes = 0;
      seconds = 0;
      velocidade = 0.0;
      pirFirstTime = false; // Define como falso após a primeira vez
    } else {
      diffTime = (lastPIRTime - lastPIRTriggerTime) / 1000;
      hours = diffTime / 3600;
      minutes = (diffTime % 3600) / 60;
      seconds = diffTime % 60;
      // Ler o valor do potenciômetro e converter para velocidade em km/h
      float potValue = analogRead(pot_Pin);
      velocidade = map(potValue, 0, 1023, 30, 280); // Mapeia de 30 a 280 km/h
    }

    // Atualizar o último tempo de acionamento do PIR
    lastPIRTriggerTime = lastPIRTime;

    // Atualizar o tempo total registrado
    totalElapsedTime += diffTime;

    // Calcular tempo total em horas, minutos e segundos
    unsigned long totalHours = totalElapsedTime / 3600;
    unsigned long totalMinutes = (totalElapsedTime % 3600) / 60;
    unsigned long totalSeconds = totalElapsedTime % 60;

    // Exibir dados no monitor serial
    Serial.println();
    Serial.print("Gen2 Car (Carro de Formula E) passou pela linha de chegada: ");
    Serial.println();
    Serial.print(adjustedTime.day(), DEC);
    Serial.print('/');
    Serial.print(adjustedTime.month(), DEC);
    Serial.print('/');
    Serial.print(adjustedTime.year(), DEC);
    Serial.print(" ");
    Serial.print(adjustedTime.hour() < 10 ? "0" : ""); // Adiciona zero à esquerda se hora for menor que 10
    Serial.print(adjustedTime.hour());
    Serial.print(':');
    if (adjustedTime.minute() < 10) {
      Serial.print('0'); // Adiciona zero à esquerda se minuto for menor que 10
    }
    Serial.print(adjustedTime.minute(), DEC);
    Serial.print(':');
    if (adjustedTime.second() < 10) {
      Serial.print('0'); // Adiciona zero à esquerda se segundo for menor que 10
    }
    Serial.println(adjustedTime.second(), DEC);
    Serial.print("Volta: ");
    Serial.print(hours, DEC);
    Serial.print(":");
    if (minutes < 10) {
      Serial.print('0'); // Adiciona zero à esquerda se minuto for menor que 10
    }
    Serial.print(minutes, DEC);
    Serial.print(":");
    if (seconds < 10) {
      Serial.print('0'); // Adiciona zero à esquerda se segundo for menor que 10
    }
    Serial.print(seconds, DEC);
    Serial.print("        Velocidade: ");
    Serial.print(velocidade);
    Serial.println(" km/h");

    // Adicionar linha para o tempo total registrado
    Serial.print("Tempo total registrado: ");
    Serial.print(totalHours, DEC);
    Serial.print(":");
    if (totalMinutes < 10) {
        Serial.print('0'); // Adiciona zero à esquerda se minuto for menor que 10
    }
    Serial.print(totalMinutes, DEC);
    Serial.print(":");
    if (totalSeconds < 10) {
        Serial.print('0'); // Adiciona zero à esquerda se segundo for menor que 10
    }
    Serial.println(totalSeconds, DEC);

    Serial.print("Temperatura: ");
    Serial.print(tempValue, 1); // 1 casa decimal
    Serial.print(" C");
    Serial.print("      Umidade: ");
    Serial.print(humidityValue, 1); // 1 casa decimal
    Serial.println(" %");
    Serial.println();

    // Ativar buzzer e LED
    tone(buzzer_Pin, 1000);
    digitalWrite(LED_BUILTIN, HIGH);
    digitalWrite(orangeLED_Pin, HIGH); // Acender o LED laranja
    delay(1000); // Manter o buzzer e LED ativos por 1 segundo
    noTone(buzzer_Pin);
    digitalWrite(LED_BUILTIN, LOW);
    digitalWrite(orangeLED_Pin, LOW); // Apagar o LED laranja

    // Mostrar os dados do PIR no LCD
    showPIRData(diffTime, totalElapsedTime);
  }

  // Atualizar o display LCD
  updateDisplay(adjustedTime, tempValue, humidityValue);

  // Registrar dados na EEPROM a cada minuto
  if (now.minute() != lastLoggedMinute) {
    lastLoggedMinute = now.minute();
    logData(tempValue, humidityValue);
  }
}

// Função para atualizar o display LCD
void updateDisplay(DateTime adjustedTime, float temp, float humidity) {
  char tempStr[6];
  char humidityStr[6];
  dtostrf(temp, 4, 1, tempStr); // Formata a temperatura com 1 casa decimal
  dtostrf(humidity, 4, 1, humidityStr); // Formata a umidade com 1 casa decimal

  if (displayMode == 1) {
    lcd.setCursor(0, 0);
    lcd.print("DATA: ");
    lcd.print(adjustedTime.day() < 10 ? "0" : ""); // Adiciona zero à esquerda se dia for menor que 10
    lcd.print(adjustedTime.day());
    lcd.print("/");
    lcd.print(adjustedTime.month() < 10 ? "0" : ""); // Adiciona zero à esquerda se mês for menor que 10
    lcd.print(adjustedTime.month());
    lcd.print("/");
    lcd.print(adjustedTime.year());

    lcd.setCursor(0, 1);
    lcd.print("HORA: ");
    lcd.print(adjustedTime.hour() < 10 ? "0" : ""); // Adiciona zero à esquerda se hora for menor que 10
    lcd.print(adjustedTime.hour());
    lcd.print(":");
    lcd.print(adjustedTime.minute() < 10 ? "0" : ""); // Adiciona zero à esquerda se minuto for menor que 10
    lcd.print(adjustedTime.minute());
    lcd.print(":");
    lcd.print(adjustedTime.second() < 10 ? "0" : ""); // Adiciona zero à esquerda se segundo for menor que 10
    lcd.print(adjustedTime.second());
  } else {
    // Aparece a umidade e Temperatura
    lcd.setCursor(0, 0);
    lcd.print("Temp: ");
    lcd.print(tempStr);
    lcd.print(" C");
    
    lcd.setCursor(0, 1);
    lcd.print("Umid: ");
    lcd.print(humidityStr);
    lcd.print(" %");

    checkAndUpdateLogos(temp, humidity);  // Atualiza os logos
  }
}

void checkAndUpdateLogos(float temp, float humidity) {
  lcd.setCursor(14, 0); // Posição para os logos
  if (temp > 30.0) {
    // Temperatura alta
    lcd.write(byte(3));
    lcd.write(byte(1));
  } else if (temp >= 20 && temp <= 30) {
    // Temperatura média
    lcd.write(byte(5));
    lcd.write(byte(1));
  } else {
    // Temperatura baixa
    lcd.write(byte(4));
    lcd.write(byte(1));
  }

  lcd.setCursor(14, 1); // Posição para os logos
  if (humidity > 70) {
    // Umidade alta
    lcd.write(byte(3));
    lcd.write(byte(2));
  } else if (humidity >= 40 && humidity <= 70) {
    // Umidade Media
    lcd.write(byte(5));
    lcd.write(byte(2));
  } else {
    // Umidade Baixa
    lcd.write(byte(4));
    lcd.write(byte(2));
  }
}

void showPIRData(unsigned long diffTime, unsigned long totalElapsedTime) {
  // Limpar o display LCD
  lcd.clear();
  
  // Calcular o tempo da última volta em horas, minutos e segundos
  unsigned long hours = diffTime / 3600;
  unsigned long minutes = (diffTime % 3600) / 60;
  unsigned long seconds = diffTime % 60;
  
  // Calcular o tempo total registrado em horas, minutos e segundos
  unsigned long totalHours = totalElapsedTime / 3600;
  unsigned long totalMinutes = (totalElapsedTime % 3600) / 60;
  unsigned long totalSeconds = totalElapsedTime % 60;
  
  // Formatar e exibir o tempo da última volta
  lcd.setCursor(0, 0);
  lcd.print("Volta = ");
  if (hours < 10) lcd.print('0');
  lcd.print(hours);
  lcd.print(':');
  if (minutes < 10) lcd.print('0');
  lcd.print(minutes);
  lcd.print(':');
  if (seconds < 10) lcd.print('0');
  lcd.print(seconds);
  
  // Formatar e exibir o tempo total registrado
  lcd.setCursor(0, 1);
  lcd.print("Total = ");
  if (totalHours < 10) lcd.print('0');
  lcd.print(totalHours);
  lcd.print(':');
  if (totalMinutes < 10) lcd.print('0');
  lcd.print(totalMinutes);
  lcd.print(':');
  if (totalSeconds < 10) lcd.print('0');
  lcd.print(totalSeconds);

  // Manter a exibição por 3 segundos
  delay(3000);

  // Limpar o display novamente
  lcd.clear();
}

// Função para registrar dados na EEPROM
void logData(float tempValue, float humidityValue) {
  if (currentAddress + recordSize > endAddress) {
    currentAddress = startAddress;
  }
  DateTime now = RTC.now();
  EEPROM.put(currentAddress, now.unixtime());
  currentAddress += sizeof(uint32_t);
  EEPROM.put(currentAddress, tempValue);
  currentAddress += sizeof(float);
  EEPROM.put(currentAddress, humidityValue);
  currentAddress += sizeof(float);
}


# Challenge 1 - Edge Computing
Challenge 1 - 1ESPG - Edge Computing


**Link Youtube:** https://youtu.be/rOfhde_NBV4?si=qbFftL_nAMX3Fzee


**Link do Wokwi:** https://wokwi.com/projects/400585744404940801

**Integrantes:**

Renan Dias Utida - RM 558540

Gustavo Melanda da Nova - RM 556081

Murilo Justi - RM 554512

Fabricio Muniz - RM 555017

# Sensores Inteligentes para Pistas de Corrida da Formula E

**Descrição do Projeto**
Este projeto utiliza um microcontrolador para monitorar e registrar condições ambientais e detectar a passagem de um veículo em uma pista de corrida. Utilizando sensores de temperatura e umidade, um sensor PIR para detectar movimento, e um display LCD para exibir as informações, o sistema proporciona uma solução integrada para monitoramento em tempo real e registro de dados ambientais e de eventos de corrida.


# Funcionalidades
**Monitoramento Ambiental:**
Temperatura: Medição contínua da temperatura ambiente.
Umidade: Medição contínua da umidade relativa do ar.
Exibição: Dados exibidos no display LCD com indicadores de níveis.

**Detecção de Movimento:**
Sensor PIR: Detecta a passagem do veículo pela linha de chegada.
Cálculo de Velocidade: Estimativa da velocidade do veículo usando um potenciômetro.

**Registro de Dados:**
EEPROM: Armazena dados de temperatura e umidade a cada minuto.
Exibição Serial: Mostra os tempos de volta e o tempo total registrado.

**Alarme e Indicadores:**
Buzzer: Emite um som quando o veículo é detectado.
LED Laranja: Acende quando o veículo é detectado.

**Animação de Introdução:**
LCD: Exibe uma animação de introdução com sons de boas-vindas.


# Componentes Utilizados
**Microcontrolador:** Placa compatível com Arduino.
**Display LCD:** LiquidCrystal I2C 16x2.
**RTC (Relógio de Tempo Real):** RTC DS1307.
**Sensor de Temperatura e Umidade:** DHT22.
**Sensor PIR:** Para detecção de movimento.
**Buzzer:** Para sinalização sonora.
**LED Laranja:** Indicador visual.
**EEPROM:** Para armazenamento persistente dos dados.
**Potenciômetro:** Para ajustar a velocidade do veículo.


# Requisitos e Dependências
**Hardware**
Placa compatível com Arduino (ex: Arduino Uno)
Display LCD 16x2 com interface I2C
RTC DS1307
Sensor DHT22
Sensor PIR
Buzzer
LED Laranja
EEPROM
Potenciômetro
Jumpers e protoboard para conexões

**Software**
Arduino IDE
Bibliotecas Arduino:
  LiquidCrystal_I2C
  RTClib
  DHT
  Wire
  EEPROM


# Funcionamento do Sistema
**Inicialização:** Ao ligar, o sistema exibirá uma animação de introdução no display LCD e emitirá um som de boas-vindas.
**Monitoramento Contínuo:** O sistema monitorará continuamente a temperatura e a umidade, exibindo os valores no display LCD.
**Detecção de Veículo:** Quando o sensor PIR detectar movimento, o sistema calculará a velocidade do veículo e exibirá os tempos de volta e totais.
**Alarme e Indicadores:** O buzzer emitirá um som e o LED laranja acenderá brevemente para indicar a detecção do veículo.
**Registro de Dados:** A cada minuto, os dados de temperatura e umidade serão registrados na EEPROM.


# Licença
Este projeto está licenciado sob a licença MIT - consulte o arquivo LICENSE.md para obter mais detalhes.

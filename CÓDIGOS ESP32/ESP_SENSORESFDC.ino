/*
      ESP_SensoresFdC
      PROJETO:  Desenvolvimento de um sistema, utilizando protocolo MQTT, com Raspberry Pi 4 e ESP32 
                para controlar bancada eletropneumática/eletrohidráulica através de um aplicativo mobile.
      LINK:     https://github.com/MAPL-UFU/mqtt-app-controle
      ALUNO:    Matheus Alves de Paula
      PROF.:    Prof. Dr. José Jean-Paul Zanlucchi de Souza Tavares
      DATA:     05/01/2024
*/

// INCLUSÃO DE BIBLIOTECAS
#include <WiFi.h>          // biblioteca WiFi
#include "esp_wpa2.h"      // biblioteca wpa2 para conexão em redes Enterprise
#include <PubSubClient.h>  // biblioteca PubSubClient para conexão MQTT

byte mac[6];

// PARÂMETROS PARA CONEXÃO WIFI COM A REDE EDUROAM
#define EAP_ANONYMOUS_IDENTITY "anonymous@ufu.br"  // anonymous@example.com, or you can use also nickname@example.com
#define EAP_IDENTITY " XXXXXX @ufu.br"         // nickname@example.com, at some organizations should work nickname only without realm, but it is not recommended
#define EAP_PASSWORD " XXXXXX "                 // password for eduroam account

const char* ssid = "eduroam";  // "eduroam" SSID

// PARÂMETROS DO BROKER PARA CONEXÃO MQTT
const char* mqttServer = "10.32.80.12";           // endereço do broker a ser conectado
const int mqttPort = 1883;                        // porta mqtt
const char* mqttUser = "localufulem";            // usuário mqtt do referido broker
const char* mqttPassword = "r4spb3rryp!@L3M";    // senha mqtt do referido broker

// INSTANCIANDO OBJETOS DAS CLASSES DAS BIBLIOTECAS
WiFiClient espClient;          // definindo objeto espClient
PubSubClient MQTT(espClient);  // cliente MQTT passando o objeto espClient

// INSTANCIANDO OBJETOS TASKHANDLE PARA PROCESSAMENTO DUAL CORE
TaskHandle_t Task1;
TaskHandle_t Task2;

// DEFINIÇÃO DO CLIENT ID
#define id_mqtt "ESP-SENSORES"  // definindo identificador para o esp32

// DEFINIÇÃO DE PINOS
#define A0 2  // definindo a ligação do sensor fim de curso na porta 2
#define A1 4  // definindo a ligação do sensor fim de curso na porta 4

#define B0 16  // definindo a ligação do sensor fim de curso na porta 5
#define B1 17  // definindo a ligação do sensor fim de curso na porta 19

#define C0 18  // definindo a ligação do sensor fim de curso na porta 12
#define C1 19  // definindo a ligação do sensor fim de curso na porta 14

// DECLARAÇÃO DOS PROTÓTIPOS DAS FUNÇÕES
void conectaWiFi();                                     // realiza conexão com wifi
void conectaMQTT();                                     // realiza conexão com broker mqtt
void mantemConexoes();                                  // garantir as conexoes wifi e broker mqtt ativas
void publishSensorA0A1State(int a0Value, int a1Value);  // realiza publicações nos tópicos referente aos sensores do atuador A
void publishSensorB0B1State(int b0Value, int b1Value);  // realiza publicações nos tópicos referente aos sensores do atuador B
void publishSensorC0C1State(int c0Value, int c1Value);  // realiza publicações nos tópicos referente aos sensores do atuador C

void setup() {

  Serial.begin(115200);  // inicia a comunicação serial em 115200 bauds
  delay(10);
  Serial.print(F("Conectando à rede: "));
  Serial.println(ssid);
  Serial.println("\n");
  WiFi.disconnect(true);                                                                 // disconnect from WiFi to set new WiFi connection
  WiFi.mode(WIFI_STA);                                                                   // init wifi mode
  WiFi.begin(ssid, WPA2_AUTH_PEAP, EAP_ANONYMOUS_IDENTITY, EAP_IDENTITY, EAP_PASSWORD);  // WITHOUT CERTIFICATE - WORKING WITH EXCEPTION ON RADIUS SERVER

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
    if (WiFi.status() == WL_CONNECT_FAILED || WiFi.status() == WL_CONNECTION_LOST) {
      conectaWiFi();  // Reconecta Wi-Fi em caso de falha
    }
  }

  Serial.println("\n");
  Serial.println(F("Rede WiFi está conectada!"));
  Serial.println(F("Endereço IP recebido: "));
  Serial.println(WiFi.localIP());  // print LAN IP

  WiFi.macAddress(mac);
  Serial.print("MAC address: ");
  Serial.print(mac[0], HEX);
  Serial.print(":");
  Serial.print(mac[1], HEX);
  Serial.print(":");
  Serial.print(mac[2], HEX);
  Serial.print(":");
  Serial.print(mac[3], HEX);
  Serial.print(":");
  Serial.print(mac[4], HEX);
  Serial.print(":");
  Serial.println(mac[5], HEX);  // print MAC Address

  // DECLARANDO PINOS DE INPUT
  pinMode(A0, INPUT_PULLUP);  // inicializando a variável do sensor A0 como entrada com resistor pull-up interno habilitado
  pinMode(A1, INPUT_PULLUP);  // inicializando a variável do sensor A1 como entrada com resistor pull-up interno habilitado

  pinMode(B0, INPUT_PULLUP);  // inicializando a variável do sensor B0 como entrada com resistor pull-up interno habilitado
  pinMode(B1, INPUT_PULLUP);  // inicializando a variável do sensor B1 como entrada com resistor pull-up interno habilitado

  pinMode(C0, INPUT_PULLUP);  // inicializando a variável do sensor C0 como entrada com resistor pull-up interno habilitado
  pinMode(C1, INPUT_PULLUP);  // inicializando a variável do sensor C1 como entrada com resistor pull-up interno habilitado

  MQTT.setServer(mqttServer, mqttPort);
  conectaMQTT();

    // cria uma tarefa que será executada na função Task1code(), com prioridade 1 e execução no core 0
  xTaskCreatePinnedToCore(
    Task1code, /* função que implementa a tarefa. */
    "Task1",   /* nome da tarefa. */
    10000,     /* número de palavras a serem alocadas para uso com a pilha da tarefa */
    NULL,      /* parâmetro de entrada para a tarefa (pode ser NULL) */
    0,         /* prioridade da tarefa (0 a N) */
    &Task1,    /* referência para a tarefa (pode ser NULL) */
    0);        /* núcleo que executará a tarefa (0 ou 1) */
  delay(500);

  // cria uma tarefa que será executada na função Task2code(), com prioridade 1 e execução no core 1
  xTaskCreatePinnedToCore(
    Task2code, /* função que implementa a tarefa. */
    "Task2",   /* nome da tarefa. */
    10000,     /* número de palavras a serem alocadas para uso com a pilha da tarefa */
    NULL,      /* parâmetro de entrada para a tarefa (pode ser NULL) */
    1,         /* prioridade da tarefa (0 a N) */
    &Task2,    /* referência para a tarefa (pode ser NULL) */
    1);        /* núcleo que executará a tarefa (0 ou 1) */
  delay(500);
}

// FUNÇÃO EXECUTADA NO NÚCLEO 0
void Task1code(void* pvParameters) {
  Serial.print("Publish status running on core ");
  Serial.println(xPortGetCoreID());

  for (;;) {
    const char* msg_status = "ESP32 SENSORES CONECTADO";
    MQTT.publish("/status/espSensores", msg_status);  // publica o status do ESP32
    //Serial.print("Mensagem publicada no tópico: /status/espSensores");
    //Serial.print(" Mensagem: ");
    //Serial.println(msg_status);
    delay(3000);
  }
}

// FUNÇÃO EXECUTADA NO NÚCLEO 1
void Task2code(void* pvParameters) {
  Serial.print("Conexões (Wi-Fi e MQTT) running on core ");
  Serial.println(xPortGetCoreID());

  for (;;) {
    mantemConexoes();  // função para reconectar wifi ou broker em caso de queda
    MQTT.loop();       // mantém a comunicação MQTT ativa

    // variavel auxilar para tratar dados do sensores
    int a0Value = digitalRead(A0);  // Lê o estado do sensor fim de curso a0
    int a1Value = digitalRead(A1);  // Lê o estado do sensor fim de curso a1

    int b0Value = digitalRead(B0);  // Lê o estado do sensor fim de curso b0
    int b1Value = digitalRead(B1);  // Lê o estado do sensor fim de curso b1

    int c0Value = digitalRead(C0);  // Lê o estado do sensor fim de curso c0
    int c1Value = digitalRead(C1);  // Lê o estado do sensor fim de curso c1

    publishSensorA0A1State(a0Value, a1Value);  // Publica o estado do sensor
    publishSensorB0B1State(b0Value, b1Value);  // Publica o estado do sensor
    publishSensorC0C1State(c0Value, c1Value);  // Publica o estado do sensor

    delay(1000);  // intervalo de 1 segundo entre as leituras
  }
}

void loop() {
}

// FUNÇÃO DE RECONEXÃO MQTT E WIFI
void mantemConexoes() {
  if (!MQTT.connected()) {
    conectaMQTT();
  }

  conectaWiFi();
}

// FUNÇÃO CONEXÃO REDE WIFI
void conectaWiFi() {

  if (WiFi.status() == WL_CONNECTED) {
    return;
  }

  Serial.println("Falha na conexão com a rede WiFi.");
  Serial.print("Reconectando-se na rede: ");
  Serial.print(ssid);

  WiFi.disconnect(true);  //disconnect from WiFi to set new WiFi connection
  WiFi.mode(WIFI_STA);    //init wifi mode
  WiFi.begin(ssid, WPA2_AUTH_PEAP, EAP_ANONYMOUS_IDENTITY, EAP_IDENTITY, EAP_PASSWORD);

  int counter = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    counter++;

    if (counter >= 60) {
      ESP.restart();  // Reinicia o ESP32 após 30 segundos de tentativas de conexão
    }

    Serial.println();
    Serial.print("Conectado com sucesso na rede: ");
    Serial.println(ssid);
  }
}

// FUNÇÃO CONEXÃO MQTT
void conectaMQTT() {
  while (!MQTT.connected()) {
    Serial.print("Conectando ao Broker MQTT: ");
    Serial.println(mqttServer);

    if (MQTT.connect(id_mqtt, mqttUser, mqttPassword)) {
      Serial.println("Conexao realizada com sucesso!");
    } else {
      Serial.println("Falha na conexão com o broker MQTT.");
      Serial.println("Nova tentativa de conexão em 5s...");
      delay(5000);
    }
  }
}

// FUNÇÃO SENSORES ATUADOR A
void publishSensorA0A1State(int a0Value, int a1Value) {
  if (a0Value == LOW && a1Value == HIGH) {
    MQTT.publish("/estado/atuadorA", "Atuador A Recuado");
    Serial.println("Mensagem publicada no tópico: /estado/atuadorA Mensagem: Atuador A Recuado");

  } else if (a0Value == HIGH && a1Value == LOW) {
    MQTT.publish("/estado/atuadorA", "Atuador A Avançado");
    Serial.println("Mensagem publicada no tópico: /estado/atuadorA Mensagem: Atuador A Avançado");

  } else if (a0Value == LOW && a1Value == LOW) {
    MQTT.publish("/estado/atuadorA", "Sensor(es) do Atuador A com mau contato!");
    Serial.println("Mensagem publicada no tópico: /estado/atuadorA Mensagem: Sensor(es) do Atuador A com mau contato!");

  } else {
    //Serial.println("\n ... \n");
  }
}

// FUNÇÃO SENSORES ATUADOR B
void publishSensorB0B1State(int b0Value, int b1Value) {
  if (b0Value == LOW && b1Value == HIGH) {
    MQTT.publish("/estado/atuadorB", "Atuador B Recuado");
    Serial.println("Mensagem publicada no tópico: /estado/atuadorB Mensagem: Atuador B Recuado");

  } else if (b0Value == HIGH && b1Value == LOW) {
    MQTT.publish("/estado/atuadorB", "Atuador B Avançado");
    Serial.println("Mensagem publicada no tópico: /estado/atuadorB Mensagem: Atuador B Avançado");

  } else if (b0Value == LOW && b1Value == LOW) {
    MQTT.publish("/estado/atuadorB", "Sensor(es) do atuador B com mau contato!");
    Serial.println("Mensagem publicada no tópico: /estado/atuadorB Mensagem: Sensor(es) do Atuador B com mau contato!");

  } else {
    //Serial.println("\n ... \n");
  }
}

// FUNÇÃO SENSORES ATUADOR C
void publishSensorC0C1State(int c0Value, int c1Value) {
  if (c0Value == LOW && c1Value == HIGH) {
    Serial.println("Mensagem publicada no tópico: /estado/atuadorC Mensagem: Atuador C Recuado");
    Serial.println("\n Atuador C Recuado \n");

  } else if (c0Value == HIGH && c1Value == LOW) {
    MQTT.publish("/estado/atuadorC", "Atuador C Avançado");
    Serial.println("Mensagem publicada no tópico: /estado/atuadorC Mensagem: Atuador C Avançado");

  } else if (c0Value == LOW && c1Value == LOW) {
    MQTT.publish("/estado/atuadorC", "Sensor(es) do atuador C com mau contato!");
    Serial.println("Mensagem publicada no tópico: /estado/atuadorC Mensagem: Sensor(es) do Atuador C com mau contato!");

  } else {
    //Serial.println("\n ... \n");
  }
}
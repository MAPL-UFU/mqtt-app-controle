/*
      ESP_ValvB
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
#define id_mqtt "ESP-VALV-B"  // definindo identificador para o esp32

// DEFINIÇÃO DE PINOS
#define valvB 5  // definindo a ligação da válvula na porta 5

// DECLARAÇÃO DOS PROTÓTIPOS DAS FUNÇÕES
void conectaWiFi();                                              // realiza conexão com wifi
void conectaMQTT();                                              // realiza conexão com broker mqtt
void mantemConexoes();                                           // garantir as conexoes wifi e broker mqtt ativas
void callback(char* topic, byte* message, unsigned int length);  // função de retorno de chamada MQTT
void openValve();                                                // função para abrir a válvula
void closeValve();                                               // função para fechar a válvula

void setup() {

  Serial.begin(115200);  // inicia a comunicação serial em 115200 bauds
  delay(10);
  Serial.print(F("Connecting to network: "));
  Serial.println(ssid);
  WiFi.disconnect(true);                                                                 // disconnect from WiFi to set new WiFi connection
  WiFi.mode(WIFI_STA);                                                                   // init wifi mode
  WiFi.begin(ssid, WPA2_AUTH_PEAP, EAP_ANONYMOUS_IDENTITY, EAP_IDENTITY, EAP_PASSWORD);  // WITHOUT CERTIFICATE - WORKING WITH EXCEPTION ON RADIUS SERVER

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
    if (WiFi.status() == WL_CONNECT_FAILED || WiFi.status() == WL_CONNECTION_LOST) {
      conectaWiFi();  // reconecta Wi-Fi em caso de falha
    }
  }

  Serial.println("");
  Serial.println(F("WiFi is connected!"));
  Serial.println(F("IP address set: "));
  Serial.println(WiFi.localIP());  // exibe LAN IP

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
  Serial.println(mac[5], HEX);  // exibe MAC Address

  // DECLARANDO OS PINOS
  pinMode(valvB, OUTPUT);    // define a saída para a válvula B como OUTPUT
  digitalWrite(valvB, LOW);  // estado inicial, a válvula B está fechada (LOW)

  MQTT.setServer(mqttServer, mqttPort);
  MQTT.setCallback(callback);  // configura a função de retorno de chamada MQTT
  conectaMQTT(); // chama a função para conexão com o broker MQTT

    // cria uma tarefa que será executada na função Task1code(), com prioridade 1 e execução no core 0
  xTaskCreatePinnedToCore(
    Task1code, /* função que implementa a tarefa. */
    "Task1",   /* nome da tarefa. */
    10000,     /* número de palavras a serem alocadas para uso com a pilha da tarefa */
    NULL,      /* parâmetro de entrada para a tarefa (pode ser NULL) */
    1,         /* prioridade da tarefa (0 a N) */
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
    const char* msg_status = "ESP32 VALVULA B CONECTADO";
    MQTT.publish("/status/espValvB", msg_status);  // publica o status do ESP32
    //Serial.print("Mensagem publicada no tópico: /status/espValvB");
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
    //delay(10);
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

      MQTT.subscribe("/comando/valvB");              // subscreve para receber comandos

    } else {
      Serial.println("Falha na conexão com o broker MQTT.");
      Serial.println("Nova tentativa de conexão em 5s...");
      delay(5000);
    }
  }
}

// FUNÇÃO DE RETORNO DE CHAMADA MQTT
void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Mensagem recebida no tópico: ");
  Serial.print(topic);
  Serial.print(". Mensagem: ");

  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
  }
  Serial.println();

  int command = message[0] - '0';  // converte o primeiro caractere da mensagem em um valor inteiro
  
  if (command == 1) {
    openValve();  // avança o atuador
  } else if (command == 0) {
    closeValve();  // recua o atuador
  }
}

// FUNÇÃO ABRIR VÁLVULA
void openValve() {
  digitalWrite(valvB, HIGH);  // define o pino da válvula B como HIGH (aberto)
}
// FUNÇÃO FECHAR VÁLVULA
void closeValve() {
  digitalWrite(valvB, LOW);  // define o pino da válvula B como LOW (fechado)
}
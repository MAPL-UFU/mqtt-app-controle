# mqtt-app-controle
Desenvolvimento de um aplicativo mobile para controlar bancada eletropneumática via protocolo MQTT usando Raspberry Pi 4 e ESP32.

## ⚙️ Configuração do Raspberry Pi
A instalação e configuração detalhada do Sistema Operacional do Raspberry, além das configurações de conexão com a rede de internet da UFU e conexão MQTT estão descritas no relatório do projeto.

## ⚙️ Configuração do ESP32
- ### Configurando a IDE do Arduino para ESP32
Utilizou-se a IDE do Arduino para a programação dos microcontroladores ESP32. 
Para isso, configura-se a IDE seguindo o [vídeo tutorial](https://www.youtube.com/watch?v=ROkhP5oWRUU).

- ### Conectando a ESP32 à rede Wi-Fi da UFU e ao broker MQTT
Inicialmente, deve-se incluir as bibliotecas para realizar as conexões.

![Incluindo bibliotecas para conexões Wi-Fi e MQTT](https://github.com/MAPL-UFU/mqtt-app-controle/assets/73907821/f7da857d-bcb8-4a5a-95a0-6faa60595136)

Para a conexão com a rede Wi-Fi, deve-se informar nos campos **EAP_IDENTITY** e **EAP_PASSWORD** o e-mail institucional e senha, respectivamente.

![Conexão com a rede Wi-Fi EDUROAM](https://github.com/MAPL-UFU/mqtt-app-controle/assets/73907821/26c8c209-cff4-4741-9d72-54cc17907964)

Além disso, deve-se especificar esse tipo de conexão na função void setup().

![Modo Wi-Fi](https://github.com/MAPL-UFU/mqtt-app-controle/assets/73907821/e905ebf7-6603-48d4-8cca-37bd694703bb)

Para a conexão com o broker MQTT, deve-se informar o endereço do broker, nome de usuário, senha e porta. As informações apresentadas abaixo são referentes ao broker local instalado no Raspberry Pi que foi configurado e conectado via rede Ethernet no roteador localizado no mesanino do LEM3.

![Parâmetros broker MQTT](https://github.com/MAPL-UFU/mqtt-app-controle/assets/73907821/d0319dd3-dd63-4e95-a009-d52a6f5f0b88)

Além disso, deve-se inicializar a chamada da função callback na função void setup().

## 📁 Acesso aos códigos de configuração do Raspberry Pi
Os códigos de configuração do Raspberry Pi podem ser acessados através [deste diretório](https://github.com/MAPL-UFU/mqtt-app-controle/tree/main/C%C3%93DIGOS%20RASPBERRY).

## 📁 Acesso aos códigos dos microcontroladores
Os códigos para cada microcontrolador podem ser acessados através [deste diretório](https://github.com/MAPL-UFU/mqtt-app-controle/tree/main/C%C3%93DIGOS%20ESP32).

## 📁 Acesso ao projeto do aplicativo
O projeto do aplicativo pode ser acessado através do código fonte neste diretório ou se preferir, pode-se realizar a instalação diretamente no dispositivo através do APK.

### 📲 Abrindo e executando o projeto no Android Studio
Lembrando que para abrir o projeto do aplicativo, deve-se instalar o software Android Studio.
Para não ocorrer erros na execução do projeto, siga os passos apresentados a seguir:
- Vá até o diretório do projeto e clique em ...;
- O arquivo será baixado no formato .zip. Assim, é necessário extraí-lo para conseguir acessar;
- Abra o Android Studio e clique em...;

## ✅ Resultados
Foram realizados testes e ajustes em bancada no Laboratório de Ensino de Mecatrônica.
Os resultados finais estão apresentados em vídeos disponíveis nos links a seguir:

- Demonstração 01 de 07: [TELA DE COMANDO](https://youtu.be/MyiuIrd5wE8);
- Demonstração 02 de 07: [TELA DE DASHBOARD](https://youtu.be/5tTWuPnENv8);
- Demonstração 03 de 07: [TELA DE CONTROLE DE DOIS ATUADORES (PRIMEIRA SEQUÊNCIA)](https://youtu.be/RBTWL6hKgfo);
- Demonstração 04 de 07: [TELA DE CONTROLE DE DOIS ATUADORES (SEGUNDA SEQUÊNCIA)](https://youtu.be/ZZiYtdUzc9o);
- Demonstração 05 de 07: [TELA DE CONTROLE DE DOIS ATUADORES (TERCEIRA SEQUÊNCIA)](https://youtu.be/LQ93bLfJFuQ);
- Demonstração 06 de 07: [TELA DE CONTROLE DE TRÊS ATUADORES](https://youtu.be/s2FILRrS43g);
- Demonstração 07 de 07: [TELA DE CONTROLE VIA BROKER EM NUVEM](https://youtu.be/ezOWhJ70hYo).

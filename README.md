# mqtt-app-controle
Desenvolvimento de um aplicativo mobile para controlar bancada eletropneumática via protocolo MQTT usando Raspberry Pi 4 e ESP32.

## ⚙️ Configuração do Raspberry Pi
A instalação e configuração detalhada do Sistema Operacional do Raspberry, além das configurações de conexão com a rede de internet da UFU e conexão MQTT estão descritas no relatório do projeto.

## ⚙️ Configuração do ESP32
- ### Configurando a IDE do Arduino para ESP32
Utilizou-se a IDE do Arduino para a programação dos microcontroladores ESP32. 
Para isso, configura-se a IDE seguindo o [vídeo tutorial](https://www.youtube.com/watch?v=ROkhP5oWRUU).

- ### Conectando a ESP32 à rede Wi-Fi da Universidade
Inicialmente, deve-se incluir as bibliotecas para realizar as conexões.

![Incluindo bibliotecas para conexões Wi-Fi e MQTT](https://github.com/MAPL-UFU/mqtt-app-controle/assets/73907821/f7da857d-bcb8-4a5a-95a0-6faa60595136)

Para a conexão com a rede Wi-Fi, deve-se informar nos campos **EAP_IDENTITY** e **EAP_PASSWORD** o e-mail institucional e senha, respectivamente.

![Conexão com a rede Wi-Fi EDUROAM](https://github.com/MAPL-UFU/mqtt-app-controle/assets/73907821/26c8c209-cff4-4741-9d72-54cc17907964)

Além disso, deve-se especificar esse tipo de conexão na função void setup().

![Modo Wi-Fi](https://github.com/MAPL-UFU/mqtt-app-controle/assets/73907821/e905ebf7-6603-48d4-8cca-37bd694703bb)

- ### Conectando a ESP32 ao broker MQTT (local e nuvem)

Para a conexão com o broker MQTT, deve-se informar o endereço do broker, nome de usuário, senha e porta. As informações apresentadas a seguir são referentes ao broker local instalado no Raspberry Pi que foi configurado e conectado via rede Ethernet no roteador localizado no mesanino do LEM3.

![Parâmetros broker MQTT local](https://github.com/MAPL-UFU/mqtt-app-controle/assets/73907821/d0319dd3-dd63-4e95-a009-d52a6f5f0b88)

Para a conexão com o broker em nuvem, utilizou-se um serviço gratuito da empresa Sternenbauer. Escolheu-se esse serviço dentre outros brokers gratuitos que foram encontrados, pois este possui um dashboard com informações de dispositivos conectados, tópicos e fluxos de mensagens em tempo real. O acesso ao dashboard é livre através [deste link](https://ufumapl.cloud.shiftr.io). Os parâmetros utilizados para a conexão com este broker em nuvem são mostrador a seguir. Vale lembrar que por ser um serviço gratuito, o tempo de conexão direta é de no máximo 6 horas. Sendo que após esse período, o broker entra em modo de suspensão para o recarregamento das horas. 

![Parâmetros broker MQTT Shiftr.io em nuvem](https://github.com/MAPL-UFU/mqtt-app-controle/assets/73907821/edc3e3e3-32e3-4258-8f95-63ca6b9287f6)

Por conta da limitação do tempo de conexão direta com o broker Shiftr.io, colocou-se no aplicativo a opção de conectar com outro broker gratuito em nuvem. Assim, escolheu-se o broker da empresa HiveMQ. Sendo que neste broker não há limite de conexão direta, porém não há dashboard em tempo real. Os parâmetros utilizados para a conexão com este broker em nuvem são mostrador a seguir.

![Parâmetros broker MQTT HiveMQ em nuvem](https://github.com/MAPL-UFU/mqtt-app-controle/assets/73907821/64c057dd-f54d-4341-89ce-7ecd7d2d8ab2)

## 📁 Acesso aos códigos de configuração do Raspberry Pi
Os códigos de configuração do Raspberry Pi podem ser acessados através [deste diretório](https://github.com/MAPL-UFU/mqtt-app-controle/tree/main/C%C3%93DIGOS%20RASPBERRY).

## 📁 Acesso aos códigos dos microcontroladores
Os códigos para cada microcontrolador podem ser acessados através [deste diretório](https://github.com/MAPL-UFU/mqtt-app-controle/tree/main/C%C3%93DIGOS%20ESP32).

## 📁 Acesso ao projeto do aplicativo
O projeto do aplicativo pode ser acessado através do código fonte [neste diretório](https://github.com/MAPL-UFU/mqtt-app-controle/blob/main/Aplicativo/Android%20Studio/MQTT-APP-AndroidStudio.zip). Lembrando que, o arquivo está em formato .zip. Logo, deve-se realizar o Download para extraí-lo. Ou se preferir, pode-se realizar a instalação diretamente no dispositivo Android através do [APK](https://github.com/MAPL-UFU/mqtt-app-controle/blob/main/Aplicativo/APK/MQTT-App-vFinal.apk). Para realizar o download, deve-se clicar nos três pontos no canto superior direito e depois em "Download".

### 📲 Abrindo e executando o projeto no Android Studio
Lembrando que para abrir o projeto do aplicativo, deve-se instalar o software Android Studio. Utilizou-se no desenvolvimento deste projeto a versão Android Studio Giraffe | 2022.3.1 Patch 1.

Para não ocorrer erros na execução do projeto, siga os passos apresentados a seguir:
- O arquivo será baixado no formato .zip. Assim, é necessário extraí-lo para conseguir acessar;
- Abra o Android Studio e clique em "Open";
- Depois, vá até o diretório onde o arquivo foi extraído e selecione o arquivo;
![AndroidStudio](https://github.com/MAPL-UFU/mqtt-app-controle/assets/73907821/a5fce767-d266-471e-bdf4-b0d8bb05983f)
- Por fim, clique em "Ok" e o projeto será aberto no Android Studio. 

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

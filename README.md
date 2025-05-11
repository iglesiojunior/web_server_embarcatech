
# 🌐 WebServer Embarcado com Raspberry Pi Pico W

Projeto desenvolvido como atividade prática da Residência Tecnológica EmbarcaTech, no núcleo do Instituto Federal do Piauí (IFPI) — Campus Teresina Central. Este sistema integra sensores físicos com um servidor na nuvem usando a placa **BitDogLab** (baseada no **Raspberry Pi Pico W**), permitindo monitoramento remoto via ThingSpeak e uma interface web.

## 📌 Visão Geral

O sistema realiza:

- Leitura digital de **botões físicos**
- Leitura analógica de **joystick** e **sensor externo**
- Leitura digital de **temperatura e umidade** via **DHT11**
- Conexão com **Wi-Fi**
- Envio de dados para o serviço **ThingSpeak** via protocolo **HTTP GET**
- Visualização remota dos dados através de um **site web personalizado**

---

## 🎯 Objetivos

- Aplicar leitura digital e analógica em microcontroladores
- Compreender e implementar bibliotecas de rede para IoT
- Enviar dados de sensores para a nuvem via HTTP
- Interpretar posição do joystick em uma **rosa dos ventos**
- Utilizar o ThingSpeak para monitoramento remoto
- Criar uma interface web responsiva para visualização dos dados

---

## 📚 Bibliotecas Utilizadas

| Biblioteca              | Função                                |
|------------------------|----------------------------------------|
| `pico/stdlib.h`        | GPIOs e delays                         |
| `hardware/adc.h`       | Leitura analógica                      |
| `pico/cyw43_arch.h`    | Controle de Wi-Fi                      |
| `lwip/tcp.h`, `dns.h`  | Comunicação TCP/IP e resolução DNS     |
| `stdio.h`, `string.h`  | Manipulação de strings e E/S           |
| `math.h`               | Cálculo de direção do joystick         |

---

## 🧠 Funcionamento

1. **Leitura de Botões e Joystick**
2. **Conversão da direção do joystick** em direção textual e numérica (rosa dos ventos)
3. **Leitura do sensor DHT11** via GPIO com protocolo manual
4. **Montagem de requisição HTTP GET** com os dados dos sensores
5. **Envio para ThingSpeak** via TCP
6. **Visualização na web** via interface em JavaScript

---

## 🔧 Estrutura de Dados Enviados ao ThingSpeak

| Campo   | Conteúdo                           |
|---------|------------------------------------|
| field1  | Estado do botão A (0 ou 1)         |
| field2  | Estado do botão B (0 ou 1)         |
| field3  | Valor X do joystick (ADC)          |
| field4  | Valor Y do joystick (ADC)          |
| field5  | Direção da rosa dos ventos (0–8)   |
| field6  | Temperatura (°C) do DHT11          |
| field7  | Umidade relativa (%) do DHT11      |

---

## 🌐 Interface Web

👉 Acesse a versão online do site hospedado no Vercel:  
[https://v0-real-time-web-app-ten.vercel.app/](https://v0-real-time-web-app-ten.vercel.app/)

🔑 Para conectar o site ao seu banco de dados no ThingSpeak, é necessário informar o **Channel ID** e a **API Key de leitura** correspondentes ao canal criado na plataforma. Esses dados são inseridos na interface do site e armazenados no navegador via `localStorage`.


👉 Acesse a versão online do site hospedado no Vercel:
[https://v0-real-time-web-app-ten.vercel.app/](https://v0-real-time-web-app-ten.vercel.app/)

A página HTML utiliza JavaScript para:

- Receber Channel ID e API Key do usuário
- Realizar chamadas periódicas à API do ThingSpeak
- Exibir dados em tempo real (botões, direção, sensores)
- Exibir direção na **rosa dos ventos**
- Utilizar **localStorage** para persistência

---

## 🧪 Resultados

- Dados atualizados a cada **15 segundos**
- Interface web com atualização automática a cada **2 segundos**
- Comunicação bem-sucedida com a API REST da ThingSpeak
- Debug possível via monitor serial

---

## 🚫 Limitações e Desafios

- 📡 **Sem HTTPS:** lwIP não suporta TLS por padrão
- ⏱️ **Rate Limit:** mínimo de 15s entre envios
- 🌐 **Sensibilidade à perda de conexão**
- 🌡️ **Precisão limitada do DHT11**
- 💾 **Memória limitada da Pico W**

---

## 📎 Repositório e Recursos

- 🔗 Código completo + vídeo de funcionamento:  
  [github.com/iglesiojunior/web_server_embarcatech](https://github.com/iglesiojunior/web_server_embarcatech)

- 🔍 Documentação da API ThingSpeak:  
  [ThingSpeak REST API - MathWorks](https://www.mathworks.com/help/thingspeak/rest-api.html)

---

## 👨‍💻 Autor

**Iglésio Oliveira de Carvalho Júnior**  
Aluno do curso de Análise e Desenvolvimento de Sistemas  
Instituto Federal do Piauí - Campus Teresina Central

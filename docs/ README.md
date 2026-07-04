# SEE — Sistema Inteligente de Economia de Energia Residencial

Sistema embarcado, baseado em Arduino Uno, que monitora presença e luminosidade em um ambiente para identificar situações de desperdício de energia, como luzes e ar-condicionado ligados sem necessidade. Ao detectar um desperdício, o sistema envia uma sugestão de economia ao usuário, que pode confirmá-la pelo smartphone via comunicação Bluetooth. Cada confirmação rende pontos e níveis em uma mecânica de gamificação que incentiva o hábito de economizar energia.

O projeto foi desenvolvido como um **Mínimo Produto Viável (MVP)**, representando, em escala reduzida, a aplicação do sistema em um ambiente residencial. O LED simula a iluminação do ambiente, enquanto os servomotores representam o acionamento do ar-condicionado e a movimentação da cortina.

Projeto desenvolvido para a disciplina **Trabalho Interdisciplinar I**, da PUC Minas — Instituto de Ciências Exatas e Informática, Departamento de Ciência da Computação.

## Alinhamento com os ODS

- **ODS 7** — Energia Limpa e Acessível
- **ODS 12** — Consumo e Produção Responsáveis

## Como funciona

1. Um sensor **PIR** detecta presença no ambiente, enquanto um **LDR** mede a luminosidade. As leituras do LDR são tratadas por um **Filtro de Média Móvel**, reduzindo oscilações e tornando a análise da luminosidade mais estável.
2. Se o ambiente permanece vazio por mais de 30 segundos com a luz e/ou o ar-condicionado ligados, ou se há iluminação natural suficiente enquanto a luz artificial permanece acesa, o sistema aciona o **buzzer** e envia uma sugestão de economia ao usuário.
3. O usuário confirma a sugestão enviando o comando **`S`** pelo smartphone via Bluetooth. A comunicação Serial via USB também pode ser utilizada para testes e demonstrações.
4. Ao confirmar, a ação correspondente é executada: a iluminação ou o ar-condicionado pode ser desligado e, quando aplicável, a cortina pode ser aberta. A confirmação também atualiza a pontuação da gamificação.
5. Botões físicos permitem ligar ou desligar a iluminação e o ar-condicionado manualmente a qualquer momento, cancelando sugestões pendentes.
6. Quando a luminosidade cai abaixo do limite definido, o sistema fecha automaticamente a cortina, sem necessidade de confirmação do usuário.

### Gamificação

- Cada sugestão de economia confirmada rende **1 ponto**.
- Confirmações realizadas dentro da janela de tempo definida em `JANELA_STREAK` mantêm a sequência de pontos (*streak*).
- Caso o intervalo entre as confirmações ultrapasse essa janela, a sequência é reiniciada.
- A cada **5 pontos**, o usuário sobe de **nível**.
- Uma barra de progresso permite acompanhar a proximidade do próximo nível.

## Hardware utilizado

| Componente | Pino Arduino | Tipo |
|---|:---:|---|
| LDR (fotorresistor) | A0 | Entrada analógica |
| Botão Luz | D4 | Entrada digital |
| Botão AC | D3 | Entrada digital |
| Sensor PIR | D6 | Entrada digital |
| LED | D7 | Saída digital |
| Servo AC | D9 | Saída PWM |
| Servo Cortina | D10 | Saída PWM |
| Buzzer | D13 | Saída digital |
| RX / TX (Bluetooth HC-05) | D0 / D1 | Comunicação Serial |

## Montagem do circuito

O circuito utiliza um Arduino Uno como unidade central de processamento, conectado aos sensores, botões, atuadores e ao módulo Bluetooth HC-05.

O sensor PIR monitora a presença no ambiente, enquanto o LDR realiza a leitura da luminosidade. Os botões permitem o controle manual da iluminação e do ar-condicionado, enquanto o LED e os servomotores representam os equipamentos controlados pelo sistema.

Diagrama de montagem completo no Tinkercad:

https://www.tinkercad.com/things/gcGslgnGDBR-copy-of-simee?sharecode=HlKcOtBjiTTVc4Y-x6fCA2wVtfq33_Im0bU0FhDV7hU

## Estrutura do repositório

. 
├── SEE.ino # Código-fonte do projeto 
├── docs/ # Relatório técnico e imagens da montagem 
└── README.md

## Como executar

1. Monte o circuito conforme o diagrama do Tinkercad ou o esquema de pinos apresentado acima.
2. Abra o arquivo `SEE.ino` na Arduino IDE e faça o upload para o Arduino Uno.
3. Inicie a comunicação a **9600 baud**, utilizando o módulo Bluetooth HC-05 ou, para testes e demonstrações, o Monitor Serial via USB.
4. Interaja com o sistema:
   - envie o comando **`S`** para confirmar uma sugestão de economia;
   - utilize os botões físicos para ligar ou desligar manualmente a iluminação e o ar-condicionado;
   - acompanhe as sugestões, a pontuação e a evolução de nível pela comunicação Serial.

## Melhorias futuras

- [ ] Adicionar **potenciômetro** para calibração manual da sensibilidade do sensor de luminosidade.

## Equipe

- Eduarda de Oliveira Neves
- Hebert William de Souza
- Matheus Nassif Salles Campolina
- Samuel Henrique de Souza Nogueira

**Professores:** Bernardo Guerra Pereira Cunha, Gustavo de Lins e Horta e João Pedro Santos Pereira.

# SEE — Sistema Inteligente de Economia de Energia Residencial

Sistema embarcado, baseado em Arduino Uno, que monitora presença e luminosidade em um ambiente para identificar situações de desperdício de energia (luzes e ar-condicionado ligados sem necessidade) e sugere ações de economia ao usuário, que as confirma pelo Monitor Serial. Cada confirmação rende pontos e níveis, em uma mecânica de gamificação que incentiva o hábito de economizar energia.

Projeto desenvolvido para a disciplina **Trabalho Interdisciplinar I**, PUC Minas — Instituto de Ciências Exatas e Informática, Departamento de Ciência da Computação.

## Alinhamento com os ODS

- **ODS 7** — Energia Limpa e Acessível
- **ODS 12** — Consumo e Produção Responsáveis

## Como funciona

1. Um sensor **PIR** detecta presença no ambiente; um **LDR** mede a luminosidade.
2. Se o ambiente fica vazio por mais de 30s com a luz e/ou o AC ligados, ou se há luz natural suficiente com a luz artificial acesa, o sistema soa o **buzzer** e imprime uma sugestão de economia no Monitor Serial.
3. O usuário confirma a sugestão digitando **`S`** + Enter no Monitor Serial (porta que também está disponível para um módulo Bluetooth HC-05, permitindo confirmação futura via smartphone).
4. Ao confirmar, o atuador correspondente é acionado (LED apagado, servo do AC desligado, servo da cortina aberto) e o usuário ganha **1 ponto**.
5. Botões físicos permitem ligar/desligar a luz e o AC manualmente a qualquer momento, cancelando sugestões pendentes.

### Gamificação

- Confirmações dentro de uma janela de tempo (`JANELA_STREAK`) somam pontos em sequência (streak).
- Ultrapassar essa janela sem confirmar nenhuma economia reinicia o streak.
- A cada **5 pontos** o usuário sobe de **nível**, com uma barra de progresso exibida no Serial.

## Hardware utilizado

| Componente              | Pino Arduino | Tipo               |
|--------------------------|:------------:|---------------------|
| LDR (fotorresistor)      | A0           | Entrada analógica    |
| Botão Luz                | D4           | Entrada digital      |
| Botão AC                 | D3           | Entrada digital      |
| Sensor PIR                | D6           | Entrada digital      |
| LED                       | D7           | Saída digital        |
| Servo AC                  | D9           | Saída PWM             |
| Servo Cortina              | D10          | Saída PWM             |
| Buzzer                    | D13          | Saída digital        |
| RX / TX (reservado p/ Bluetooth HC-05) | D0 / D1 | Serial de hardware |

Diagrama de montagem completo no Tinkercad:
🔗 https://www.tinkercad.com/things/gcGslgnGDBR-copy-of-simee?sharecode=HlKcOtBjiTTVc4Y-x6fCA2wVtfq33_Im0bU0FhDV7hU

## Estrutura do repositório

```
.
├── SEE.ino              # Código-fonte do projeto
├── docs/                # Relatório técnico e imagens da montagem
└── README.md
```

## Como executar

1. Monte o circuito conforme o diagrama do Tinkercad (ou o esquema de pinos acima).
2. Abra o `.ino` na Arduino IDE e faça o upload para o Arduino Uno.
3. Abra o Monitor Serial em **9600 baud**.
4. Interaja com o sistema:
   - Digite **`S`** + Enter para confirmar uma sugestão de economia exibida na tela.
   - Use os botões físicos para ligar/desligar a luz e o AC manualmente.

## Melhorias futuras

- [ ] Conectar fisicamente um módulo **Bluetooth HC-05** aos pinos RX/TX para confirmação sem fio via smartphone.
- [ ] Adicionar **potenciômetro** para calibração manual da sensibilidade de luminosidade.

## Equipe

- Eduarda de Oliveira Neves
- Hebert William de Souza
- Matheus Nassif Salles Campolina
- Samuel Henrique de Souza Nogueira

**Professores:** Bernardo Guerra Pereira Cunha, Gustavo de Lins e Horta, João Pedro Santos Pereira

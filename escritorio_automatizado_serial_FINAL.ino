/*
    Autores: Eduarda de Oliveira Neves 
             Hebert William de Souza 
             Matheus Nassif Salles Campolina
             Samuel Henrique de Souza Nogueira
    Data: 04/07/2026
*/

#include <Servo.h>

// Declaração dos componentes e seus respectivos pinos
const byte LDRPin = A0;
const byte potenciometroPin = A1;

const byte buttonLight = 3;
const byte buttonAC = 2;
const byte PIRPin = 6;
const byte LEDPin = 7;
const byte servoACPin = 9;
const byte servoCortinaPin = 10;
const byte buzzerPin = 13;

// Criação dos objetos para os Servos
Servo servoAC;
Servo servoCortina;

bool estadoLED = LOW;
bool ultimoEstadoBotaoLuz = LOW;

bool estadoAC = LOW;
bool ultimoEstadoBotaoAC = LOW;

int valorLDR = 0;
int limiteDiaNoite = 600;

bool cortinaAberta = false;

unsigned long ultimoTempoMovimento = 0;
const unsigned long tempoAusencia = 30000;

const int buzzerFrequency = 2000;

byte mensagemAtual = 0;
bool atualizarMensagemPadrao = true;
bool ultimoEstadoPresenca = LOW;

// Declaração pinos Bluetooth
const byte RX = 0, TX = 1;


// ==========================================
// GAMIFICAÇÃO
// ==========================================

int pontosEconomia = 0;
int nivelEconomia = 1;

const int PONTOS_PARA_NIVEL = 5;

unsigned long ultimoPontoTempo = 0;

const unsigned long JANELA_STREAK = 30000UL;


void setup()
{
  pinMode(LDRPin, INPUT);
  pinMode(potenciometroPin, INPUT);

  pinMode(PIRPin, INPUT);

  pinMode(buttonLight, INPUT);
  pinMode(buttonAC, INPUT);

  pinMode(LEDPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  digitalWrite(LEDPin, estadoLED);

  servoAC.attach(servoACPin);
  servoCortina.attach(servoCortinaPin);

  servoAC.write(0);
  servoCortina.write(0);

  // Bluetooth
  // pinMode(TX, OUTPUT);
  // pinMode(RX, INPUT);

  Serial.begin(9600);

  Serial.println(F(" Sistema Pronto"));
  Serial.println(F("=== Sistema de Economia de Energia ==="));
  Serial.println(F("Digite 'S' e ENTER no Serial para"));
  Serial.println(F("confirmar uma sugestao de economia."));
}


void loop()
{
  // ==========================================
  // 1. LEITURA DOS SENSORES, BOTÕES E SERIAL
  // ==========================================

  bool presenca = digitalRead(PIRPin);

  valorLDR = analogRead(LDRPin);

  // O potenciometro ajusta o limite de luminosidade
  limiteDiaNoite = map(
    analogRead(potenciometroPin),
    0,
    1023,
    300,
    800
  );

  bool luzSuficiente =
    (valorLDR > (limiteDiaNoite + 50));

  bool leituraBotaoLight =
    digitalRead(buttonLight);

  bool leituraBotaoAC =
    digitalRead(buttonAC);


  char comandoSerial = 0;

  if (Serial.available() > 0)
  {
    comandoSerial = Serial.read();
  }


  // ==========================================
  // 2. ATUALIZAÇÃO DO CRONÔMETRO DO PIR
  // ==========================================

  if (presenca == HIGH)
  {
    ultimoTempoMovimento = millis();

    if (ultimoEstadoPresenca == LOW &&
        mensagemAtual > 0)
    {
      mensagemAtual = 0;

      atualizarMensagemPadrao = true;

      Serial.println(F("Presenca detectada!"));
    }
  }

  ultimoEstadoPresenca = presenca;


  bool salaVazia =
    (millis() - ultimoTempoMovimento >
     tempoAusencia);


  // ==========================================
  // CANCELAMENTO DE ALERTA OBSOLETO
  // ==========================================

  // Se o alerta foi causado por luz natural suficiente,
  // mas a condição deixou de existir, cancela a sugestão.

  if ((mensagemAtual == 4 ||
       mensagemAtual == 5) &&
       !luzSuficiente)
  {
    mensagemAtual = 0;

    atualizarMensagemPadrao = true;

    Serial.println(
      F("Alerta cancelado: luminosidade insuficiente.")
    );
  }


  // ==========================================
  // 3. MATRIZ DE PRIORIDADES
  // ==========================================

  if (mensagemAtual == 0)
  {

    // 1º CASO:
    // Sala vazia + luz ligada + AC ligado

    if (salaVazia &&
        estadoLED == HIGH &&
        estadoAC == HIGH)
    {
      mensagemAtual = 1;

      atualizarMensagemPadrao = true;

      Serial.println(
        F("Vazio. Desligar luz e AC? (S)")
      );
    }


    // 2º CASO:
    // Sala vazia + luz ligada

    else if (salaVazia &&
             estadoLED == HIGH)
    {
      mensagemAtual = 2;

      atualizarMensagemPadrao = true;

      Serial.println(
        F("Vazio. Desligar luz? (S)")
      );
    }


    // 3º CASO:
    // Sala vazia + AC ligado

    else if (salaVazia &&
             estadoAC == HIGH)
    {
      mensagemAtual = 3;

      atualizarMensagemPadrao = true;

      Serial.println(
        F("Vazio. Desligar AC? (S)")
      );
    }


    // 4º CASO:
    // Cortina aberta + luz suficiente + luz ligada

    else if (cortinaAberta &&
             luzSuficiente &&
             estadoLED == HIGH)
    {
      mensagemAtual = 4;

      atualizarMensagemPadrao = true;

      Serial.println(
        F("Dia. Desligar luz? (S)")
      );
    }


    // 5º CASO:
    // Cortina fechada + luz suficiente + luz ligada

    else if (!cortinaAberta &&
             luzSuficiente &&
             estadoLED == HIGH)
    {
      mensagemAtual = 5;

      atualizarMensagemPadrao = true;

      Serial.println(
        F("Dia. Abrir cortina e apagar luz? (S)")
      );
    }


    // Se entrou em algum cenário, toca o buzzer

    if (mensagemAtual > 0)
    {
      tone(
        buzzerPin,
        buzzerFrequency,
        1000
      );
    }


    // Estado padrão

    else
    {
      if (atualizarMensagemPadrao)
      {
        Serial.println(
          F("Sistema OK, sem desperdicio")
        );

        atualizarMensagemPadrao = false;
      }
    }
  }


  // ==========================================
  // 4. AUTOMAÇÃO DE FECHAMENTO AO ANOITECER
  // ==========================================

  if (valorLDR <
      (limiteDiaNoite - 50) &&
      cortinaAberta)
  {
    servoCortina.write(0);

    cortinaAberta = false;

    Serial.println(
      F("Cortina Fechada")
    );
  }


  // ==========================================
  // 5. CONFIRMAÇÃO VIA SERIAL
  // ==========================================

  if ((comandoSerial == 'S' ||
       comandoSerial == 's') &&
       mensagemAtual > 0)
  {

    if (mensagemAtual == 1)
    {
      estadoLED = LOW;

      digitalWrite(
        LEDPin,
        estadoLED
      );

      estadoAC = LOW;

      servoAC.write(0);
    }


    else if (mensagemAtual == 2)
    {
      estadoLED = LOW;

      digitalWrite(
        LEDPin,
        estadoLED
      );
    }


    else if (mensagemAtual == 3)
    {
      estadoAC = LOW;

      servoAC.write(0);
    }


    else if (mensagemAtual == 4)
    {
      estadoLED = LOW;

      digitalWrite(
        LEDPin,
        estadoLED
      );
    }


    else if (mensagemAtual == 5)
    {
      servoCortina.write(90);

      cortinaAberta = true;

      estadoLED = LOW;

      digitalWrite(
        LEDPin,
        estadoLED
      );
    }


    Serial.println(
      F("Acao Executada!")
    );

    registrarPontoEconomia();

    mostrarBarraProgresso();

    mensagemAtual = 0;

    atualizarMensagemPadrao = true;
  }


  // ==========================================
  // 6. BOTÃO MANUAL DA LUZ
  // ==========================================

  if (leituraBotaoLight == HIGH &&
      ultimoEstadoBotaoLuz == LOW)
  {
    delay(50);

    if (digitalRead(buttonLight) == HIGH)
    {
      estadoLED = !estadoLED;

      digitalWrite(
        LEDPin,
        estadoLED
      );


      if (mensagemAtual != 0)
      {
        mensagemAtual = 0;

        atualizarMensagemPadrao = true;
      }
    }
  }

  ultimoEstadoBotaoLuz =
    leituraBotaoLight;


  // ==========================================
  // 7. BOTÃO MANUAL DO AC
  // ==========================================

  if (leituraBotaoAC == HIGH &&
      ultimoEstadoBotaoAC == LOW)
  {
    delay(50);

    if (digitalRead(buttonAC) == HIGH)
    {
      estadoAC = !estadoAC;


      if (estadoAC == HIGH)
      {
        servoAC.write(90);
      }
      else
      {
        servoAC.write(0);
      }


      if (mensagemAtual != 0)
      {
        mensagemAtual = 0;

        atualizarMensagemPadrao = true;
      }
    }
  }

  ultimoEstadoBotaoAC =
    leituraBotaoAC;


  delay(20);
}


// ==========================================
// FUNÇÕES DE GAMIFICAÇÃO
// ==========================================

void registrarPontoEconomia()
{
  unsigned long agora = millis();


  if (ultimoPontoTempo == 0 ||
      (agora - ultimoPontoTempo)
      <= JANELA_STREAK)
  {
    pontosEconomia++;
  }

  else
  {
    pontosEconomia = 1;

    Serial.println(
      F("Demorou dms! Score perdido")
    );
  }


  ultimoPontoTempo = agora;


  if (pontosEconomia >=
      PONTOS_PARA_NIVEL)
  {
    pontosEconomia = 0;

    nivelEconomia++;


    Serial.print(F("Nivel "));

    Serial.print(nivelEconomia);

    Serial.println(F("! Parabens!"));
  }
}


void mostrarBarraProgresso()
{
  Serial.print(F("Nivel "));

  Serial.print(nivelEconomia);

  Serial.print(F("  "));

  Serial.print(pontosEconomia);

  Serial.print(F("/"));

  Serial.print(PONTOS_PARA_NIVEL);

  Serial.print(F("  ["));


  const byte tamanhoBarra = 16;


  byte preenchido =
    map(
      pontosEconomia,
      0,
      PONTOS_PARA_NIVEL,
      0,
      tamanhoBarra
    );


  for (byte i = 0;
       i < tamanhoBarra;
       i++)
  {
    if (i < preenchido)
    {
      Serial.print("#");
    }

    else
    {
      Serial.print("-");
    }
  }


  Serial.println(F("]"));
}

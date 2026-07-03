/*
	Autores: Eduarda de Oliveira Neves 
    		 Hebert William de Souza 
             Matheus Nassif Salles Campolina
             Samuel Henrique de Souza Nogueira
    Data: 09/06/2026
    
    Alterações (gamificação + confirmação via Serial + sem LCD):
    - O botão de confirmação (buttonConfirm) foi removido. A autorização
      das ações de economia agora é feita digitando 'S' no Monitor Serial.
    - O display LCD foi removido. Todas as mensagens que antes iam para
      o LCD agora são impressas via Serial.print/println.
    - A mensagem de estado padrão ("Sistema OK, sem desperdicio") só é
      impressa UMA VEZ quando o sistema entra nesse estado (controlado
      pela flag atualizarMensagemPadrao), evitando spam no Serial a
      cada iteração do loop.
    - Sistema de pontuação: cada confirmação aceita rende 1 ponto, desde
      que o intervalo entre uma confirmação e a anterior seja de no
      máximo JANELA_STREAK. Se o usuário ficar mais tempo que isso sem
      confirmar nenhuma economia, o streak de pontos é reiniciado.
    - A cada 5 pontos (PONTOS_PARA_NIVEL) o usuário sobe de nível e uma
      barra de progresso é exibida via Serial.
*/

#include <Servo.h>

//Declaração dos componentes e seus respectivos pinos
const byte LDRPin = A0;
const byte buttonLight = 4;
const byte buttonAC = 3;
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
const int limiteDiaNoite = 600;
bool cortinaAberta = false;

unsigned long ultimoTempoMovimento = 0;
const unsigned long tempoAusencia = 30000;

const int buzzerFrequency = 2000;

byte mensagemAtual = 0;
bool atualizarMensagemPadrao = true;
bool ultimoEstadoPresenca = LOW; // usado pra detectar borda de subida do PIR

//Declaração pinos Bluetooth
const byte RX = 0, TX = 1;

// ==========================================
// GAMIFICAÇÃO
// ==========================================
int pontosEconomia = 0;              // pontos dentro do nível atual
int nivelEconomia = 1;               // nível atual do usuário
const int PONTOS_PARA_NIVEL = 5;     // pontos necessários para subir de nível
unsigned long ultimoPontoTempo = 0;  // millis() da última confirmação aceita
const unsigned long JANELA_STREAK = 30000UL; //30s em ms(valor pra teste)   //3600000UL; // - 1 hora em ms(valor valido real)

void setup()
{
  pinMode(LDRPin, INPUT);
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

  //Bluetooth
  //pinMode(TX, OUTPUT);
  //pinMode(RX, INPUT);

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
  bool luzSuficiente = (valorLDR > (limiteDiaNoite + 50));
  bool leituraBotaoLight = digitalRead(buttonLight);
  bool leituraBotaoAC = digitalRead(buttonAC);

  char comandoSerial = 0;
  if (Serial.available() > 0){
    comandoSerial = Serial.read();
  }
 
  // ==========================================
  // 2. ATUALIZAÇÃO DO CRONÔMETRO DO PIR
  // ==========================================
  if (presenca == HIGH){
    ultimoTempoMovimento = millis(); // sempre atualiza o cronometro de ausencia

    // só imprime/zera a sugestão na BORDA de subida (evita repetir a cada
    // loop enquanto o PIR permanece em HIGH por vários segundos)
    if (ultimoEstadoPresenca == LOW && mensagemAtual > 0){
      mensagemAtual = 0;
      atualizarMensagemPadrao = true;
      Serial.println(F("Presenca detectada!"));
    }
  }
  ultimoEstadoPresenca = presenca;

  // Verifica se a sala está vazia há mais de 5 minutos
  bool salaVazia = (millis() - ultimoTempoMovimento > tempoAusencia);


  // ==========================================
  // 3. MATRIZ DE PRIORIDADES (IF / ELSE IF)
  // ==========================================
  if (mensagemAtual == 0){

    // 1º CASO: Sala vazia + luz ligada + AC ligado
    if (salaVazia && estadoLED == HIGH && estadoAC == HIGH){
      mensagemAtual = 1;
      atualizarMensagemPadrao = true;
      Serial.println(F("Vazio. Desligar luz e AC? (S)"));
    }
    
    // 2º CASO: Sala vazia + luz ligada
    else if (salaVazia && estadoLED == HIGH){
      mensagemAtual = 2;
      atualizarMensagemPadrao = true;
      Serial.println(F("Vazio. Desligar luz? (S)"));
    }
    
    // 3º CASO: Sala vazia + AC ligado
    else if (salaVazia && estadoAC == HIGH){
      mensagemAtual = 3;
      atualizarMensagemPadrao = true;
      Serial.println(F("Vazio. Desligar AC? (S)"));
    }
    
    // 4º CASO: Cortina aberta + luz suficiente + luz ligada
    else if (cortinaAberta && luzSuficiente && estadoLED == HIGH){
      mensagemAtual = 4;
      atualizarMensagemPadrao = true;
      Serial.println(F("Dia. Desligar luz? (S)"));
    }
    
    // 5º CASO: Cortina fechada + luz suficiente + luz ligada
    else if (!cortinaAberta && luzSuficiente && estadoLED == HIGH){
      mensagemAtual = 5;
      atualizarMensagemPadrao = true;
      Serial.println(F("Dia. Abrir cortina e apagar luz? (S)"));
    }
    // --- SE ACABOU DE ENTRAR EM UM DOS 5 CASOS, DA UM BEEP COM TONE ---
    if (mensagemAtual > 0){
      // tone(pino, frequência_em_Hz, duração_em_ms);
      tone(buzzerPin, buzzerFrequency, 1000); 
    }
    
    // --- STATUS PADRÃO DA CASA (Se não entrou em nenhum caso) ---
    else{
      if (atualizarMensagemPadrao){
        Serial.println(F("Sistema OK, sem desperdicio"));
        atualizarMensagemPadrao = false; // impede repetir essa msg a cada loop
      }
    }
  }


  // ==========================================
  // 4. AUTOMAÇÃO DE FECHAMENTO SE ANOITECER
  // ==========================================
  if (valorLDR < (limiteDiaNoite - 50) && cortinaAberta){
    servoCortina.write(0);
    cortinaAberta = false;
    Serial.println(F("Cortina Fechada"));
  }


  // ==========================================
  // 5. CONFIRMAÇÃO VIA SERIAL (substitui buttonConfirm)
  // ==========================================
  if ((comandoSerial == 'S' || comandoSerial == 's') && mensagemAtual > 0){

    if (mensagemAtual == 1){ // Desliga luz e AC
      estadoLED = LOW;
      digitalWrite(LEDPin, estadoLED);
      estadoAC = LOW;
      servoAC.write(0);
    } 
    else if (mensagemAtual == 2){ // Desliga apenas luz
      estadoLED = LOW;
      digitalWrite(LEDPin, estadoLED);
    } 
    else if (mensagemAtual == 3){ // Desliga apenas AC
      estadoAC = LOW;
      servoAC.write(0);
    } 
    else if (mensagemAtual == 4){ // Desliga apenas luz
      estadoLED = LOW;
      digitalWrite(LEDPin, estadoLED);
    } 
    else if (mensagemAtual == 5){ // Abre cortina e desliga luz
      servoCortina.write(90);  
      cortinaAberta = true;
      estadoLED = LOW;
      digitalWrite(LEDPin, estadoLED);
    }

    Serial.println(F("Acao Executada!"));
    registrarPontoEconomia();
    mostrarBarraProgresso();

    mensagemAtual = 0;
    atualizarMensagemPadrao = true;
  }


  // ==========================================
  // 6. LÓGICA DO BOTÃO DA LUZ MANUAL (buttonLight)
  // ==========================================
  if (leituraBotaoLight == HIGH && ultimoEstadoBotaoLuz == LOW){
    delay(50); 
    if (digitalRead(buttonLight) == HIGH) {
      estadoLED = !estadoLED;          
      digitalWrite(LEDPin, estadoLED); 
      
      if (mensagemAtual != 0){
        mensagemAtual = 0;
        atualizarMensagemPadrao = true;
      }
    }
  }
  ultimoEstadoBotaoLuz = leituraBotaoLight;


  // ==========================================
  // 7. LÓGICA DO BOTÃO DO AC MANUAL (buttonAC)
  // ==========================================
  if (leituraBotaoAC == HIGH && ultimoEstadoBotaoAC == LOW){
    delay(50); 
    if (digitalRead(buttonAC) == HIGH) {
      estadoAC = !estadoAC; 
      
      if (estadoAC == HIGH) {
        servoAC.write(90);  
      } else {
        servoAC.write(0);   
      }
      
      if (mensagemAtual != 0){
        mensagemAtual = 0;
        atualizarMensagemPadrao = true;
      }
    }
  }
  ultimoEstadoBotaoAC = leituraBotaoAC; 
  
  delay(20);
}


// ==========================================
// FUNÇÕES DE GAMIFICAÇÃO
// ==========================================

// Chamada sempre que o usuário confirma (via Serial) uma sugestão de economia.
// Mantém um "streak": se a confirmação atual ocorreu dentro de JANELA_STREAK
// da última, soma 1 ponto; caso contrário, o streak quebra e reinicia em 1 ponto.
void registrarPontoEconomia(){
  unsigned long agora = millis();

  if (ultimoPontoTempo == 0 || (agora - ultimoPontoTempo) <= JANELA_STREAK){
    pontosEconomia++;
  } else {
    // streak quebrado por passar mais tempo que JANELA_STREAK sem confirmar
    pontosEconomia = 1;
    Serial.println(F("Demorou dms! Score perdido"));
  }
  ultimoPontoTempo = agora;

  if (pontosEconomia >= PONTOS_PARA_NIVEL){
    pontosEconomia = 0;
    nivelEconomia++;
    Serial.print(F("Nivel "));
    Serial.print(nivelEconomia);
    Serial.println(F("! Parabens!"));
  }
}

// Imprime via Serial: nível atual, pontos e uma barra de progresso feita
// com caracteres # (preenchido) e - (vazio), proporcional aos pontos atuais.
void mostrarBarraProgresso(){
  Serial.print(F("Nivel "));
  Serial.print(nivelEconomia);
  Serial.print(F("  "));
  Serial.print(pontosEconomia);
  Serial.print(F("/"));
  Serial.print(PONTOS_PARA_NIVEL);
  Serial.print(F("  ["));

  const byte tamanhoBarra = 16;
  byte preenchido = map(pontosEconomia, 0, PONTOS_PARA_NIVEL, 0, tamanhoBarra);

  for (byte i = 0; i < tamanhoBarra; i++){
    if (i < preenchido){
      Serial.print("#");
    } else {
      Serial.print("-");
    }
  }
  Serial.println(F("]"));
}

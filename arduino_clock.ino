#include <RTClib.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Ultrasonic.h>
#include <Keypad.h>

//Inicializa o display no endereco 0x27
LiquidCrystal_I2C lcd(0x27,16,2);

Ultrasonic ultrasonic(4, 5);

RTC_DS3231 rtc;

int distance, horaDefinida, minutoDefinido, segundoDefinido;
int contadorLed = 0;
int contadorLed2 = 0;
int primeiroDigito;
bool alarmeSetado = true;
String hora, minuto;

bool alarmeLigado = false;
bool setarAlarme = false;

const int BUZZER = 2;

unsigned long startMillis;
unsigned long currentMillis = 0;
const unsigned long PERIOD = 1000;

const byte ROWS = 4; 
const byte COLS = 4;

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'},
};

byte rowPins[ROWS] = {13, 12, 11, 10}; 
byte colPins[COLS] = {9, 8, 7, 6};

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);
 
void setup()
{
  lcd.init();

  horaDefinida = 16;
  minutoDefinido = 18;
  segundoDefinido = 0;

  pinMode(BUZZER, OUTPUT);
  noTone(BUZZER);

  Serial.begin(9600);
  
  // initializing the rtc
  if(!rtc.begin()) {
      Serial.println("Couldn't find RTC!");
      Serial.flush();
      while (1) delay(10);
  }

  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  startMillis = millis();
}
 
void loop()
{
  currentMillis = millis();

  lcd.setCursor(13, 1);
  alarmeSetado ? lcd.print("ON ") : lcd.print("OFF");

  char customKey = customKeypad.getKey();
  
  if (customKey) { 
    int num = customKey - '0';
    Serial.print(customKey);

    if (customKey == '*') {
      lcd.clear();
      contadorLed = 0;
      hora = "";
      minuto = "";
      horaDefinida = 0;
      minutoDefinido = 0;
    }

    if (customKey == '#') {
      alarmeSetado = !alarmeSetado;
    }
    
    Serial.print(customKey);
    if (contadorLed < 5 && customKey != '*' && customKey != '#' && customKey != 'A' && customKey != 'B' && customKey != 'C' && customKey != 'D') {
      lcd.setCursor(contadorLed++, 1);

      if (contadorLed == 1 && num < 3) {
        hora += customKey;
        lcd.print(customKey);
        primeiroDigito = num;
      }
      
      if (contadorLed == 2) {
        if (primeiroDigito == 2 && num < 4) {
          hora += customKey;
          lcd.print(customKey);
        } else if (primeiroDigito < 2) {
          hora += customKey;
          lcd.print(customKey);
        } else {
          contadorLed--;
        }
      }

      if (contadorLed == 2) {
        lcd.setCursor(contadorLed++, 1);
        lcd.print(":");
      }

      if (contadorLed == 4) {
        if (num < 6) {
          minuto += customKey;
          lcd.print(customKey);
        } else {
          contadorLed--;
        }
      }

      if (contadorLed == 5) {
        minuto += customKey;
        lcd.print(customKey);

        horaDefinida = hora.toInt();
        minutoDefinido = minuto.toInt();

        Serial.print("HORARIO DEFINIDO");
        Serial.print(horaDefinida);
        Serial.print(minutoDefinido);
      }
    }
  }

  if (currentMillis - startMillis >= PERIOD) { //test whether the period has elapsed
    noTone(BUZZER);
    //lcd.clear();
    lcd.setBacklight(HIGH);
    lcd.setCursor(0,0);

    DateTime now = rtc.now();

    if (now.hour() < 10) lcd.print("0");
    lcd.print(now.hour(), DEC);
    lcd.print(" : ");
    if (now.minute() < 10) lcd.print("0");
    lcd.print(now.minute(), DEC);
    lcd.print(" : ");
    if (now.second() < 10) lcd.print("0");
    lcd.print(now.second(), DEC);

    distance = ultrasonic.read();

    if (now.hour() == horaDefinida && now.minute() == minutoDefinido && now.second() == segundoDefinido && alarmeSetado) {
      ligarAlarme();
    }

    startMillis = currentMillis;  //IMPORTANT to save the start time of the current LED state.
  }
}

void ligarAlarme() {
  alarmeLigado = true;

  while (alarmeLigado) {
    currentMillis = millis();

    if (currentMillis - startMillis >= 2000) {
      tone(BUZZER, 262, 1000);
      startMillis = currentMillis; 
    }
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("ALARME");
    //lcd.setCursor(0, 1);
    //lcd.print("APROXIME A MÃO PARA DESLIGAR");

    distance = ultrasonic.read();
    if (distance < 30) {
      alarmeLigado = false;
      lcd.setCursor(0, 1);
      lcd.print(horaDefinida);
      lcd.setCursor(2, 1);
      lcd.print(":");
      lcd.setCursor(3, 1);
      lcd.print(minutoDefinido);
      //lcd.clear();
    }
  }
}
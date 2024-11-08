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
String hora, minuto;

bool alarmeLigado = false;
bool setarAlarme = false;

const int BUZZER = 2;

unsigned long startMillis;
unsigned long currentMillis = 0;
const unsigned long PERIOD = 1000;

const byte ROWS = 4; 
const byte COLS = 3;

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

byte rowPins[ROWS] = {13, 12, 11, 10}; 
byte colPins[COLS] = {9, 8, 7};

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

  char customKey = customKeypad.getKey();
  
  if (customKey) {
    if (customKey == '#') {
      Serial.print(customKey);
      setarAlarme = true;
    } else if (setarAlarme) {
      Serial.print(customKey);
      if (contadorLed < 5) {
        lcd.setCursor(contadorLed++, 1);
        lcd.print(customKey);

        if (contadorLed < 3) {
          hora += customKey;
        }
        
        if (contadorLed == 2) {
          lcd.setCursor(contadorLed++, 1);
          lcd.print(":");
        }

        if (contadorLed > 3) {
          minuto += customKey;
        }

        if (contadorLed == 5) {
          horaDefinida = hora.toInt();
          minutoDefinido = minuto.toInt();

          Serial.print("HORARIO DEFINIDO");
          Serial.print(horaDefinida);
          Serial.print(minutoDefinido);
        }
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

    if (now.hour() == horaDefinida && now.minute() == minutoDefinido && now.second() == segundoDefinido) {
      ligarAlarme();
    }

    startMillis = currentMillis;  //IMPORTANT to save the start time of the current LED state.
  }
}

void ligarAlarme() {
  alarmeLigado = true;

  while (alarmeLigado) {
    tone(BUZZER, 262);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("ALARME");
    lcd.setCursor(0, 1);
    lcd.print("APROXIME A MÃO PARA DESLIGAR");

    distance = ultrasonic.read();
    if (distance < 30) {
      alarmeLigado = false;
    }
  }
}
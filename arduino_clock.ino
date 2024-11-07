#include <RTClib.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Ultrasonic.h>

//Inicializa o display no endereco 0x27
LiquidCrystal_I2C lcd(0x27,16,2);

Ultrasonic ultrasonic(4, 5);

RTC_DS3231 rtc;

int distance, horaDefinida, minutoDefinido, segundoDefinido;

bool alarmeLigado = false;

const int BUZZER = 2;
 
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
}
 
void loop()
{
  noTone(BUZZER);
  lcd.clear();
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

  lcd.setCursor(0, 1);
  Serial.print("distance " + distance);
  lcd.print(rtc.getTemperature());

  if (now.hour() == horaDefinida && now.minute() == minutoDefinido && now.second() == segundoDefinido) {
    ligarAlarme();
  }

  delay(1000);
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
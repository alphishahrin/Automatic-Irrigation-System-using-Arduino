#include <LiquidCrystal.h> //LCD Library
#include <SoftwareSerial.h> //GSM Library

#define NOTE_C4 262
#define NOTE_D4 294
#define NOTE_E4 330
#define NOTE_F4 349
#define NOTE_G4 392
#define NOTE_A4 440
#define NOTE_B4 494
#define NOTE_C5 523
#define echo 15
#define trigger 14

int temp = 0;
int temp1 = 0;
int T_Sensor = A3;
int M_Sensor = A0;
int R_Sensor = 16;
int W_led = 7;
int Pump_led = 6;
int P_led = 13;
int T_led = 10;
int Speaker = 9;
int Speaker1 = 8;
int val;
int cel;
String text;
long duration;
int distance;
int distance_percent;
int rain;

SoftwareSerial SIM900(0, 1);
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup()
{
  lcd.begin(20, 4);
  lcd.clear();
  pinMode(13, OUTPUT);
  pinMode(7, INPUT);
  pinMode(10, OUTPUT);
  pinMode(6, INPUT);
  SIM900.begin(9600);
  Serial.begin(9600);
  pinMode(9, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(trigger, OUTPUT);
  digitalWrite(trigger, LOW);

  val = analogRead(T_Sensor); //Read Temperature sensor value
  int mv = ( val / 1024.0) * 5000;
  cel = mv / 10;

  lcd.setCursor(0, 0);
  lcd.print("MBSD Project");
  lcd.setCursor(0, 1);
  lcd.print("Group-5");
  delay(1000);
}

void loop()
{
  lcd.clear();
  int Moisture = analogRead(M_Sensor); //Read Moisture Sensor Value
  // LEVEL SENSOR
  digitalWrite(trigger, LOW);
  delayMicroseconds(2);
  digitalWrite(trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger, LOW);
  duration = pulseIn(echo, HIGH);
  distance = duration * 0.017;
  distance_percent = map( distance, 0, 1023, 0, 100);

  rain = digitalRead(R_Sensor); //Read Rain sensor value

  lcd.setCursor(0, 0);
  lcd.print("TANK LEVEL = ");
  lcd.print(distance_percent);
  lcd.print("%");
  lcd.setCursor(0, 1);
  lcd.print("TEMP:");
  lcd.setCursor(5, 1);
  lcd.print(cel);
  lcd.setCursor(7, 1);
  lcd.print("*C");

  while (Serial.available())
  {
    delay(10);
    char c = Serial.read();
    text += c;
  }

  if (text.length() > 0)
  {
    Serial.println(text);
    if (text == "on")
    {
      digitalWrite(6, HIGH);
      digitalWrite(7, HIGH);
    }
    if (text == "off")
    {
      digitalWrite(6, LOW);
      digitalWrite(7, LOW);
    }
    text = "";
  }

  if (Moisture > 700)  // for dry soil
  {
    lcd.setCursor(11, 2);
    lcd.print("DRY SOIL");
    //lcd.setCursor(15, 1);
    //lcd.print("SOIL");
    if (digitalRead(W_led) == 1 && rain == 0) //test the availability of water in storage
    {
      digitalWrite(13, HIGH);
      sms2();
      lcd.setCursor(0, 2);
      lcd.print("W_PUMP:ON");
    }
    else if (digitalRead(W_led) == 1 && rain == 1)
    {
      lcd.setCursor(11, 1);
      lcd.print("RAINING");
      digitalWrite(13, LOW);
      sms();
      lcd.setCursor(0, 2);
      lcd.print("W_PUMP:OFF");
    }
    else if (digitalRead(W_led) == 0 && rain == 1)
    {
      lcd.setCursor(11, 1);
      lcd.print("RAINING");
      digitalWrite(13, LOW);
      sms();
      lcd.setCursor(0, 2);
      lcd.print("W_PUMP:OFF");
    }
    else
    {
      digitalWrite(13, LOW);
      sms();
      lcd.setCursor(0, 2);
      lcd.print("W_PUMP:OFF");

      tone(Speaker, NOTE_C4, 500);
      delay(500);
      tone(Speaker, NOTE_D4, 500);
      delay(500);
      tone(Speaker, NOTE_E4, 500);
      delay(500);
      tone(Speaker, NOTE_F4, 500);
      delay(500);
      tone(Speaker, NOTE_G4, 500);
      delay(500);
    }
  }

  else if (Moisture >= 300 && Moisture <= 700) //for Moist Soil
  {
    lcd.setCursor(11, 2);
    lcd.print("MOISTSOIL");
    //lcd.setCursor(11, 2);
    //lcd.print("SOIL");
    digitalWrite(13, LOW);
    sms();
    lcd.setCursor(0, 2);
    lcd.print("W_PUMP:OFF");
    if (rain == 1)
    {
      lcd.setCursor(11, 1);
      lcd.print("RAINING");
    }
  }

  else if (Moisture < 300)  // For Soggy soil
  {
    lcd.setCursor(11, 2);
    lcd.print("SOGGYSOIL");
    digitalWrite(13, LOW);
    sms();
    lcd.setCursor(0, 2);
    lcd.print("W_PUMP:OFF");
    if (rain == 1)
    {
      lcd.setCursor(11, 1);
      lcd.print("RAINING");
    }
  }

  if (distance_percent > 85 && temp == 0) // Water Level
  {
    digitalWrite(10, LOW);
    sms1();
    lcd.setCursor(0, 3);
    lcd.print("T_PUMP:OFF");
    temp = 1;
    temp1 = 0;
  }
  if (distance_percent > 46 && temp == 1)
  {
    digitalWrite(10, LOW);
    sms1();
    lcd.setCursor(0, 3);
    lcd.print("T_PUMP:OFF");
  }
  if (distance_percent < 45 && temp1 == 0)
  {
    if (digitalRead(Pump_led) == 1) //test the availability of tank in storage
    {
      digitalWrite(10, HIGH);
      sms3();
      lcd.setCursor(0, 3);
      lcd.print("T_PUMP:ON");
      temp1 = 1;
      temp = 0;
    }
    else
    {
      digitalWrite(10, LOW);
      sms1();
      lcd.setCursor(0, 3);
      lcd.print("T_PUMP:OFF");

      tone(Speaker, NOTE_C4, 500);
      delay(500);
      tone(Speaker, NOTE_D4, 500);
      delay(500);
      tone(Speaker, NOTE_E4, 500);
      delay(500);
      tone(Speaker, NOTE_F4, 500);
      delay(500);
      tone(Speaker, NOTE_G4, 500);
      delay(500);
    }
  }
  if (distance_percent < 86 && temp1 == 1)
  {
    if (digitalRead(Pump_led) == 1) //test the availability of tank in storage
    {
      digitalWrite(10, HIGH);
      sms3();
      lcd.setCursor(0, 3);
      lcd.print("T_PUMP:ON");
      temp1 = 1;
    }
    else
    {
      digitalWrite(10, LOW);
      sms1();
      lcd.setCursor(0, 3);
      lcd.print("T_PUMP:OFF");

      tone(Speaker, NOTE_C4, 500);
      delay(500);
      tone(Speaker, NOTE_D4, 500);
      delay(500);
      tone(Speaker, NOTE_E4, 500);
      delay(500);
      tone(Speaker, NOTE_F4, 500);
      delay(500);
      tone(Speaker, NOTE_G4, 500);
      delay(500);
    }
  }
  delay(1000);
}

void sms()
{
  SIM900.print("AT+CMGF=1\r");
  delay(200);
  SIM900.println("AT + CMGS = \"+91xxxxxxxxxx\"");// recipient's mobile number
  SIM900.println("WATERING PUMP IS OFF");// message to send
  Serial.println("WATERING PUMP IS OFF");
  SIM900.println((char)26); // End AT command with a ^Z, ASCII code 26
  Serial.println((char)26);
  //delay(200);
  SIM900.println();
}

void sms1()
{
  SIM900.print("AT+CMGF=1\r");
  SIM900.println("AT + CMGS = \"+91xxxxxxxxxx\"");// recipient's mobile number
  SIM900.println("TANK PUMP IS OFF"); // message to send
  Serial.println("TANK PUMP IS OFF");
  SIM900.println((char)26); // End AT command with a ^Z, ASCII code 26
  Serial.println((char)26);
  //delay(200);
  SIM900.println();
}

void sms2()
{
  SIM900.print("AT+CMGF=1\r");
  SIM900.println("AT + CMGS = \"+91xxxxxxxxxx\"");// recipient's mobile number
  SIM900.println("WATERING PUMP IS ON"); // message to send
  Serial.println("WATERING PUMP IS ON");
  //delay(200);
  SIM900.println((char)26); // End AT command with a ^Z, ASCII code 26
  Serial.println((char)26);
  //delay(200);
  SIM900.println();
}

void sms3()
{
  SIM900.print("AT+CMGF=1\r");
  delay(200);
  SIM900.println("AT + CMGS = \"+91xxxxxxxxxx\"");// recipient's mobile number
  SIM900.println("TANK PUMP IS ON"); // message to send
  Serial.println("TANK PUMP IS ON");
  //delay(200);
  SIM900.println((char)26); // End AT command with a ^Z, ASCII code 26
  Serial.println((char)26);
  //delay(200);
  SIM900.println();
}

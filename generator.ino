#include <avr/io.h>
#include <avr/interrupt.h>
#define wyjscie 9
// #define kwarc 16000000UL //znamionowa częstotliwość taktowania
#define kwarc 15992220UL //zmierzona częstotliwość kwarcu, k... prawie 8kHz odchyłki!!!


String odebraneDane = ""; //Pusty ciąg odebranych danych
float czestotliwosc;
unsigned long podzielnik;


void ustawCzestotliwosc(float czestotliwosc) {
  uint32_t ocr;
  uint16_t prescaler[5] = {1, 8, 64, 256, 1024};
  byte i;
  if (czestotliwosc > kwarc / 2 + 1) {
    Serial.println(F("Za duza czestotliwosc"));
    return;
  }
  if (czestotliwosc < 0.1) {
    Serial.println(F("Za mala czestotliwosc"));
    return;
  }

  i = 0;
  while ( i < 5 ) {
    ocr = kwarc / (2 * prescaler[i] * czestotliwosc) - 0.5;

    if (ocr <= 0xFFFFL) {
      break;
    }
    i++;
  }

  if (i < 5) {
    /*  Serial.print("ocr = ");
        Serial.println(ocr);
        Serial.print("prescaler = ");
        Serial.println(prescaler[i]); */
  } else {
    Serial.println(F("Za mala  czestotliwosc"));
    return;
  }
  Serial.println(F("OK"));
  Serial.print(F("Wprowadzono f = "));
  Serial.print(czestotliwosc, 4);
  Serial.println(F(" Hz"));
  Serial.print(F("Na wyjsciu f = "));
  czestotliwosc = kwarc / (prescaler[i] * (1 + ocr) * 2.0);
  Serial.print(czestotliwosc, 4);
  Serial.println(F(" Hz\n"));

  noInterrupts();
  TCCR1A = ( (1 << COM1A0));  // Toggle OC1A/OC1B on Compare Match.
  TCCR1B = 0; // Timer/Counter1 Control Register
  TCCR1B |= (1 << WGM12);
  switch (i) {
    case 0:
      // prescaler 1
      TCCR1B |= (1 << CS10) ;
      break;
    case 1:
      // prescaler 8
      TCCR1B |=  (1 << CS11);
      break;
    case 2:
      //prescaler 64
      TCCR1B |=   (1 << CS10) | (1 << CS11);
      break;
    case 3:
      // prescaler 256
      TCCR1B |=  (1 << CS12);
      break;
    case 4:
      // prescaler 1024
      TCCR1B |= (1 << CS12) | (1 << CS10);
      break;
  }

  OCR1A = ocr;
  interrupts();
}


void setup() {
  Serial.begin(9600); //Uruchomienie komunikacji
  pinMode(wyjscie, OUTPUT); //Konfiguracja wyjść
  digitalWrite(wyjscie, LOW); //Wyłączamy diody
  ustawCzestotliwosc(10);
}


void loop() {
  if (Serial.available() > 0) { //Czy Arduino odebrało dane
    //Jeśli tak, to odczytujemy je do znaku końca linii i zapisz w zmiennej odebraneDane
    odebraneDane = Serial.readStringUntil('\n');
    odebraneDane.trim();  // na ewentualny CR z Putty

    czestotliwosc = odebraneDane.toFloat();

    if (odebraneDane.equalsIgnoreCase("A") ) {
      czestotliwosc =  440.0;
      Serial.println(F("A"));
    }
    if (odebraneDane.equalsIgnoreCase("AIS") ) {
      czestotliwosc =  466.1637615180899;
      Serial.println(F("Ais"));
    }
    if (odebraneDane.equalsIgnoreCase( "H") ) {
      czestotliwosc =  493.8833012561241;
      Serial.println(F("H"));
    }
    if (odebraneDane.equalsIgnoreCase( "C") ) {
      czestotliwosc =  523.2511306011972;
      Serial.println(F("C"));
    }
    if (odebraneDane.equalsIgnoreCase("CIS") ) {
      czestotliwosc =  554.3652619537442;
      Serial.println(F("Cis"));
    }
    if (odebraneDane.equalsIgnoreCase("D" )) {
      czestotliwosc =  587.3295358348151;
      Serial.println(F("D"));
    }
    if (odebraneDane.equalsIgnoreCase("DIS") ) {
      czestotliwosc =  622.2539674441618;
      Serial.println(F("Dis"));
    }
    if (odebraneDane.equalsIgnoreCase("E" )) {
      czestotliwosc =  659.2551138257398;
      Serial.println(F("E"));
    }
    if (odebraneDane.equalsIgnoreCase("F" )) {
      czestotliwosc =  698.4564628660078;
      Serial.println(F("F"));
    }
    if (odebraneDane.equalsIgnoreCase( "FIS") ) {
      czestotliwosc =  739.9888454232688;
      Serial.println(F("Fis"));
    }
    if (odebraneDane.equalsIgnoreCase("G" )) {
      czestotliwosc =  783.9908719634985;
      Serial.println(F("G"));
    }
    if (odebraneDane.equalsIgnoreCase("Gis") ) {
      czestotliwosc =  830.6093951598903;
      Serial.println(F("Gis"));
    }

    if (odebraneDane.length() )
      ustawCzestotliwosc(czestotliwosc);
    odebraneDane = "";
  }
}

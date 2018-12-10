#include <avr/io.h>
#include <avr/interrupt.h>
#define wyjscie 9
#define kwarc 16000000L

String odebraneDane = ""; //Pusty ciąg odebranych danych
float czestotliwosc;
unsigned long podzielnik;


void ustawCzestotliwosc(float czestotliwosc) {
  uint32_t ocr;
  uint16_t prescaler[5] = {1, 8, 64, 256, 1024};
  byte i;
  if (czestotliwosc > kwarc / 2 + 1) {
    Serial.println("Za duza czestotliwosc");
    return;
  }
  if (czestotliwosc < 0.1) {
    Serial.println("Za mala czestotliwosc");
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
    Serial.println("Za mala  czestotliwosc");
    return;
  }
  Serial.println("OK");
  Serial.print("Wprowadzono f = ");
  Serial.print(czestotliwosc, 4);
  Serial.println("Hz");
  Serial.print("Na wyjsciu f = ");
  czestotliwosc = kwarc / (prescaler[i] * (1 + ocr) * 2.0);
  Serial.print(czestotliwosc, 4);
  Serial.println("Hz");

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
    czestotliwosc = odebraneDane.toFloat();
    ustawCzestotliwosc(czestotliwosc);
  }
}

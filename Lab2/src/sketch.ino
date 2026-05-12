// Biblioteke tajmera i sleep mode-ova
#include <LowPower.h>
#include <TimerOne.h>

// Definiranje pinova
#define BUTTON 2 //Tipkalo na pin 2
#define LED 12 //LED na pin 12

unsigned long lastDebounceTime = 0;  // Zadnji put kada se prekid dogodio
unsigned long debounceDelay = 50;    // Trazeni minimalni razmak izmedu prekida

// Zastavica za budenje sustava
volatile bool awakeFlag = true;


void setup() {
  Serial.begin(115200); //Postavljanje serijske konekcije

  // Konfiguracija pinova
  pinMode(LED, OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP);
  
  // Postavljanje prekida na tipku sa FALLING bridom
  attachInterrupt(digitalPinToInterrupt(BUTTON), debounce, FALLING);

  // Pokretanje timera
  Timer1.initialize(10000000); // Postavljanje vremena (10s)
  Timer1.attachInterrupt(wakeUp); // Postavljanje prekida na tajmer
}

void loop() {

  //Provjera je li sistem budan za izvršiti funkciju
  if (awakeFlag) {
    blink(2);
    awakeFlag = false;
    sleepMode();
  }

}

//Vraćanje sustava u energetski rezim
void sleepMode() {

  Serial.print(millis()/1000);
  Serial.println("s - Sustav spava \n");

  // Prebacivanje mikrokontrolera u Power-down mode
  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);  //Iskljucujemo ADC i BOD

  // Prebacivanje mikrokontrolera u Standby mode
  //LowPower.powerStandby(SLEEP_FOREVER, ADC_OFF, BOD_OFF); 

  // Prebacivanje mikrokontrolera u Idle mode
  //LowPower.idle(SLEEP_8S, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF, SPI_OFF, USART0_OFF, TWI_OFF);
}

//Funkcija za treptanje LED diodom
void blink(int blinkCount) {

  Serial.print(millis()/1000);
  Serial.println("s - Sustav izvrsava funkciju");

  for (int i = 0; i < blinkCount; i++) {
    digitalWrite(LED, HIGH);
    delay(250);
    digitalWrite(LED, LOW);
    delay(250);
  }

}

//Softverski debounce
void debounce(){

  //Provjera je li proslo dovoljno milisekundi od zadnjeg zvanja prekida
  if (millis() - lastDebounceTime > debounceDelay) {
    //Ako je, zovemo budenje sustava
    wakeUp();
  }

  //Postavljanje trenutnog vremena na lastDebounceTime
  lastDebounceTime = millis();

}

//Funkcija za okretanje zastavice budnog sustava
void wakeUp() {

  Serial.print(millis()/1000);
  Serial.println("s - Sustav se budi");
  
  awakeFlag = true;

}

/**
 * @file sketch.ino
 * @mainpage RUS - Rad s Ugradbenim Sustavima
 * 
 * @section overview Opis sustava
 * Ova vježba koristi ESP32 mikrokontroler za upravljanje svim vanjskim uređajima, te FreeRTOS realtime operacijski sustav za stvaranje tajmerskih prekide.
 * Korišteni vanjski uređaji su dva gumba, senzor plina i senzor kretanja. Cilj vježbe je uspješno implmentirati i upravljati višestrukim prekidima različitih prioriteta.
 *
 * 
 * @seection button_interrupt Prekidi gumba
 * Sustav koristi dva gumba povezana na GND i digitalni ulaz kako bi detektirali pritisak
 * - Gumb 1 je povezan na pin 25
 * - Gumb 2 je povezan na pin 26
 * 
 * Gumbima je svojstvo "bounce" softverski postavljeno na 0 eleminirajući potrebu za debouncing dijelom koda. Gumbi su povezani na prekid koji mijenja dodijeljene zastavice (`flag_button1`, `flag_button2`), 
 * te prolaskom kroz glavnu petlju hijerarhijski odlučuje kako se pali LED povezane sa zastavicama.
 * 
 * @section gas_interrupt Prekid senzora plina
 * Senzor plina MQ2 je povezan na GND, 5V napajanje, te analogni ulaz na pinu 34. Senzor kontinuirano motri (simuliranu) razinu plina, te dok je ona iznad 5000ppm pali
 * LED ovisno o hijerarhiji provedenih prekida.
 * 
 * @section movement_interrupt Prekid senzora kretanja
 * Senzor kretanja PIR je povezan na GND, 5V napajanje, te digitalni ulaz na pinu 27. Kada senzor osjeti pokret, šalje digitalni signal i prekidom okreće dodijeljenju zastavicu (`flag_motion`). Time pali LED 
 * ovisno o trenutnim prekidima, uračunavajući da je najniži prekid na listi prioriteta.
 * 
 * @section timer_interrupt Prekid timera
 * Za implementaciju timera se koristi FreeRTOS software timer. Timer je postavljen da generira prekid svakih 2 sekunde, te okreće dodijeljenu zastavicu (`flag_timer`). Time se uvijeek prva pali povezana LED zbog 
 * statusa najvišeg prioriteta timerskog prekida.
 * 
 * 
 * @section interrupt_priority Prekidi i prioriteti
 * 
 * | ID           | Opis                                                                   | Prioritet    |
 * |--------------|------------------------------------------------------------------------|--------------|
 * | ISR_TIME     | Svaki tajmerski prekid ima najviši prioritet                           | Najviši      |
 * | ISR_1        | Sustav detektira pritisak gumba na pinu 25                             | Gumb (Visok) |
 * | ISR_2        | Sustav detektira pritisak gumba na pinu 26                             | Gumb (Niski) | 
 * | ISR_GAS      | Sustav detektira razinu plina iznad 5000ppm                            | Srednji      |
 * | ISR_MOTION   | Sustav deetektira kretanje uz pomoć senzora                            | Najniži      |
 * 
 * @note 
 * Iako ESP32 podržava ugnježđene prekide, zbog manjka pokrivenosti tog materijala na internetu i 
 * nedovoljnog znanja o tehnologiji, u nemogućnosti sam ih pravilno (na razini sklopovlja) implementirati. 
 * Zbog toga će pravilna odrada prekida po prioritetima biti simulirana kao u danome primjeru
 * 
 * @author Mislav Maljković
 * @date 25. ožujak 2026.
 * @version 1.0
*/


//Definiranje pinova za tipkala i LED diode
#define LED_1 18    ///< Pin za (plavi) LED indikator prekida ISR_TIME (Najviši prioritet)
#define LED_2 19    ///< Pin za (crveni) LED indikator prekida ISR_1 (Viši gumb prioritet)
#define LED_3 21    ///< Pin za (rozi) LED indikator prekida ISR_2 (Niži gumb prioritet)
#define LED_4 22    ///< Pin za (zuti) LED indikator prekida ISR_GAS (Srednji prioritet)
#define LED_5 23    ///< Pin za (zeleni) LED indikator prekida ISR_MOTION (Najmanji prioritet)

#define BUTTON_1 25   ///< Pin za gumb koji aktivira prekid ISR_1
#define BUTTON_2 26   ///< Pin za gumb koji aktivira prekid ISR_2


//Definiranje pinova za senzor pokreta i senzora plina
#define MOTION_SENS 27      ///< Pin za digitalni ulaz senzora kretanja
#define GAS_SENS 34         ///< Pin za analogni ulaz senzora plina


//Globalne varijable za praćenje stanja prekida
volatile bool flag_button1 = false;     ///< Zastavica koja signalizira prekid ISR_1
volatile bool flag_button2 = false;     ///< Zastavica koja signalizira prekid ISR_2
volatile bool flag_timer = false;       ///< Zastavica koja signalizira prekid timera
volatile bool flag_motion = false;      ///< Zastavica koja signalizira prekid senzora kretanja
volatile bool canInterrupt = true;      ///< Zastavica koja pokazuje dostupnost novih prekida 
int analog = 0;                         ///< Konstantno mjerena vrijednost senzora plina
int blinkTiming = 400;                  ///< Vrijeme svijetljenja signala na LED intdikatoru (u milisekundama)


//Handler za timer koji koristimo
TimerHandle_t priorityTimer = NULL;     ///< Oznaka korištenog timera

/**
 * @brief Inicijalizacija LED pinova, pinova za gumbe, te kreacija prekida i timera
 *
 * Ova funkcija postavlja smjer pinova za LED indikatore i tipkala, postavlja vanjske prekide (tipkalo i senzor pokreta)
 * na rastući brid signala. Također konfigurira i pokreće timer koji se koristi za prekid.
 * Na kraju pokreće serisku komunikaciju kako bi omogućila poruke.
 */
void setup() {

  //Postavljanje LED pinova
  pinMode(LED_1, OUTPUT);     //Timer prekid (Najvisi prioritet)
  pinMode(LED_2, OUTPUT);     //Prvi Gumb prekid (Visi gumb prioritet)
  pinMode(LED_3, OUTPUT);     //Drugi Gumb prekid (Nizi gumb prioritet)
  pinMode(LED_4, OUTPUT);     //Prekid senzora plina (Srednji prioritet)
  pinMode(LED_5, OUTPUT);     //Preekid senzora pomaka (Najnizi prioritet)

  //Postavljanje pinova za gumbe
  pinMode(BUTTON_1, INPUT_PULLUP);
  pinMode(BUTTON_2, INPUT_PULLUP);

  //Kreiranje veze između pinova i prekida
  attachInterrupt(digitalPinToInterrupt(BUTTON_1), ISR_1, RISING);
  attachInterrupt(digitalPinToInterrupt(BUTTON_2), ISR_2, RISING);
  attachInterrupt(digitalPinToInterrupt(MOTION_SENS), ISR_MOT, RISING);

  //Definiranje svojstava i pokretanje tajmera
  priorityTimer = xTimerCreate(
    "PriorityTimer",                   //Ime timera
    2000 / portTICK_PERIOD_MS,         //Frekvencija timera
    pdTRUE,                            //Ponovno pokretajne timera
    NULL,                              //Timer ID
    PriorityCallback                   //Funkcija koja se zove svakih X
  );
  xTimerStart(priorityTimer, 0);       //Pokretanje timera

  //Pokretanje serijske komunikacije
  Serial.begin(115200);
}

/**
 * @brief Očitavanje vrijednosti senzora i rukovanj prekidima
 *
 * Ova funkcija svakim prolazom cita i zapisuje vrijednost na senzoru plina.
 * Ovisno o vrijednostima zastavica i analognog čitanja, koristeći zadanu hijerarhiju obrađuje
 * prekid najvišega prioriteta svakim prolazom, te im resetira zastavicu kad su odrađeni.
 */
void loop() {
  delay(10); //Delay za ubrzanje simulacije

  //Citanje vrijednosti senzora plina
  analog = analogRead(GAS_SENS);
  Serial.println(analog); 

  //Prioritiziramo timer interrupt, te nakon hijerarhijski po redu radimo pozvane prekide
  if(flag_timer){
    Blink(LED_1);
    flag_timer = false;
  }else{
    
    if(flag_button1 && canInterrupt){
      Blink(LED_2);
      flag_button1 = false;
    }else if(flag_button2 && canInterrupt){
      Blink(LED_3);
      flag_button2 = false;
    }else if(analog > 3900 && canInterrupt){
      Blink(LED_4);
    }else if(flag_motion && canInterrupt){
      Blink(LED_5);
      flag_motion = false;
    }

  }
}
  
//ARDUINO_ISR_ATTR atribut za prekide
/**
 * @brief Upravljanje zastavicom prekida gumba 1
 *
 * Kada je gumb 1 stisnut i dođe do prekida, ova funkcija mijenja stanje zastavice flag_button1
 */
void ISR_1(){
  flag_button1 = true;
}

/**
 * @brief Upravljanje zastavicom prekida gumba 2
 *
 * Kada je gumb 2 stisnut i dođe do prekida, ova funkcija mijenja stanje zastavice flag_button2
 */
void ISR_2(){
  flag_button2 = true;
}

/**
 * @brief Upravljanje zastavicom prekida gumba senzora kretanja
 *
 * Kada senzor pokreta osjeti kretanje, dolazi do prekida te ova funkcija mijenja stanje zastavice flag_motion
 */
void ISR_MOT(){
  flag_motion = true;
}

/**
 * @brief Upravljanje zastavicom timera
 *
 * Kada dođe do promjene prekida timera ova funkcija se aktivira. Onda mijenja stanje zastavice flag_timer, te
 * onemogućuje sve ostale prekide dajući timeru prioritet
 */
void PriorityCallback(TimerHandle_t xTimer) {
  canInterrupt = false;
  flag_timer = true;
}

/**
 * @brief Bljeskanje LED
 *
 * Pozivom ove funkcije s predaje broj LED indikatora. Taj indikator se pali, te nakon određenog odgađanja 
 * ponovno gasi. U tome vremenskom periodu je onemogućena obrada drugih prekida
 */
void Blink(int pinNumber){
  //FlagReset();
  canInterrupt = false;   
  digitalWrite(pinNumber, HIGH);
  delay(blinkTiming);
  digitalWrite(pinNumber, LOW);
  canInterrupt = true;
}

/*void FlagReset(){
  flag_button1 = false;
  flag_button2 = false;
  flag_motion = false;
}*/

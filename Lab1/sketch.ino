/*Iako ESP32 podržava ugnježđene prekide, zbog manjka pokrivenosti tog materijala na internetu i 
nedovoljnog znanja o tehnologiji, u nemogućnosti sam ih pravilno (na razini sklopovlja) implementirati. 
Zbog toga će pravilna odrada prekida po prioritetima biti simulirana kao u danome primjeru*/


//Definiranje pinova za tipkala i LED diode
#define LED_1 18
#define LED_2 19
#define LED_3 21
#define LED_4 22
#define LED_5 23

#define BUTTON_1 25
#define BUTTON_2 26


//Definiranje pinova za senzor pokreta i senzora plina
#define MOTION_SENS 27      //Digitalni izlaz senzora kretanja na pinu 27
#define GAS_SENS 34         //Analogni izlaz senzora plina na pinu 34


//Globalne varijable za praćenje stanja prekida
volatile bool flag_button1 = false;     //Zastavica gumba 1
volatile bool flag_button2 = false;     //Zastavica gumba 2
volatile bool flag_timer = false;       //Zastavica za timer
volatile bool flag_motion = false;      //Zastavica za senzor pokreta
volatile bool canInterrupt = true;      //Zastavica za prekide u tijeku
int analog = 0;                         //Vrijednost senzora plina
int blinkTiming = 400;                    //Vrijeme upaljene led


//Handler za timer koji koristimo
TimerHandle_t priorityTimer = NULL;

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
//ISR gumba 1
void ISR_1(){
  flag_button1 = true;
}

//ISR gumba 2
void ISR_2(){
  flag_button2 = true;
}

//ISR senzora kretanja
void ISR_MOT(){
  flag_motion = true;
}

//ISR timera
void PriorityCallback(TimerHandle_t xTimer) {
  canInterrupt = false;
  flag_timer = true;
}

//Kod za blinkanje određene led
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
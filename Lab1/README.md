# Lab1: Prekidi u ugradbenim sustavima

## 1. Opis sustava
Ova vježba koristi ESP32 mikrokontroler za upravljanje svim vanjskim uređajima, te FreeRTOS realtime operacijski sustav za stvaranje tajmerskih prekide.
Korišteni vanjski uređaji su dva gumba, senzor plina i senzor kretanja. Cilj vježbe je uspješno implmentirati i upravljati višestrukim prekidima različitih prioriteta.

---

## 2. Funkcionalnost

### 2.1. Prekidi gumba
Sustav koristi dva gumba povezana na GND i digitalni ulaz kako bi detektirali pritisak
- Gumb 1 je povezan na pin **25**
- Gumb 2 je povezan na pin **26**

Gumbima je svojstvo "bounce" softverski postavljeno na 0 eleminirajući potrebu za debouncing dijelom koda. Gumbi su povezani na prekid koji mijenja dodijeljene zastavice (`flag_button1`, `flag_button2`), 
te prolaskom kroz glavnu petlju hijerarhijski odlučuje kako se pali LED povezane sa zastavicama.

### 2.2. Prekid senzora plina
Senzor plina je povezan na GND, 5V napajanje, te analogni ulaz na pinu **34**. Senzor kontinuirano motri (simuliranu) razinu plina, te dok je ona iznad 5000ppm pali
LED ovisno o hijerarhiji provedenih prekida.

### 2.3. Prekid senzora kretanja
Senzor kretanja je povezan na GND, 5V napajanje, te digitalni ulaz na pinu **27**. Kada senzor osjeti pokret, šalje digitalni signal i prekidom okreće dodijeljenju zastavicu (`flag_motion`). Time pali LED 
ovisno o trenutnim prekidima, uračunavajući da je najniži prekid na listi prioriteta.

### 2.4. Prekid timera
Za implementaciju timera se koristi FreeRTOS software timer. Timer je postavljen da generira prekid svakih 2 sekunde, te okreće dodijeljenu zastavicu (`flag_timer`). Time se uvijeek prva pali povezana LED zbog 
statusa najvišeg prioriteta timerskog prekida.

---

## 3, Prekidi i prioriteti

| ID           | Opis                                                                   | Prioritet    |
|--------------|------------------------------------------------------------------------|--------------|
| ISR_TIME     | Svaki tajmerski prekid ima najviši prioritet                           | Najviši      |
| ISR_1        | Sustav detektira pritisak gumba na pinu **25**                         | Gumb (Visok) |
| ISR_2        | Sustav detektira pritisak gumba na pinu **26**                         | Gumb (Niski) | 
| ISR_GAS      | Sustav detektira razinu plina iznad 5000ppm                            | Srednji      |
| ISR_MOTION   | Sustav deetektira kretanje uz pomoć senzora                            | Najniži      |

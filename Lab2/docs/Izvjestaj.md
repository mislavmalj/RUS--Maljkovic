# Lab2: Upravljanje potrošnjom energije mikrokontrolera

## Sažetak



## 1. Opis sustava

Uz pomoć Wokwi simulacijskog okruženja i Arduino mikrokontrolera, simuliramo i uspoređujemo režime mirovanja mikrokontrolera, te njihov utjecaj na potrošnju energije. Sustav nakon definiranog zadatka ulazi u zadani režim mirovanja, te je konfiguriran vratite se natrag u aktivno stanje nakon vanjskog ili tajmerskog mehanizma buđenja. Definirani zadatak u ovoj implementaciji su dva bljeska led diode, nakon kojih mikrokontroler započne mirovati.

## 2. Razine režima mirovanja Arduino mikrokontrolera (LowPower.h bibilioteka)

- PowerDown - **LowPower.powerDown()**, Najdublje stanje mirovanja, Većina periferije onesposobljena i najmanja potrošnja energijee
- Standby - **LowPower.powerStandby()**, Sličan PowerDown režime uz jednu glavnu razliku, Jedan vanjski oscilator ostaje u funkciji što omogućava brzo buđenje
- Idle - **LowPower.idle()**, Koristi najviše energije od režima mirovanja, Zaustavlja rad procesora i brzo se budi

## 3. Korišteni načini buđenja sustava

### 3.1. Prekid tipkalom

Kao eksterni prekid sustav koristi tipkalo povezano na digitalni **pin 2**. Tipkalo koristi **FALLING** brid za aktivaciju prekida. Aktivacijom prekida budi sustav i okreće zastavicu koju sustav koristi za izvedbu glavne funkcije (treptanje led diode).

### 3.2. Prekid tajmerom

Uz bibilioteku **TimerOne**, kreiran je tajmer koji generira novi prekid svaki određeni vremenski period, u ovome slučaju 10 sekundi. Svakim tajmerskim prekidom se zove funkcija koja okreće zastavicu **awakFlag**, te izvršava glavnu funkciju.

### 4. Opis implementacije

Ova laboratorijska vježba u svojoj Arduino implementaciji koristi dvije bibilioteke. **LowPower** biblioteka omogućuj jednostavno upravljanje režimima mirovanja, te dodatnu prilagodbu dijlova mikrokontrolera koje imamo želju onesposobiti. Podržava sve tražene režim mirovanja (PowerDown, Standby i Idle). Druga bibilioteka, TimerOne, omogućava jednostavno kreiranj tajmera svih vrsta, što ju ćini idealnom za izradu potrebnog tajmerskog prekida. 
Od vanjskih elemnata koristimo jednu led diodu spojenu na digitalni pin 12 i tipkalo spojeno na pin 2 koje služi za aktivaciju eksternog prekida.

### 5. Ograničenja simulacije

Makar je iznimno snažno, Wokwi simulacijsko okruženje sadrži neka kritična ograničenja koja onemogućavaju izvršnenje svih zadataka ove laboratorijsk vježbe. Zbog nedostatka fizičke opreme, mjerenje potrošnje energije postaje ekstremno teško, ako ne i nemoguće, što nas navodi da temu ove vježbe sagledamo više iz logičke i teorijske perspektive. Drugi veliki nedostatak simuiliranog okruženja je minimalna razlika između režima mirovanja, koja je puno izraženija na pravome sklopovlju. Zbog tih faktora ovaj izvještaj se svodi više na teorijska znanja, koristeći vlastitu implementaciju kao priliku prolaska kroz sve glavne dijelove ciklusa režima mirovanja.

### 6. Zaključak

Uz pametno korištenje režima mirovanja i njegovu implementaciju u kodu, potrošnja energije se efikasno optimizira. Iako simulacijski nije moguće u potpunosti procjeniti stvarnu potrošnju energije, naša implementacija prikazuje logiku kojom bi se došlo do smanjenja gubitka energije, te postavlja temelje za pravilnu implementaciju na fizičkom sklopovlju.

### 7. Podzadatak: Pametni poštanski sandučić - Debouncing

#### 7.1. Uzrok problema

**Bounce** je problem kod tipkala gdje se jedan pritisak registrira kao njih više. Do bouncinga dolazi eksluzivno na fizičkom sklopovlju (po potrebi može biti simulirano) kada se zbog mehaničkih problma zbog jednog pritiska više puta ostvari oscilirajući kontakt. Zbog bouncinga dolazi do neželjnih aktivacija prekida i drugih problema, te ga se treba odmah rješiti.

#### 7.2. Odabrano rješenje

Za rješenj je odabrano softversko debouncanje. Koristimo novu funkciju **debounce()**, te varijable **lastDebounceTime** i **debounceDelay**. Cilj funkcije je računati koliko se često poziva ISR na tipkalu, te ako je vrijeme aktivacije manje od vrijdnosti **debounceDelay**, ne proslijediti na daljnu funkciju i buđenjee sustava. U **lastDebounceTim** spremamo vrijednost zadnjeg zvanja prekida tipkala kako bi ga mogli usporediti sa sljedećim zvanjem.

#### 7.3. Utjecaj debouncinga na vremensku učinkovitost

Eliminiranjem bouncanja kontakta tipkala oslobađamo sustav od bespotrebnih prolaza i izvršavanja koda. Time koristimo manje energije, ranije završavamo kod, te prije ulazimo natrag u stanje mirovanja za daljnju eenergetski učinkovitu izvedbu koda.

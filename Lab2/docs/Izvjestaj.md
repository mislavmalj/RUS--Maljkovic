Lab2: Upravljanje potrošnjom energije mikrokontrolera
1. Opis sustava

Uz pomoć Wokwi simulacijskog okruženja i Arduino mikrokontrolera, simuliramo i uspoređujemo režime mirovanja mikrokontrolera, te njihov utjecaj na potrošnju energije. Sustav nakon definiranog zadatka ulazi u zadani režim mirovanja, te je konfiguriran vratite se natrag u aktivno stanje nakon vanjskog ili tajmerskog mehanizma buđenja. Definirani zadatak u ovoj implementaciji su dva bljeska led diode, nakon kojih mikrokontroler započne mirovati.
2. Razine režima mirovanja Arduino mikrokontrolera (LowPower.h bibilioteka)

    PowerDown - LowPower.powerDown(), Najdublje stanje mirovanja, Većina periferije onesposobljna i najmanja potrošnja energijee
    Standby - LowPower.powerStandby(), Sličan PowerDown režime uz jednu glavnu razliku, Jedan vanjski oscilator ostaje u funkciji što omogućava brzo buđenje
    Idle - LowPower.idle(), Koristi najviše energije od režima mirovanja, Zaustavlja rad procesora i brzo se budi

3. Korišteni načini buđenja sustava
3.1. Prekid tipkalom

Kao eksterni prekid sustav koristi tipkalo povezano na digitalni pin 2. Tipkalo koristi FALLING brid za aktivaciju prekida. Aktivacijom prekida budi sustav i okreće zastavicu koju sustav koristi za izvedbu glavne funkcije (treptanje led diode).
3.2. Prekid tajmerom

Uz bibilioteku TimerOne, kreiran je tajmer koji generira novi prekid svaki određeni vremenski period, u ovome slučaju 10 sekundi. Svakim tajmerskim prekidom se zove funkcija koja okreće zastavicu awakFlag, te izvršava glavnu funkciju.

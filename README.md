DEBUGING PCB NOTES:

IP5306 - nie nadaje się do tego zastosowania bo aktualne obciążenie jest za małe co IP5306 nie traktuje jako faktycznie obciążenie, które ma zasilić i nie wystawia output. Bateria bezpośrednio pod 5V_BUS --> utrata możliwości ładowania ogniwa

Tranzystory lepiej jakby były typu p i sterowane LOW. Bo teraz przy high np na pompkę będzie szło około 2,5V zamiast 5V

SWD jak z wcześniejszego taska ---> zastąpiono piny oscylatora, na sterowanie tranzystorów aby swd zostały

Podmieniono ścieżki (zmiana D i S)  ale zły Vth (Dla linii 3.3V jest dobrze jak połączyłem tranzystor zewnętrznymi przewodami)Zmiana połączenia tranzystora od sterowania +5V i trzeba to dodać do pompki jak na zdjęciu (nie wiem czemu 3.3 na G daje na wyjściu D 5V gdzie powinno być odwrotnie ale nie wnikam bo logika steruje poprawnie)

Ultrasonic sensor not working --> start measuring distance pin was set wrongly
--> probably 10us time on trig utrasonic pin timing is also wrong

@Stepper Motor@:
stepper motor to 5pin connector wrong trace


@SD@:
- zmieniono NSS na hardware output z disable ale nic to nie działa
- zmieniono NSS na zwykły GPIO (reset) tak jak wcześniej dla stm nucleo ale to też nic nie dało
- ŹLE PLIKI ZGRANE DO NOWEGO PROJEKTU? BO SIĘ RÓŻNIA OD TEGO CO BYŁ NA NUCLEO
- Zgrano pliki i zmieniono konfigurację na zużycie mniejszej ilości FLASH i RAM + jeszcze optymalizacja kodu na size i dalej gówno z zapisami na karte sd

-zmiana na optymalizacje na zero - nie buguje kodu ze nagle jest shoot ale dalej nie zapisuje

- Nucleo zapisuje na połączeniu do PCB
- Przy jednoczesnym zasilaniu PCB nie zapisuje (pewnie G0 z G4 zakłócają się nawzajem) ale przy połączeniu do modułu sd zapisywane są logi przy zasilonym PCB, który zasila moduł ale przypadkowo moduł też zasila G4

---> Błąd z długością plików ustawione na krótki format czyl i8.3 filenames a nazwa przekraczała limit 8 znaków
---> być może FATSD.c było w nim coś źle skopiowane ale ciężko teraz potwierdzić
---> POPRAWIENIE ZIMNEGO LUTU NA CS NAPRAWIŁO ZAPISY



------------
@WKUP@:
- czemu nie wybudza?

- wywalić diodę?
- doprowadzić zasilanie do mic?

--> wow doprowadzone 5V do nóżki mic i zwarłem diode (tak, żeby zamiast niej była ścieżka) i wtedy uderzenie bezpośrednio w mikrofon wybudza układ ---> trzeba sprawdzić kręcenie potencjometrem czy klaśnięcie wybudzi

--> kręcenie potencjometrem tak jakby nic nie zmieniało i nawet rezystancja się nie zmienia (dalej sprawdzać co jest) odłączono 5V od nóżki mic i też wybudza przy bezpośrednim uderzeniu --- chyba dioda po prostu blokowała przez jej spadek napięcia?

--> na G4 detektor też był zasilany na 5V i digital out szedł na WKUP i wszystko działało ale może na 3V3 zamiast 5V będzie lepiej


--> Widocznie na ścieżce płytki gdzieś musi być jakaś doga mniejszej rezystancji bo multimetr pokazuje około 1,4kOhm dla potencjometru w duch skrajnych pozycjach.
Przekręcanie potencjometru wpływa jednak na napięcie jake jakie pojawia się na - komparatora. 
-----> Przy progu napięcia ustawionym na około 2,5V wykrywanie jest lepsze: głośne klaśnięcie wybudza, mowa nie wybudza i nie widać przypadkowych wybudzeń. Więc najpewniej po prostu niepoprawne ustawienie progu + zwarcie diody

- sprawdzić czy potencjometr zmienia R --> NIE
- sprawdzić czy potencjometr zmienia V na próg komparatora --> TAK
- sprawdzić czy bez C24 wybudzanie będzie łatwiejsze --> NIE ZROBIONO

- sprawdzić bez zwarcia diody? --> NIE ZROBIONO
- inny mic? --> NIE ZROBIONO

-----------------
@ULTRASONIC@:
- czemu nie odczytuje?

-gnd źle polutowane ale poprawa połączenia i tak nie daje dobrych odczytów

- yhym więc jak zasilanie czujnika pod 3v3 to nagle działa teraz

--> Niby działa ale odczyty wyglądają niestabilnie i tak jakby nie odpowiadały realnej odległości

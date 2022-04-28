# Splachovací záchod

Tento repozitář obsahuje řešení úkolu pro přijímací řízení do společnosti Jablotron. Zadání je následující:

> Zpracování algoritmu a vytvoření programu pro splachovací záchod v libovolném programovacím jazyce. (s preferenci C,
C++)

# Řešení

Dané zadání je velice široké a plytké. Myslím si, že každý by se měl před další implementací čehokoliv doptat na 
alespoň několik základních otázek.

Proto jsem požádal o doplňující informace:

- Poběží software přímo na “záchodě”, nebo jde o software, který bude záchod ovládat na dálku?
    - Pokud na dálku:
        - Jaký se má použít komunikační (MQTT, REST, COM, UART) a transportní (UDP, TCP) protokol?
        - Má být komunikace mezi záchodem a ovládacím software zabezpečená? Například šifrováním.
        - Je komunikace mezi záchodem a serverem oboustranná, nebo jde pouze o odesílání příkazů na záchod?
    - Pokud lokálně:
        - Jaká omezení má MCU? Paměť RAM, EEPROM, FLASH, počet jader, dynamická alokace (pokud bude dávat smysl použít)
        a další, ...
- Jaké jsou vstupní a výstupní periferie?
    - Předpokládám, že půjde minimálně o splachovací tlačítko (vstup) a ventil (výstup). 
        - Je tlačítko hardwarově debouncované?
        - Jde o klasické tlačítko, které přerušuje okruh, nebo nějaké “chytřejší” - např. s kontrolou úrovně 
        “zmáčknutí”?
            - Pokud jde o chytré tlačítko, jakým způsobem komunikuje s hlavní jednotkou? Analogová hodnota, I2C, UART,
            BT, ...
        - Je tlačítko rozděleno na “velké” a “malé” spláchnutí?
        - Je možné ventil pouze otevřít/zavřít, nebo má více úrovní otevření?
        - Jakým způsobem ventil komunikuje s hlavní jednotkou?
    - Je v zásobníku splachovací vody plovák detekující “naplnění”?
        - Jakým způsobem plovák komunikuje?
        - Jaký je rozsah přijímaných dat (horní/spodní limit)?
        - Má se reagovat na úroveň naplnění? Například při stisku splachovacího tlačítka během napouštění si pamatovat,
        že se má provést spláchnutí a po naplnění nádoby spláchnutí provést dodatečně? Nebo se splachuje ihned po
        stisku tlačítka bez ohledu na stav naplnění?
        - Má se úroveň naplnění někam reportovat? Blikání diody, display, hlasové hlášení, …
        - Má se uzavřít přívodový ventil po naplnění zásobníku, nebo je tato funkcionalita řešena “hardwarově” jako u
        standardních záchodů?
- Komunikuje záchod se svým okolím?
    - Odesílají se data na server?
        - Pokud ano, jak, kdy a jaká data?
        - Má do běhu algoritmu zasahovat externí zařízení (server)?
            - Pokud ano, jak?
- Jak má záchod fungovat?
    - Pouze otevřít výstupní ventil po stisku tlačítka?
    - Má se využít stavový automat? Pokud ano, jaké jsou stavy, jejich spouštěče a přechodové podmínky?
    - Má jít o separátní task (na MCU poběží další software), nebo jde o jediný software běžící na zařízení?
        - Pokud task, jaké RTOS se má použít?
        - Pokud jediný software, o jak se jmenuje vstupní funkce volaná po startu zařízení?
- Jak má vypadat kód?
    - Máte definované naming conventions? Formát kódu?
    - Má jít o OOP implementaci?
    - Má se použít event dispatch/event loop, polling na hodnotu vstupního tlačítka, interrupty, …?

Dostal jsem následující odpověď:

> Dobrý den,
>
> je to tak, ze dáváme úplně volnou ruku a pak se podíváme, jak jste pri zpracování úkolu premyslel. Čili představivosti
se meze nekladou :-) U nás to funguje tak, ze máme zadání a postup a řešení už je na nás jako týmu. Když to nebude
zákazníkovi vyhovovat, zkusíme to jinak. Vím, ze zadání je hodně obecne, ale zcela koreluje s tím, jak pracujeme.
Zkuste to, prosím, jak si myslíte, ze byste to vyřešil nejlíp, a pak se o tom budeme na schůzce bavit.

Což by šlo pochopit i tak, že v Jablotronu se nedoptávají zákazníka, pokud jim na stole přistane špatné, reps. 
nedostatečné zadání. Místo toho "něco" napíší a pak zkusí, co zákazník na to...

# Návrh řešení

Pro návrh vlastního řešení budu vycházet z toho, jak funguje klasický záchod:

![Klasický záchod](https://web.mit.edu/2.972/www/reports/toilet/Image13.gif)

Bude třeba nahradit následující komponenty:

- tlačítko otevírající výstupní ventil
- výstupní ventil
- plovák ovládající plnění záchodu

Navrhuji následující řešení:

- Software beží lokálně na MCU umístěném na chytrém záchodu. 
- Jednotka nekomunikuje se serverem.
- Při řešení není uvažováno omezení pamětí ani výkonem. 
- Na jednotce neběží RTOS a jde o jediný software, který na jednotce poběží.
- Výrobce MCU dodává HAL knihovnou.
- Jednotka má následující periferie:
    - **tlačítko plného spláchnutí** - HW debouncované tlačítko
    - **tlačítko polovičního spláchnutí** - HW debouncované tlačítko
    - **výška vodní hladiny** - měřák naplnění nádoby s vodou
    - **výstupní ventil** - výpusťový ventil
    - **vstupní ventil** - ventil umístěný na přívodu vody do záchodu

## Periferie

### Tlačítka

Na jednotku jsou připojena dvě tlačítka. Obě dvě jsou hardwarově debouncované, tím pádem není třeba ošetřovat rychlé
změny stisku tlačítka.

**Tlačítko plného spláchnutí** slouží k vyprázdnění celého zásobníku s vodou. Stav vyprázdnění je kontrolován vůči výšce
vodní hladiny určené pomocí k tomu určeného senzoru. Po vyprázdnění celého zásobníku je uzavřen výstupní ventil a
otevřen ventil vstupní.

Jestliže je tlačítko stisknuto během naplňování zásobníku, je uzavřen přívodový ventil a otevřen ventil výstupní. Poté
se opakuje postup popsaný výše. 

**Tlačítko polovičního spláchnutí** slouží pro vyprázdnění pouze poloviny zásobníku s vodou. Stav vyprázdnění je 
kontrolován vůči výšce vodní hladiny určené pomocí k tomu určeného senzoru. Po vyprázdnění poloviny zásobníku dojde
k uzavření výstupního ventilu a otevření ventilu vstupního.

Pokud je tlačítko stisknuto během naplňování, je je uzavřen přívodový ventil a otevřen ventil výstupní. Poté
se opakuje postup popsaný výše. 

### Ventily

**Vstupní ventil** je elektrický ventil, který se otevře/zavře po pulzu delším než 30 ms na vstupním pinu. Pro svou 
funkci potřebuje vodu pod tlakem. To je vhodné, protože přívodní hadice je pod tlakem. 

Ventil slouží pro ovládání plnění nádoby s vodou. Jde o elektrickou alternativu ke klasickému plováku v běžných
záchodech.

**Výstupní ventil** je elektromagnetický ventil, který se otevře pouze pokud je pod napětím. V opačném případě je
ventil uzavřený. Nepotřebuje vodu pod tlakem. Zvolen byl z toho důvodu, že tlak vody v nádobce by nemusel být dostatečný
k otevření. Jde o vysokoprůtokové řešení, které simuluje zvednutí západky u klasického záchodu.

### Výška vodní hladiny

Jde o klasické odporové měření výšky vodní hladiny. Čím výše je vodní hladina, tím vyšší hodnotu lze ze senzoru vyčíst.

## Algoritmus

Algoritmus je jednoduchý - kdykoliv se stiskne tlačítko plného nebo částečného spláchnutí, zavře se vstupní ventil. Poté
se provede kontrola aktuální výšky vodní hladiny. Následně je otevřen výstupní ventil. Periodicky je kontrolována výška
vodní hladiny a čeká se na některý z následujících stavů:

- Bylo stisknuto tlačítko plného spláchnutí a nádoba s vodou je prázdná.
- Bylo stisknuto tlačítko částečného spláchnutí. Nádoba s vodou byla v době stisku plná a nyní je poloprázdná.
- Bylo stisknuto tlačítko částečného spláchnutí. Nádoba s vodou nebyla v době stisku plná a nyní je prázdná.

Poté je uzavřen výstupní ventil. Je otevřen ventil vstupní. Čeká se na naplnění nádobky s vodou. Naplňování však může
být kdykoliv přerušeno stiskem splachovacích tlačítek.

### Stavový automat

Pro záznam jednotlivých stavů jednotky je použit následující stavový automat. 

![UML diagram](http://www.plantuml.com/plantuml/proxy?cache=no&src=https://raw.githubusercontent.com/malja/hw_jablotron/master/diagram.iuml)

# Spuštění a kompilace

Velká část kódu je reprezentována HAL knihovnou, která je pouze boilerplate pro imaginární MCU. Z toho důvodu nejde
kód kompilovat a potažmo spustit.

Kód lze tedy chápat spíše jako pseudoód s algoritmem ovládání záchodu, jehož syntaxe se podobá C-čku.

# Návrhy na zlepšení

- Implementace generování souboru "config.h" z KConfig.
- Implementace HAL funkcí, aby bylo možné kód spustit a debugovat.
- Vytvoření Makefile pro sestavení.
- Vygenerování dokumentace.
- Odesílání LOG zpráv na server pro vyhodncoení.
- Rozšíření o možnost OTA, debug výstupu (dioda?) atd.

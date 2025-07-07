## SPECIFIKACIJA SISTEMA

### 1. KLIJENTSKA APLIKACIJA (Konzolna)

#### 1.1 Opis rada aplikacije:
  1. Aplikacija se konektuje na server i ceka odgovor servera (Dio ucitavanja)
  2. Nakon sto dobije odgovor servera aplikacija je povezana (Izlisttati osnovni meni)
  3. Nakon ovoga Klijentska aplikacija moze psolati zahtjeve iz osnovnog menija (login, Register ...)
  4. Nakon odgovora da je regiestracija uspjela, ili da su login podaci dobri server vraca potvrdu i podatke 
        korisnika na ekranu prikazati podatke korisnika i ispod toga korisnicki meni,
        ako ipak login podaci nisu uredu ili je doso do greske pri registraciji server vraca tip greske i treba ga ispisati
  5. Omogucit korisniku da ka serveru posalje skup trazenih koamndi a kroz intuitivan korinicki meni za pretragu
        u opciji pretraga korisniku se nudi da unese filtere pretrage
         - prikazi sve
         - ili da unosi filter polja i uzavisnosti koja filter polaj korisnik ostavi prazna ili ih ipak popuni salje neku od koamda
            primer: Ako su sva filter polja prazna searchAll, ako su neka ili samo jedno polje puno salje koamndu Search
  6. Po slanju komande server vraca sva vozila koja zadovoljavaju filtere pretrafge i potrebno ih je ispisati tabelarno
  7. Nakon ispisa podataka korisniku je potrebno ponuditi da obsvi novu pretragu
        Rezervise vozilo 
        vrati se u glavni meni
  8. Korisnik ima opciju da zatrazi pregled svih iznajmlejnih vozila 
        Gdje server ponovo vraca spisak i podatke svih vozila a na klie ntskoj strani se ispisuju tabelarno
        - Omoguciti UndoReserve komadu koja vraca vec rezervisano vozilo po IDu
  9. Kreirati opciju za Rezervaciju vozila po IDu 
        Ako je rezervacija moguca auto se dodaje korisniku i vraca se koamnda potvrde
        U slucaju greske vraca se kod greske
        Na osnovu toga dati ispis
  10. Nakon sto posaljes odjavnu komandu i dobijes povratnu koamndu 
        vrati se na osnovni meni i ocisti sesiju

 ### Za sve zahtjeve za prikaz vozila iz baze se prvo salje komanda da je akcija uspjeno obavljena
    A zatim stize tabela podatak kao posebna koamnda od servera

Osnovni meni:
    -Registruj se
    -Uloguj se
    -O aplikaciji
    -Izlaz

Korisnicki meni:
    [Podaci korisnika] // Ime Prezime KOrisnicko ime 
    -Pretraga
    -Provjera statusa
    -Rezervisanje
    -LogOut

- Omsilsiliti izgled Pretraga i Rezervisanje, Provjera Statusa djelova


### 2. SERVERSKA APLIKACIJA

#### 2.1 Opis rada aplikacije:
  1. Server se pokreće i inicijalizuje bazu podataka vozila i korisnika
  2. Server sluša na određenom portu i čeka konekcije klijenata
  3. Za svaku novu konekciju klijenta server kreira novu sesiju
  4. Server prima i obrađuje komande od klijenta i šalje odgovarajuće odgovore
  5. Server upravlja bazom podataka korisnika i vozila
  6. Server validira korisničke privilegije pre izvršavanja komandi
  7. Server održava sesije korisnika i prati rezervacije
  8. Server loguje sve aktivnosti za potrebe debugiranja i monitoring-a

#### 2.2 Struktura podataka:

**Korisnik:**
- ID korisnika
- Korisničko ime
- Šifra 
- Ime
- Prezime


**Vozilo:**
- ID vozila
- Proizvođač
- Model
- Godina proizvodnje
- Boja
- Registarske tablice
- Status (dostupno/rezervisano)
- ID korisnika koji je rezervisao (ako je rezervisano)


#### Skup komandi komunikacionog protokola ########

Sintaksa komande # KOMANDA ATRBUTI #
    - # pocetak i kraj komande start i end 
    - kod imena i atributa sa vrednostima pokriti [u zagradam se nalaze podaci ime i slicno da bih se podrzali razmaci]

--Login
    od klijenta:
        # LOGIN [user_name] [password] #
    od servera:
        # LOGIN SUCCES {PODACI KORISNIKA} #
        # LOGIN ERROR CODE_ERROR #
    kodovi greske:
        101 - Pogresna lozinka
        102 - Nepostojeci korisnik (lose korisnicko ime)

-- Registration
    od klijenta:
        # REGISTRATION [name] [surname] [username] [password] #
    od servera:
        # REGISTRATION SUCCES ID #
        # REGISTRATION ERROR CODE #
            -kodovi greske:
                201 - korisnik vec postoji
                202 - zauzeto korisnicko ime 

-- Logout
    od klijenta:
        # LOGOUT USER_ID #
    od servera:
        # LOGOUT SUCCES #
        # LOGOUT ERROR #

-! Za pretragu ce se zbog standardizacije kristiti samo jedna komanda kod parametara
     ako se ne koristi neki filter salti null za njeaga
-- Search
    od klijenta:
        # SEARCH [ID]/null [manufacturer]/null [carName]/null [year]/null #
    od servera:
        # SEARCH ERROR #
        # SEARCH SUCCES # -- nakon ove komande stize posebnom komandom skup trazenih podataka

-- Prikaz podataka 
    od servera:
        # TABELA [NumberElements] {[ID] [Proizvodjac] [Naziv] [Godina] [Boja] [Tablice] [Zauzeto]} #
            --! broj elemenata se salje da bih se olaksao postupak citaja podatak
                podaci jednog vozial u viticastim zagradam izmedju klasicno odvojeni razmakom 
                zatim razmak pa nova seriaj podataka u viticastim zagradama 
            --! komanda se koristi svaki put kada se sa server dostavlaju podaci o vozilima

-- CheckStatus
    - od klijenta:
        # CHECKSTATUS USER_ID #
    - od servera:
        # CHECKSTATUS SUCESS # -- nakon toga dolaze podaci preko TABELA
        # CHECKSTATUS ERROR # -- naknadno definisati moguce greske

-- Reserve
    - od klijenta:
        # RESERVE USER_ID CAR_ID #
    - od servera:
        # RESERVE SUCCES #
        # RESERVE ERROR CODE_ERR #
            - KOdovi greske:
                301 - Vozilo ne postoji
                302 - Vozilo je zauzeto
                303 - Vec ste rezervisali vozilo
                
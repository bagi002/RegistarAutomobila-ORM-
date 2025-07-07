# RegistarAutomobila-ORM-
Projekat iz predmeta osnovni racunarskih mreza 1

Elektronski registar za evidenciju izdatih automobila

Realizovati primer namenskog sistema elektronskog registra za evidenciju izdatih automobila pomoću TCP protokola. Primer se sastoji iz klijentske strane (korisnik) i serverske strane (elektronski registar). Klijent preko tastature može da zadaje sledeće komande
 koje se šalju serveru:
    Login – Prijava/Registracija korisnika (npr. putem korisničkog imena ili broja članske kartice i korisničke šifre).
    Logout – Odjava korisnika.
    Search – Pretraga dostupnih automobila u elektronskom registru.
    SearchAll - Pretraga svih automobila u elektronskom registru.
    Search [id:ID] [manufacturer:MANUFACTURER] [carname:CARNAME] [year:YEAR] – Pretraga za zadate kriterijume (sve ili pojedinačno navedene).
    CheckStatus - Provera rezervisanih automobila prijavljenog korisnika.
    Reserve[id:ID] – Rezervacija automobila od strane prijavljenog korisnika po ID-u automobila.


Server na date komande šalje odgovarajuće odgovore koji se prikazuju na ekranu klijentske aplikacije.


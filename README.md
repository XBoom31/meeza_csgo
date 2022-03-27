# meeza_csgo

1. Rularea programului
Programul se poate rula prin injectarea fisierului binar (dll) in procesul ‘csgo.exe’.

Se poate folosi un dll injector sau loader-ul specific.

Odata injectat in proces, meniul poate fi activat prin apasarea tastei INSERT.

Programul poate fi eliminat din joc prin apasarea tastei DELETE.

![image](https://user-images.githubusercontent.com/27855611/160269300-2275efd2-35fa-4005-b1ae-cdc127d293a2.png)
![image](https://user-images.githubusercontent.com/27855611/160269417-9b649fd5-5cf8-441a-bb08-03186495e138.png)


2. Functiile programului
    
    • Legitbot ( asistenta pentru tintire care incerca sa para umana si naturala )
    
    Legitbot master switch – activeaza legitbot-ul
    
    Legitbot mode – Selecteaza modul de operare a legitbot-ului
    
      ▪ Simple – legitbot-ul foloseste o singura configuratie pentru toate armele
    
      ▪ Normal – nu este implementat
    
      ▪ Advanced – legitbot-ul foloseste o configuratie diferita pentru fiecare arma
	Configuratie legitbot pentru o arma:
	
  Active – activeaza legitbot-ul pentru arma
	
  Silent – tinta se va teleporta la punctul dorit fara ca utilizatorul sa vada acest lucru
	
  Field of view – raza de actiune a legitbot-ului (distanta intre tinta si inamic)
	
  RCS(Recoil Control System) Strenght – Controleaza recoil-ul armei pentru utilizator
	
  Humanize Strenght – Induce zgomot si variaza viteza legitbot-ului pentru a evita detectarea aimbot-ului de catre sistemele de anti-cheat ale jocului.
	
  Smoothing (sau viteza de tintire) – Nivelul de fluiditate cu care este mutata tinta in punctul dorit ( cu cat mai mica este aceasta valoare, cu atat mai repede va sari tinta in punctul dorit)
	
  Backtrack – Teleporteaza inamicul inapoi in timp cu maxim 14 game tick-uri (200ms) daca acesta s-ar afla atunci in tinta utilizatorului. Practic teleporteaza inamicul in tinta ta. Aceasta functie exploiteaza lag compensation-ul implementat in joc.
	
  Triggerbot – Trage automat atunci cand inamicul se afla in tinta utilizatorului
	
  Triggerbot key – Selecteaza tasta pe care trebuie sa o ti apasata ca triggerbot-ul sa functioneze
	
  One tap mode – Daca inamicul ar putea fi ucis cu un singur glont si inamicul se afla in raza de actiune aleasa, atunci acest mod va preveni utilizatorul din a trage, pana cand tinta ajunge la punctul dorit. Acest mod va duce tinta intr-un mod fin spre inamic, iar cand distanta intre tinta si inamic devine mai mica decat valoare din ‘one tap mode snap’ tinta se va teleporta in locul dorit si legitbot-ul va trage.
	
  One tap mode safe – Va trage atunci doar atunci cand tinta ajunge in punctul dorit folosindu-se de triggerbot
	
  One tap mode snap – Dintanta dintre tinta si inamic in care tinta se va teleporta in punctul dorit.
  
     Ragebot (tintire automata care nu incearca sa para naturala, se foloseste pentru lupta cu un alt utilizator care foloseste un program asemanator)
	  
    Active – Activeaza ragebot-ul
	  
    Field of view – Raza de actiune a ragebot-ului ( distanta intre tinta si inamic)
	  
    Autowall – Tragerea gloantelor in inamic prin pereti atunci cand este posibil
	  
    Autoshoot – Tragerea automata a gloantelor atunci cand inamic indeplineste conditiile setate
	  
    No recoil – Combate recoil-ul armei
	  
    Silent – Nu misca tinta pe care o vede utilizatorul atunci cand ragebot-ul trage
	
    Hitscan – Gaseste puctul in care se ragebot-ul va produce cele mai mari daune posibile inamicului cu un singur glont.
	  
    Resolver – Gaseste pozitia reala a capului inamicului
	  
    Minimum damage – Daunele minime pentru care ragebot-ul va trage in inamic
	  
    Hitchance – Procentajul pentru care ragebot-ul va nimeri inamicul, iar daca este prea mic, ragebot-ul nu va trage
	  
    Anti aim – Pacaleste inamicii cu astfel de programe, ascunzand pozitia reala a capului utilizatorului.
	  
    Backup anti aim – Plaseaza pozitia reala a utilizatorului intr-o pozitie aleasa de utilizator
	  
    Backup anti aim switch – Activeaza backup anti aim
	  
    Auto revolver – Specific pentru arma revolver, o antreneaza pentru a putea fi trasa mai repede decat in mod normal
	  
    Auto stop – Opreste utilizatorul din a se misca pana cand Hitchance-ul atinge valoarea minima pentru a trage

    • Visuals (ajutor vizual pe care programul il ofera utilizatorului)
    
    ◦ Pentru jucatori
	  
    Box – deseneaza o cutie in jurul jucatorului
	  
    Name – Afiseaza numele deasupra jucatorului
	  
    Healthbar – Deseneaza o bara care informeaza utilizatorul despre nivelul de viata al altor jucatori
	  
    Armourbar – Deseneaza o bara care informeaza utilizatorul despre nivelul de armura al altor jucatori
	  
    Skeleton – Deseneaza scheletul jucatorului peste corpul sau
	  
    Weapon – Afiseaza arma jucatorului
	  
    Ammo – Afiseaza munitia jucatorului pentru arma activa
	
    Snaplines – Traseaza o linie de la centrul de jos al ecranului la jucator
	
    Information – Afiseaza informatii despre utilizator ( daca are casca, bomba, daca este orbit de un flashbang, daca sta jos, daca foloseste luneta, daca pozitia capului este falsa, daca reincarca arma, daca pozitia capului este falsa cu mai mult de 60 de grade decat cea reala)
	  
    Dead only – Activeaza vizualele doar cand utilizatorul este decedat.
	  
    Enemies only – Activeaza vizualele doar pentru inamici
	  
    Chams – Coloreaza jucatorii pentru a putea fi vazuti mai clar si mai usor
	  
    Chams flat – Coloreaza jucatorii cu o singura culoare, fara umbre pe modelul jucatorului
	  
    Chams xqz – Coloreaza jucatorii si atunci cand nu sunt vizibili
	  
    Chams enemies only – Coloreaza doar inamicii
	  
    Glow – Contureaza jucatorul cu o stralucire
	
    Glow enemies only – Contureaza doar inamicii
    
      ◦ Pentru lume
      
    Weapon esp – Deseneaza o cutie cu numele si munitia unei arme care este pe jos
		
    Weapon glow – Contrueaza armele care se gasesc pe jos
		
    Bomb esp – Deseneaza o cutie in jurul bombei care se afla pe jos
		
    Bomb glow – Contureaza bomba
		
    Defuse kit esp – Deseneaza o cutie in jurul kit-ului de dezamorsare 
		
    Defuse kit glow – Contureaza kit-ul de dezamorsare
		
    Nightmode – Transforma orice harta in noapte si schimba cerul
		
    Update – Aplica nightmode-ul
		
    No smoke – Elimina grenadele cu emisie de fum ( vezi prin fum)
    
    No flash – Nu poti fi orbit de grenade
		
    No post processing – Pentru o vizibilitate mai buna a inamicilor si o performanta mai buna a jocului
		
    Fields of view – Schimba field of view-ul camerei peste limitele admise de joc	
	
    Viewmodel field of view – Schimba field of view-ul al bratelor si ale armei care este vizibila utilizatorului

    • Misc
    
	  Anti report – previne alti utilizatori din a te reporta pentru folosirea asfel de programe
    
	  Thirdperson – Vizualizeaza jucatorul din persoana a 3-a
    
	  Thirdperson key – Selectezi tasta pe care trebuie sa o apesi pentru a activa persoana a 3-a
    
	  Bhop – Sare automat exact cand utilizatorul atinge iar pamantul pentru a se folosi de un bug al jocului pentru ca utilizatorul sa se deplaseze mai rapid decat normal
	  
    Autostrafe – Accelereaza viteza automat atunci cand bhop-ul este activat
	  
    Hitmarker – Atentioneaza utilizatorul ca legitbot-ul sau ragebot-ul a nimerit un inamic
	  
    Fakelag – Sacadeaza miscarea utilizatorului fata de ceilalti jucatori pentru a fi mai  greu de tintit
	  
    AA lines – Afiseaza pozitile reale si false ale capului utilizatorului

    • Settings
	  
    Selecteaza configuratia programului, o salveaza sau o incarca
	  
    Selecteaza culorile pentru toate vizualele si a meniului

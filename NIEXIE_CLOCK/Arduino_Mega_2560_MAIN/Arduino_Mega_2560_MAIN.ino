#define A1 54  // Na pinie A0 jest wpięte A od drivera pierwszego dlatego A1
#define B1 55
#define C1 56
#define D1 57
#define A2 24
#define B2 26
#define C2 28
#define D2 30
#define A3 29
#define B3 27
#define C3 25
#define D3 23
#define A4 37
#define B4 39
#define C4 41
#define D4 43
#define A5 8
#define B5 9
#define C5 10
#define D5 11
#define A6 58
#define B6 59
#define C6 60
#define D6 61
#define COLN1 62
#define COLN2 63
#define buffsize 48
//#include <SoftwareSerial.h>
//SoftwareSerial softSerial(19, 18); // 19=RX, 18=TX
bool START = false;
bool odtruwanie = false;
bool mruganie = false;
char input[buffsize];
uint16_t data[6];  // jakie i ile  zmiennych odbieramy z UART
uint16_t s1 = data[5];
uint16_t s2 = data[4];
uint16_t m1 = data[3];
uint16_t m2 = data[2];
uint16_t g1 = data[1];
uint16_t g2 = data[0];
unsigned long aktualnyCzas = 0;
unsigned long zapamietanyCzas = 0;
unsigned long roznicaCzasu = 0;

unsigned long OstatniCzasOdOdtruwania = 0;
unsigned long roznicaZatruwania = 0;
unsigned long zapamietanyCzasOdtruwania = 0;

unsigned long aktualnyCzasZatrucieKatod = 0;
unsigned long zapamietanyCzasZatrucieKatod = 0;
unsigned long roznicaCzasuZatrucieKatod = 0;

unsigned long CzasMrugania = 0;
unsigned long roznicaMrugania = 0;
unsigned long zapamietanyCzasMrugania = 0;

unsigned long CzasMrugania2 = 0;
unsigned long roznicaMrugania2 = 0;
unsigned long zapamietanyCzasMrugania2 = 0;
byte index = 0;
boolean stringComplete = false;  // gdy cale polecenie/napis odebrany
int digits[10][4]{
  // D C B A
  { 0, 0, 0, 0 },  // 0
  { 0, 0, 0, 1 },  // 1
  { 0, 0, 1, 0 },  // 2
  { 0, 0, 1, 1 },  // 3
  { 0, 1, 0, 0 },  // 4
  { 0, 1, 0, 1 },  // 5
  { 0, 1, 1, 0 },  // 6
  { 0, 1, 1, 1 },  // 7
  { 1, 0, 0, 0 },  // 8
  { 1, 0, 0, 1 }   // 9
};

// %%%%%%%%%%%%%%%%%%PROGRAM%%%%%%%%%%%%%%%%%%%%
void setup() {
  Serial.begin(115200);
  // NIXIE 1
  pinMode(A1, OUTPUT);
  pinMode(B1, OUTPUT);
  pinMode(C1, OUTPUT);
  pinMode(D1, OUTPUT);
  // NIXIE 2
  pinMode(A2, OUTPUT);
  pinMode(B2, OUTPUT);
  pinMode(C2, OUTPUT);
  pinMode(D2, OUTPUT);
  // NIXIE 3
  pinMode(A3, OUTPUT);
  pinMode(B3, OUTPUT);
  pinMode(C3, OUTPUT);
  pinMode(D3, OUTPUT);
  // NIXIE 4
  pinMode(A4, OUTPUT);
  pinMode(B4, OUTPUT);
  pinMode(C4, OUTPUT);
  pinMode(D4, OUTPUT);
  // NIXIE 5
  pinMode(A5, OUTPUT);
  pinMode(B5, OUTPUT);
  pinMode(C5, OUTPUT);
  pinMode(D5, OUTPUT);
  // NIXIE 6
  pinMode(A6, OUTPUT);
  pinMode(B6, OUTPUT);
  pinMode(C6, OUTPUT);
  pinMode(D6, OUTPUT);
  // COLN1
  pinMode(COLN1, OUTPUT);
  digitalWrite(COLN1, HIGH);
  // COLN2
  pinMode(COLN2, OUTPUT);
  digitalWrite(COLN2, HIGH);  // turn the LED on (HIGH is the voltage level)               
}

void parsujpolecenia() {
  //polecenie ma wygladac tak: cmd=zmienna1,zmienna2,zmienna3,zmienna4  z zalozenia sa to 4 liczby calkowite
  // do 5 cyfr, dlatego buffsize32 ma zapas, dla innych trzeba zmodyfikowac bufor, tablice zmiennych, funkcje
  // do konwersji napisu na liczby
  //cmd=234,342,553,3432

  uint8_t index = 0;
  char* polecenie = input;
  Serial.print("Otrzymane polecenie: ");
  Serial.println(polecenie);

  char* command1 = strtok(polecenie, "=");  //sprawdzenie czy czesc przed znakiem = jest rowna czas


  if (strcmp(command1, "czas") == 0) {
    while (command1 != 0) {
      command1 = strtok(NULL, ",");  //dzielimy reszte napisu w miejscach gdzie jest ","
      // Find the next command in input string
      if (index < 6) data[index] = atoi(command1);  //konwersja napisu na INT i zapisanie do kolejnej pozycji w tablicy
      index++;
    }
  } else Serial.println("Polecenie nieprawidlowe");
  /*Serial.println("Aktualne parametry:");
 Serial.print("Zmienna 1 = ");
 Serial.println(g2);
 Serial.print("Zmienna 2 = ");
 Serial.println(g1);
 Serial.print("Zmienna 3 = ");
 Serial.println(m2);
 Serial.print("Zmienna 4 = ");
 Serial.println(m1);
 Serial.print("Zmienna 5 = ");
 Serial.println(s2);
 Serial.print("Zmienna 6 = ");
 Serial.println(s1);
 */
  stringComplete = false;
}

void serialEvent() {
  while (Serial.available() > 0) {
    uint16_t dane = Serial.read();
    if (dane == '\n') {
      // wykryto znak konca linii \n, koniec odbioru
      input[index] = 0;
      index = 0;
      stringComplete = true;
    } else {
      input[index] = dane;
      if (index < (buffsize - 1)) index++;  //jesli napis bedzie dluzszy niz bufor to nadmiar leci w kosz
      input[index] = '\0';                  // Na koncu napisu wstawiamy 0
    }
  }
}

void PobierzCzas() {
    s1 = data[5];
    s2 = data[4];
    m1 = data[3];
    m2 = data[2];
    g1 = data[1];
    g2 = data[0];
    START = true;
    mruganie = true;
}
void wyswietlajCzas() {
    
  if (START == true) {
    s1++;
    if (s1 == 10) {
      s1 = 0;
      s2++;
    }
    if (s2 == 6) {
      s2 = 0;
      m1++;
    }
    if (m1 == 10) {
      m1 = 0;
      m2++;
    }
    if (m2 == 6) {
      m2 = 0;
      g1++;
    }
    if (g1 == 10) {
      g1 = 0;
      g2++;
    }
    if (g2 == 2 && g1 == 4) {
      g2 = 0;
      g1 = 0;
    }
    digitalWrite(A1, digits[s1][3]);
    digitalWrite(B1, digits[s1][2]);
    digitalWrite(C1, digits[s1][1]);
    digitalWrite(D1, digits[s1][0]);

    digitalWrite(A2, digits[s2][3]);
    digitalWrite(B2, digits[s2][2]);
    digitalWrite(C2, digits[s2][1]);
    digitalWrite(D2, digits[s2][0]);

    digitalWrite(A3, digits[m1][3]);
    digitalWrite(B3, digits[m1][2]);
    digitalWrite(C3, digits[m1][1]);
    digitalWrite(D3, digits[m1][0]);


    digitalWrite(A4, digits[m2][3]);
    digitalWrite(B4, digits[m2][2]);
    digitalWrite(C4, digits[m2][1]);
    digitalWrite(D4, digits[m2][0]);

    digitalWrite(A5, digits[g1][3]);
    digitalWrite(B5, digits[g1][2]);
    digitalWrite(C5, digits[g1][1]);
    digitalWrite(D5, digits[g1][0]);

    digitalWrite(A6, digits[g2][3]);
    digitalWrite(B6, digits[g2][2]);
    digitalWrite(C6, digits[g2][1]);
    digitalWrite(D6, digits[g2][0]);

    Serial.println("Aktualna godzina1: ");
    Serial.print(g2);
    Serial.print(g1);
    Serial.print(":");
    Serial.print(m2);
    Serial.print(m1);
    Serial.print(":");
    Serial.print(s2);
    Serial.println(s1);
  }
  else {
    Serial.println("Aktualna godzina2: ");
    Serial.print(g2);
    Serial.print(g1);
    Serial.print(":");
    Serial.print(m2);
    Serial.print(m1);
    Serial.print(":");
    Serial.print(s2);
    Serial.println(s1);
  }
}

void ZatrucieKatod() {
  Serial.println("Proces odtruwania: ");
  for (int i = 0; i < 11;) {
    aktualnyCzasZatrucieKatod = millis();
    roznicaCzasuZatrucieKatod = aktualnyCzasZatrucieKatod - zapamietanyCzasZatrucieKatod;
    //Jeśli różnica wynosi ponad sekundę
    if (roznicaCzasuZatrucieKatod >= 80UL) {
      //Zapamietaj aktualny czas
      zapamietanyCzasZatrucieKatod = aktualnyCzasZatrucieKatod;
      digitalWrite(A1, digits[i][3]);
      digitalWrite(B1, digits[i][2]);
      digitalWrite(C1, digits[i][1]);
      digitalWrite(D1, digits[i][0]);

      digitalWrite(A2, digits[i][3]);
      digitalWrite(B2, digits[i][2]);
      digitalWrite(C2, digits[i][1]);
      digitalWrite(D2, digits[i][0]);

      digitalWrite(A3, digits[i][3]);
      digitalWrite(B3, digits[i][2]);
      digitalWrite(C3, digits[i][1]);
      digitalWrite(D3, digits[i][0]);

      digitalWrite(A4, digits[i][3]);
      digitalWrite(B4, digits[i][2]);
      digitalWrite(C4, digits[i][1]);
      digitalWrite(D4, digits[i][0]);

      digitalWrite(A5, digits[i][3]);
      digitalWrite(B5, digits[i][2]);
      digitalWrite(C5, digits[i][1]);
      digitalWrite(D5, digits[i][0]);

      digitalWrite(A6, digits[i][3]);
      digitalWrite(B6, digits[i][2]);
      digitalWrite(C6, digits[i][1]);
      digitalWrite(D6, digits[i][0]);
      i++;
    }
  }
}

void COLN(){               
  CzasMrugania = millis();
  roznicaMrugania = CzasMrugania - zapamietanyCzasMrugania;

  CzasMrugania2 = millis();
  roznicaMrugania2 = CzasMrugania2 - zapamietanyCzasMrugania2;
  //Jeśli różnica wynosi ponad sekundę
  if (roznicaMrugania >= 500UL) {
    //Zapamietaj aktualny czas
    zapamietanyCzasMrugania = CzasMrugania;
    digitalWrite(COLN1, LOW);  // turn the LED on (HIGH is the voltage level)             
    digitalWrite(COLN2, LOW);   // turn the LED off by making the voltage LOW 
}
 if (roznicaMrugania2 >= 1000UL){
    zapamietanyCzasMrugania2 = CzasMrugania2;    
    digitalWrite(COLN1, HIGH);  // turn the LED on (HIGH is the voltage level)             
    digitalWrite(COLN2, HIGH);   // turn the LED off by making the voltage LOW  
}
}

void loop() {
  if(mruganie==true){
    COLN();    
  }  
  //Pobierz liczbe milisekund od startu
  aktualnyCzas = millis();
  OstatniCzasOdOdtruwania = millis();
  roznicaCzasu = aktualnyCzas - zapamietanyCzas;
  roznicaZatruwania = OstatniCzasOdOdtruwania - zapamietanyCzasOdtruwania;
  //Jeśli różnica wynosi ponad sekundę
  if (roznicaCzasu >= 1000UL) {
    //Zapamietaj aktualny czas
    zapamietanyCzas = aktualnyCzas;
    wyswietlajCzas();
        
  }
  if (roznicaZatruwania >= 60000UL) {
    //Zapamietaj aktualny czas
    zapamietanyCzasOdtruwania = OstatniCzasOdOdtruwania;
    ZatrucieKatod();
  }
  if (stringComplete) parsujpolecenia();
  if (Serial.available() > 0) {
    // Teraz mozna cos zrobic ze zmiennymi
    PobierzCzas();
    Serial.print("Czas pobrany: ");
    Serial.print(g2);
    Serial.print(g1);
    Serial.print(":");
    Serial.print(m2);
    Serial.print(m1);
    Serial.print(":");
    Serial.print(s2);
    Serial.println(s1);
    
  }
}
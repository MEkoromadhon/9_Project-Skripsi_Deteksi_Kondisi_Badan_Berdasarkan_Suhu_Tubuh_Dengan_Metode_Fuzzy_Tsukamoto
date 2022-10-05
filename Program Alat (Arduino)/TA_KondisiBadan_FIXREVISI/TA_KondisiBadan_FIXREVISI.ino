///////////// PROGRAM DETEKSI KONDISI BADAN BERDASARKAN SUHU TUBUH DENGAN METODE FUZZY LOGIC TSUKAMOTO /////////////

#include <ESP8266WiFi.h>             // Library berfungsi untuk komunikasi wi-fi pada ESP8266
#include <LiquidCrystal_I2C.h>       // Library berfungsi untuk komunikasi LCD 16x2
#include <math.h>                    // Library berfungsi untuk operasi matematika (Fuzzy Logic Tsukamoto)
#include <Wire.h>                    // Library berfungsi untuk komunikasi serial 
#include <Adafruit_MLX90614.h>       // Library berfungsi untuk komunikasi sensor MLX90614 

//Konfigurasi WiFi (Hotspot)
const char *ssid = "Romad27";         // Nama Wi-Fi yang digunakan
const char *password = "27011997";    // Password Wi-Fi yang digunakan
const char *host = "192.168.43.240";  // IP Address Server yang terpasang XAMPP (Pada Laptop) Muhammad Eko Romadhon

Adafruit_MLX90614 mlx = Adafruit_MLX90614(); // Library berfungsi untuk komunikasi sensor MLX90614 

LiquidCrystal_I2C lcd(0x27,16,4);    // Mengatur alamat LCD 0x27 untuk tampilan 16 karakter dan 2 baris
int echoPin = 14;      // Inisialisasi Led Hijau pada pin GPIO14 (D5) - Kabel warna Hitam
int triggerPin = 12;   // Inisialisasi Led Hijau pada pin GPIO12 (D6) - Kabel warna Putih
int Buzzer = 13;       // Inisialisasi Buzzer pada pin GPIO13 (D7) - Kabel warna Kuning
int Led_Merah = 15;    // Inisialisasi Led Hijau pada pin GPIO15 (D8) - Kabel warna Merah
int Led_Hijau = 3;     // Inisialisasi Led Hijau pada pin GPIO3 (RX) - Kabel warna Hijau
int i;                 // Inisialisasi i untuk perulangan (Led Merah Berkedip)
                       // Pin SCL berada di pin GPIO5 (D1) - Kabel warna Ungu
                       // Pin SDA berada di pin GPIO4 (D2) - Kabel warna Abu-abu

float suhutubuh,suhu;                                       // Variabel float
String kondisibadan;                                        // Variabel String
float suhutubuh_dingin, suhutubuh_normal, suhutubuh_panas;  // Himpunan Linguistik pada variabel suhu tubuh
float Alpha_Predikat1, Alpha_Predikat2, Alpha_Predikat3;    // Variabel untuk hasil (Proses Defuzzyfikasi)
float minimal, maximal;                                     // Variabel untuk pemilihan nilai Defuzzyfikasi (Max)
float TargetFahrenheit;                                     // Variabel float

// ============================== MENGIDENTIFIKASI VARIABEL DAN HIMPUNAN ===================================
// 1. Variabel = Suhu Tubuh     - Himpunan Linguistik (Dingin (Hipotermia), Normal dan Panas) - Himpunan Numerik (36 s/d 38.5)
// 2. Variabel = Kondisi Badan  - Himpunan Linguistik (Sehat & Sakit)                         - Himpunan Numerik (36.5 & 38.5)

// =================================== BASIS PENGETAHUAN (BASIS RULE) ======================================
// [R1] = If Suhu Tubuh Dingin, Then Kondisi Badan Dingin
// [R2] = If Suhu Tubuh Normal, Then Kondisi Badan Normal
// [R3] = If Suhu Tubuh Panas, Then Kondisi Badan Panas

// Note : Kekurangan diangka 38 dihasil dinyatakan sama 0,5.

// ========================================== PROSES FUZZYFIKASI ===========================================
// ============================ Derajat Keanggotaan untuk Variabel Suhu Tubuh ==============================
float u_suhutubuh_dingin ()                               // Fungsi untuk Derajat Keanggotaan Dingin (36 s/d 36,5 diperoleh dari batas nilai suhu Dingin & batas nilai suhu Normal)
{
  if (suhutubuh <= 36)
    {
      suhutubuh_dingin = 1;
    }
  else if (suhutubuh >= 36 && suhutubuh <= 36.5)
    {
      suhutubuh_dingin = (36.5 - suhutubuh) / 0.5;       // Dibagi 0.5 karena 36,5 - 36 = 0,5
    }
  else if (suhutubuh >= 36.5)
    {
      suhutubuh_dingin = 0;
    }
}

float u_suhutubuh_normal ()                               // Fungsi untuk Derajat Keanggotaan Normal (36,5 s/d 37,5 diperoleh dari batas nilai suhu Dingin & batas nilai suhu Panas)
{
  if (suhutubuh <= 36)
    {
      suhutubuh_normal = 0;
    }
  else if (suhutubuh >= 36 && suhutubuh <= 36.5)
    {
      suhutubuh_normal = (suhutubuh - 36) / 0.5;          // Dibagi 0.5 karena 36,5 - 36 = 0,5
    }
  else if (suhutubuh >= 36.5 && suhutubuh <= 37.5)
    {
      suhutubuh_normal = 1;
    }
  else if (suhutubuh >= 37.5 && suhutubuh <= 38.5)
    {
      suhutubuh_normal = (38.5 - suhutubuh) / 1;          // Dibagi 1 karena 38,5 - 37,5 = 1
    }
  else if (suhutubuh >= 38.5)
    {
      suhutubuh_normal = 0;
    }
}

float u_suhutubuh_panas ()                                // Fungsi untuk Derajat Keanggotaan Panas (37,5 s/d 38,5 diperoleh dari batas nilai suhu Normal & batas nilai suhu Panas)
{
  if (suhutubuh <= 37.5)
    {
      suhutubuh_panas = 0;
    }
  else if (suhutubuh >= 37.5 && suhutubuh <= 38.5)
    {
      suhutubuh_panas = (suhutubuh - 37.5) / 1;           // Dibagi 1 karena 38,5 - 37,5 = 1
    }
  else if (suhutubuh >= 38.5)
    {
      suhutubuh_panas = 1;
    }
}

void setup() {
  mlx.begin();                         // Komunikasi atau setup untuk sensor MLX90614
  lcd.init();                          // Inisialisasi LCD
  lcd.backlight();                     // Menyalakan Cahaya pada LCD
  Serial.begin(9600);                  // Komunikasi dengan Serial Monitor dan kecepatan komunikasi (baudrate)
  pinMode (Led_Hijau, OUTPUT);         // Menjadikan Led Hijau sebagai OUTPUT
  pinMode (Led_Merah, OUTPUT);         // Menjadikan Led Merah sebagai OUTPUT
  pinMode (Buzzer, OUTPUT);            // Menjadikan Buzzer sebagai OUTPUT
  pinMode(echoPin, INPUT);             // Menjadikan Ultrasonik Echo sebagai INPUT
  pinMode(triggerPin, OUTPUT);         // Menjadikan Ultrasonik Trigger sebagai OUTPUT
       lcd.setCursor(0,0);             // Angka 0 = Kolom & angka 0 = baris atas (baris 1)
       lcd.print("    DETEKSI     ");  // Menampilkan kalimat Deteksi pada LCD
       lcd.setCursor(0,1);             // Angka 0 = Kolom & angka 1 = baris bawah (baris 2)
       lcd.print(" KONDISI BADAN  ");  // Menampilkan kalimat Kondisi Badan pada LCD

// =============================== Proses Membaca Koneksi WiFi yang dipakai ================================
// Proses membaca koneksi wi-fi dengan status pencarian berdasarkan nama wi-fi & kata sandi yang dilakukan secara berulang-ulang sampai alamat wi-fi yang dipakai benar.  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  Serial.print("Terkoneksi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
}

void loop() {
   long duration, jarak;
   digitalWrite(triggerPin, LOW);
   delayMicroseconds(2); 
   digitalWrite(triggerPin, HIGH);
   delayMicroseconds(10); 
   digitalWrite(triggerPin, LOW);
   duration = pulseIn(echoPin, HIGH);
   jarak = (duration/2) / 29.1;
   Serial.print(" Jarak : ");
   Serial.print(jarak);
   Serial.println(" cm");
   delay(2000);
   // Proses pembacaan jarak dan suhu tubuh beserta output fuzzyfikasi.
    if (jarak <= 3){
        lcd.clear();                    // Menghapus Layar LCD
        lcd.setCursor(0,0);             // Angka 0 = Kolom & angka 0 = baris atas (baris 1)
        lcd.print("    Proses...   ");  // Menampilkan kalimat Proses pada LCD
       Serial.println (" ");
       Serial.println ("------------------------- STUDI KASUS - HASIL MASUKAN SENSOR -------------------------");
       TargetFahrenheit = mlx.readAmbientTempF(); // Menyimpan nilai suhu tubuh dalam satuan Fahrenheit
       //suhutubuh = mlx.readObjectTempC();         // Menyimpan nilai suhu tubuh dalam satuan Celcius
          suhu = mlx.readObjectTempC();         // Program bagian untuk mengkalibrasikan sensor MLX90614
          suhutubuh = suhu + 3.5;
       Serial.print   (" Suhu Tubuh = ");
       Serial.print   (suhutubuh);
       Serial.println (" C");                     // Satuan Derajat Suhu
       Serial.println (" ");
       Serial.println ("---------------------------- HASIL PERHITUNGAN FUZZYFIKASI ---------------------------");
       // Proses output fuzzyfikasi berdasarkan fungsi derajat keanggotaan variabel suhu tubuh.
       Serial.print   (u_suhutubuh_dingin ());
       Serial.print   (" : Derajat Keanggotaan Suhu Tubuh ( Dingin ) = ");
       Serial.println (suhutubuh_dingin);
       Serial.print   (u_suhutubuh_normal ());
       Serial.print   (" : Derajat Keanggotaan Suhu Tubuh ( Normal ) = ");
       Serial.println (suhutubuh_normal);
       Serial.print   (u_suhutubuh_panas ());
       Serial.print   (" : Derajat Keanggotaan Suhu Tubuh ( Panas ) = ");
       Serial.println (suhutubuh_panas);
       Serial.println (" ");
       Serial.println ("----------------------------- PROSES INFERENSI / IMPLIKASI ---------------------------");
       Serial.println ("     Pada penelitian ini, karena tidak terdapat nilai z dari output,");
       Serial.println ("  maka inferensi dilakukan dengan menggunakan alpha predikat, yaitu sebagai berikut :");
       Serial.println ("  [R1] = If Suhu Tubuh Dingin, Then Kondisi Badan Dingin");
       Serial.print   ("         Alpha Predikat_1 = ");
       Serial.println (suhutubuh_dingin);
       Serial.println ("--------------------------------------------------------------------------------------");
       Serial.println ("  [R2] = If Suhu Tubuh Normal, Then Kondisi Badan Normal");
       Serial.print   ("         Alpha Predikat_2 = ");
       Serial.println (suhutubuh_normal);
       Serial.println ("--------------------------------------------------------------------------------------");
       Serial.println ("  [R3] = If Suhu Tubuh Panas, Then Kondisi Badan Panas");
       Serial.print   ("         Alpha Predikat_3 = ");
       Serial.println (suhutubuh_panas);  
       Serial.println ("--------------------------------- PROSES DEFUZZYFIKASI -------------------------------");
       Serial.println ("    Himpunan fuzzy yang dihasilkan dari komponen aturan tahapan fuzzy, ");
       Serial.println (" maka diperlukan pengambilan nilai tegas (crips) terbesar yang diambil sebagai output-nya.");
       Serial.print   (" Hasil Defuzzyfikasi =");
       // Proses output pengambilan nilai terbesar (max) dan penentuan kondisi badan
          Alpha_Predikat1 = suhutubuh_dingin;
          Alpha_Predikat2 = suhutubuh_normal;
          Alpha_Predikat3 = suhutubuh_panas;
        if(Alpha_Predikat1 >= Alpha_Predikat2 && Alpha_Predikat1 >= Alpha_Predikat3){
            maximal = Alpha_Predikat1;
            Serial.print (" Kondisi Dingin");
            kondisibadan = "Dingin";
        }
        else if(Alpha_Predikat2 >= Alpha_Predikat3){
            maximal = Alpha_Predikat2;
            Serial.print (" Kondisi Normal");
            kondisibadan = "Normal";
        }
        else{
            maximal = Alpha_Predikat3;
            Serial.print (" Kondisi Panas");
            kondisibadan = "Panas";
        }
        if(Alpha_Predikat1 <= Alpha_Predikat2 && Alpha_Predikat1 <= Alpha_Predikat3){
            minimal = Alpha_Predikat1;
        }
        else if(Alpha_Predikat2 <= Alpha_Predikat3){
            minimal = Alpha_Predikat2;
        }
        else{
            minimal = Alpha_Predikat3;
        }
       Serial.print   (" (dengan nilai = "); Serial.print  (maximal); Serial.println (")");  Serial.println (" ");
       Serial.println ("------------------------------- HASIL KONDISI BADAN ----------------------------------");
       Serial.print   (" Jadi, Hasil pengukuran Suhu tubuh pada angka ");
       Serial.print   (suhutubuh); Serial.println (" Celcius,");
       Serial.print   ("       yaitu termasuk ke dalam Kondisi Badan ");
      
       if (Alpha_Predikat1 && maximal){         // Jika Kondisi Badan Dingin dan Hasil Defuzzyfikasi, maka
           Serial.println ("Kondisi Dingin");   // Menampilkan Kondisi Badan pada Serial Monitor
             lcd.setCursor(0,0);                // Angka 0 = Kolom & angka 0 = baris atas (baris 1)
             lcd.print("SUHU    : ");           // Menampilkan kalimat pada LCD
             lcd.print(suhutubuh);              // Menampilkan hasil pengukuran suhu tubuh
             lcd.setCursor(0,1);                // Angka 0 = Kolom & angka 1 = baris bawah (baris 2)
             lcd.print("KONDISI : DINGIN");     // Menampilkan kalimat Kondisi DINGIN pada LCD
                digitalWrite (Led_Merah, LOW);  // Mematikan Led Merah (LOW = Tidak Memberi tegangan pada LED)
                kondisibadan = "DINGIN";        // Untuk mengirim data kalimat DINGIN ke DATABASE
           for (i=0; i<6; i=i+1){               // Led Hijau Berkedip sebanyak 6x
                digitalWrite (Led_Hijau, HIGH); // Menyalakan Led Hijau (HIGH = Memberi tegangan pada LED)
                   delay (150);                 // Memberi waktu tunda 0.15 detik
                digitalWrite (Led_Hijau, LOW);  // Mematikan Led Hijau (LOW = Tidak Memberi tegangan pada LED)
                   delay (150);                 // Memberi waktu tunda 0.15 detik
                digitalWrite (Led_Hijau, HIGH); // Menyalakan Led Hijau (HIGH = Memberi tegangan pada LED)
           }
                delay(7000);                    // Menampilkan Hasil Suhu Tubuh dan Kondisi Badan selama 7 detik pada LCD
                digitalWrite (Led_Hijau, LOW);  // Mematikan Led Hijau (LOW = Tidak Memberi tegangan pada LED)
                lcd.clear();                    // Menghapus Layar LCD
                lcd.setCursor(0,0);             // Angka 0 = Kolom & angka 0 = baris atas (baris 1)
                lcd.print("    DETEKSI     ");  // Menampilkan kalimat Deteksi pada LCD
                lcd.setCursor(0,1);             // Angka 0 = Kolom & angka 1 = baris bawah (baris 2)
                lcd.print(" KONDISI BADAN  ");  // Menampilkan kalimat Kondisi Badan pada LCD
       }
       else if (Alpha_Predikat2 && maximal){    // Jika Kondisi Badan Normal dan Hasil Defuzzyfikasi, maka
           Serial.println ("Kondisi Normal");   // Menampilkan Kondisi Badan pada Serial Monitor
             lcd.setCursor(0,0);                // Angka 0 = Kolom & angka 0 = baris atas (baris 1)
             lcd.print("SUHU    : ");           // Menampilkan kalimat pada LCD
             lcd.print(suhutubuh);              // Menampilkan hasil pengukuran suhu tubuh
             lcd.setCursor(0,1);                // Angka 0 = Kolom & angka 1 = baris bawah (baris 2)
             lcd.print("KONDISI : NORMAL");     // Menampilkan kalimat Kondisi NORMAL pada LCD
                digitalWrite (Led_Merah, LOW);  // Mematikan Led Merah (LOW = Tidak Memberi tegangan pada LED)
                kondisibadan = "NORMAL";        // Untuk mengirim data kalimat NORMAL ke DATABASE
           for (i=0; i<6; i=i+1){               // Led Hijau Berkedip sebanyak 6x
                digitalWrite (Led_Hijau, HIGH); // Menyalakan Led Hijau (HIGH = Memberi tegangan pada LED)
                   delay (150);                 // Memberi waktu tunda 0.15 detik
                digitalWrite (Led_Hijau, LOW);  // Mematikan Led Hijau (LOW = Tidak Memberi tegangan pada LED)
                   delay (150);                 // Memberi waktu tunda 0.15 detik
                digitalWrite (Led_Hijau, HIGH); // Menyalakan Led Hijau (HIGH = Memberi tegangan pada LED)
           }
                delay(7000);                    // Menampilkan Hasil Suhu Tubuh dan Kondisi Badan selama 7 detik pada LCD
                digitalWrite (Led_Hijau, LOW);  // Mematikan Led Hijau (LOW = Tidak Memberi tegangan pada LED)
                lcd.clear();                    // Menghapus Layar LCD
                lcd.setCursor(0,0);             // Angka 0 = Kolom & angka 0 = baris atas (baris 1)
                lcd.print("    DETEKSI     ");  // Menampilkan kalimat Deteksi pada LCD
                lcd.setCursor(0,1);             // Angka 0 = Kolom & angka 1 = baris bawah (baris 2)
                lcd.print(" KONDISI BADAN  ");  // Menampilkan kalimat Kondisi Badan pada LCD
       }
       else if (Alpha_Predikat3 && maximal){    // Jika Kondisi Badan Panas dan Hasil Defuzzyfikasi, maka
           Serial.println ("Kondisi Panas");    // Menampilkan Kondisi Badan pada Serial Monitor
             lcd.setCursor(0,0);                // Angka 0 = Kolom & angka 0 = baris atas (baris 1)
             lcd.print("SUHU    : ");           // Menampilkan kalimat pada LCD
             lcd.print(suhutubuh);              // Menampilkan hasil pengukuran suhu tubuh
             lcd.setCursor(0,1);                // Angka 0 = Kolom & angka 1 = baris bawah (baris 2)
             lcd.print("KONDISI : PANAS ");     // Menampilkan kalimat Kondisi PANAS pada LCD
                digitalWrite (Led_Hijau, LOW);  // Mematikan Led Hijau (LOW = Tidak Memberi tegangan pada LED)
                kondisibadan = "PANAS";         // Untuk mengirim data kalimat PANAS ke DATABASE
           for (i=0; i<4; i=i+1){               // Led Merah Berkedip sebanyak 4x
                digitalWrite (Led_Merah, HIGH); // Menyalakan Led Merah (HIGH = Memberi tegangan pada LED)
                digitalWrite (Buzzer, HIGH);    // Buzzer Berbunyi (HIGH = Memberi tegangan pada Buzzer)
                lcd.display();                  // Memunculkan Tulisan pada LCD
                   delay (500);                 // Memberi waktu tunda 0.5 detik
                digitalWrite (Led_Merah, LOW);  // Mematikan Led Merah (LOW = Tidak Memberi tegangan pada LED)
                digitalWrite (Buzzer, LOW);     // Buzzer Tidak Berbunyi (LOW = Tidak Memberi tegangan pada Buzzer)
                lcd.noDisplay();                // Mematikan Tulisan pada LCD
                   delay (500);                 // Memberi waktu tunda 0.5 detik
                digitalWrite (Led_Merah, HIGH); // Menyalakan Led Merah (HIGH = Memberi tegangan pada LED)
                lcd.display();                  // Memunculkan Tulisan pada LCD
           }
                delay(7000);                    // Menampilkan Hasil Suhu Tubuh dan Kondisi Badan selama 7 detik pada LCD
                digitalWrite (Led_Merah, LOW);  // Mematikan Led Merah (LOW = Tidak Memberi tegangan pada LED)
                lcd.clear();                    // Menghapus Layar LCD
                lcd.setCursor(0,0);             // Angka 0 = Kolom & angka 0 = baris atas (baris 1)
                lcd.print("    DETEKSI     ");  // Menampilkan kalimat Deteksi pada LCD
                lcd.setCursor(0,1);             // Angka 0 = Kolom & angka 1 = baris bawah (baris 2)
                lcd.print(" KONDISI BADAN  ");  // Menampilkan kalimat Kondisi Badan pada LCD
       }

// ================================== Proses Pengiriman Data pada Database =================================
/* Proses pengiriman data pada database dengan alamat sesuai wi-fi yang dipakai. 
   Pengiriman data ini apabila alamat wi-fi salah, maka data gagal terkirim, 
   Apabila alamat wi-fi benar maka isi data suhu tubuh dan kondisi badan yang diperoleh akan dikirim ke alamat IP ESP8266 dan mengirim request data ke server web*/
       Serial.println ("------------------------------ KIRIM DATA KE DATABASE --------------------------------");
       Serial.print   ("Wi-Fi (Hotspot) : ");
       Serial.println (ssid);
       Serial.print   ("Koneksi dari IP Address : ");
       Serial.println (host);

// =============================== Mengirimkan ke alamat host dengan port 80 ===============================
       WiFiClient client;
       const int httpPort = 80;
       if (!client.connect(host, httpPort)) {
          Serial.println("Koneksi Gagal");
          return;
       }

// ======================== Isi Konten yang dikirim adalah alamat dari IP ESP8266 ==========================
       String url = "/Eko-TugasAkhir-KondisiBadan/tabel-write-data.php?Suhu-Tubuh=";   // Mengarah ke File eko-tugas-akhir di folder htdocs (HTML)
       url += suhutubuh;
       url += "&Kondisi-Badan=";
       url += kondisibadan;
          Serial.print   ("Requesting URL : ");
          Serial.println (url);
          Serial.println ("--------------------------------------------------------------------------------------");

// ======================== Mengirimkan Request Data yang didapat ke Server (Web) ==========================
       client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n");
       unsigned long timeout = millis();
       while (client.available() == 0) {
            if (millis() - timeout > 1000) {
               Serial.println(">>> Client Timeout !");
               client.stop();
               return;
            }
       } 
    } 
} 

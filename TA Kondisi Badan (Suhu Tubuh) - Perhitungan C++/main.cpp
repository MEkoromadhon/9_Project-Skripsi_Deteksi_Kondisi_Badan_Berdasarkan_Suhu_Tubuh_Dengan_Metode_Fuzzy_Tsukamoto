#include <stdio.h>
#include <stdlib.h>

float suhutubuh, kondisibadan; // Variabel
float suhutubuh_dingin, suhutubuh_normal, suhutubuh_panas; // Himpunan Linguistik pada variabel suhu tubuh
float kondisibadan_sehat, kondisibadan_sakit; // Himpunan Linguistik pada variabel kondisi badan
float Alpha_Predikat1, Alpha_Predikat2, Alpha_Predikat3;
float min, max;
float Z_Total;
int i;

// ======================= PROSES MENGIDENTIFIKASI VARIABEL DAN HIMPUNAN FUZZY =======================
//Variabel (Suhu Tubuh)    - Himpunan Linguistik (Dingin (Hipotermia), Normal dan Panas) - Himpunan Numerik (36 s/d 38.5)
//Variabel (Kondisi Badan) - Himpunan Linguistik (Sehat & Sakit) - Himpunan Numerik (36.5 & 38.5)

// ======================= PROSES BASIS PENGETAHUAN / BASIS RULE (RULE BASE) =========================
// [R1] = If Suhu Tubuh Dingin, Then Kondisi Badan Dingin
// [R2] = If Suhu Tubuh Normal, Then Kondisi Badan Normal
// [R3] = If Suhu Tubuh Panas, Then Kondisi Badan Panas

// Note : Kekurangan diangka 38 dihasil dinyatakan sama 0,5.

// ======================================= PROSES FUZZYFIKASI ========================================
// ==================== Fungsi Derajat Keanggotaan untuk Variabel Suhu Tubuh ========================
void
u_suhutubuh_dingin ()
{
  if (suhutubuh <= 36)
    {
      suhutubuh_dingin = 1;
    }
  else if (suhutubuh >= 36 && suhutubuh <= 36.5)
    {
      suhutubuh_dingin = (36.5 - suhutubuh) / 0.5; // Dibagi 0.5 karena 36,5 - 36 = 0,5
    }
  else if (suhutubuh >= 36.5)
    {
      suhutubuh_dingin = 0;
    }
}

void
u_suhutubuh_normal ()
{
  if (suhutubuh <= 36)
    {
      suhutubuh_normal = 0;
    }
  else if (suhutubuh >= 36 && suhutubuh <= 36.5)
    {
      suhutubuh_normal = (suhutubuh - 36) / 0.5; // Dibagi 0.5 karena 36,5 - 36 = 0,5
    }
  else if (suhutubuh >= 36.5 && suhutubuh <= 37.5)
    {
      suhutubuh_normal = 1;
    }
  else if (suhutubuh >= 37.5 && suhutubuh <= 38.5)
    {
      suhutubuh_normal = (38.5 - suhutubuh) / 1; // Dibagi 1 karena 38,5 - 37,5 = 1
    }
  else if (suhutubuh >= 38.5)
    {
      suhutubuh_normal = 0;
    }
}

void
u_suhutubuh_panas ()
{
  if (suhutubuh <= 37.5)
    {
      suhutubuh_panas = 0;
    }
  else if (suhutubuh >= 37.5 && suhutubuh <= 38.5)
    {
      suhutubuh_panas = (suhutubuh - 37.5) / 1; // Dibagi 1 karena 38,5 - 37,5 = 1
    }
  else if (suhutubuh >= 38.5)
    {
      suhutubuh_panas = 1;
    }
}

// Proses pemanggilan variabel (Fuzzyfikasi)
void
fuzzyfikasi ()
{
  u_suhutubuh_dingin ();
  u_suhutubuh_normal ();
  u_suhutubuh_panas ();
}

int
main ()
{
  printf ("---------------------------------- STUDI KASUS --------------------------------\n");
  printf (" Masukan Suhu Tubuh : ");
  scanf ("%f", &suhutubuh);
  fuzzyfikasi ();

  printf ("\n------------------------- HASIL PERHITUNGAN FUZZYFIKASI -----------------------\n\n");
  printf (" Derajat Keanggotaan Suhu Tubuh ( Dingin / Hipotermia ) : ");
  printf ("%.1f\n", suhutubuh_dingin);
  printf (" Derajat Keanggotaan Suhu Tubuh ( Normal )              : ");
  printf ("%.1f\n", suhutubuh_normal);
  printf (" Derajat Keanggotaan Suhu Tubuh ( Panas )               : ");
  printf ("%.1f\n", suhutubuh_panas);
// ===================================== PROSES INFERENSI / IMPLIKASI ================================
  printf ("\n----------------------------- PROSES INFERENSI --------------------------------\n\n");
  printf ("  Pada penelitian ini, karena tidak terdapat nilai z dari output,\n");
  printf ("  maka inferensi dilakukan dengan menggunakan alpha predikat, sebagai berikut :\n");
  printf ("  [R1] = If Suhu Tubuh Dingin, Then Kondisi Badan Dingin\n");
  printf ("       Alpha Pedikat_1 = ");
    printf ("%.1f\n", suhutubuh_dingin);
  printf ("\n  -----------------------------------------------------------------------\n");
  printf ("  [R2] = If Suhu Tubuh Normal, Then Kondisi Badan Normal\n");
  printf ("       Alpha Pedikat_2 = ");
    printf ("%.1f\n", suhutubuh_normal);
  printf ("\n  -----------------------------------------------------------------------\n");
  printf ("  [R3] = If Suhu Tubuh Panas, Then Kondisi Badan Panas\n");
  printf ("       Alpha Pedikat_3 = ");
    printf ("%.1f\n", suhutubuh_panas);

 // ===================================== PROSES INFERENSI / IMPLIKASI ================================
  printf ("\n\n--------------------------- PROSES DEFUZZYFIKASI ------------------------------\n");
  printf (" Himpunan fuzzy yang dihasilkan dari komponen aturan tahapan fuzzy, \n");
  printf (" maka diperlukan pengambilan nilai tegas (crips) terbesar sebagai output-nya.\n\n");
  printf (" Hasil Defuzzyfikasi = ");
   Alpha_Predikat1 = suhutubuh_dingin;
   Alpha_Predikat2 = suhutubuh_normal;
   Alpha_Predikat3 = suhutubuh_panas;
    if(Alpha_Predikat1 >= Alpha_Predikat2 && Alpha_Predikat1 >= Alpha_Predikat3){
        max = Alpha_Predikat1;
        printf (" Kondisi Dingin");
        }
    else if(Alpha_Predikat2 >= Alpha_Predikat3){
        max = Alpha_Predikat2;
        printf (" Kondisi Normal");
        }
    else{
        max = Alpha_Predikat3;
        printf (" Kondisi Panas");
        }

    if(Alpha_Predikat1 <= Alpha_Predikat2 && Alpha_Predikat1 <= Alpha_Predikat3){
        min = Alpha_Predikat1;
        //printf (" Kondisi Dingin\n");
        }
    else if(Alpha_Predikat2 <= Alpha_Predikat3){
        min = Alpha_Predikat2;
        //printf (" Kondisi Normal\n");
        }
    else{
        min = Alpha_Predikat3;
        //printf (" Kondisi Panas\n");
        }
    printf (" (dengan nilai %.1f)\n", max);
    //printf ("Nilai terkecil           : %.1f\n", min);
    //printf ("Nilai terbesar           : %.1f\n", max);

    printf ("\n\n-------------------------------------------------------------------------------\n");
   system ("pause");
   system("cls"); // UNTUK MEMBERSIHKAN LAYAR
   main (); // KEMBALI KE PROSES AWAL
    return 0;
}

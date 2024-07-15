#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <dirent.h>

typedef struct {
    int fiyat, hazirlamaSuresi;
    int asciNo;
    char yemekAdi[15];
    char kullanici[20];
    char siparisZamaniStr[25]; // Siparis zamanini tutacak dizi
    char hazirlanmaZamaniStr[25]; // Hazirlanma zamanini tutacak dizi
    char siparisID[25];
    char Mevcut[15];
    char Durum[10];
} siparis;
siparis s1;

int sipidno=1;
int anaMenu();
void yeniSiparis(const char *dosyaYolu);
void olusturSiparisID(char *siparisID, int siparisNumarasi);
int kullaniciAdiKontrolu(const char *dosyaYolu, const char *kullanici);
void mevcutSiparis(const char *dosyaYolu);
void oncekiSiparis(const char *dosyaYolu);

int main(){

    int secim = anaMenu();

    while (secim != 0) {
        switch (secim) {
            case 1: yeniSiparis("C:\\Users\\furka\\Desktop\\yemeklistesi.txt"); break;
            case 2: mevcutSiparis("C:\\Users\\furka\\Desktop\\mutfak.txt"); break;
            case 3: oncekiSiparis("C:\\Users\\furka\\Desktop\\siparisler.txt"); break;
            case 0: break;
            default: printf("Hatali secim!\n"); break;
        }
        secim = anaMenu();
    }
    return 0;
}

int anaMenu() {
    int secim;
    printf("\nRestoran Yemek Takip Projesi (ReYeTaP)\n");
    printf("1. Yeni Siparis\n");
    printf("2. Mevcut Siparis Durumu\n");
    printf("3. Daha Onceki Siparislerim\n");
    printf("0. Cikis\n");

    printf("Seciminizi yapiniz: ");
    scanf("%d", &secim);

    system("cls");

    return secim;
}

void yeniSiparis(const char *dosyaYolu) {
    int numara = 0;
    char kullanici[20];
    char onay;
    int yemekbulundu = 0;
    int sira = 1;
    int mevcutSiparis = 0;
    int hazirlamaSuresi = 0;

    printf("Yemek listesi:\n");
    printf("%2s %16s %15s %22s\n", "NO", "YEMEK ADI", "FIYAT", "HAZIRLANMA SURESI");

    FILE *dosya = fopen(dosyaYolu, "r");
    if (dosya != NULL) {
        while (fscanf(dosya, "%s%d%d%s", s1.yemekAdi, &s1.fiyat, &s1.hazirlamaSuresi, s1.Mevcut) == 4) {
            if (strcmp(s1.Mevcut, "mevcut") == 0) {
                printf("%2d %16s %13dTL %18ddk\n", sira, s1.yemekAdi, s1.fiyat, s1.hazirlamaSuresi);
                yemekbulundu = 1;
                sira++;
            }
        }
        fclose(dosya);
    } else {
        perror("Dosya acilamadi!");
        return;
    }

    if (!yemekbulundu) {
        printf("Mevcut yemek bulunamadi!\n");
        return;
    }

    do {
        printf("Kullanici adinizi giriniz: ");  scanf("%s", kullanici);

        if (kullaniciAdiKontrolu("C:\\Users\\furka\\Desktop\\siparisler.txt", kullanici)==1) {
            printf("Bu kullanici adi zaten alinmis. Lutfen baska bir kullanici adi girin.\n");
        } else {
            break;
        }
    } while (1);

    int toplamSiparis = sira - 1;
    while (numara != -1) {
        printf("Siparis numarasini giriniz, bitirmek icin -1 tuslayiniz : ");
        scanf("%d", &numara);

        if (numara == -1){
            system("cls");
            break;
        }
        else if (numara < 1 || numara > toplamSiparis) {
            printf("Gecersiz giris!\n");
            continue;
        }

        dosya = fopen(dosyaYolu, "r");
        sira = 1;
        if (dosya != NULL) {
            while (fscanf(dosya, "%s%d%d%s", s1.yemekAdi, &s1.fiyat, &s1.hazirlamaSuresi, s1.Mevcut) == 4) {
                if (strcmp(s1.Mevcut, "mevcut") == 0) {
                        if (sira == numara) { // Eğer doğru sıradaki yemeği bulduysak
                        mevcutSiparis = 1;
                        hazirlamaSuresi = s1.hazirlamaSuresi;
                            break;
                        }
                    sira++;
                }
            }
            fclose(dosya);
        } else {
            perror("Yemek dosyasi acilamadi!");
            continue;
        }

        if (!mevcutSiparis) {
            printf("Gecersiz siparis!\n");
            continue;
        }

        do {
            printf("Siparisi onayliyor musunuz? (E/H): ");
            scanf(" %c", &onay);

            if (onay == 'E' || onay == 'e') {
                printf("Siparis onaylandi.\n");
                break;
            } else if (onay == 'H' || onay == 'h') {
                printf("Siparis iptal edildi.\n");
                break;
            } else {
                printf("Gecersiz giris! Lutfen 'E' veya 'H' girin.\n");
            }
        } while (1);

        time_t simdiki_zaman;
        struct tm *zaman_bilgisi;
        char zaman_dizisi[20];

        time(&simdiki_zaman);
        zaman_bilgisi = localtime(&simdiki_zaman);
        strftime(zaman_dizisi, sizeof(zaman_dizisi), "%Y.%m.%d-%H.%M", zaman_bilgisi);
        strcpy(s1.siparisZamaniStr, zaman_dizisi);

        // Hazirlanma zamani olarak siparis zamani + yemek hazirlama suresi eklenerek ayarla
        zaman_bilgisi->tm_min += hazirlamaSuresi;
        mktime(zaman_bilgisi);
        strftime(zaman_dizisi, sizeof(zaman_dizisi), "%Y.%m.%d-%H.%M", zaman_bilgisi);
        strcpy(s1.hazirlanmaZamaniStr, zaman_dizisi);
        strcpy(s1.Durum,"beklemede");
        strcpy(s1.kullanici,kullanici);

        olusturSiparisID(s1.siparisID, sipidno);
        // Dosyaya yaz
        FILE *siparisDosyasi = fopen("C:\\Users\\furka\\Desktop\\siparisler.txt", "a");
        if (siparisDosyasi != NULL) {
            fprintf(siparisDosyasi, "%10s ", s1.siparisID);
            fprintf(siparisDosyasi, "%13s ", s1.yemekAdi);
            fprintf(siparisDosyasi, "%5d ", s1.fiyat);
            fprintf(siparisDosyasi, "%13s %13s ", s1.siparisZamaniStr, s1.hazirlanmaZamaniStr);
            fprintf(siparisDosyasi, "%13s ", s1.Durum);
            fprintf(siparisDosyasi, "%13s", s1.kullanici);
            fclose(siparisDosyasi);
        } else {
            perror("Siparis dosyasi acilamadi!");
        }
    }
    sipidno++;
}

void olusturSiparisID(char *siparisID, int siparisNumarasi) {
    time_t simdiki_zaman;
    struct tm *zaman_bilgisi;
    char tarih_dizisi[10];
    char numara_dizisi[4];

    // Şu anki zamanı al
    time(&simdiki_zaman);
    zaman_bilgisi = localtime(&simdiki_zaman);

    // Tarih bilgisini biçimlendir
    strftime(tarih_dizisi, sizeof(tarih_dizisi), "%d%m%y", zaman_bilgisi);

    // Sipariş numarasını dizeye dönüştür
    sprintf(numara_dizisi, "%03d", siparisNumarasi);

    // Sipariş ID'sini oluştur
    strcpy(siparisID, "SIP");
    strcat(siparisID, tarih_dizisi);
    strcat(siparisID, "_");
    strcat(siparisID, numara_dizisi);
}

int kullaniciAdiKontrolu(const char *dosyaYolu, const char *kullanici) {
    FILE *dosya = fopen(dosyaYolu, "r");
    if (dosya != NULL) {
        while (fscanf(dosya, "%s %s %d %s %s %s %s", s1.siparisID, s1.yemekAdi, &s1.fiyat, s1.siparisZamaniStr, s1.hazirlanmaZamaniStr, s1.Durum, s1.kullanici) == 7) {
            if (strcmp(kullanici, s1.kullanici) == 0) {
                fclose(dosya);
                return 1; // Kullanıcı adı zaten kullanılmış
            }
        }
        fclose(dosya);
        return 0; // Dosyada kullanıcı adı bulunamadı
    } else {
        perror("Dosya acilamadi!");
        return -1; // Dosya açılamadı
    }
}

void mevcutSiparis(const char *dosyaYolu) {
    char kullanici[20];
    int toplam = 0;
    double maxKalanSure = -1;  // En fazla kalan süreyi takip etmek için
    char maxKalanSureStr[25];  // En geç teslim zamanı stringini saklamak için

    printf("Kullanici adinizi giriniz: ");
    scanf("%19s", kullanici);

    FILE *siparisDosyasi = fopen(dosyaYolu, "r");
    if (siparisDosyasi != NULL) {
        time_t simdikiZaman;
        time(&simdikiZaman);
        int kullaniciBulundu = 0;
        struct tm *zaman_bilgisi = localtime(&simdikiZaman);

        printf(" yemek adi\tfiyat \t  tahmini teslim    durum\n");
        while (fscanf(siparisDosyasi, "%s %s %d %d %s %s %s %s A%d", s1.siparisID, s1.yemekAdi, &s1.fiyat, &s1.hazirlamaSuresi, s1.siparisZamaniStr, s1.hazirlanmaZamaniStr, s1.Durum, s1.kullanici, &s1.asciNo) == 9) {
            if (strcmp(kullanici, s1.kullanici) == 0) {
                int yil, ay, gun, saat, dakika;
                if (sscanf(s1.hazirlanmaZamaniStr, "%d.%d.%d-%d.%d", &yil, &ay, &gun, &saat, &dakika) == 5) {
                    struct tm hazirlanmaZamani = {0};
                    hazirlanmaZamani.tm_year = yil - 1900;
                    hazirlanmaZamani.tm_mon = ay - 1;
                    hazirlanmaZamani.tm_mday = gun;
                    hazirlanmaZamani.tm_hour = saat;
                    hazirlanmaZamani.tm_min = dakika;
                    hazirlanmaZamani.tm_sec = 0;

                    time_t hazirlanmaZamaniEpoch = mktime(&hazirlanmaZamani);

                    if (hazirlanmaZamaniEpoch != -1) {
                        double zamanFarki = difftime(hazirlanmaZamaniEpoch, simdikiZaman);

                        if (zamanFarki > 0) {
                            printf("%13s %4dTL %13s %13s\n", s1.yemekAdi, s1.fiyat, s1.hazirlanmaZamaniStr, s1.Durum);
                            kullaniciBulundu = 1;
                            toplam += s1.fiyat;

                            if (zamanFarki > maxKalanSure) {
                                maxKalanSure = zamanFarki;
                                strcpy(maxKalanSureStr, s1.hazirlanmaZamaniStr);
                            }
                        }
                    }
                }
            }
        }

        printf("Sepet tutari: %d\n\n\n", toplam);
        if (maxKalanSure > 0) {
            int saat = (int)(maxKalanSure / 3600);
            int dakika = (int)((maxKalanSure - saat * 3600) / 60);
            printf("siparisiniz yaklasik %d saat %d dakika icinde hazir", saat, dakika);
        }
        if (!kullaniciBulundu) {
            printf("Mevcut siparisiniz bulunmamaktadir!\n");
        }
        fclose(siparisDosyasi);
    } else {
        perror("Siparis dosyasi acilamadi!");
    }
}

void oncekiSiparis(const char *dosyaYolu) {
    char kullanici[20];
    int kullaniciBulundu = 0;
    printf("Kullanici adinizi giriniz: ");  scanf("%s", kullanici);

    FILE *siparisDosyasi = fopen(dosyaYolu, "r");
    if (siparisDosyasi != NULL) {
        time_t simdikiZaman;
        time(&simdikiZaman);
        while (fscanf(siparisDosyasi, "%s %s %d %s %s %s %s", s1.siparisID, s1.yemekAdi, &s1.fiyat, s1.siparisZamaniStr, s1.hazirlanmaZamaniStr,s1.Durum, s1.kullanici) == 7) {
            if (strcmp(kullanici, s1.kullanici) == 0) {
                int yil, ay, gun, saat, dakika, saniye;
                if (sscanf(s1.hazirlanmaZamaniStr, "%d.%d.%d-%d.%d", &yil, &ay, &gun, &saat, &dakika) == 5) {
                    struct tm hazirlanmaZamani = {0};
                    hazirlanmaZamani.tm_year = yil -1900;
                    hazirlanmaZamani.tm_mon = ay -1;
                    hazirlanmaZamani.tm_mday = gun;
                    hazirlanmaZamani.tm_hour = saat;
                    hazirlanmaZamani.tm_min = dakika;
                    hazirlanmaZamani.tm_sec = 0;

                    time_t hazirlanmaZamaniEpoch = mktime(&hazirlanmaZamani);

                    if (hazirlanmaZamaniEpoch != -1) {
                        double zamanFarki = difftime(simdikiZaman, hazirlanmaZamaniEpoch);
                        if (zamanFarki > 0) {
                            printf("%15s %5dTL %20s\n", s1.yemekAdi, s1.fiyat, s1.siparisZamaniStr);
                            kullaniciBulundu = 1;
                        }
                    }
                }
            }
        }
        if (!kullaniciBulundu) {
            printf("Gecmis siparisiniz bulunmamaktadir!\n");
        }
        fclose(siparisDosyasi);
    } else {
        perror("Siparis dosyasi acilamadi!");
}
}

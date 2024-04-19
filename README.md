<div align=center>

# Laporan Pengerjaan - Praktikum Modul 2 Sistem Operasi

</div>


## Kelompok IT05 - MH
Fikri Aulia As Sa'adi - 5027231026

Aisha Ayya Ratiandari - 5027231056

Gandhi Ert Julio - 5027231081

## _Soal 1_

## _Soal 2_

## _Soal 3_

## _Soal 4_
### Dikerjakan Oleh Fikri Aulia As Sa'adi (5027231026)
Salomo memiliki passion yang sangat dalam di bidang sistem operasi. Saat ini, dia ingin mengotomasi kegiatan-kegiatan yang ia lakukan agar dapat bekerja secara efisien. Bantulah Salomo untuk membuat program yang dapat mengotomasi kegiatan dia!
(NB: Soal di uji coba dan berhasil di sistem yang menggunakan MacOS dan linux yang menggunakan dual boot dan VM. Untuk teman-teman yang menggunakan WSL bisa mengerjakan soal yang lain ya)

a. Salomo ingin dapat membuka berbagai macam aplikasi dengan banyak jendela aplikasi dalam satu command. Namai file program tersebut setup.c

b. Program dapat membuka berbagai macam aplikasi dan banyak jendela aplikasi sekaligus (bisa membuka 1 atau lebih aplikasi dengan 1 atau lebih window (kecuali aplikasi yang tidak bisa dibuka dengan banyak window seperti discord)) dengan menjalankan: 

./setup -o <app1> <num1> <app2> <num2>.....<appN> <numN>

Contoh penggunaannya adalah sebagai berikut: 

./setup -o firefox 2 wireshark 2

Program akan membuka 2 jendela aplikasi firefox dan 2 jendela aplikasi wireshark.

c. Program juga dapat membuka aplikasi dengan menggunakan file konfigurasi dengan menggunakan 

./setup -f file.conf 

Format file konfigurasi dibebaskan, namun pastikan dapat menjalankan fitur dari poin 2.

Contoh isi file.conf:

Firefox 2
Wireshark 3

Ketika menjalankan command contoh, program akan membuka 2 jendela aplikasi firefox dan 3 jendela aplikasi wireshark.

d. Program dapat mematikan semua aplikasi yg dijalankan oleh program tersebut dengan: 

./setup -k

e. Program juga dapat mematikan aplikasi yang dijalankan sesuai dengan file konfigurasi. 

Contohnya: 

./setup -k file.conf 

Command ini hanya mematikan aplikasi yang dijalankan dengan 

./setup -f file.conf

## Penyelesaian
```
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_APPS 10
#define MAX_NAME_LENGTH 20

void open_apps(int num_apps, char *apps[], int *num_windows);
void open_apps_from_config(const char *config_file);
void close_all_apps();
void close_apps_from_config(const char *config_file);
pid_t spawn_process(const char *app);
void kill_process(const char *app);

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s [-o <app1> <num1> <app2> <num2> ... | -f <config_file> | -k]\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "-o") == 0) {
        int num_apps = (argc - 2) / 2;
        char *apps[MAX_APPS];
        int num_windows[MAX_APPS];

        for (int i = 0; i < num_apps; i++) {
            apps[i] = argv[2 + i * 2];
            num_windows[i] = atoi(argv[3 + i * 2]);
        }

        open_apps(num_apps, apps, num_windows);
    } else if (strcmp(argv[1], "-f") == 0) {
        if (argc != 3) {
            printf("Usage: %s -f <config_file>\n", argv[0]);
            return 1;
        }
        open_apps_from_config(argv[2]);
    } else if (strcmp(argv[1], "-k") == 0) {
        close_all_apps();
    } else {
        printf("Invalid option\n");
        return 1;
    }

    return 0;
}

void open_apps(int num_apps, char *apps[], int *num_windows) {
    for (int i = 0; i < num_apps; i++) {
        for (int j = 0; j < num_windows[i]; j++) {
            spawn_process(apps[i]);
        }
    }
}

void open_apps_from_config(const char *config_file) {
    FILE *file = fopen(config_file, "r");
    if (file == NULL) {
        perror("fopen");
        exit(1);
    }

    char app[MAX_NAME_LENGTH];
    int num_windows;

    while (fscanf(file, "%s %d", app, &num_windows) == 2) {
        for (int j = 0; j < num_windows; j++) {
            spawn_process(app);
        }
    }

    fclose(file);
}

void close_all_apps() {
    kill_process("firefox");
    kill_process("wireshark");
}

void close_apps_from_config(const char *config_file) {
    FILE *file = fopen(config_file, "r");
    if (file == NULL) {
        perror("fopen");
        exit(1);
    }

    char app[MAX_NAME_LENGTH];

    while (fscanf(file, "%s", app) == 1) {
        kill_process(app);
    }

    fclose(file);
}

pid_t spawn_process(const char *app) {
    pid_t pid = fork();
    if (pid == 0) {
        execlp(app, app, NULL);
        perror("execlp");
        exit(1);
    } else if (pid < 0) {
        perror("fork");
        exit(1);
    }
    return pid;
}

void kill_process(const char *app) {
    pid_t pid;
    int status;

    while ((pid = fork()) == -1);

    if (pid == 0) {
        execlp("pkill", "pkill", "-f", app, NULL);
        perror("execlp");
        exit(1);
    } else {
        waitpid(pid, &status, 0);
    }
}
```
### Penjelasan
#### 4. Salomo memiliki passion yang sangat dalam di bidang sistem operasi. Saat ini, dia ingin mengotomasi kegiatan-kegiatan yang ia lakukan agar dapat bekerja secara efisien. Bantulah Salomo untuk membuat program yang dapat mengotomasi kegiatan dia! (NB: Soal di uji coba dan berhasil di sistem yang menggunakan MacOS dan linux yang menggunakan dual boot dan VM. Untuk teman-teman yang menggunakan WSL bisa mengerjakan soal yang lain ya)

#### a. Salomo ingin dapat membuka berbagai macam aplikasi dengan banyak jendela aplikasi dalam satu command. Namai file program tersebut setup.c

- Langkah pertama untuk mengerjakan no.4 adalah membuat file setup.c. Bisa dibuat langsung di VSCode atau melalui terminal menggunakan nano.
```
nano setup.c
```
- Kemudian di dalam file setup.c, impor library yang akan digunakan.
```
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
```
>Library yang perlu diimpor untuk menggunakan fungsi-fungsi sistem seperti fork, execlp, dan waitpid.

- Selanjutnya definisikan konstanta yang akan digunakan.
```
#define MAX_APPS 10
#define MAX_NAME_LENGTH 20
```
>Mendefinisikan batasan maksimum jumlah aplikasi yang dapat dibuka dan panjang maksimum nama aplikasi.

- Lanjutkan dengan membuat fungsi-fungsi yang akan digunakan untuk memenuhi permintaan soal.
```
int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s [-o <app1> <num1> <app2> <num2> ... | -f <config_file> | -k]\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "-o") == 0) {
        int num_apps = (argc - 2) / 2;
        char *apps[MAX_APPS];
        int num_windows[MAX_APPS];

        for (int i = 0; i < num_apps; i++) {
            apps[i] = argv[2 + i * 2];
            num_windows[i] = atoi(argv[3 + i * 2]);
        }

        open_apps(num_apps, apps, num_windows);
    } else if (strcmp(argv[1], "-f") == 0) {
        if (argc != 3) {
            printf("Usage: %s -f <config_file>\n", argv[0]);
            return 1;
        }
        open_apps_from_config(argv[2]);
    } else if (strcmp(argv[1], "-k") == 0) {
        close_all_apps();
    } else {
        printf("Invalid option\n");
        return 1;
    }

    return 0;
}
```
>Fungsi utama program. Ini menerima perintah dari pengguna dan memeriksa opsi yang diberikan untuk menentukan tindakan yang akan dilakukan. (Misal -o , -k, -f, dll)

```
void open_apps(int num_apps, char *apps[], int *num_windows)
```
>Fungsi ini membuka aplikasi-aplikasi yang diberikan oleh pengguna sejumlah num_apps dengan jumlah jendela yang ditentukan oleh num_windows.

```
void open_apps_from_config(const char *config_file)
```

>Fungsi ini membaca daftar aplikasi dari file konfigurasi dan membukanya dengan jumlah jendela yang ditentukan di dalam file tersebut.

```
void close_all_apps()
```
>Fungsi ini menutup semua aplikasi yang sudah didefinisikan secara langsung di dalam kode.

```
void close_apps_from_config(const char *config_file)
```
>Fungsi ini menutup aplikasi-aplikasi yang didefinisikan dalam file konfigurasi.

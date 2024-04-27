<div align=center>

# Laporan Pengerjaan - Praktikum Modul 2 Sistem Operasi

</div>


## Kelompok IT05 - MH
Fikri Aulia As Sa'adi - 5027231026

Aisha Ayya Ratiandari - 5027231056

Gandhi Ert Julio - 5027231081

## _Soal 1_
### Dikerjakan oleh Aisha Ayya (5027231056)

```cpp
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <dirent.h>
#include <time.h>
```

Header yang digunakan, berupa

- unistd.h = untuk mengerjakan fungsi dan proses seperti fork, exec, exit,  dan melakukan aksi kepada sebuah file seperti membuka, dan membaca
- sys/types.h = untuk menggunakan tipe data seperti id_t dan size_t
- sys/stat.h = untuk mengurus status dan izin file
- fcntl.h = untuk melakukan aksi ke file
- dirent.h = membaca direktori dan file
- time.h = untuk memasukkan waktu secara real-time

```cpp
#define LOG_FILE "virus.log"
```

men-define sebuah file log dengan nama “virus.log”

```cpp
void replaceStringInFile(const char *filePath, const char *searchString, const char *replaceString) {
    FILE *file = fopen(filePath, "r+");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }
    
    size_t searchLen = strlen(searchString);
    size_t replaceLen = strlen(replaceString);

    // Membaca karakter demi karakter
    int c;
    while ((c = fgetc(file)) != EOF) {
        // Jika karakter cocok dengan karakter pertama dari string pencarian
        if (c == searchString[0]) {
            long startPos = ftell(file) - 1; // Simpan posisi awal
            int match = 1;
            // Periksa karakter berikutnya untuk memeriksa kesesuaian string pencarian
            for (size_t i = 1; i < searchLen; ++i) {
                c = fgetc(file);
                if (c == EOF) {
                    match = 0;
                    break;
                }
                if (c != searchString[i]) {
                    match = 0;
                    break;
                }
            }
            // Jika string pencarian cocok, ganti dengan string pengganti
            if (match) {
                fseek(file, startPos, SEEK_SET); // Kembali ke posisi awal
                fputs(replaceString, file); // Menulis string pengganti
                // Mengganti karakter yang tersisa dengan spasi jika string pengganti lebih pendek
                for (size_t i = searchLen; i < replaceLen; ++i) {
                    fputc(' ', file);
                }
            }
        }
    }

    fclose(file);
}
```

menggunakan parameter *filePath, *searchString, dan *replaceString

- fungsi membaca file yang diberikan dengan “+r”
- size_t searchlen dan replacelen untuk menghitung panjang string yang dicari dan yang akan diganti
- lalu jika menemukan huruf/karakter pertama yang sesuai dengan yang dicari, maka akan dilanjutkan penyesuaian karakter-karakter berikutnya
    - jika beda, maka pencariannya di stop
    - kalau sesuai semua maka, string akan diganti dan posisi string pengganti akan dimasukkan

```cpp
void logReplacement(const char *fileName) {
    time_t now;
    struct tm *timeinfo;
    char timestamp[20];
    time(&now);
    timeinfo = localtime(&now);
    strftime(timestamp, sizeof(timestamp), "[%d-%m-%Y][%H:%M:%S]", timeinfo);

    // Open virus.log in append mode
    FILE *logFile = fopen(LOG_FILE, "a");
    if (logFile == NULL) {
        perror("Error opening log file");
        return;
    }

    fprintf(logFile, "%s Suspicious string at %s successfully replaced!\n", timestamp, fileName);
    fclose(logFile);
}
```

fungsi untuk mengambil waktu real-time

- mengambil timestamp waktu sekarang yang nanti disimpan dalam variabel “now”
- lalu mengubah waktu yang sudah didapatkan lagi dan dijadikan sesuai format [hari-bulan-tahun][jam:menit:detik]
- lalu membuka file log-nya dan menambahkan informasi waktu tersebut ke dalamnya

```cpp
int main(int argc, char *argv[]) {
    // Create virus.log file if it doesn't exist
    FILE *logFile = fopen(LOG_FILE, "a");
    if (logFile == NULL) {
        perror("Error creating log file");
        return EXIT_FAILURE;
    }
    fclose(logFile);

    // Ensure correct command line arguments
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <folder_path>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Perform string replacement in files within the specified folder
    DIR *dir;
    struct dirent *entry;
    dir = opendir(argv[1]);
    if (dir == NULL) {
        perror("Error opening directory");
        return EXIT_FAILURE;
    }

    char filePath[1024];
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG && !strstr(entry->d_name, ".c")) { // Skip .c files
            snprintf(filePath, sizeof(filePath), "%s/%s", argv[1], entry->d_name);
            replaceStringInFile(filePath, "m4LwAr3", "[MALWARE]");
            replaceStringInFile(filePath, "5pYw4R3", "[SPYWARE]");
            replaceStringInFile(filePath, "R4nS0mWaR3", "[RANSOMWARE]");
            logReplacement(entry->d_name);
        }
    }
    closedir(dir);

        // Implement daemon
    pid_t pid, sid;
    pid = fork();
    if (pid < 0) {
        exit(EXIT_FAILURE);
    }
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }
    umask(0);
    sid = setsid();
    if (sid < 0) {
        exit(EXIT_FAILURE);
    }
    if ((chdir("/")) < 0) {
        exit(EXIT_FAILURE);
    }
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    // Main loop to run the program as a daemon with a 15-second interval
    while (1) {
        // Tasks to be performed in the main loop go here
        sleep(15); // Wait for 15 seconds
    }

    return EXIT_SUCCESS;
}
```

dalam fungsi main

- membuka file virus.log, jika file tidak ada, maka akan dibuat file baru
    - dengan stderr, mengecek apakah command argumen yang diberikan user sudah sesuai
- melakukan pergantian string sesuai soal dengan membuka file yang dituju dan menggunakan fungsi **replaceStringInFile()**

implementasi daemon

- program mencoba menjadi daemon dengan membuat fork yang mana diproses oleh proses anaknya lalau berganti menjadi root
- menutup file descriptor (STD)
- dan masuk ke loop untuk menund prosesnya setiap 15 detik

Dokumentasi saat program dijalankan:

![Screen Shot 2024-04-27 at 00.54.09.png](Nomor%201%20bray%20857fe133899842fea739061e73eac245/Screen_Shot_2024-04-27_at_00.54.09.png)

menggunakan command:

- gcc -o virus virus.c
    
    untuk membuka compiler untuk c-nya
    
- ./virus /path-ke-file-tujuan
    
    untuk menjalankan program terhadap file yang ingin diubah
    
- cat virus.log
    
    untuk mengecek apakah string berhasil diubah di file virus.log
    

Before

![Screen Shot 2024-04-27 at 00.56.31.png](Nomor%201%20bray%20857fe133899842fea739061e73eac245/Screen_Shot_2024-04-27_at_00.56.31.png)

After

![Screen Shot 2024-04-27 at 00.56.55.png](Nomor%201%20bray%20857fe133899842fea739061e73eac245/Screen_Shot_2024-04-27_at_00.56.55.png)


## _Soal 2_
### Dikerjakan Oleh Gandhi Ert Julio (5027231081)
Soal nomor 2 ini berfokus untuk membuat program manajemen file sederhana dari downloading, unzip, dekripsi dan instruksi lainnya.

```bash
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <pwd.h>
#include <dirent.h>
#include <curl/curl.h>
#include <libgen.h>
#include <zip.h>

#define LOG_FILE_PATH "/home/user/history.log"
#define WORKING_DIR "/home/user"
#define BACKUP_DIR_PATH "/home/user/library/backup"
#define DOWNLOAD_LINK "https://drive.google.com/file/d/1rUIZmp10lXLtCIH3LAZJzRPeRks3Crup"
#define ZIP_FILE_PATH "/home/user/file.zip"
```
Script bash yang pertama akan menentukan library header yang akan kita gunakan, `LOG_FILE_PATH`, `WORKING_DIR`, dan `BACKUP_DIR_PATH` menunjukkan program ini mengatur lingkungan kerja yang stabil, 
`BACKUP_DIR_PATH` menunjukkan program ini mungkin melakukan backup file penting secara reguler.

Menggunakan Library dari `<dirent.h>` bisa berarti bahwa program ini juga mengelola isi direktori, seperti listing atau menghapus file.
`DOWNLOAD_LINK` dan `<curl/curl.h>` mengindikasikan program ini bisa mendownload file dari internet.
`ZIP_FILE_PATH` dan `<zip.h>` berarti setelah file di-download, program mungkin meng-unzip atau memanipulasi file zip tersebut.

Kode ini juga termasuk beberapa headers untuk operasi dasar sistem dan file, seperti `<sys/stat.h>` dan `<fcntl.h>`, yang digunakan untuk mengelola metadata file, permissions, dan file descriptors.
`<time.h>` dan `<pwd.h>` menunjukkan kemungkinan adanya pengelolaan waktu dan user data dalam proses ini.

```bash
volatile sig_atomic_t sig_flag = 0;
```
Variabel ini digunakan dalam penanganan sinyal untuk menandai terjadinya event tertentu secara aman tanpa risiko terganggu oleh interupsi.
```bash

// Function to log messages
void write_log(char *filename, char *message) {
    FILE *log_file = fopen(filename, "a");
    if (!log_file) return;
    fprintf(log_file, "%s\n", message);
    fclose(log_file);
}

// Function to log actions
void log_activity(const char *activity, const char *filename) {
    time_t current_time = time(NULL);
    struct tm *time_info = localtime(&current_time);
    char time_str[100];
    strftime(time_str, sizeof(time_str), "%H:%M:%S", time_info);

    struct passwd *pw = getpwuid(getuid());
    char log_buffer[1024];
    snprintf(log_buffer, sizeof(log_buffer), "[%s][%s] - %s - %s", pw ? pw->pw_name : "unknown", time_str, filename, activity);
    write_log(LOG_FILE_PATH, log_buffer);
}
```
Kode ini mendefinisikan dua fungsi untuk mencatat pesan dan aktivitas ke dalam file log. Fungsi `log_activity` mencatat aktivitas pengguna, waktu saat ini, dan nama file yang berhubungan ke dalam file log yang ditentukan.

```bash
// Function to handle signals
void signal_handler(int sig) {
    if (sig == SIGRTMIN) {
        sig_flag = 4;
    } else {
        switch(sig) {
            case SIGTERM:
                sig_flag = 1;
                break;
            case SIGUSR1:
                sig_flag = 2;
                break;
            case SIGUSR2:
                sig_flag = 3;
                break;
        }
    }
}

// Function to daemonize the process
void make_daemon() {
    pid_t pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) exit(EXIT_SUCCESS);

    if (setsid() < 0) exit(EXIT_FAILURE);
    
    signal(SIGCHLD, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
    
    pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) exit(EXIT_SUCCESS);

    umask(0);
    chdir(WORKING_DIR);

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    open("/dev/null", O_RDONLY);
    open("/dev/null", O_WRONLY);
    open("/dev/null", O_RDWR);


    signal(SIGTERM, signal_handler);
    signal(SIGUSR1, signal_handler);
    signal(SIGUSR2, signal_handler);
    signal(SIGRTMIN, signal_handler);
}

```
 Fungsi `make_daemon` memisahkan proses dari terminal kontrol dan memastikan proses berjalan di latar belakang. Proses ini dilakukan dengan fork() untuk menciptakan proses anak dan memisahkan dari grup proses, mengatur ulang direktori kerja.

 Fungsi `signal_handler` menetapkan nilai ke variabel global `sig_flag` berdasarkan tipe sinyal yang diterima (misalnya SIGTERM, SIGUSR1, SIGUSR2, SIGRTMIN), yang menunjukkan jenis operasi atau perubahan mode yang harus dilakukan daemon.
 
```bash
int main() {
    make_daemon();
    
    while (1) {
      if (sig_flag == 1) {
          write_log(LOG_FILE_PATH, "Daemon is shutting down due to SIGTERM");
          break;
      } else if (sig_flag == 2) {
          // Backup logic
          log_activity("Mode change", "Backup mode activated");
      } else if (sig_flag == 3) {
          // Restore logic
          log_activity("Mode change", "Restore mode activated");
      } else if (sig_flag == 4) {
          // Default operation
          log_activity("Mode change", "Default mode activated");
      }
      sleep(1);
    }
    return EXIT_SUCCESS;
}
```
 Dalam fungsi main, daemon terus berjalan dan memeriksa nilai `sig_flag` untuk menentukan tindakan yang harus diambil—apakah itu menutup daemon, mengaktifkan mode backup, mode restore, atau operasi standar.

 Daemon juga mencatat berbagai aktivitas ke file log berdasarkan tindakan yang dilakukan sebagai respons terhadap sinyal yang diterima.
 
 ```bash
size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
}
```
`write_data` adalah fungsi bantuan yang digunakan untuk menulis data ke dalam sebuah stream file.  Parameter fungsi mencakup:

`ptr`: Pointer ke data yang akan ditulis.
`size`: Ukuran setiap elemen data yang akan ditulis.
`nmemb`: Jumlah elemen yang akan ditulis.
`stream`: Pointer ke FILE yang menjadi target penulisan.

```bash
void download_file(const char *url, const char *output_path) {
    CURL *curl;
    FILE *fp;
    CURLcode res;    
    curl = curl_easy_init();
    if (curl) {
     fp = fopen(output_path, "wb");
     if (fp == NULL) {
         perror("Cannot open file for writing");
         return;
     }

     curl_easy_setopt(curl, CURLOPT_URL, url);
     curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
     curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
     curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L);

     res = curl_easy_perform(curl);
     if (res != CURLE_OK) {
         fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
     }

     fclose(fp);
     curl_easy_cleanup(curl);
    }
}

```
Pada baris kode ini `download_file` ini bertujuan untuk mengunduh file dari internet menggunakan pustaka cURL. Konfigurasi cURL:
`CURLOPT_URL`: Menetapkan URL yang akan diunduh.
`CURLOPT_WRITEFUNCTION`: Menetapkan fungsi callback write_data untuk menulis data yang diterima ke dalam file.
`CURLOPT_WRITEDATA`: Menetapkan file pointer sebagai tujuan data yang ditulis oleh write_data.
`CURLOPT_FAILONERROR`: Konfigurasi ini menyebabkan cURL menghentikan operasi dengan kesalahan.

Menggunakan `curl_easy_perform()` untuk melakukan pengunduhan. Jika ada kesalahan selama proses, pesan kesalahan akan dicetak ke stderr.

```bash
void unzip_file(const char *zip_path, const char *extract_dir) {
    struct zip *za;
    struct zip_file *zf;
    struct zip_stat sb;
    char buf[100];
    int err;
    int i, len;
    int fd;
    long long sum;

    if ((za = zip_open(zip_path, 0, &err)) == NULL) {
     zip_error_to_str(buf, sizeof(buf), err, errno);
     fprintf(stderr, "cannot open zip archive `%s': %s\n", zip_path, buf);
     return;
    }

    for (i = 0; i < zip_get_num_entries(za, 0); i++) {
     if (zip_stat_index(za, i, 0, &sb) == 0) {
         len = strlen(sb.name);
         fprintf(stdout, "Extracting %s\n", sb.name);
         zf = zip_fopen_index(za, i, 0);
         if (!zf) continue;

         sprintf(buf, "%s/%s", extract_dir, sb.name);
         fd = open(buf, O_RDWR | O_TRUNC | O_CREAT, 0644);
         if (fd < 0) {
             fprintf(stderr, "Cannot open `%s' for writing\n", buf);
             zip_fclose(zf);
             continue;
         }

         sum = 0;
         while (sum != sb.size) {
             len = zip_fread(zf, buf, 100);
             if (len < 0) {
                 fprintf(stderr, "Read error\n");
                 break;
             }
             write(fd, buf, len);
             sum += len;
         }
         close(fd);
         zip_fclose(zf);
     }
    }
    zip_close(za);
}
```

fungsi `unzip_file` ini bertujuan untuk mengekstrak isi dari file arsip zip ke direktori tertentu.
`zip_open` untuk membuka file zip yang ditentukan oleh `zip_path`
 Menggunakan `zip_get_num_entries` untuk mendapatkan jumlah total entri dalam arsip zip, dan `zip_stat_index` untuk mendapatkan informasi tentang masing-masing entri.
Membuka entri zip dengan `zip_fopen_index`.
Membuat atau membuka file output di lokasi `extract_dir` dengan nama yang diperoleh dari entri zip.


```bash
void process_files(const char *directory) {
    DIR *dir;
    struct dirent *entry;

    if ((dir = opendir(directory)) == NULL) {
     perror("Failed to open directory");
     return;
    }

    while ((entry = readdir(dir)) != NULL) {
     if (entry->d_type == DT_REG) {
         char filepath[1024];
         snprintf(filepath, sizeof(filepath), "%s/%s", directory, entry->d_name);

         if (strstr(entry->d_name, "d3Let3")) {
             remove(filepath);
             log_activity("Successfully deleted", entry->d_name);
         } else if (strstr(entry->d_name, "r3N4mE")) {
             char new_name[1024] = {0};
             if (strstr(entry->d_name, ".ts")) {
                 strcpy(new_name, "helper.ts");
             } else if (strstr(entry->d_name, ".py")) {
                 strcpy(new_name, "calculator.py");
             } else if (strstr(entry->d_name, ".go")) {
                 strcpy(new_name, "server.go");
             } else {
                 strcpy(new_name, "renamed.file");
             }

             char new_filepath[1024];
             snprintf(new_filepath, sizeof(new_filepath), "%s/%s", directory, new_name);
             rename(filepath, new_filepath);
             log_activity("Successfully renamed", new_name);
         }
     }
    }

    closedir(dir);
}
```
fungsi `process_files` ini berfungsi untuk mengelola file dalam sebuah direktori spesifik berdasarkan kriteria tertentu.
parameter `directory` membuka direktori yang ditunjuk. `readdir` untuk membaca setiap entri dalam direktori secara berulang.

Jika nama file mengandung substring "d3Let3", file tersebut akan dihapus menggunakan fungsi `remove`. Fungsi juga mencatat kegiatan penghapusan tersebut melalui `log_activity`.

Jika nama file mengandung substring "r3N4mE", fungsi akan menentukan nama baru berdasarkan ekstensi file:
`.ts` diubah `menjadi helper.ts,`
`.py` menjadi `calculator.py,`
`.go` menjadi `server.go,`
nama lain diubah menjadi `renamed.file`

Setelah selesai dengan semua entri, direktori ditutup dengan `closedir`.

```bash
void backup_file(const char *filepath) {
    char modifiable_filepath[1024]; // Create a modifiable copy of filepath
    strncpy(modifiable_filepath, filepath, sizeof(modifiable_filepath));
    char *base = basename(modifiable_filepath); // Pass the modifiable copy to basename()

    char backup_filepath[1024];
    snprintf(backup_filepath, sizeof(backup_filepath), "%s/%s", BACKUP_DIR_PATH, base);

    rename(filepath, backup_filepath);
    log_activity("Successfully moved to backup", base);
}
```

`backup_file` fungsi ini dirancang untuk memindahkan file ke direktori backup yang ditentukan.

Fungsi ini mengambil path file yang diberikan, membuat salinan yang dapat dimodifikasi dari path tersebut ke dalam array `modifiable_filepath`.
`basename` digunakan untuk mengekstrak nama file dari path lengkap. Hasilnya disimpan dalam pointer `base` yang mengarah ke bagian dari `modifiable_filepath`.

Menggabungkan direktori backup yang didefinisikan oleh `BACKUP_DIR_PATH` dengan nama file yang diperoleh untuk membentuk path lengkap ke lokasi backup.

Fungsi menggunakan `rename` untuk memindahkan file dari lokasi aslinya ke lokasi backup yang baru.

fungsi mencatat kegiatan ini menggunakan `log_activity`.

```bash
void restore_file(const char *backup_dir) {
    DIR *dir;
    struct dirent *entry;

    if ((dir = opendir(backup_dir)) == NULL) {
     perror("Failed to open backup directory");
     return;
    }

    while ((entry = readdir(dir)) != NULL) {
     if (entry->d_type == DT_REG) {
         char backup_filepath[1024];
         snprintf(backup_filepath, sizeof(backup_filepath), "%s/%s", backup_dir, entry->d_name);
         char original_filepath[1024];
         snprintf(original_filepath, sizeof(original_filepath), "%s/%s", WORKING_DIR, entry->d_name);

         rename(backup_filepath, original_filepath);
         log_activity("Successfully restored from backup", entry->d_name);
     }
    }

    closedir(dir);
}
```

`restore_file` ini dirancang untuk mengembalikan file dari direktori backup ke direktori kerja asli. Fungsi ini mencoba membuka direktori backup yang diberikan. Menggunakan `readdir` untuk membaca setiap entri dalam direktori.

Untuk setiap file yang ditemukan di direktori backup:

`backup_filepath` dibuat menggunakan direktori backup dan nama file.
`original_filepath` dibuat menggunakan direktori kerja (`WORKING_DIR`) dan nama file.
`rename` untuk memindahkan file dari direktori backup kembali ke direktori kerja asli.

 direktori ditutup dengan `closedir`.

### Dokumentasi
![image](https://github.com/Gandhiert/BARU-NYOBA/assets/136203533/1046e691-c576-4661-9e28-11c80b69cacd)

Struktur akhir dalam folder soal 2, karena implementasi dan pemanggilan yang tidak berhasil dari instruksi soal yang ada.

![image](https://github.com/Gandhiert/BARU-NYOBA/assets/136203533/29ba75df-580e-425b-9d5e-8263bf00454f)

daemon disaat telah menerima sinyal `SIGTERM` beberapa kali, yang merupakan sinyal untuk meminta program berhenti berjalan. Ini berarti ada beberapa kejadian di mana program hanya diminta untuk shutdown. 



## _Soal 3_
### Dikerjakan oleh Aisha Ayya (5027231056)

```cpp
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include <dirent.h>
#include <errno.h>
```

Header yang digunakan, berupa

- unistd.h = untuk mengerjakan fungsi dan proses seperti fork, exec, exit,  dan melakukan aksi kepada sebuah file seperti membuka, dan membaca
- sys/types.h = untuk menggunakan tipe data seperti id_t dan size_t
- sys/wait.h = untuk melakukan proses seperti wait dan waitpid
- sys/stat.h = untuk mengurus status dan izin file
- fcntl.h = untuk melakukan aksi ke file
- errno.h = untuk menangani eror

```cpp
#define NAMA_USER 50
#define LOG_FILE_EXTENSION ".log"
#define STATUS_FILE_EXTENSION ".txt"
```

men-define nama USER dengan batas 50 karakter, lalu untuk sebuah file log akan dinamakan “.log” dan untuk file tambahan dimana disana akan disimpan nilai prosesnya (apakah berhasil atau tidak sebagai nilai boolean)

```cpp
void log_activity(const char *username, const char *activity_name, pid_t pid, const char *status) {
    // Membuat string waktu
    time_t raw_time;
    struct tm *time_info;
    char time_str[20];
    time(&raw_time);
    time_info = localtime(&raw_time);
    strftime(time_str, sizeof(time_str), "%d:%m:%Y-%H:%M:%S", time_info);

    // Membuat nama file log
    char log_filename[NAMA_USER + sizeof(LOG_FILE_EXTENSION)];
    snprintf(log_filename, sizeof(log_filename), "%s%s", username, LOG_FILE_EXTENSION);

    // Membuka file log
    FILE *log_file = fopen(log_filename, "a");
    if (log_file == NULL) {
        perror("Gagal membuka file log");
        exit(EXIT_FAILURE);
    }

    // Menulis ke file log
    fprintf(log_file, "[%s]-pid_%d-%s_%s\n", time_str, pid, activity_name, status);
    fclose(log_file);
}
```

fungsi untuk mencatat aktivitas pengguna yang nanti akan disimpan di file log

- mengambil informasi waktu secar areal time yang nanti akan disimpan di dalam fungsi localtime() dna memformatnya menjadi **[dd:mm:yyyy]-[hh:mm:ss]**
- lalu membuat array untuk nama file lognya, disini kita mengambil nama username untuk dijadikan nama file
- lalu membuka file lognya dan menulis data ke dalam file log sesuai format yaitu waktu proses, pid, aktivitasnya, dan status

```cpp
int is_process_running(pid_t pid) {
    char proc_path[32];
    snprintf(proc_path, sizeof(proc_path), "/proc/%d", pid);

    // Mencoba membuka direktori proc
    DIR *dir = opendir(proc_path);
    if (dir != NULL) {
        struct dirent *entry;
        int found = 0; // Deklarasi variabel entry dan found
        while ((entry = readdir(dir)) != NULL) {
            if (entry->d_type == DT_DIR) {
                char *endptr;
                long current_pid = strtol(entry->d_name, &endptr, 10); // Menggunakan variabel current_pid untuk pid proses saat ini
                if (*endptr == '\0') {
                    char filename[128];
                    snprintf(filename, sizeof(filename), "/proc/%ld/comm", current_pid);
                    FILE *comm = fopen(filename, "r");
                    if (comm != NULL) {
                        char cmd[64];
                        fgets(cmd, sizeof(cmd), comm);
                        fclose(comm);
                        if (strcmp(cmd, "bash\n") == 0) { // Mengganti proc_name dengan string "bash\n" karena kita ingin memeriksa apakah proses tersebut adalah bash
                            found = 1;
                            break;
                        }
                    }
                }
            }
        }
        closedir(dir);
        return found;
    }

    return 0; // Proses tidak berjalan
}
```

fungsi untuk memeriksa apakah proses dengan PID tertentu sedang berjalan atau tidak

- membuat lamat baru ke direktori /proc
- dimana nanti nama proses dalam direktori tersebut sama dengan nama PID-nya, dimana dengan PID tersebut akan diketahui apakah proses berjalan atau tidak

- dari direktori tersebut, akan kita loop untuk membaca setiap entrinya, untuk mencari folder/proses tertentu
- entri tersebut akan dikonversi ke format long dimana kita mendapatkan PID_process nya
- lalu kita memformat pathnya ke file “comm” dan membuka filenya
- dalam file akan di cek nama perintahnya, jika hasilnya bash maka  proses tersebut adalah shell bash = sedang berjalan
    - Jika proses tersebut ditemukan, kita mengembalikan nilai **`1`** untuk menunjukkan bahwa proses sedang berjalan.
    - Jika tidak ditemukan, kita mengembalikan nilai **`0`** untuk menunjukkan bahwa proses tidak berjalan.

```cpp
void start_monitoring(const char *username) {
    // Memulai pemantauan aktivitas pengguna
    pid_t pid = fork();
    if (pid == 0) {
        // Proses anak
        setsid(); // Membuat sesi baru
        
        // Loop untuk pemantauan aktivitas pengguna
        while (1) {
            // Membaca status dari file
            char status_filename[NAMA_USER + sizeof(STATUS_FILE_EXTENSION)];
            snprintf(status_filename, sizeof(status_filename), "%s%s", username, STATUS_FILE_EXTENSION);
            FILE *status_file = fopen(status_filename, "r");
            if (status_file == NULL) {
                perror("Gagal membuka file status");
                exit(EXIT_FAILURE);
            }
            int status;
            fscanf(status_file, "%d", &status);
            fclose(status_file);
            
            // Menentukan status
            const char *status_str = (status == 0) ? "GAGAL" : "JALAN";

            // Mencatat aktivitas pengguna setiap detik
            log_activity(username, "aktivitas", getpid(), status_str);

            sleep(1);
        }
        exit(EXIT_SUCCESS); // Exit setelah selesai
    }
}
```

fungsi untuk berhenti memantau aktivitas pengguna

- membuat string dengan nama file lognya untuk menghapus file lognya
- membuka direktori /proc dan dengan loop membaca semua proses PID yang sedang berjalan dalam sistem
- lalu fungsi akan mengecek apakah proses adalah proses anak yang sedang berjalan dengan membuka cmdlinenya
- jika baris perintah dari entri tersebut berisi “admin -m” maka akan kita kirim sinyal SIGTERM  menggunakan kill (pid,SIGTERM)

```cpp
void block_user_activity(const char *username) {
    // Memblokir aktivitas pengguna
    char status_filename[NAMA_USER + sizeof(STATUS_FILE_EXTENSION)];
    snprintf(status_filename, sizeof(status_filename), "%s%s", username, STATUS_FILE_EXTENSION);
    FILE *status_file = fopen(status_filename, "w");
    if (status_file == NULL) {
        perror("Gagal membuka file status");
        exit(EXIT_FAILURE);
    }
    fprintf(status_file, "0\n"); // Status: GAGAL
    fclose(status_file);
}
```

fungsi untuk memblokir aktivitas pengguna dan menambahkannya menjadi status terbaru

- membuat filename berdasarkan username lalu membuka file tersebut
- lalu menulis status dalam file tersebut menjadi “0” yang artinya gagal

 

```cpp
void unblock_user_activity(const char *username) {
    // Membuka blokir aktivitas pengguna
    char status_filename[NAMA_USER + sizeof(STATUS_FILE_EXTENSION)];
    snprintf(status_filename, sizeof(status_filename), "%s%s", username, STATUS_FILE_EXTENSION);
    FILE *status_file = fopen(status_filename, "w");
    if (status_file == NULL) {
        perror("Gagal membuka file status");
        exit(EXIT_FAILURE);
    }
    fprintf(status_file, "1\n"); // Status: JALAN
    fclose(status_file);
}
```

fungsi untuk membuka blokiran aktivitas pengguna dan menambahkan status terbaru

- membuat filename berdasarkan username lalu membuka file tersebut
- lalu menulis status dalam file tersebut menjadi “1” yang artinya jalan

```cpp
int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Penggunaan: %s -m/-s/-c/-a <user>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *option = argv[1];
    char *username = argv[2];

    signal(SIGTERM, handle_signal);

    // Membuat file log dan file status secara otomatis saat program dijalankan
    create_log_file(username);

    if (strcmp(option, "-m") == 0) {
        // Memulai pemantauan aktivitas pengguna
        start_monitoring(username);
    } else if (strcmp(option, "-s") == 0) {
        // Menghentikan pemantauan aktivitas pengguna
        stop_monitoring(username);
    } else if (strcmp(option, "-c") == 0) {
        // Memblokir aktivitas pengguna
        stop_monitoring(username); // Memastikan proses pemantauan dihentikan terlebih dahulu
        block_user_activity(username);
    } else if (strcmp(option, "-a") == 0) {
        // Membuka blokir aktivitas pengguna
        unblock_user_activity(username);
        start_monitoring(username); // Memulai pemantauan kembali
    } else {
        printf("Opsi tidak valid\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}
```

fungsi main

- mengecek argumen yang diberikan user
- program mengambil opsi dan user yang akan diproses nantinya
- program mengatur sinyalnya dengan SIGTERM
- membuat file log dengan nama file menggunakan username dengan cara memanggil fungsi yang telah dibuat sebelumnya
- melanjutkan program sesuai dengan opsi yang dipilih user
    - jika memilih “-m” maka akan memanggil fungsi start_monitoring
    - jika memilih opsi “-s” maka akan memanggil fungsi stop_monitoring
    - jika memilih opsi “-c_ maka akan memanggil fungsi block_user_activity
    - jika memilih opsi “-a” maka akan memanggil fungsi unblock_user_activity dan menjalankan kembali program dengan fungsi start_monitoring

Dokumentasi saat program dijalankan:

![Screen Shot 2024-04-27 at 01.12.05.png](Nomor%203%20brayy%20dbb4136d5df24177bc6b2cc217cc1eee/Screen_Shot_2024-04-27_at_01.12.05.png)

menjalankan dengan command:

- gcc -o admin admin.c
    
    untuk menjalankan file c dengan compiler
    
- ./admin -m vagrant
    
    command soal untuk memantau proses apa saja yang dilakukan oleh user.
    
- cat vagrant.txt
    
    untuk melihat apakah kode berhasil JALAN atau GAGAL
    
- cat vagrant.log
    
    untuk melihat kode berjalan setiap detiknya sesuai dengan format
    
    ![Screen Shot 2024-04-27 at 01.14.59.png](Nomor%203%20brayy%20dbb4136d5df24177bc6b2cc217cc1eee/Screen_Shot_2024-04-27_at_01.14.59.png)
    
- ./admin -c vagrant
    
    command soal untuk memeberhentikan proses yang diinginkan 
    

![Screen Shot 2024-04-27 at 01.18.43.png](Nomor%203%20brayy%20dbb4136d5df24177bc6b2cc217cc1eee/Screen_Shot_2024-04-27_at_01.18.43.png)

![Screen Shot 2024-04-27 at 01.19.33.png](Nomor%203%20brayy%20dbb4136d5df24177bc6b2cc217cc1eee/Screen_Shot_2024-04-27_at_01.19.33.png)

- ./admin -a vagrant
    
    untuk mematikan fitur sebelumnya
    

![Screen Shot 2024-04-27 at 01.21.04.png](Nomor%203%20brayy%20dbb4136d5df24177bc6b2cc217cc1eee/Screen_Shot_2024-04-27_at_01.21.04.png)

- ./admin -s vagrant
    
    untuk mematikan fitur -m yang pertama kali kita jalankan
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

```
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
```
>Fungsi untuk memulai sebuah proses aplikasi

```
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
>Fungsi untuk menutup proses aplikasi yang berjalan

#### b. Program dapat membuka berbagai macam aplikasi dan banyak jendela aplikasi sekaligus (bisa membuka 1 atau lebih aplikasi dengan 1 atau lebih window (kecuali aplikasi yang tidak bisa dibuka dengan banyak window seperti discord)) dengan menjalankan: 
#### ./setup -o <app1> <num1> <app2> <num2>.....<appN> <numN>
#### Contoh penggunaannya adalah sebagai berikut: 
#### ./setup -o firefox 2 wireshark 2
#### Program akan membuka 2 jendela aplikasi firefox dan 2 jendela aplikasi wireshark.

- Untuk menyelesaikan soal ini, kita bisa menggunakan for loops untuk menjalankan fungsi "spawn_process" untuk menjalankan aplikasi sesuai permintaan dengan jumlah tertentu.

```
void open_apps(int num_apps, char *apps[], int *num_windows) {
    for (int i = 0; i < num_apps; i++) {
        for (int j = 0; j < num_windows[i]; j++) {
            spawn_process(apps[i]);
        }
    }
}
```

#### c. Program juga dapat membuka aplikasi dengan menggunakan file konfigurasi dengan menggunakan 
#### ./setup -f file.conf 
#### Format file konfigurasi dibebaskan, namun pastikan dapat menjalankan fitur dari poin 2.
#### Contoh isi file.conf:
#### Firefox 2
#### Wireshark 3
#### Ketika menjalankan command contoh, program akan membuka 2 jendela aplikasi firefox dan 3 jendela aplikasi wireshark.

- Soal ini dapat diselesaikan dengan membuat fungsi yang dapat membaca isi dari file config yang kemudian akan dianggap sebagai "app" dan "num_apps" yang kemudian akan diproses kurang lebih sama seperti poin b.

```
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
```

#### d. Program dapat mematikan semua aplikasi yg dijalankan oleh program tersebut dengan: 
#### ./setup -k

- Untuk menutup aplikasi yang sudah dibuka, bisa memanggil fungsi "kill_process" untuk aplikasi yang akan ditutup. Disini saya menggunakan contoh firefox dan wireshark.

```
void close_all_apps() {
    kill_process("firefox");
    kill_process("wireshark");
}
```

#### e. Program juga dapat mematikan aplikasi yang dijalankan sesuai dengan file konfigurasi. 
#### Contohnya: 
#### ./setup -k file.conf 
#### Command ini hanya mematikan aplikasi yang dijalankan dengan 
#### ./setup -f file.conf

- Terakhir, agar dapat menutup aplikasi sesuai dengan isi dari config file, dapat diimplementasikan fungsi seperti poin b dengan modifikasi dimana fungsi yang dipanggil adalah "kill_process"

```
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
```

### Dokumentasi

![Screenshot from 2024-04-22 23-15-05](https://github.com/Gandhiert/Sisop-2-2024-MH-IT05/assets/142889150/60a87814-9cbf-4420-a7a0-cdd7395f0018)
> Contoh implementasi command -o.

![Screenshot from 2024-04-22 23-17-31](https://github.com/Gandhiert/Sisop-2-2024-MH-IT05/assets/142889150/0d0f69ff-3c8f-4c7d-a821-4428c7b9710c)
> Aplikasi akan terbuka sesuai dengan permintaaan user, yaitu firefox dan wireshark sejumlah yang diminta user.

![Screenshot from 2024-04-22 23-18-34](https://github.com/Gandhiert/Sisop-2-2024-MH-IT05/assets/142889150/f2d0c780-75bf-4924-a7a1-2d6e8896b6ca)
>Aplikasi yang dibuka akan tertutup.

![Screenshot from 2024-04-22 23-34-29](https://github.com/Gandhiert/Sisop-2-2024-MH-IT05/assets/142889150/eac0224b-41ec-482c-88ba-77d3b2d8a8d2)
> Command untuk membuka aplikasi berdasarkan file config.

![image](https://github.com/Gandhiert/Sisop-2-2024-MH-IT05/assets/142889150/d179a2c1-eab6-4f50-8ba3-9ab82feb0ca4)
> Contoh isi file config.








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

#define NAMA_USER 50
#define LOG_FILE_EXTENSION ".log"
#define STATUS_FILE_EXTENSION ".txt"

// Fungsi untuk mencatat aktivitas pengguna ke dalam file log
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

// Fungsi untuk menangani sinyal SIGTERM
void handle_signal(int sig) {
    // Menangani sinyal terminasi
    exit(EXIT_SUCCESS);
}

// Fungsi untuk membuat file log
void create_log_file(const char *username) {
    // Membuat nama file log
    char log_filename[NAMA_USER + sizeof(LOG_FILE_EXTENSION)];
    snprintf(log_filename, sizeof(log_filename), "%s%s", username, LOG_FILE_EXTENSION);

    // Membuka file log
    FILE *log_file = fopen(log_filename, "a");
    if (log_file == NULL) {
        perror("Gagal membuat file log");
        exit(EXIT_FAILURE);
    }
    fclose(log_file);

    // Membuat file status untuk menyimpan status keberhasilan
    char status_filename[NAMA_USER + sizeof(STATUS_FILE_EXTENSION)];
    snprintf(status_filename, sizeof(status_filename), "%s%s", username, STATUS_FILE_EXTENSION);
    FILE *status_file = fopen(status_filename, "w");
    if (status_file == NULL) {
        perror("Gagal membuat file status");
        exit(EXIT_FAILURE);
    }
    fprintf(status_file, "1\n"); // Default status: JALAN
    fclose(status_file);
}

// Fungsi untuk memeriksa apakah sebuah proses sedang berjalan
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

// Fungsi untuk memulai pemantauan aktivitas pengguna
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

// Fungsi untuk menghentikan pemantauan aktivitas pengguna
void stop_monitoring(const char *username) {
    // Menghentikan pemantauan aktivitas pengguna
    char log_filename[NAMA_USER + sizeof(LOG_FILE_EXTENSION)];
    snprintf(log_filename, sizeof(log_filename), "%s%s", username, LOG_FILE_EXTENSION);
    if (remove(log_filename) != 0) {
        perror("Gagal menghapus file log");
        exit(EXIT_FAILURE);
    }

    // Mengirim sinyal SIGTERM ke semua proses anak yang sedang berjalan
    DIR *dir;
    struct dirent *entry;
    if ((dir = opendir("/proc")) != NULL) {
        while ((entry = readdir(dir)) != NULL) {
            pid_t pid = strtol(entry->d_name, NULL, 10);
            if (pid > 0) {
                // Memeriksa apakah proses adalah anak yang sedang berjalan
                char cmdline_path[128];
                snprintf(cmdline_path, sizeof(cmdline_path), "/proc/%d/cmdline", pid);
                FILE *cmdline_file = fopen(cmdline_path, "r");
                if (cmdline_file != NULL) {
                    char cmdline[256];
                    fgets(cmdline, sizeof(cmdline), cmdline_file);
                    fclose(cmdline_file);
                    if (strstr(cmdline, username) != NULL && strstr(cmdline, "admin -m") != NULL) {
                        // Mengirim sinyal SIGTERM ke proses anak
                        kill(pid, SIGTERM);
                    }
                }
            }
        }
        closedir(dir);
    }
}

// Fungsi untuk memblokir aktivitas pengguna
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

// Fungsi untuk membuka blokir aktivitas pengguna
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

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

#define LOCK_FILE_PATH "/home/user/daemon.lock"
#define LOG_FILE_PATH "/home/user/history.log"
#define WORKING_DIR "/home/user"
#define BACKUP_DIR_PATH "/home/user/library/backup"
#define DOWNLOAD_LINK "https://drive.google.com/file/d/1rUIZmp10lXLtCIH3LAZJzRPeRks3Crup"
#define ZIP_FILE_PATH "/home/user/file.zip"

volatile sig_atomic_t sig_flag = 0;

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

    int lock_file = open(LOCK_FILE_PATH, O_RDWR|O_CREAT, 0640);
    if (lock_file < 0) exit(EXIT_FAILURE);
    if (lockf(lock_file, F_TLOCK, 0) < 0) exit(EXIT_FAILURE);

    signal(SIGTERM, signal_handler);
    signal(SIGUSR1, signal_handler);
    signal(SIGUSR2, signal_handler);
    signal(SIGRTMIN, signal_handler);
}

// Main function
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

// Function to write data
size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
}

// Function to download file
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

// Function to unzip file
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

// Function to process files
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

// Function to backup file
void backup_file(const char *filepath) {
    char modifiable_filepath[1024]; // Create a modifiable copy of filepath
    strncpy(modifiable_filepath, filepath, sizeof(modifiable_filepath));
    char *base = basename(modifiable_filepath); // Pass the modifiable copy to basename()

    char backup_filepath[1024];
    snprintf(backup_filepath, sizeof(backup_filepath), "%s/%s", BACKUP_DIR_PATH, base);

    rename(filepath, backup_filepath);
    log_activity("Successfully moved to backup", base);
}

// Function to restore file
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

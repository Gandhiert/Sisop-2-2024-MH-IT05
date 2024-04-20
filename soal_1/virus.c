#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <dirent.h>
#include <time.h>

#define LOG_FILE "virus.log" // Define the log file name

// Function to perform string replacement in a file
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

// Function to log string replacements in virus.log file
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

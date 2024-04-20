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

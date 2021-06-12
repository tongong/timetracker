#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define MAX_PROGRAM_NAME  30   /* longer program names get cut off */
#define WRITE_INTERVAL    600  /* 10 minutes */

#define FILENAME   "/timetracker/data" /* relative to $XDG_DATA_HOME */
#define FILENAME2  "/.local/share/timetracker/data" /* relative to $HOME */


/* returns path to data file; should be free()d */
char * filelocation() {
    char *loc = NULL;
    char *xdg_data_home = getenv("XDG_DATA_HOME");
    if (xdg_data_home) {
        loc = malloc(strlen(xdg_data_home) + strlen(FILENAME) + 1);
        strcpy(loc, xdg_data_home);
        strcpy(loc + strlen(xdg_data_home), FILENAME);
    } else {
        char *xdg_home = getenv("HOME");
        loc = malloc(strlen(xdg_home) + strlen(FILENAME2) + 1);
        strcpy(loc, xdg_home);
        strcpy(loc + strlen(xdg_home), FILENAME2);
    }
    return loc;
}


/* creates .local/share/timetracker (or $XDG_DATA_HOME/timetracker) */
/* assumes .local/share is already there */
void createdatadir() {
    char * loc = filelocation();
    for (int i = strlen(loc) - 1; i >= 0; i--) {
        if (loc[i] == '/') {
            loc[i] = '\0';
            break;
        }
    }
    /* fails if directory exists already but that is no problem */
    mkdir(loc, 0777);
    free(loc);
}


struct program {
    char name[MAX_PROGRAM_NAME];
    int time;
};

char lastline[MAX_PROGRAM_NAME];
time_t lasttime = 0;

time_t writetime;

struct program *programs = NULL;
int programnum = 0; /* number of programs stored in the programs array */


/* returns current time (this does not make sense but it is convenient) */
/* this function was created to be able to add programs/write data file from
 * inside and outside the while loop in main(); the current solution with many
 * global variables is rather dirty but it works */
time_t addprogram(_Bool forcewrite) {
    time_t newtime = time(NULL);

    if (lasttime) {
        /* is the program already in the list? (and where?) */
        int index = -1;
        for (int i = 0; i < programnum; i++) {
            if (!strcmp(lastline, programs[i].name)) {
                index = i;
            }
        }
        if (index == -1) {
            /* extend/create list if necessary */
            if (programnum % 10 == 0) {
                programs = realloc(programs, sizeof(struct program) *
                        (10 + programnum));
            }
            /* add new program to list */
            strncpy(programs[programnum].name, lastline, MAX_PROGRAM_NAME);
            programs[programnum].time = newtime - lasttime;
            programnum++;
        } else {
            /* update time for existing program */
            programs[index].time += newtime - lasttime;
        }

        if (writetime <= newtime || forcewrite) {
            /* write to data file */
            char *loc = filelocation();
            FILE *dataFile = fopen(loc, "a");;
            free(loc);

            fprintf(dataFile, "%d,", newtime);
            for (int i = 0; i < programnum - 1; i++) {
                fprintf(dataFile, "%s:%d,", programs[i].name, programs[i].time);
            }
            fprintf(dataFile, "%s:%d\n", programs[programnum - 1].name, programs[programnum - 1].time);
            fclose(dataFile);

            /* reset everything */
            free(programs);
            programs = NULL;
            programnum = 0;
            writetime = newtime + WRITE_INTERVAL;
        }
    }
    return newtime;
}


/* cause getline() to throw error end return from main function */
void exithandler(int dummy) {
    fclose(stdin);
}


int main() {
    createdatadir();

    signal(SIGHUP, exithandler);
    signal(SIGINT, exithandler);
    signal(SIGTERM, exithandler);

    /* ensures that no two timestamps in output file are identical */
    sleep(1);

    writetime = time(NULL) + WRITE_INTERVAL;

    size_t size = 0; /* i do not need this value but getline does */
    char *line = NULL; /* current line / program name */
    while (getline(&line, &size, stdin) != -1) {
        /* remove \n at the end of the line */
        line[strlen(line) - 1] = '\0';
        /* remove ":" and "," from program name because they would mess up
         * the output file */
        for (int i = 0; i < strlen(line); i++) {
            if (line[i] == ':' || line[i] == ',') line[i] = '-';
        }

        /* for the next iteration */
        lasttime = addprogram(0);
        strncpy(lastline, line, MAX_PROGRAM_NAME);
        lastline[MAX_PROGRAM_NAME - 1] = '\0';
    }
    free(line);
    addprogram(1);

    /* is this successful or unsuccessful? the program is meant to operate in
     * an infinite loop and be quit by sigterm */
    return 0;
}

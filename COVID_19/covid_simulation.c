#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define POPULATION 10000
#define DAYS 180

#define CONTACTS_PER_DAY 10
#define INFECTION_PROBABILITY 0.03  // 3% chance per contact
#define INCUBATION_DAYS 5
#define INFECTIOUS_DAYS 7

#define MORTALITY_RATE 0.01

enum State { SUSCEPTIBLE, EXPOSED, INFECTIOUS, RECOVERED, DEAD };

typedef struct {
    enum State state;
    int days_in_state;
} Person;

Person people[POPULATION];

FILE *fp;

void initialize_population() {
    for (int i = 0; i < POPULATION; i++) {
        people[i].state = SUSCEPTIBLE;
        people[i].days_in_state = 0;
    }

    // 初期感染者
    for (int i = 0; i < 10; i++) {
        people[i].state = EXPOSED;
        people[i].days_in_state = 0;
    }
}

void simulate_day(int day) {
    int susceptible = 0, exposed = 0, infectious = 0, recovered = 0, dead = 0;

    // 一時配列で状態変化を管理
    for (int i = 0; i < POPULATION; i++) {
        switch (people[i].state) {
            case SUSCEPTIBLE: susceptible++; break;
            case EXPOSED:     exposed++;     break;
            case INFECTIOUS:  infectious++;  break;
            case RECOVERED:   recovered++;   break;
            case DEAD:        dead++;        break;
        }
    }

    // 感染拡大処理
    for (int i = 0; i < POPULATION; i++) {
        if (people[i].state == INFECTIOUS) {
            for (int c = 0; c < CONTACTS_PER_DAY; c++) {
                int target = rand() % POPULATION;
                if (people[target].state == SUSCEPTIBLE && ((double)rand()/RAND_MAX < INFECTION_PROBABILITY)) {
                    people[target].state = EXPOSED;
                    people[target].days_in_state = 0;
                }
            }
        }
    }

    // 状態更新
    for (int i = 0; i < POPULATION; i++) {
        if (people[i].state == DEAD) continue;

        people[i].days_in_state++;

        if (people[i].state == EXPOSED && people[i].days_in_state >= INCUBATION_DAYS) {
            people[i].state = INFECTIOUS;
            people[i].days_in_state = 0;
        }
        else if (people[i].state == INFECTIOUS && people[i].days_in_state >= INFECTIOUS_DAYS) {
            if ((double)rand()/RAND_MAX < MORTALITY_RATE) {
                people[i].state = DEAD;
            } else {
                people[i].state = RECOVERED;
            }
            people[i].days_in_state = 0;
        }
    }

    // ログ出力
    fprintf(fp, "%d,%d,%d,%d,%d,%d\n", day, susceptible, exposed, infectious, recovered, dead);
}

int main() {
    srand(time(NULL));

    fp = fopen("covid_simulation_output.csv", "w");
    if (!fp) {
        printf("出力ファイルを開けませんでした。\n");
        return 1;
    }

    fprintf(fp, "Day,Susceptible,Exposed,Infectious,Recovered,Dead\n");

    initialize_population();

    for (int day = 1; day <= DAYS; day++) {
        simulate_day(day);
    }

    fclose(fp);
    printf("COVID-19シミュレーションの結果を 'covid_simulation_output.csv' に出力しました。\n");

    return 0;
}


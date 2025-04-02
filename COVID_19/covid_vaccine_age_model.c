#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define POPULATION 10000
#define DAYS 365

// 年齢層
#define CHILD 0
#define ADULT 1
#define ELDERLY 2

// 状態
enum State { SUSCEPTIBLE, EXPOSED, INFECTIOUS, RECOVERED, DEAD };

// ワクチン効果
#define VACCINE_EFFECT_INFECTION 0.5
#define VACCINE_EFFECT_DEATH 0.7

// 感染・致死率（年齢別）
double infection_rate[3] = {0.02, 0.03, 0.04};
double mortality_rate[3] = {0.0001, 0.005, 0.02};
int contact_rate[3] = {15, 10, 6};
double vaccine_rate[3] = {0.2, 0.6, 0.9};

#define INCUBATION_DAYS 5
#define INFECTIOUS_DAYS 7

typedef struct {
    enum State state;
    int days_in_state;
    int age_group;
    int vaccinated;
} Person;

Person people[POPULATION];
FILE *fp;

void initialize_population() {
    for (int i = 0; i < POPULATION; i++) {
        people[i].state = SUSCEPTIBLE;
        people[i].days_in_state = 0;
        people[i].age_group = rand() % 3;
        double r = (double)rand() / RAND_MAX;
        people[i].vaccinated = (r < vaccine_rate[people[i].age_group]) ? 1 : 0;
    }
    // 初期感染者を10人
    for (int i = 0; i < 10; i++) {
        people[i].state = EXPOSED;
        people[i].days_in_state = 0;
    }
}

void simulate_day(int day) {
    int s=0, e=0, i_c=0, r=0, d=0;

    // 状態カウント
    for (int i = 0; i < POPULATION; i++) {
        switch (people[i].state) {
            case SUSCEPTIBLE: s++; break;
            case EXPOSED:     e++; break;
            case INFECTIOUS:  i_c++; break;
            case RECOVERED:   r++; break;
            case DEAD:        d++; break;
        }
    }

    // 感染処理
    for (int i = 0; i < POPULATION; i++) {
        if (people[i].state != INFECTIOUS) continue;
        int age = people[i].age_group;
        for (int c = 0; c < contact_rate[age]; c++) {
            int t = rand() % POPULATION;
            if (people[t].state != SUSCEPTIBLE) continue;

            double inf_prob = infection_rate[people[t].age_group];
            if (people[t].vaccinated)
                inf_prob *= (1.0 - VACCINE_EFFECT_INFECTION);

            if ((double)rand() / RAND_MAX < inf_prob) {
                people[t].state = EXPOSED;
                people[t].days_in_state = 0;
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
            double m_rate = mortality_rate[people[i].age_group];
            if (people[i].vaccinated)
                m_rate *= (1.0 - VACCINE_EFFECT_DEATH);

            if ((double)rand() / RAND_MAX < m_rate) {
                people[i].state = DEAD;
            } else {
                people[i].state = RECOVERED;
            }
            people[i].days_in_state = 0;
        }
    }

    // 出力
    fprintf(fp, "%d,%d,%d,%d,%d,%d\n", day, s, e, i_c, r, d);
}

int main() {
    srand(time(NULL));
    fp = fopen("covid_high_precision_sim.csv", "w");
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
    printf("シミュレーション完了：covid_high_precision_sim.csv に出力しました。\n");
    return 0;
}


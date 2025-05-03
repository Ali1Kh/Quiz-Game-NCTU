#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_QUESTIONS 100
#define MAX_CATEGORIES 20
#define MAX_LINE_LENGTH 512

struct Question {
    char question[256];
    char correct_answer[128];
    char incorrect_answers[3][128];
    char category[64];
    char difficulty[16];
};

struct Question questions[MAX_QUESTIONS];
int question_count = 0;
int score = 0;
int categories_count = 0;
char categories[MAX_CATEGORIES][64];

// 🔄 يحوّل النص إلى small letters
void toLower(char *str) {
    for (int i = 0; str[i]; i++) str[i] = tolower(str[i]);
}

// ⬆️ يحوّل النص إلى Capitalize أول حرف
void capitalize(char *str) {
    if (strlen(str) == 0) return;
    str[0] = toupper(str[0]);
    for (int i = 1; str[i]; i++) str[i] = tolower(str[i]);
}

// 🧠 يقرأ الأسئلة من ملف
void readQuestions() {
    FILE *file = fopen("./data/questions.txt", "r");
    if (!file) {
        perror("questions.txt not found");
        exit(1);
    }

    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        if (question_count >= MAX_QUESTIONS) break;
        struct Question *q = &questions[question_count];
        sscanf(line, "%[^|]|%[^|]|%[^|]|%[^|]|%[^|]|%[^\n]",
            q->question,
            q->correct_answer,
            q->incorrect_answers[0],
            q->incorrect_answers[1],
            q->incorrect_answers[2],
            q->category);

        // التحقق من الصعوبة من اسم التصنيف (مثلاً easy أو hard داخل التصنيف)
        if (strstr(q->category, "easy")) strcpy(q->difficulty, "easy");
        else if (strstr(q->category, "medium")) strcpy(q->difficulty, "medium");
        else if (strstr(q->category, "hard")) strcpy(q->difficulty, "hard");
        else strcpy(q->difficulty, "medium");  // default

        question_count++;
    }

    fclose(file);
}

// 🗂️ يقرأ التصنيفات من ملف
void readCategories() {
    FILE *file = fopen("./data/categories.txt", "r");
    if (!file) {
        perror("categories.txt not found");
        exit(1);
    }

    char line[64];
    while (fgets(line, sizeof(line), file)) {
        if (categories_count >= MAX_CATEGORIES) break;
        line[strcspn(line, "\n")] = 0;  // remove newline
        strcpy(categories[categories_count++], line);
    }

    fclose(file);
}

// 💾 يسجل آخر نتيجة في ملف
void saveLastScore(int score) {
    FILE *f = fopen("last_score.txt", "w");
    if (f) {
        fprintf(f, "%d", score);
        fclose(f);
    }
}

// 📖 يقرأ آخر نتيجة من ملف
void readLastScore() {
    FILE *f = fopen("last_score.txt", "r");
    if (f) {
        fscanf(f, "%d", &score);
        fclose(f);
    } else {
        score = 0;
    }
}

// ✅ يتحقق من صحة الإجابة ويزيد النتيجة
void changeScore(int correct) {
    if (correct) score++;
}

// 🔀 يرتب الإجابات ترتيب عشوائي
void shuffle(char **array, int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        char *temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}

// 📋 يُرجع جميع الإجابات في مصفوفة عشوائية
void allAnswers(struct Question q, char **choices) {
    choices[0] = q.correct_answer;
    choices[1] = q.incorrect_answers[0];
    choices[2] = q.incorrect_answers[1];
    choices[3] = q.incorrect_answers[2];
    shuffle(choices, 4);
}

#ifndef QUIZ_H
#define QUIZ_H

typedef struct {
    char question[256];
    char options[4][100];
    char correct_answer;
} Question;

void load_questions(Question *questions, int *count);
int start_quiz(Question *questions, int total_questions);

#endif

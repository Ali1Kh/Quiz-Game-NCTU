#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <ctype.h>

struct Question
{
    char difficulty[10];
    char category[50];
    char question[256];
    char correct_answer[100];
    char incorrect_answers[3][100];
};

struct Question questions[100];
int question_count = 0;

char categories[100][50];
int categories_count = 0;

int score = 0;

void readQuestions()
{
    FILE *file = fopen("./data/questions.txt", "r");
    if (file == NULL)
    {
        printf("Error reading questions!\n");
        return;
    }
    char line[1024];
    int i = 0;

    while (fgets(line, sizeof(line), file))
    {
        sscanf(line, "difficulty : %[^,], category : %[^,], question : %[^&]& correct_answer : %[^$]$ incorrect_answers : [%[^,], %[^,], %[^]]]",
               questions[i].difficulty, questions[i].category, questions[i].question, questions[i].correct_answer,
               questions[i].incorrect_answers[0], questions[i].incorrect_answers[1], questions[i].incorrect_answers[2]);
        i++;
    }
    question_count = i;

    fclose(file);
}
void readCategories()
{
    FILE *file = fopen("./data/categories.txt", "r");
    if (file == NULL)
    {
        printf("Error reading categories!\n");
        return;
    }
    char line[1024];
    int i = 0;
    while (fgets(line, sizeof(line), file))
    {
        line[strlen(line) - 1] = '\0';
        strcpy(categories[i], line);
        i++;
    }
    categories_count = i;
    fclose(file);
}
void changeScore(bool correct)
{
    if (correct)
    {
        score += 1;
    }
}

void shuffleAnswers(char *answers[], int size)
{
    for (int i = size - 1; i > 0; i--)
    {
        int j = rand() % (i + 1);
        char *temp = answers[i];
        answers[i] = answers[j];
        answers[j] = temp;
    }
}

void allAnswers(struct Question q, char *choices[4])
{
    choices[0] = q.correct_answer;
    for (int i = 0; i < 3; i++)
    {
        choices[i + 1] = q.incorrect_answers[i];
    }
    shuffleAnswers(choices, 4);
}

int checkAnswer(struct Question q, const char *user_answer)
{
    return strcmp(q.correct_answer, user_answer) == 0;
}

void save_last_score(int score)
{
    FILE *file = fopen("last_score.txt", "w");
    if (file == NULL)
    {
        printf("Error saving score!\n");
        return;
    }
    fprintf(file, "%d\n", score);
    fclose(file);
}

int readLastScore()
{
    FILE *file = fopen("last_score.txt", "r");
    if (file == NULL)
    {
        return 0;
    }
    int score;
    fscanf(file, "%d", &score);
    printf("Welcome back! Your last score was: %d\n", score);
    fclose(file);
    return score;
}

int main()
{
    readCategories();
    readQuestions();
    readLastScore();

    char category[50];
    while (1)
    {
        printf("Enter a question category: ");
        scanf("%s", category);

        int isValid = 0;
        for (int i = 0; i < categories_count; i++)
        {
            if (strcasecmp(category, categories[i]) == 0)
            {
                isValid = 1;
                break;
            }
        }

        if (!isValid)
        {
            printf("Invalid category. Try again.\n");
        }
        else
        {
            break;
        }
    }

    int quantity;
    do
    {
        printf("Enter the number of questions (max 10): ");
        scanf("%d", &quantity);
        if (quantity > 10 || quantity < 1)
        {
            printf("Invalid number. Enter a number from 1 to 10.\n");
        }
    } while (quantity > 10 || quantity < 1);

    char difficulty[20];
    while (1)
    {
        printf("Enter the Difficulty (Easy, Medium, Hard): ");
        scanf("%s", difficulty);

        if (strcmp(difficulty, "Easy") == 0 || strcmp(difficulty, "easy") == 0)
        {
            strcpy(difficulty, "easy");
            break;
        }
        else if (strcmp(difficulty, "Medium") == 0 || strcmp(difficulty, "medium") == 0)
        {
            strcpy(difficulty, "medium");
            break;
        }
        else if (strcmp(difficulty, "Hard") == 0 || strcmp(difficulty, "hard") == 0)
        {
            strcpy(difficulty, "hard");
            break;
        }
        else
        {
            printf("Invalid Difficulty. Try again.\n");
        }
    }

    struct Question filteredQuestions[10];
    int counter = 0;

    for (int i = 0; i < 100; i++)
    {
        if (strcasecmp(difficulty, questions[i].difficulty) == 0 &&
            strcasecmp(category, questions[i].category) == 0)
        {
            if (counter < quantity)
            {
                filteredQuestions[counter] = questions[i];
                counter++;
            }
        }
    }

    if (counter == 0)
    {
        printf("No questions available for this category and difficulty.\n");
        return 1;
    }

    if (counter < quantity)
    {
        printf("Only %d questions available. Proceeding with available questions.\n", counter);
        quantity = counter;
    }

    char *choices[4];
    char userAnswer[100];

    for (int i = 0; i < quantity; i++)
    {
        printf("\nQ%d: %s\n", i + 1, filteredQuestions[i].question);

        allAnswers(filteredQuestions[i], choices);
        for (int j = 0; j < 4; j++)
        {
            printf("%c. %s\n", 'A' + j, choices[j]);
        }

        char answerChar;
        printf("Your answer (A, B, C, D): ");
        scanf(" %c", &answerChar);

        answerChar = toupper(answerChar);

        int index = answerChar - 'A';
        if (index >= 0 && index < 4)
        {
            strcpy(userAnswer, choices[index]);
            if (checkAnswer(filteredQuestions[i], userAnswer))
            {
                printf("Correct!\n");
                changeScore(true);
            }
            else
            {
                printf("Wrong! Correct answer is: %s\n", filteredQuestions[i].correct_answer);
                changeScore(false);
            }
        }
        else
        {
            printf("Invalid input. Question skipped.\n");
            changeScore(false);
        }
    }

    printf("\nYour final score is: %d out of %d\n", score, quantity);
    save_last_score(score);

    return 0;
}

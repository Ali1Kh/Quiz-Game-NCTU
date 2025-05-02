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

struct Question questions[100] = {
    {.difficulty = "easy",
     .category = "GeneralKnowledge",
     .question = "According to Sherlock Holmes, \"If you eliminate the impossible, whatever remains, however improbable, must be the...\"",
     .correct_answer = "Truth",
     .incorrect_answers = {"Answer", "Cause", "Source"}},
    {.difficulty = "easy",
     .category = "GeneralKnowledge",
     .question = "What does the F stand for in the FBI?",
     .correct_answer = "Federal",
     .incorrect_answers = {"Foreign", "Formal", "First"}},
    {.difficulty = "easy",
     .category = "GeneralKnowledge",
     .question = "What is the profession of Elon Musk's mom, Maye Musk?",
     .correct_answer = "Model",
     .incorrect_answers = {"Professor", "Biologist", "Musician"}},
    {.difficulty = "easy",
     .category = "GeneralKnowledge",
     .question = "Terry Gilliam was an animator that worked with which British comedy group?",
     .correct_answer = "Monty Python",
     .incorrect_answers = {"The Goodies", "The League of Gentlemen", "The Penny Dreadfuls"}},
    {.difficulty = "easy",
     .category = "GeneralKnowledge",
     .question = "Which of these Marvel games was released on the Playstation 2?",
     .correct_answer = "Spider-Man 2",
     .incorrect_answers = {"Silver Surfer", "Howard the Duck", "Wolverine: Adamantium Rage"}},
    {.difficulty = "easy",
     .category = "GeneralKnowledge",
     .question = "Which of these colours is NOT featured in the logo for Google?",
     .correct_answer = "Pink",
     .incorrect_answers = {"Yellow", "Blue", "Green"}},
    {.difficulty = "easy",
     .category = "GeneralKnowledge",
     .question = "Virgin Trains, Virgin Atlantic and Virgin Racing, are all companies owned by which famous entrepreneur?",
     .correct_answer = "Richard Branson",
     .incorrect_answers = {"Alan Sugar", "Donald Trump", "Bill Gates"}},
    {.difficulty = "easy",
     .category = "GeneralKnowledge",
     .question = "What is the name of the Jewish New Year?",
     .correct_answer = "Rosh Hashanah",
     .incorrect_answers = {"Elul", "New Year", "Succoss"}},
    {.difficulty = "medium",
     .category = "GeneralKnowledge",
     .question = "What is the unit of currency in Laos?",
     .correct_answer = "Kip",
     .incorrect_answers = {"Ruble", "Konra", "Dollar"}},
    {.difficulty = "medium",
     .category = "GeneralKnowledge",
     .question = "What was the original name of the search engine \"Google\"?",
     .correct_answer = "BackRub",
     .incorrect_answers = {"CatMassage", "SearchPro", "Netscape Navigator"}},
    {.difficulty = "medium",
     .category = "GeneralKnowledge",
     .question = "Which iconic Disneyland attraction was closed in 2017 to be remodeled as a \"Guardians of the Galaxy\" themed ride?",
     .correct_answer = "Twilight Zone Tower of Terror",
     .incorrect_answers = {"The Haunted Mansion", "Pirates of the Caribbean", "Peter Pan's Flight"}},
    {.difficulty = "medium",
     .category = "GeneralKnowledge",
     .question = "In which country was the 1992 Summer Olympics Games held?",
     .correct_answer = "Spain",
     .incorrect_answers = {"Russia", "Korea", "USA"}},
    {.difficulty = "hard",
     .category = "GeneralKnowledge",
     .question = "Disney's Haunted Mansion is home to a trio of Hitchhiking Ghosts. Which of these is NOT one of them?",
     .correct_answer = "Harry",
     .incorrect_answers = {"Gus", "Phineas", "Ezra"}},
    {.difficulty = "hard",
     .category = "GeneralKnowledge",
     .question = "Which of the following chemicals are found in eggplant seeds?",
     .correct_answer = "Nicotine",
     .incorrect_answers = {"Mescaline", "Cyanide", "Psilocybin"}},
    {.difficulty = "hard",
     .category = "GeneralKnowledge",
     .question = "What is the romanized Korean word for \"heart\"?",
     .correct_answer = "Simjang",
     .incorrect_answers = {"Aejeong", "Jeongsin", "Segseu"}},
    {.difficulty = "hard",
     .category = "GeneralKnowledge",
     .question = "De Eemhof, Port Zelande and Het Heijderbos are holiday villas owned by what company?",
     .correct_answer = "Center Parcs",
     .incorrect_answers = {"Yelloh Village", "Keycamp", "Villa Plus"}},
    {.difficulty = "hard",
     .category = "GeneralKnowledge",
     .question = "The words \"bungalow\" and \"shampoo\" originate from the languages of which country?",
     .correct_answer = "India",
     .incorrect_answers = {"Papua New Guinea", "Ethiopia", "China"}},
    {.difficulty = "hard",
     .category = "GeneralKnowledge",
     .question = "What type of dog is 'Handsome Dan', the mascot of Yale University?",
     .correct_answer = "Bulldog",
     .incorrect_answers = {"Yorkshire Terrier", "Boxer", "Pug"}},
    {.difficulty = "hard",
     .category = "GeneralKnowledge",
     .question = "What is the romanized Arabic word for \"moon\"?",
     .correct_answer = "Qamar",
     .incorrect_answers = {"Najma", "Kawkab", "Shams"}}};

int score = 0;

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

int check_answer(struct Question q, const char *user_answer)
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

int read_last_score()
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
    read_last_score();

    srand(time(NULL));

    const char *categories[] = {
        "GeneralKnowledge", "Books", "Film", "Music", "Television",
        "VideoGames", "Science&Nature", "Sports", "History",
        "Politics", "Art", "Animals", "Vehicles", "Comics"};

    char category[50];
    while (1)
    {
        printf("Enter a question category: ");
        scanf("%s", category);

        int isValid = 0;
        for (int i = 0; i < 14; i++)
        {
            if (strcmp(category, categories[i]) == 0)
            {
                isValid = 1;
                break;
            }
        }

        if (!isValid)
        {
            printf("Invalid choice. Try again.\n");
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
        if (strcmp(difficulty, questions[i].difficulty) == 0 &&
            strcmp(category, questions[i].category) == 0)
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
    char user_answer[100];

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
            strcpy(user_answer, choices[index]);
            if (check_answer(filteredQuestions[i], user_answer))
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

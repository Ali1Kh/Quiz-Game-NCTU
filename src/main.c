#include <windows.h>
#include <commctrl.h>
#include <stdio.h>

#define ID_CATEGORY 1
#define ID_QUESTIONS 2
#define ID_DIFFICULTY 3
#define ID_START_BTN 4
#define ID_ANSWER1 5
#define ID_ANSWER2 6
#define ID_ANSWER3 7
#define ID_ANSWER4 8
#define ID_NEXT_BTN 9
#define ID_SCORE_LABEL 10

// Declare these variables globally
HWND hCategory, hDifficulty, hQuestions, hStartBtn, hAnswer1, hAnswer2, hAnswer3, hAnswer4, hNextBtn, hScoreLabel;

// Define some dummy questions and answers
typedef struct
{
    char question[256];
    char answers[4][256];
    int correctAnswer;
} Question;

Question questions[] = {
    {"What is the capital of France?", {"Paris", "London", "Berlin", "Madrid"}, 0},
    {"Which planet is known as the Red Planet?", {"Earth", "Mars", "Jupiter", "Saturn"}, 1},
    {"Who wrote 'Romeo and Juliet'?", {"Shakespeare", "Dickens", "Hemingway", "Fitzgerald"}, 0},
    {"What is the largest ocean on Earth?", {"Atlantic", "Indian", "Pacific", "Arctic"}, 2}};

int currentQuestion = 0;
int score = 0;

LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
void ShowQuestion(HWND hwnd);

int main()
{
    // Initialize common controls
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_LISTVIEW_CLASSES;
    InitCommonControlsEx(&icex);

    // Window class setup
    const char g_szClassName[] = "QuizGameClass";
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProcedure;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = g_szClassName;

    if (!RegisterClass(&wc))
    {
        MessageBox(NULL, "Window class registration failed!", "Error", MB_ICONERROR | MB_OK);
        return 1;
    }

    // Create the window
    HWND hwnd = CreateWindow(g_szClassName, "Quiz Game",
                             WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
                             400, 300, NULL, NULL, wc.hInstance, NULL);
    if (hwnd == NULL)
    {
        MessageBox(NULL, "Window creation failed!", "Error", MB_ICONERROR | MB_OK);
        return 1;
    }

    // Show and update window
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    // Main message loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
}

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    switch (msg)
    {
    case WM_CREATE:
    {
        // Category combo box
        hCategory = CreateWindow("COMBOBOX", "", WS_CHILD | WS_VISIBLE | CBS_DROPDOWN, 50, 50, 120, 200, hwnd, (HMENU)ID_CATEGORY, NULL, NULL);
        SendMessage(hCategory, CB_ADDSTRING, 0, (LPARAM) "Science");
        SendMessage(hCategory, CB_ADDSTRING, 0, (LPARAM) "History");
        SendMessage(hCategory, CB_ADDSTRING, 0, (LPARAM) "Geography");

        // Number of questions spinner
        hQuestions = CreateWindow("EDIT", "5", WS_CHILD | WS_VISIBLE | WS_BORDER, 50, 100, 40, 20, hwnd, (HMENU)ID_QUESTIONS, NULL, NULL);

        // Difficulty combo box
        hDifficulty = CreateWindow("COMBOBOX", "", WS_CHILD | WS_VISIBLE | CBS_DROPDOWN, 50, 150, 120, 200, hwnd, (HMENU)ID_DIFFICULTY, NULL, NULL);
        SendMessage(hDifficulty, CB_ADDSTRING, 0, (LPARAM) "Easy");
        SendMessage(hDifficulty, CB_ADDSTRING, 0, (LPARAM) "Medium");
        SendMessage(hDifficulty, CB_ADDSTRING, 0, (LPARAM) "Hard");

        // Start button
        hStartBtn = CreateWindow("BUTTON", "Start", WS_CHILD | WS_VISIBLE, 50, 200, 100, 30, hwnd, (HMENU)ID_START_BTN, NULL, NULL);
        break;
    }

    case WM_COMMAND:
    {
        if (LOWORD(wp) == ID_START_BTN)
        {
            // Hide the initial setup controls
            ShowWindow(hCategory, SW_HIDE);
            ShowWindow(hDifficulty, SW_HIDE);
            ShowWindow(hQuestions, SW_HIDE);
            ShowWindow(hStartBtn, SW_HIDE);

            // Show the question UI
            ShowQuestion(hwnd);
        }

        if (LOWORD(wp) == ID_NEXT_BTN)
        {
            // Check the selected answer and update the score
            if (IsDlgButtonChecked(hwnd, ID_ANSWER1) && questions[currentQuestion].correctAnswer == 0)
            {
                score++;
            }
            else if (IsDlgButtonChecked(hwnd, ID_ANSWER2) && questions[currentQuestion].correctAnswer == 1)
            {
                score++;
            }
            else if (IsDlgButtonChecked(hwnd, ID_ANSWER3) && questions[currentQuestion].correctAnswer == 2)
            {
                score++;
            }
            else if (IsDlgButtonChecked(hwnd, ID_ANSWER4) && questions[currentQuestion].correctAnswer == 3)
            {
                score++;
            }

            currentQuestion++;

            // If there are no more questions, show the score
            if (currentQuestion >= sizeof(questions) / sizeof(questions[0]))
            {
                char scoreMessage[100];
                sprintf(scoreMessage, "Your score is: %d", score);
                MessageBox(hwnd, scoreMessage, "Quiz Finished", MB_OK);
                PostQuitMessage(0);
            }
            else
            {
                ShowQuestion(hwnd);
            }
        }
        break;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, msg, wp, lp);
    }
    return 0;
}

void ShowQuestion(HWND hwnd)
{
    // Remove existing controls if they exist
    if (hAnswer1)
        DestroyWindow(hAnswer1);
    if (hAnswer2)
        DestroyWindow(hAnswer2);
    if (hAnswer3)
        DestroyWindow(hAnswer3);
    if (hAnswer4)
        DestroyWindow(hAnswer4);
    if (hNextBtn)
        DestroyWindow(hNextBtn);

    // Display the current question
    CreateWindow("STATIC", questions[currentQuestion].question, WS_CHILD | WS_VISIBLE, 50, 50, 300, 40, hwnd, NULL, NULL, NULL);

    // Display 4 answer options as radio buttons
    hAnswer1 = CreateWindow("BUTTON", questions[currentQuestion].answers[0], WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON, 50, 100, 200, 30, hwnd, (HMENU)ID_ANSWER1, NULL, NULL);
    hAnswer2 = CreateWindow("BUTTON", questions[currentQuestion].answers[1], WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON, 50, 140, 200, 30, hwnd, (HMENU)ID_ANSWER2, NULL, NULL);
    hAnswer3 = CreateWindow("BUTTON", questions[currentQuestion].answers[2], WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON, 50, 180, 200, 30, hwnd, (HMENU)ID_ANSWER3, NULL, NULL);
    hAnswer4 = CreateWindow("BUTTON", questions[currentQuestion].answers[3], WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON, 50, 220, 200, 30, hwnd, (HMENU)ID_ANSWER4, NULL, NULL);

    // Next button
    hNextBtn = CreateWindow("BUTTON", "Next", WS_CHILD | WS_VISIBLE, 50, 260, 100, 30, hwnd, (HMENU)ID_NEXT_BTN, NULL, NULL);
}

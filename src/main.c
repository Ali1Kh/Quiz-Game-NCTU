#include <windows.h>
#include <commctrl.h>
#include <stdio.h>

#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "gdi32.lib")

#define ID_START      1
#define ID_CATEGORY1  2
#define ID_CATEGORY2  3
#define ID_EASY       4
#define ID_MEDIUM     5
#define ID_HARD       6
#define ID_ANSWER1    7
#define ID_ANSWER2    8
#define ID_ANSWER3    9
#define ID_ANSWER4    10
#define ID_NEXT       11

HWND hCategory1, hCategory2, hEasy, hMedium, hHard;
HWND hStartBtn, hQuestionText, hAnswer1, hAnswer2, hAnswer3, hAnswer4, hNextBtn, hScoreLabel;
int currentQuestion = 0;
int score = 0;

const char* questions[] = {
    "What is the capital of France?",
    "Which language is used for system programming?",
    "What does RAM stand for?"
};

const char* answers[][4] = {
    {"Paris", "London", "Berlin", "Rome"},
    {"Python", "C", "HTML", "PHP"},
    {"Random Access Memory", "Read Access Memory", "Run Accept Memory", "Ready Auto Machine"}
};

const int correctAnswers[] = {0, 1, 0};

int selectedAnswer = -1;

HBRUSH hBlueBrush, hBackgroundBrush;

void ShowQuestion(HWND hwnd) {
    if (currentQuestion >= 3) {
        char result[50];
        sprintf(result, "Your Score: %d / 3", score);
        SetWindowText(hScoreLabel, result);
        ShowWindow(hScoreLabel, SW_SHOW);
        ShowWindow(hAnswer1, SW_HIDE);
        ShowWindow(hAnswer2, SW_HIDE);
        ShowWindow(hAnswer3, SW_HIDE);
        ShowWindow(hAnswer4, SW_HIDE);
        ShowWindow(hNextBtn, SW_HIDE);
        return;
    }

    SetWindowText(hQuestionText, questions[currentQuestion]);
    SetWindowText(hAnswer1, answers[currentQuestion][0]);
    SetWindowText(hAnswer2, answers[currentQuestion][1]);
    SetWindowText(hAnswer3, answers[currentQuestion][2]);
    SetWindowText(hAnswer4, answers[currentQuestion][3]);

    CheckRadioButton(hwnd, ID_ANSWER1, ID_ANSWER4, 0);
    selectedAnswer = -1;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch(msg) {
        case WM_CREATE:
            hBlueBrush = CreateSolidBrush(RGB(30, 144, 255));
            hBackgroundBrush = CreateSolidBrush(RGB(245, 245, 245));

            CreateWindow("STATIC", "Choose Category:", WS_VISIBLE | WS_CHILD,
                20, 20, 150, 20, hwnd, NULL, NULL, NULL);
            hCategory1 = CreateWindow("BUTTON", "General", WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
                20, 50, 100, 20, hwnd, (HMENU)ID_CATEGORY1, NULL, NULL);
            hCategory2 = CreateWindow("BUTTON", "Tech", WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
                130, 50, 100, 20, hwnd, (HMENU)ID_CATEGORY2, NULL, NULL);

            CreateWindow("STATIC", "Choose Difficulty:", WS_VISIBLE | WS_CHILD,
                20, 90, 150, 20, hwnd, NULL, NULL, NULL);
            hEasy = CreateWindow("BUTTON", "Easy", WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
                20, 120, 100, 20, hwnd, (HMENU)ID_EASY, NULL, NULL);
            hMedium = CreateWindow("BUTTON", "Medium", WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
                130, 120, 100, 20, hwnd, (HMENU)ID_MEDIUM, NULL, NULL);
            hHard = CreateWindow("BUTTON", "Hard", WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
                240, 120, 100, 20, hwnd, (HMENU)ID_HARD, NULL, NULL);

            hStartBtn = CreateWindow("BUTTON", "Start Quiz", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                100, 160, 120, 30, hwnd, (HMENU)ID_START, NULL, NULL);

            hQuestionText = CreateWindow("STATIC", "", WS_CHILD | SS_CENTER,
                20, 210, 400, 30, hwnd, NULL, NULL, NULL);
            hAnswer1 = CreateWindow("BUTTON", "", WS_CHILD | BS_AUTORADIOBUTTON,
                20, 250, 400, 20, hwnd, (HMENU)ID_ANSWER1, NULL, NULL);
            hAnswer2 = CreateWindow("BUTTON", "", WS_CHILD | BS_AUTORADIOBUTTON,
                20, 280, 400, 20, hwnd, (HMENU)ID_ANSWER2, NULL, NULL);
            hAnswer3 = CreateWindow("BUTTON", "", WS_CHILD | BS_AUTORADIOBUTTON,
                20, 310, 400, 20, hwnd, (HMENU)ID_ANSWER3, NULL, NULL);
            hAnswer4 = CreateWindow("BUTTON", "", WS_CHILD | BS_AUTORADIOBUTTON,
                20, 340, 400, 20, hwnd, (HMENU)ID_ANSWER4, NULL, NULL);
            hNextBtn = CreateWindow("BUTTON", "Next", WS_CHILD,
                170, 380, 100, 30, hwnd, (HMENU)ID_NEXT, NULL, NULL);

            hScoreLabel = CreateWindow("STATIC", "", WS_CHILD | SS_CENTER,
                20, 420, 400, 30, hwnd, NULL, NULL, NULL);
            break;

        case WM_COMMAND:
            switch(LOWORD(wParam)) {
                case ID_START:
                    ShowWindow(hQuestionText, SW_SHOW);
                    ShowWindow(hAnswer1, SW_SHOW);
                    ShowWindow(hAnswer2, SW_SHOW);
                    ShowWindow(hAnswer3, SW_SHOW);
                    ShowWindow(hAnswer4, SW_SHOW);
                    ShowWindow(hNextBtn, SW_SHOW);
                    ShowWindow(hCategory1, SW_HIDE);
                    ShowWindow(hCategory2, SW_HIDE);
                    ShowWindow(hEasy, SW_HIDE);
                    ShowWindow(hMedium, SW_HIDE);
                    ShowWindow(hHard, SW_HIDE);
                    ShowWindow(hStartBtn, SW_HIDE);
                    ShowQuestion(hwnd);
                    break;

                case ID_ANSWER1: selectedAnswer = 0; break;
                case ID_ANSWER2: selectedAnswer = 1; break;
                case ID_ANSWER3: selectedAnswer = 2; break;
                case ID_ANSWER4: selectedAnswer = 3; break;

                case ID_NEXT:
                    if (selectedAnswer == correctAnswers[currentQuestion])
                        score++;

                    currentQuestion++;
                    ShowQuestion(hwnd);
                    break;
            }
            break;

        case WM_CTLCOLORSTATIC:
        case WM_CTLCOLORBTN:
            return (LRESULT)hBackgroundBrush;

        case WM_DESTROY:
            DeleteObject(hBlueBrush);
            DeleteObject(hBackgroundBrush);
            PostQuitMessage(0);
            break;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszClassName = "QuizGameClass";

    RegisterClass(&wc);

    HWND hwnd = CreateWindow("QuizGameClass", "Quiz Game - NCTU", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 480, 520, NULL, NULL, hInstance, NULL);

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg = {0};
    while(GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
}

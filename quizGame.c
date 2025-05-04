#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_CATEGORIES 50
#define MAX_CATEGORY_LENGTH 100
#define MAX_QUESTIONS 100
#define MAX_FILTERED_QUESTIONS 10

typedef struct {
    char difficulty[32];
    char category[64];
    char question[512];
    char correct_answer[128];
    char incorrect_answers[3][128];
} Question;

// Global variables
char categories[MAX_CATEGORIES][MAX_CATEGORY_LENGTH];
int categories_count = 0;

Question questions[MAX_QUESTIONS];
int question_count = 0;

Question filteredQuestions[MAX_FILTERED_QUESTIONS];
int filtered_count = 0;

int current_question_index = 0;
int score = 0;

// Global application state
GtkApplication *global_app = NULL;
GtkWidget *question_window = NULL;
GtkWidget *score_label;
GtkWidget *question_label;
GtkWidget *info_label;
GtkWidget *grid;
GtkWidget *timer_label;

guint timer_id;
int remaining_time = 10;

// Function declarations
void readCategories();
void readQuestions();
gboolean filter_questions(const char *category, const char *difficulty, int quantity);
void show_question_page();
void show_start_page();
void update_question_display();
void shuffleAnswers(char *answers[], int size);
void saveLastScore(int score);
int readLastScore();
void show_error_window(GtkWindow *parent, const char *message);
gboolean update_timer(gpointer user_data);

// Shuffle answers array with random numbers
void shuffleAnswers(char *answers[], int size) {
    for (int i = size - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        char *temp = answers[i];
        answers[i] = answers[j];
        answers[j] = temp;
    }
}

// Save last score to file
void saveLastScore(int score) {
    FILE *file = fopen("last_score.txt", "w");
    if (file == NULL) {
        printf("Error saving score!\n");
        return;
    }
    fprintf(file, "%d\n", score);
    fclose(file);
}

// Read last score from file
int readLastScore() {
    FILE *file = fopen("last_score.txt", "r");
    if (file == NULL) {
        return 0;
    }
    int score;
    fscanf(file, "%d", &score);
    fclose(file);
    return score;
}

// Read categories from file
void readCategories() {
    FILE *file = fopen("./data/categories.txt", "r");
    if (file == NULL) {
        printf("Error reading categories!\n");
        return;
    }
    char line[1024];
    int i = 0;
    while (fgets(line, sizeof(line), file) && i < MAX_CATEGORIES) {
        line[strlen(line) - 1] = '\0'; // Remove newline character
        strcpy(categories[i], line);
        i++;
    }
    categories_count = i;
    fclose(file);
}

// Read questions from file
void readQuestions() {
    FILE *file = fopen("./data/questions.txt", "r");
    if (file == NULL) {
        printf("Error reading questions!\n");
        return;
    }
    char line[1024];
    int i = 0;

    while (fgets(line, sizeof(line), file) && i < MAX_QUESTIONS) {
        sscanf(line, "difficulty : %[^,], category : %[^,], question : %[^&]& correct_answer : %[^$]$ incorrect_answers : [%[^,], %[^,], %[^]]]",
               questions[i].difficulty,
               questions[i].category,
               questions[i].question,
               questions[i].correct_answer,
               questions[i].incorrect_answers[0],
               questions[i].incorrect_answers[1],
               questions[i].incorrect_answers[2]);
        i++;
    }
    question_count = i;
    fclose(file);
}

// Filter questions based on category, difficulty and quantity
gboolean filter_questions(const char *category, const char *difficulty, int quantity) {
    filtered_count = 0;
    for (int i = 0; i < question_count && filtered_count < quantity; i++) {
        if (strcasecmp(difficulty, questions[i].difficulty) == 0 &&
            strcasecmp(category, questions[i].category) == 0) {
            filteredQuestions[filtered_count] = questions[i];
            filtered_count++;
        }
    }
    return filtered_count > 0;
}

// Show error window
void show_error_window(GtkWindow *parent, const char *message) {
    GtkWidget *error_window;
    GtkWidget *box;
    GtkWidget *label;
    GtkWidget *button;

    error_window = gtk_window_new();
    gtk_window_set_title(GTK_WINDOW(error_window), "Input Error");
    gtk_window_set_default_size(GTK_WINDOW(error_window), 300, 150);
    gtk_window_set_modal(GTK_WINDOW(error_window), TRUE);
    gtk_window_set_transient_for(GTK_WINDOW(error_window), parent);

    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_margin_top(box, 20);
    gtk_widget_set_margin_bottom(box, 20);
    gtk_widget_set_margin_start(box, 20);
    gtk_widget_set_margin_end(box, 20);
    gtk_window_set_child(GTK_WINDOW(error_window), box);

    label = gtk_label_new(message);
    gtk_widget_set_halign(label, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(box), label);

    button = gtk_button_new_with_label("OK");
    g_signal_connect_swapped(button, "clicked", G_CALLBACK(gtk_window_destroy), error_window);
    gtk_box_append(GTK_BOX(box), button);

    gtk_window_present(GTK_WINDOW(error_window));
}

// Handler for Back to Start button click
void on_back_to_start_clicked(GtkButton *button, gpointer user_data) {
    // Close the results dialog
    GtkRoot *root = gtk_widget_get_root(GTK_WIDGET(button));
    gtk_window_destroy(GTK_WINDOW(root));
    
    // Also close the question window if it exists
    if (question_window != NULL) {
        gtk_window_destroy(GTK_WINDOW(question_window));
        question_window = NULL;
    }
    
    // Show the start page again
    show_start_page();
}

// Show a dialog with the quiz result and options
void show_result_dialog() {
    // Save score before displaying the result
    saveLastScore(score);

    char result[64];
    snprintf(result, sizeof(result), "Quiz finished! Your score: %d/%d", score, filtered_count);
    
    GtkWidget *dialog = gtk_window_new();
    gtk_window_set_title(GTK_WINDOW(dialog), "Result");
    gtk_window_set_default_size(GTK_WINDOW(dialog), 300, 150);
    
    // Set transient for the question window if available
    if (question_window != NULL) {
        gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(question_window));
        gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
    }

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_margin_top(vbox, 20);
    gtk_widget_set_margin_bottom(vbox, 20);
    gtk_widget_set_margin_start(vbox, 20);
    gtk_widget_set_margin_end(vbox, 20);
    gtk_window_set_child(GTK_WINDOW(dialog), vbox);

    GtkWidget *label = gtk_label_new(result);
    gtk_box_append(GTK_BOX(vbox), label);

    // Add "Back to Start" button
    GtkWidget *back_btn = gtk_button_new_with_label("Back to Start");
    g_signal_connect(back_btn, "clicked", G_CALLBACK(on_back_to_start_clicked), NULL);
    gtk_widget_set_margin_top(back_btn, 10);
    gtk_box_append(GTK_BOX(vbox), back_btn);

    // Add Close button
    GtkWidget *btn = gtk_button_new_with_label("Close");
    g_signal_connect_swapped(btn, "clicked", G_CALLBACK(gtk_window_destroy), dialog);
    gtk_widget_set_margin_top(btn, 5);
    gtk_box_append(GTK_BOX(vbox), btn);

    gtk_window_present(GTK_WINDOW(dialog));
}

gboolean update_timer(gpointer user_data) {
    char timer_text[32];
    if (remaining_time > 0) {
        snprintf(timer_text, sizeof(timer_text), "Time left: %d seconds", remaining_time);
        gtk_label_set_text(GTK_LABEL(timer_label), timer_text);
        remaining_time--;
        return TRUE;
    } else {
        gtk_label_set_text(GTK_LABEL(timer_label), "Time's up!");
        current_question_index++;
        if (current_question_index >= filtered_count) {
            show_result_dialog();
        } else {
            update_question_display();
        }
        return FALSE;
    }
}

void clear_grid_children(GtkWidget *grid) {
    GtkWidget *child = gtk_widget_get_first_child(grid);
    while (child != NULL) {
        GtkWidget *next = gtk_widget_get_next_sibling(child);
        gtk_widget_unparent(child);
        child = next;
    }
}

void on_answer_clicked(GtkButton *button, gpointer user_data) {
    if (timer_id != 0) {
        g_source_remove(timer_id);
        timer_id = 0;
    }

    int selected_correct = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button), "answer_index"));
    if (selected_correct) score++;

    current_question_index++;
    if (current_question_index >= filtered_count) {
        show_result_dialog();
    } else {
        update_question_display();
    }

    char score_text[32];
    snprintf(score_text, sizeof(score_text), "Score: %d", score);
    gtk_label_set_text(GTK_LABEL(score_label), score_text);
}

void update_question_display() {
    if (timer_id != 0) {
        g_source_remove(timer_id);
    }
    remaining_time = 10;
    timer_id = g_timeout_add_seconds(1, update_timer, NULL);

    Question q = filteredQuestions[current_question_index];

    char info_text[256];
    snprintf(info_text, sizeof(info_text), "<b>Category:</b> %s         %d of %d Questions", q.category, current_question_index + 1, filtered_count);
    gtk_label_set_markup(GTK_LABEL(info_label), info_text);

    gtk_label_set_text(GTK_LABEL(question_label), q.question);
    clear_grid_children(grid);

    char *answers[4];
    answers[0] = q.correct_answer;
    answers[1] = q.incorrect_answers[0];
    answers[2] = q.incorrect_answers[1];
    answers[3] = q.incorrect_answers[2];

    // Shuffle answers before displaying
    shuffleAnswers(answers, 4);

    for (int i = 0; i < 4; i++) {
        GtkWidget *button = gtk_button_new_with_label(answers[i]);
        gtk_widget_set_size_request(button, 200, 40);

        int is_correct = strcmp(answers[i], q.correct_answer) == 0;
        g_object_set_data(G_OBJECT(button), "answer_index", GINT_TO_POINTER(is_correct));
        g_signal_connect(button, "clicked", G_CALLBACK(on_answer_clicked), NULL);
        gtk_grid_attach(GTK_GRID(grid), button, i % 2, i / 2, 1, 1);
    }

    gtk_widget_set_visible(grid, TRUE);
}

void show_question_page() {
    question_window = gtk_application_window_new(global_app);
    gtk_window_set_title(GTK_WINDOW(question_window), "Quiz Game");
    gtk_window_set_default_size(GTK_WINDOW(question_window), 600, 400);

    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 15);
    gtk_widget_set_margin_top(main_box, 20);
    gtk_widget_set_margin_bottom(main_box, 20);
    gtk_widget_set_margin_start(main_box, 20);
    gtk_widget_set_margin_end(main_box, 20);
    gtk_window_set_child(GTK_WINDOW(question_window), main_box);

    info_label = gtk_label_new("");
    gtk_label_set_use_markup(GTK_LABEL(info_label), TRUE);
    gtk_widget_set_halign(info_label, GTK_ALIGN_FILL);
    gtk_box_append(GTK_BOX(main_box), info_label);

    question_label = gtk_label_new("");
    gtk_label_set_wrap(GTK_LABEL(question_label), TRUE);
    gtk_label_set_wrap_mode(GTK_LABEL(question_label), PANGO_WRAP_WORD_CHAR);
    gtk_box_append(GTK_BOX(main_box), question_label);

    grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 20);
    gtk_widget_set_halign(grid, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(main_box), grid);

    timer_label = gtk_label_new("Time left: 10 seconds");
    gtk_widget_set_halign(timer_label, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(main_box), timer_label);

    score_label = gtk_label_new("Score: 0");
    gtk_widget_set_margin_top(score_label, 10);
    gtk_widget_set_halign(score_label, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(main_box), score_label);

    update_question_display();
    gtk_window_present(GTK_WINDOW(question_window));
}

static void on_start_clicked(GtkButton *button, gpointer user_data) {
    GtkEntry *entry = GTK_ENTRY(g_object_get_data(G_OBJECT(button), "entry"));
    GtkDropDown *category_dropdown = GTK_DROP_DOWN(g_object_get_data(G_OBJECT(button), "category_dropdown"));
    GtkDropDown *difficulty_dropdown = GTK_DROP_DOWN(g_object_get_data(G_OBJECT(button), "difficulty_dropdown"));

    GtkWindow *window = GTK_WINDOW(gtk_widget_get_root(GTK_WIDGET(button)));

    const char *text = gtk_editable_get_text(GTK_EDITABLE(entry));
    int quantity = atoi(text);

    GtkStringList *category_list = GTK_STRING_LIST(gtk_drop_down_get_model(category_dropdown));
    guint category_index = gtk_drop_down_get_selected(category_dropdown);
    const char *category = gtk_string_list_get_string(category_list, category_index);

    GtkStringList *difficulty_list = GTK_STRING_LIST(gtk_drop_down_get_model(difficulty_dropdown));
    guint difficulty_index = gtk_drop_down_get_selected(difficulty_dropdown);
    const char *difficulty = gtk_string_list_get_string(difficulty_list, difficulty_index);

    gboolean has_error = FALSE;
    char error_message[512] = "Please fix the following:\n";

    if (strcmp(category, "Choose Category") == 0) {
        strcat(error_message, "- You must select a category.\n");
        has_error = TRUE;
    }
    if (strcmp(difficulty, "Choose difficulty") == 0) {
        strcat(error_message, "- You must select a difficulty.\n");
        has_error = TRUE;
    }
    if (quantity < 1 || quantity > 10) {
        strcat(error_message, "- Enter a valid number of questions (1–10).\n");
        has_error = TRUE;
    }

    if (has_error) {
        show_error_window(window, error_message);
        return;
    }

    // Reset quiz state
    current_question_index = 0;
    score = 0;
    
    if (filter_questions(category, difficulty, quantity)) {
        g_print("Found %d questions for %s (%s)!\n", filtered_count, category, difficulty);
        
        // Close the start window
        gtk_window_destroy(window);
        
        // Show the question page
        show_question_page();
    } else {
        show_error_window(window, "No questions found for this category and difficulty.\nPlease try different options.");
    }
}

void show_start_page() {
    GtkWidget *window;
    GtkWidget *box;
    GtkWidget *title;
    GtkWidget *category_dropdown;
    GtkWidget *difficulty_dropdown;
    GtkWidget *question_entry;
    GtkWidget *start_button;
    GtkWidget *last_score_label;

    window = gtk_application_window_new(global_app);
    gtk_window_set_title(GTK_WINDOW(window), "Quiz App");
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 400);
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);

    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 15);
    gtk_widget_set_margin_top(box, 40);
    gtk_widget_set_margin_bottom(box, 40);
    gtk_widget_set_margin_start(box, 60);
    gtk_widget_set_margin_end(box, 60);
    gtk_window_set_child(GTK_WINDOW(window), box);

    title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(title), "<span font='25' weight='bold'>Quiz</span>");
    gtk_widget_set_halign(title, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(box), title);

    // Setup category dropdown
    GtkStringList *category_list = gtk_string_list_new(NULL);
    gtk_string_list_append(category_list, "Choose Category");
    for (int i = 0; i < categories_count; i++) {
        gtk_string_list_append(category_list, categories[i]);
    }

    category_dropdown = gtk_drop_down_new(G_LIST_MODEL(category_list), NULL);
    gtk_drop_down_set_selected(GTK_DROP_DOWN(category_dropdown), 0);
    gtk_box_append(GTK_BOX(box), category_dropdown);

    // Setup difficulty dropdown
    GtkStringList *difficulty_list = gtk_string_list_new(NULL);
    gtk_string_list_append(difficulty_list, "Choose difficulty");
    gtk_string_list_append(difficulty_list, "Easy");
    gtk_string_list_append(difficulty_list, "Medium");
    gtk_string_list_append(difficulty_list, "Hard");

    difficulty_dropdown = gtk_drop_down_new(G_LIST_MODEL(difficulty_list), NULL);
    gtk_drop_down_set_selected(GTK_DROP_DOWN(difficulty_dropdown), 0);
    gtk_box_append(GTK_BOX(box), difficulty_dropdown);

    // Question quantity entry
    question_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(question_entry), "Number Of Questions");
    gtk_box_append(GTK_BOX(box), question_entry);

    // Start button
    start_button = gtk_button_new_with_label("Start");
    gtk_box_append(GTK_BOX(box), start_button);

    // Display last score
    int last_score = readLastScore();
    char score_text[50];
    snprintf(score_text, sizeof(score_text), "Last Score: %d", last_score);
    last_score_label = gtk_label_new(score_text);
    gtk_widget_set_margin_top(last_score_label, 20);
    gtk_widget_set_halign(last_score_label, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(box), last_score_label);

    // Set data for the start button callback
    g_object_set_data(G_OBJECT(start_button), "entry", question_entry);
    g_object_set_data(G_OBJECT(start_button), "category_dropdown", category_dropdown);
    g_object_set_data(G_OBJECT(start_button), "difficulty_dropdown", difficulty_dropdown);

    // Connect the signal to the callback function
    g_signal_connect(start_button, "clicked", G_CALLBACK(on_start_clicked), NULL);

    gtk_window_present(GTK_WINDOW(window));
}

static void activate(GtkApplication *app, gpointer user_data) {
    // Store the application reference globally
    global_app = app;
    
    // Load categories and questions
    readCategories();
    readQuestions();
    
    // Show the start page
    show_start_page();
}

int main(int argc, char *argv[]) {
    GtkApplication *app;
    int status;

    srand(time(NULL));
    app = gtk_application_new("com.example.quizapp", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);

    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}
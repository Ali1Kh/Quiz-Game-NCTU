#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_CATEGORIES 50
#define MAX_CATEGORY_LENGTH 100
#define MAX_QUESTIONS 100
char categories[MAX_CATEGORIES][MAX_CATEGORY_LENGTH];
int categories_count = 0;

struct Question {
    char difficulty[20];
    char category[20];
    char question[200];
    char correct_answer[100];
    char incorrect_answers[3][100];
};

struct Question questions[MAX_QUESTIONS]; 
int question_count = 0;

struct Question filteredQuestions[10]; 

void readCategories() {
    FILE *file = fopen("./data/categories.txt", "r");
    if (file == NULL) {
        printf("Error reading categories!\n");
        return;
    }
    char line[1024];
    int i = 0;
    while (fgets(line, sizeof(line), file) && i < MAX_CATEGORIES) {
        line[strlen(line) - 1] = '\0';
        strcpy(categories[i], line);
        i++;
    }
    categories_count = i;
    fclose(file);
}

void readQuestions() {
    FILE *file = fopen("./data/questions.txt", "r");
    if (file == NULL) {
        printf("Error reading questions!\n");
        return;
    }
    char line[1024];
    int i = 0;

    while (fgets(line, sizeof(line), file)) {
        sscanf(line, "difficulty : %[^,], category : %[^,], question : %[^&]& correct_answer : %[^$]$ incorrect_answers : [%[^,], %[^,], %[^]]]%",
               questions[i].difficulty, questions[i].category, questions[i].question, questions[i].correct_answer,
               questions[i].incorrect_answers[0], questions[i].incorrect_answers[1], questions[i].incorrect_answers[2]);
        i++;
    }
    question_count = i;
    fclose(file);
}

gboolean filter_questions(const char *category, const char *difficulty, int quantity) {
    int counter = 0;
    for (int i = 0; i < question_count; i++) {
        if (strcasecmp(difficulty, questions[i].difficulty) == 0 &&
            strcasecmp(category, questions[i].category) == 0) {
            if (counter < quantity) {
                filteredQuestions[counter] = questions[i];
                counter++;
            }
        }
    }
    return counter > 0; 
}

void show_error_window(GtkWindow *parent, const char *message) {
    GtkWidget *error_window;
    GtkWidget *box;
    GtkWidget *label;

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

    gtk_window_present(GTK_WINDOW(error_window));
}

static void on_start_clicked(GtkButton *button, gpointer user_data) {
    GtkEntry *entry = GTK_ENTRY(g_object_get_data(G_OBJECT(button), "entry"));
    GtkDropDown *category_dropdown = GTK_DROP_DOWN(g_object_get_data(G_OBJECT(button), "category_dropdown"));
    GtkDropDown *difficulty_dropdown = GTK_DROP_DOWN(g_object_get_data(G_OBJECT(button), "difficulty_dropdown"));

    GtkWindow *window = GTK_WINDOW(gtk_widget_get_root(GTK_WIDGET(entry)));

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

    readQuestions();

    if (filter_questions(category, difficulty, quantity)) {
        g_print("Found %d questions!\n", quantity);
    } else {
        g_print("No questions found for this category and difficulty.\n");
    }
}

static void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window;
    GtkWidget *box;
    GtkWidget *title;
    GtkWidget *category_dropdown;
    GtkWidget *difficulty_dropdown;
    GtkWidget *question_entry;
    GtkWidget *start_button;
    GtkWidget *last_score_label;

    window = gtk_application_window_new(app);
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

    readCategories();
    GtkStringList *category_list = gtk_string_list_new(NULL);
    gtk_string_list_append(category_list, "Choose Category");
    for (int i = 0; i < categories_count; i++) {
        gtk_string_list_append(category_list, categories[i]);
    }

    category_dropdown = gtk_drop_down_new(G_LIST_MODEL(category_list), NULL);
    gtk_drop_down_set_selected(GTK_DROP_DOWN(category_dropdown), 0);
    gtk_box_append(GTK_BOX(box), category_dropdown);

    GtkStringList *difficulty_list = gtk_string_list_new(NULL);
    gtk_string_list_append(difficulty_list, "Choose difficulty");
    gtk_string_list_append(difficulty_list, "Easy");
    gtk_string_list_append(difficulty_list, "Medium");
    gtk_string_list_append(difficulty_list, "Hard");

    difficulty_dropdown = gtk_drop_down_new(G_LIST_MODEL(difficulty_list), NULL);
    gtk_drop_down_set_selected(GTK_DROP_DOWN(difficulty_dropdown), 0);
    gtk_box_append(GTK_BOX(box), difficulty_dropdown);

    question_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(question_entry), "Number Of Questions");
    gtk_box_append(GTK_BOX(box), question_entry);

    start_button = gtk_button_new_with_label("Start");
    gtk_box_append(GTK_BOX(box), start_button);

    int last_score = 0; 
    char score_text[50];
    snprintf(score_text, sizeof(score_text), "Last Score: %d", last_score);
    last_score_label = gtk_label_new(score_text);
    gtk_widget_set_margin_top(last_score_label, 20);
    gtk_widget_set_halign(last_score_label, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(box), last_score_label);

    g_object_set_data(G_OBJECT(start_button), "entry", question_entry);
    g_object_set_data(G_OBJECT(start_button), "category_dropdown", category_dropdown);
    g_object_set_data(G_OBJECT(start_button), "difficulty_dropdown", difficulty_dropdown);

    g_signal_connect(start_button, "clicked", G_CALLBACK(on_start_clicked), NULL);

    gtk_window_present(GTK_WINDOW(window));
}

int main(int argc, char *argv[]) {
    GtkApplication *app;
    int status;

    app = gtk_application_new("com.example.quizapp", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}

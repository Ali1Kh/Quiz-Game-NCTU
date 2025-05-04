#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_QUESTIONS 100

typedef struct {
    char difficulty[32];
    char category[64];
    char question[512];
    char correct_answer[128];
    char incorrect_answers[3][128];
} Question;

Question questions[MAX_QUESTIONS];
int question_count = 0;
int current_question_index = 0;
int score = 0;

GtkWidget *score_label;
GtkWidget *question_label;
GtkWidget *info_label;
GtkWidget *grid;
GtkWidget *timer_label;

guint timer_id;
int remaining_time = 10;

void update_question_display(void);
void on_answer_clicked(GtkButton *button, gpointer user_data);

// ! Save last score to file
void saveLastScore(int score)
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
        if (current_question_index >= question_count) {
            // Save score before displaying the result
            saveLastScore(score);

            char result[64];
            snprintf(result, sizeof(result), "Quiz finished! Your score: %d/%d", score, question_count);
            GtkWidget *dialog = gtk_window_new();
            gtk_window_set_title(GTK_WINDOW(dialog), "Result");
            gtk_window_set_default_size(GTK_WINDOW(dialog), 300, 100);

            GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
            gtk_window_set_child(GTK_WINDOW(dialog), vbox);

            GtkWidget *label = gtk_label_new(result);
            gtk_box_append(GTK_BOX(vbox), label);

            GtkWidget *btn = gtk_button_new_with_label("Close");
            g_signal_connect_swapped(btn, "clicked", G_CALLBACK(gtk_window_destroy), dialog);
            gtk_box_append(GTK_BOX(vbox), btn);

            gtk_window_present(GTK_WINDOW(dialog));
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

void shuffle_answers(char *answers[], int size) {
    for (int i = size - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        char *temp = answers[i];
        answers[i] = answers[j];
        answers[j] = temp;
    }
}

void update_question_display() {
    if (timer_id != 0) {
        g_source_remove(timer_id);
    }
    remaining_time = 10;
    timer_id = g_timeout_add_seconds(1, update_timer, NULL);

    Question q = questions[current_question_index];

    char info_text[256];
    snprintf(info_text, sizeof(info_text), "<b>Category:</b> %s         %d of %d Questions", q.category, current_question_index + 1, question_count);
    gtk_label_set_markup(GTK_LABEL(info_label), info_text);

    gtk_label_set_text(GTK_LABEL(question_label), q.question);
    clear_grid_children(grid);

    char *answers[4];
    answers[0] = q.correct_answer;
    answers[1] = q.incorrect_answers[0];
    answers[2] = q.incorrect_answers[1];
    answers[3] = q.incorrect_answers[2];

    shuffle_answers(answers, 4);

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

void on_answer_clicked(GtkButton *button, gpointer user_data) {
    if (timer_id != 0) {
        g_source_remove(timer_id);
        timer_id = 0;
    }

    int selected_correct = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button), "answer_index"));
    if (selected_correct) score++;

    current_question_index++;
    if (current_question_index >= question_count) {
        // Save score before displaying the result
        saveLastScore(score);

        char result[64];
        snprintf(result, sizeof(result), "Quiz finished! Your score: %d/%d", score, question_count);

        GtkWidget *dialog = gtk_window_new();
        gtk_window_set_title(GTK_WINDOW(dialog), "Result");
        gtk_window_set_default_size(GTK_WINDOW(dialog), 300, 100);

        GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
        gtk_window_set_child(GTK_WINDOW(dialog), vbox);

        GtkWidget *label = gtk_label_new(result);
        gtk_box_append(GTK_BOX(vbox), label);

        GtkWidget *btn = gtk_button_new_with_label("Close");
        g_signal_connect_swapped(btn, "clicked", G_CALLBACK(gtk_window_destroy), dialog);
        gtk_box_append(GTK_BOX(vbox), btn);

        gtk_window_present(GTK_WINDOW(dialog));
    } else {
        update_question_display();
    }

    char score_text[32];
    snprintf(score_text, sizeof(score_text), "Score: %d", score);
    gtk_label_set_text(GTK_LABEL(score_label), score_text);
}

void show_question_page(GtkApplication *app) {
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Quiz Game");
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 400);

    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 15);
    gtk_widget_set_margin_top(main_box, 20);
    gtk_widget_set_margin_bottom(main_box, 20);
    gtk_widget_set_margin_start(main_box, 20);
    gtk_widget_set_margin_end(main_box, 20);
    gtk_window_set_child(GTK_WINDOW(window), main_box);

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
    gtk_window_present(GTK_WINDOW(window));
}

static void activate(GtkApplication *app, gpointer user_data) {
    readQuestions();
    show_question_page(app);
}

int main(int argc, char *argv[]) {
    GtkApplication *app;
    int status;

    srand(time(NULL));
    app = gtk_application_new("com.quizapp.file", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);

    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}

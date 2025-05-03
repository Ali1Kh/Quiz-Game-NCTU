#include <gtk/gtk.h>
#include <string.h>

typedef struct {
    char *question;
    char *answers[4];
    int correct_index;
    char *category;
} Question;

Question questions[] = {
    {
        .question = "What is the only Generation III Pokemon whose name begins with the letter I?",
        .answers = {"Igglybuff", "Illumise", "Infernape", "Ivysaur"},
        .correct_index = 1,
        .category = "Entertainment: Video Games"
    },
};

int current_question_index = 0;
int total_questions = 1;
int score = 0;

GtkWidget *score_label;
GtkWidget *question_label;
GtkWidget *info_label;

void update_question_display() {
    Question q = questions[current_question_index];

    char info_text[256];
    snprintf(info_text, sizeof(info_text), "%s        %d Of %d Questions", q.category, current_question_index + 1, total_questions);
    gtk_label_set_text(GTK_LABEL(info_label), info_text);
    gtk_label_set_text(GTK_LABEL(question_label), q.question);
}

void on_answer_clicked(GtkButton *button, gpointer user_data) {
    int selected = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button), "answer_index"));
    Question q = questions[current_question_index];

    if (selected == q.correct_index) score++;

    current_question_index++;
    if (current_question_index >= total_questions) {
        char result[64];
        snprintf(result, sizeof(result), "Quiz finished! Your score: %d/%d", score, total_questions);

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
    gtk_widget_set_halign(info_label, GTK_ALIGN_FILL);
    gtk_box_append(GTK_BOX(main_box), info_label);

    question_label = gtk_label_new("");
    gtk_label_set_wrap(GTK_LABEL(question_label), TRUE);
    gtk_label_set_wrap_mode(GTK_LABEL(question_label), PANGO_WRAP_WORD_CHAR);
    gtk_box_append(GTK_BOX(main_box), question_label);

    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 20);
    gtk_widget_set_halign(grid, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(main_box), grid);

    Question q = questions[current_question_index];
    for (int i = 0; i < 4; i++) {
        GtkWidget *button = gtk_button_new_with_label(q.answers[i]);
        gtk_widget_set_size_request(button, 200, 40);
        gtk_widget_set_hexpand(button, TRUE);

        // Margins for consistent layout
        gtk_widget_set_margin_top(button, 5);
        gtk_widget_set_margin_bottom(button, 5);
        gtk_widget_set_margin_start(button, 5);
        gtk_widget_set_margin_end(button, 5);

        g_object_set_data(G_OBJECT(button), "answer_index", GINT_TO_POINTER(i));
        g_signal_connect(button, "clicked", G_CALLBACK(on_answer_clicked), NULL);
        gtk_grid_attach(GTK_GRID(grid), button, i % 2, i / 2, 1, 1);
    }

    score_label = gtk_label_new("Score: 0");
    gtk_widget_set_margin_top(score_label, 10);
    gtk_widget_set_halign(score_label, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(main_box), score_label);

    update_question_display();
    gtk_window_present(GTK_WINDOW(window));
}

static void activate(GtkApplication *app, gpointer user_data) {
    show_question_page(app);
}

int main(int argc, char *argv[]) {
    GtkApplication *app;
    int status;

    app = gtk_application_new("com.quizapp.questionpage", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);

    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}

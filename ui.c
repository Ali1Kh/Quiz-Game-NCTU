#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_CATEGORIES 50
#define MAX_CATEGORY_LENGTH 100
char categories[MAX_CATEGORIES][MAX_CATEGORY_LENGTH];
int categories_count = 0;

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

int readLastScore() {
    FILE *file = fopen("last_score.txt", "r");
    if (file == NULL) {
        return 0;
    }
    int score;
    fscanf(file, "%d", &score);
    printf("Welcome back! Your last score was: %d\n", score);
    fclose(file);
    return score;
}

void show_error_window(GtkWindow *parent) {
    GtkWidget *error_window;
    GtkWidget *box;
    GtkWidget *label;

    error_window = gtk_window_new();
    gtk_window_set_title(GTK_WINDOW(error_window), "Input Error");
    gtk_window_set_default_size(GTK_WINDOW(error_window), 300, 100);
    gtk_window_set_modal(GTK_WINDOW(error_window), TRUE);
    gtk_window_set_transient_for(GTK_WINDOW(error_window), parent);

    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_margin_top(box, 20);
    gtk_widget_set_margin_bottom(box, 20);
    gtk_widget_set_margin_start(box, 20);
    gtk_widget_set_margin_end(box, 20);
    gtk_window_set_child(GTK_WINDOW(error_window), box);

    label = gtk_label_new("Invalid number. Enter a number from 1 to 10.");
    gtk_widget_set_halign(label, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(box), label);

    gtk_window_present(GTK_WINDOW(error_window));
}

gboolean validate_question_quantity(GtkEntry *entry, GtkWindow *parent_window) {
    const char *text = gtk_editable_get_text(GTK_EDITABLE(entry));
    int quantity = atoi(text);
    if (quantity < 1 || quantity > 10) {
        show_error_window(parent_window);
        return FALSE;
    }
    return TRUE;
}

static void on_start_clicked(GtkButton *button, gpointer user_data) {
    GtkEntry *entry = GTK_ENTRY(user_data);
    GtkWindow *window = GTK_WINDOW(gtk_widget_get_root(GTK_WIDGET(entry)));

    if (!validate_question_quantity(entry, window)) {
        return; 
    }

    g_print("Start button clicked and input is valid!\n");
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
    for (int i = 0; i < categories_count; i++) {
        gtk_string_list_append(category_list, categories[i]);
    }
    
    category_dropdown = gtk_drop_down_new(G_LIST_MODEL(category_list), NULL);
    gtk_drop_down_set_selected(GTK_DROP_DOWN(category_dropdown), 0);
    gtk_box_append(GTK_BOX(box), category_dropdown);

    GtkStringList *difficulty_list = gtk_string_list_new(NULL);
    gtk_string_list_append(difficulty_list, "Random");
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

    int last_score = readLastScore();
    char score_text[50];
    snprintf(score_text, sizeof(score_text), "Last Score: %d", last_score);
    last_score_label = gtk_label_new(score_text);
    gtk_widget_set_margin_top(last_score_label, 20);  
    gtk_widget_set_halign(last_score_label, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(box), last_score_label);

    g_signal_connect(start_button, "clicked", G_CALLBACK(on_start_clicked), question_entry);

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

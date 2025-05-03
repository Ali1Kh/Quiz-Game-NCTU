#include <gtk/gtk.h>

static void on_start_clicked(GtkButton *button, gpointer user_data) {
    g_print("Start button clicked!\n");
}

static void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window;
    GtkWidget *box;
    GtkWidget *title;
    GtkWidget *category_dropdown;
    GtkWidget *difficulty_dropdown;
    GtkWidget *question_entry;
    GtkWidget *start_button;

    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Quiz App");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);

    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 15);
    gtk_widget_set_margin_top(box, 40);
    gtk_widget_set_margin_bottom(box, 40);
    gtk_widget_set_margin_start(box, 60);
    gtk_widget_set_margin_end(box, 60);
    gtk_window_set_child(GTK_WINDOW(window), box);

    title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(title), "<span font='30' weight='bold'>Quiz</span>");
    gtk_widget_set_halign(title, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(box), title);

    const char *categories[] = { "Any Category", "Science", "History", "Sports" };
    GListModel *category_model = G_LIST_MODEL(gtk_string_list_new((const char * const *)categories));
    category_dropdown = gtk_drop_down_new(category_model, NULL);
    gtk_drop_down_set_selected(GTK_DROP_DOWN(category_dropdown), 0);
    gtk_box_append(GTK_BOX(box), category_dropdown);

    const char *difficulties[] = { "Random", "Easy", "Medium", "Hard" };
    GListModel *difficulty_model = G_LIST_MODEL(gtk_string_list_new((const char * const *)difficulties));
    difficulty_dropdown = gtk_drop_down_new(difficulty_model, NULL);
    gtk_drop_down_set_selected(GTK_DROP_DOWN(difficulty_dropdown), 0);
    gtk_box_append(GTK_BOX(box), difficulty_dropdown);

    question_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(question_entry), "Number Of Questions");
    gtk_box_append(GTK_BOX(box), question_entry);

    start_button = gtk_button_new_with_label("Start");
    gtk_box_append(GTK_BOX(box), start_button);

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

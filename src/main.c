#include <gtk/gtk.h>

void activate(GtkApplication *app, gpointer user_data) {
    // Create the list models using GListStore (GTK4 recommended)
    GListStore *category_list = g_list_store_new(G_TYPE_STRING);
    GListStore *questions_list = g_list_store_new(G_TYPE_STRING);
    GListStore *difficulty_list = g_list_store_new(G_TYPE_STRING);

    // Add items to the list models
    const gchar *categories[] = {"Math", "Science", "History"};
    const gchar *questions[] = {"Q1", "Q2", "Q3"};
    const gchar *difficulties[] = {"Easy", "Medium", "Hard"};

    for (int i = 0; i < 3; i++) {
        g_list_store_append(category_list, &categories[i]);
        g_list_store_append(questions_list, &questions[i]);
        g_list_store_append(difficulty_list, &difficulties[i]);
    }

    // Create drop-down widgets
    GtkWidget *category_combo = gtk_drop_down_new(G_LIST_MODEL(category_list), NULL);
    GtkWidget *questions_combo = gtk_drop_down_new(G_LIST_MODEL(questions_list), NULL);
    GtkWidget *difficulty_combo = gtk_drop_down_new(G_LIST_MODEL(difficulty_list), NULL);

    // Create a window to display the dropdowns
    GtkWidget *window = gtk_window_new();  // No need to specify GTK_WINDOW_TOPLEVEL in GTK4
    gtk_window_set_title(GTK_WINDOW(window), "Quiz App");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);
    
    // Add the drop-down widgets to the window's layout
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_box_append(GTK_BOX(box), category_combo);
    gtk_box_append(GTK_BOX(box), questions_combo);
    gtk_box_append(GTK_BOX(box), difficulty_combo);
    
    gtk_window_set_child(GTK_WINDOW(window), box);

    // Present the window
    gtk_window_present(GTK_WINDOW(window));  // Replaces gtk_widget_show for windows

    // Set the visibility for drop-downs
    gtk_widget_set_visible(category_combo, TRUE);
    gtk_widget_set_visible(questions_combo, TRUE);
    gtk_widget_set_visible(difficulty_combo, TRUE);
}

int main(int argc, char **argv) {
    // Use G_APPLICATION_DEFAULT_FLAGS instead of deprecated G_APPLICATION_FLAGS_NONE
    GtkApplication *app = gtk_application_new("com.example.quizapp", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}

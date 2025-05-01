#include <gtk/gtk.h>

// Function to handle button click event
static void on_button_clicked(GtkButton *button, gpointer user_data)
{
    g_print("Button clicked\n");
}

// Function to activate the application and display the window
static void activate(GtkApplication *app, gpointer user_data)
{
    GtkWidget *window;
    GtkWidget *button;
    GtkWidget *label;
    GtkWidget *box;

    // Create a new window
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Quiz Game");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);

    // Create a vertical box to arrange widgets inside the window
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_window_set_child(GTK_WINDOW(window), box); // Use gtk_window_set_child instead of gtk_container_add

    // Create a label and add it to the box
    label = gtk_label_new("Welcome to the Quiz Game!");
    gtk_box_append(GTK_BOX(box), label); // Use gtk_box_append instead of gtk_box_pack_start

    // Create a button and add it to the box
    button = gtk_button_new_with_label("Click Me");
    gtk_box_append(GTK_BOX(box), button); // Use gtk_box_append instead of gtk_box_pack_start

    // Connect the button's "clicked" signal to the event handler
    g_signal_connect(button, "clicked", G_CALLBACK(on_button_clicked), NULL);

    // Connect the "destroy" signal to quit the application when the window is closed
    g_signal_connect(window, "destroy", G_CALLBACK(g_application_quit), app);

    // Show the window
    gtk_window_present(GTK_WINDOW(window));
}

int main(int argc, char *argv[])
{
    GtkApplication *app;
    int status;

    // Create a new GTK application
    app = gtk_application_new("org.gtk.example", G_APPLICATION_DEFAULT_FLAGS);

    // Connect the "activate" signal to show the window
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);

    // Run the application
    status = g_application_run(G_APPLICATION(app), argc, argv);

    // Clean up resources
    g_object_unref(app);

    return status;
}

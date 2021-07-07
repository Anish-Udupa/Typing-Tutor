#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define MAX_LENGTH 40

// A custom datatype which stores the score
typedef struct ScoreManager{
    int score;
    int correct_attempt;
    int total_attempt;
    float accuracy;
    int errors;
}ScoreManager;

// A structure containing all the widgets
typedef struct AppWidgets{
    GtkWidget *w_label_score_value;
    GtkWidget *w_label_accuracy_value;
    GtkWidget *w_label_letters_value;
    GtkWidget *w_label_error_value;
    GtkWidget *w_label_row1_left;
    GtkWidget *w_label_row1_mid;
    GtkWidget *w_label_row1_right;
    GtkWidget *w_label_row2;
    GtkWidget *w_label_row3;
}AppWidgets;

typedef struct LineManager{
    char str_row1[40];
    char str_row2[40];
    char str_row3[40];
    int mid_index;
}LineManager;

// Global variables
LineManager *lineManager;
AppWidgets *widgets;
FILE *fp;
ScoreManager *scoreManager;

// Function prototype/declaration
void update_label_row1(char str[], int mid_index, AppWidgets *widgets);
void start(FILE *fp, LineManager *lineManager);
void set_labels();
void get_new_line();
void calculate_accuracy();
void update_score_board();
void isCorrect();
void isIncorrect();

int main(int argc, char *argv[])
{
    // Initializing the GTK window
    GtkBuilder      *builder; 
    GtkWidget       *window;

    // Initialising the AppWidgets
    widgets = (AppWidgets *) malloc(sizeof(AppWidgets));

    // Initializing LineManager
    lineManager = (LineManager *) malloc(sizeof(LineManager));

    // Initializing the scoreManager
    scoreManager = (ScoreManager *) malloc(sizeof(ScoreManager));

    gtk_init(&argc, &argv);

    builder = gtk_builder_new_from_file("glade/window_main.glade");

    window = GTK_WIDGET(gtk_builder_get_object(builder, "window_main"));
    gtk_builder_connect_signals(builder, widgets);

    // Initializing the pointers to the widgets
    widgets->w_label_score_value = GTK_WIDGET(gtk_builder_get_object(builder, "label_score_value"));
    widgets->w_label_accuracy_value = GTK_WIDGET(gtk_builder_get_object(builder, "label_accuracy_value"));
    widgets->w_label_letters_value = GTK_WIDGET(gtk_builder_get_object(builder, "label_letters_value"));
    widgets->w_label_error_value = GTK_WIDGET(gtk_builder_get_object(builder, "label_error_value"));
    widgets->w_label_row1_left = GTK_WIDGET(gtk_builder_get_object(builder, "label_row1_left"));
    widgets->w_label_row1_mid = GTK_WIDGET(gtk_builder_get_object(builder, "label_row1_mid"));
    widgets->w_label_row1_right = GTK_WIDGET(gtk_builder_get_object(builder, "label_row1_right"));
    widgets->w_label_row2 = GTK_WIDGET(gtk_builder_get_object(builder, "label_row2"));
    widgets->w_label_row3 = GTK_WIDGET(gtk_builder_get_object(builder, "label_row3"));

    // Creating a file pointer to paragraph.txt
    fp = fopen("file/paragraph.txt", "r");  // Error: file cant be opened
    if (fp == NULL){  
        printf("Error opening the file. The file doesnt exist.");
        exit(0);
    }
    start(fp, lineManager);

    //update_label_row1("Hello World", 2, widgets);
    
    g_object_unref(builder);

    gtk_widget_show(window);                
    gtk_main();

    // Freeing widgets and lineManager
    free(widgets);
    free(lineManager);
    widgets = NULL;
    lineManager = NULL;
    fclose(fp);
    return 0;
}



// called when window is closed
void on_window_main_destroy()
{
    gtk_main_quit();
}

void on_window_main_key_press_event(GtkWidget *widget, GdkEventKey *event, gpointer user_data){
    printf("Key pressed: %d\n", event->keyval);
    if (event->keyval == (int) lineManager->str_row1[lineManager->mid_index]){
        // Increment score
        isCorrect();
        printf("Correct.\n");
    }
    else{
        // Increment error
        isIncorrect();
        printf("Wrong.\n");
    }
    update_score_board();
    lineManager->mid_index++;
    if(lineManager->mid_index == MAX_LENGTH-1)
        get_new_line();
    else
        update_label_row1(lineManager->str_row1, lineManager->mid_index, widgets);
}

void start(FILE *fp, LineManager *lineManager){
    // Setting the scoreManager
    scoreManager->correct_attempt = 0;
    scoreManager->errors = 0;
    scoreManager->score = 0;
    scoreManager->total_attempt = 0;
    scoreManager->accuracy = 0.0;

    // Setting the lineManager
    lineManager->mid_index = 0;
    fgets(lineManager->str_row1, MAX_LENGTH, fp);
    fgets(lineManager->str_row2, MAX_LENGTH, fp);
    fgets(lineManager->str_row3, MAX_LENGTH, fp);
    set_labels();
    update_score_board();
}

void set_labels(){
    update_label_row1(lineManager->str_row1, lineManager->mid_index, widgets);
    gtk_label_set_text((GtkLabel *) widgets->w_label_row2, lineManager->str_row2);
    gtk_label_set_text((GtkLabel *) widgets->w_label_row3, lineManager->str_row3);
}

void get_new_line(){
    lineManager->mid_index = 0;
    strcpy(lineManager->str_row1, lineManager->str_row2);
    strcpy(lineManager->str_row2, lineManager->str_row3);
    fgets(lineManager->str_row3, MAX_LENGTH, fp);
    set_labels();
}

void update_label_row1(char str[], int mid_index, AppWidgets *widgets){
    char left[MAX_LENGTH], mid[MAX_LENGTH], right[MAX_LENGTH];
    strncpy(left, str, mid_index);
    left[mid_index] = '\0';
    strncpy(mid, &str[mid_index], 1);
    mid[1] = '\0';
    strcpy(right, &str[mid_index+1]);
    gtk_label_set_text((GtkLabel *)widgets->w_label_row1_left, left);
    gtk_label_set_text((GtkLabel *)widgets->w_label_row1_mid, mid);
    gtk_label_set_text((GtkLabel *)widgets->w_label_row1_right, right);
}

void calculate_accuracy(){
    scoreManager->accuracy = (float)scoreManager->correct_attempt/scoreManager->total_attempt;
}

void update_score_board(){
    char str[MAX_LENGTH];
    sprintf(str, "%d", scoreManager->score);
    gtk_label_set_text((GtkLabel *) widgets->w_label_score_value, str);
    sprintf(str, "%.2f", scoreManager->accuracy);
    gtk_label_set_text((GtkLabel *) widgets->w_label_accuracy_value, str);
    sprintf(str, "%d", scoreManager->errors);
    gtk_label_set_text((GtkLabel *) widgets->w_label_error_value, str);
    sprintf(str, "%d", scoreManager->total_attempt);
    gtk_label_set_text((GtkLabel *) widgets->w_label_letters_value, str);
}

void isCorrect(){
    scoreManager->score += 10;
    scoreManager->total_attempt ++;
    scoreManager->correct_attempt ++;
    calculate_accuracy();
}

void isIncorrect(){
    scoreManager->score -= 5;
    scoreManager->total_attempt ++;
    scoreManager->errors++;
    calculate_accuracy();
}
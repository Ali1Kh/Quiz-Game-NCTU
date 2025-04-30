#ifndef USER_H
#define USER_H

typedef struct {
    char username[50];
    char password[50];
    int score;
} User;

int register_user(const char *username, const char *password);
int login_user(const char *username, const char *password);
void save_score(const char *username, int score);

#endif

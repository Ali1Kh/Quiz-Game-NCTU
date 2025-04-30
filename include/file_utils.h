#ifndef FILE_UTILS_H
#define FILE_UTILS_H

int read_users_from_file();
int write_user_to_file(const char *username, const char *password, int score);
int read_questions_from_file();

#endif

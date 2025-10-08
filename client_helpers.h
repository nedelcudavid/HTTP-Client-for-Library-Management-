#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"
#include "parson.h"

#define MAX_COMMAND_SIZE 1024
#define HOST_SIZE 32

int only_letters(char *field);

int check_user_and_pass(char *user, char *pass);

void get_input(char *print1, char *print2, char *field1, char *field2);

char **create_body_data(char *username, char *password);

char *create_body_data_entry(char *key, char *value);

char *connect_to_server(int type, char *url, char *content, char **body_data, int data_count, 
    char **cookies, int cookies_count, char *jwt);

char *extract_error_message(char *response);

void free_array_of_array(char **array, int n);

char *book_url(char *url, char *book_id);

char *get_cookie_login(char *response, int *is_logged_in);

char *get_jwt_token(char *response, int *jwt_is_set);

char **book_infos();

void list_books(char *response, char c);

int is_number(char *number);

int only_letters(char *field);

int check_user_and_pass(char *user, char *pass);

void get_input(char *print1, char *print2, char *field1, char *field2);

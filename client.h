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
#include "client_helpers.h"

#define MAX_COMMAND_SIZE 1024
#define HOST_SIZE 32
#define PORT 8080
#define HOST "34.246.184.49"

void parse_input(int is_logged_in, char **cookies, char *jwt, int jwt_is_set);

void handle_register(int is_logged_in);

void handle_login(int *is_logged_in, char ***cookies);

void handle_enter_library(int is_logged_in, char ***cookies, char **jwt, int *jwt_is_set);

void handle_get_books(int is_logged_in, int jwt_is_set, char **cookies, char *jwt);

void handle_get_book(int is_logged_in, int jwt_is_set, char **cookies, char *jwt);

void handle_add_book(int is_logged_in, int jwt_is_set, char **cookies, char *jwt);

void handle_delete_book(int is_logged_in, int jwt_is_set, char **cookies, char *jwt);

void handle_logout(int *is_logged_in, int *jwt_is_set, char **cookies, char **jwt);

void register_user(char *url, char *content_type);

char *login_user(int *is_logged_in, char *url, char *content_type);

char *enter_library(char *url, char **cookies, int cookies_count, int *jwt_is_set);

void get_books(char *url, char **cookies, int cookies_count, char *jwt);

void get_book(char *url, char **cookies, int cookies_count, char *jwt);

void add_book(char *url, char **cookies, int cookies_count, char *jwt);

void delete_book(char *url, char **cookies, int cookies_count, char *jwt);

void logout(char *url, char **cookies, int cookies_count, char *jwt);

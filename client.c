#include "client.h"

void handle_register(int is_logged_in) {
    if (is_logged_in) {
        printf("You are already logged in\n");
    } else {
        register_user("/api/v1/tema/auth/register", "application/json");
    }
}

void handle_login(int *is_logged_in, char ***cookies) {
    if (*is_logged_in) {
        printf("You are already logged in\n");
    } else {
        (*cookies)[0] = login_user(is_logged_in, "/api/v1/tema/auth/login", "application/json");
    }
}

void handle_enter_library(int is_logged_in, char ***cookies, char **jwt, int *jwt_is_set) {
    if (!is_logged_in) {
        printf("You must be logged in to enter the library\n");
    } else {
        *jwt = enter_library("/api/v1/tema/library/access", *cookies, 1, jwt_is_set);
    }
}

void handle_get_books(int is_logged_in, int jwt_is_set, char **cookies, char *jwt) {
    if (!is_logged_in) {
        printf("You must be logged in to enter the library\n");
    } else if (!jwt_is_set) {
        printf("You must be in the library to get books\n");
    } else {
        get_books("/api/v1/tema/library/books", cookies, 1, jwt);
    }
}

void handle_get_book(int is_logged_in, int jwt_is_set, char **cookies, char *jwt) {
    if (!is_logged_in) {
        printf("You must be logged in to enter the library\n");
    } else if (!jwt_is_set) {
        printf("You must be in the library to get books\n");
    } else {
        get_book("/api/v1/tema/library/books", cookies, 1, jwt);
    }
}

void handle_add_book(int is_logged_in, int jwt_is_set, char **cookies, char *jwt) {
    if (!is_logged_in) {
        printf("You must be logged in to enter the library\n");
    } else if (!jwt_is_set) {
        printf("You must be in the library to add books\n");
    } else {
        add_book("/api/v1/tema/library/books", cookies, 1, jwt);
    }
}

void handle_delete_book(int is_logged_in, int jwt_is_set, char **cookies, char *jwt) {
    if (!is_logged_in) {
        printf("You must be logged in to enter the library\n");
    } else if (!jwt_is_set) {
        printf("You must be in the library to delete books\n");
    } else {
        delete_book("/api/v1/tema/library/books", cookies, 1, jwt);
    }
}

void handle_logout(int *is_logged_in, int *jwt_is_set, char **cookies, char **jwt) {
    if (!(*is_logged_in)) {
        printf("You are already logged out\n");
    } else {
        logout("/api/v1/tema/auth/logout", cookies, 1, *jwt);
        *is_logged_in = 0;
        *jwt_is_set = 0;
    }
}

void register_user(char *url, char *content_type) {
    // Function to register a user

    // Get the username and password from user input
    char username[MAX_COMMAND_SIZE];
    char password[MAX_COMMAND_SIZE];
    get_input("username=", "password=", username, password);

    // Check if the username and password meet the required format
    if (check_user_and_pass(username, password)) {

        char **body_data = create_body_data(username, password);
        // Connection
        char *response = connect_to_server(1, url, content_type, body_data, 2, NULL, 0, NULL);

        // Check if the response contains an error
        if (strstr(response, "error") != NULL) {
            // Extract and print the error message
            char *err_msg = extract_error_message(response);
            printf("%s\n", err_msg);
            free(err_msg);
        } else {
            printf("User registered successfully!\n");
        }
        free(response);
        free_array_of_array(body_data, 2);
    } else {
        printf("Invalid username or password format\n");
    }
}

char *login_user(int *is_logged_in, char *url, char *content_type) {
    // Function to login a user

    // Get the username and password from user input
    char username[MAX_COMMAND_SIZE];
    char password[MAX_COMMAND_SIZE];
    get_input("username=", "password=", username, password);

    // Create body data with the username and password
    char **body_data = create_body_data(username, password);

    // Connection
    char *response = connect_to_server(1, url, content_type, body_data, 2, NULL, 0, NULL);

    if (strstr(response, "error") != NULL) {
        // If the response contains an error, extract the error message and print it
        char *err_msg = extract_error_message(response);
        printf("%s\n", err_msg);
        free(err_msg);
        free(response);
        return NULL;
    } else {
        // If the response does not contain an error, get the login cookie and update the logged-in status
        char *cookie = get_cookie_login(response, is_logged_in);
        free(response);
        return cookie;
    }
}

char *enter_library(char *url, char **cookies, int cookies_count, int *jwt_is_set)
{
    // Function to enter the library

    // Connection
    char *response = connect_to_server(2, url, NULL, NULL, 0, cookies, cookies_count, NULL);

    if (strstr(response, "error") != NULL) {
        // If the response contains an error, extract the error message and print it
        char *err_msg = extract_error_message(response);
        *jwt_is_set = 0;
        printf("%s\n", err_msg);
        free(err_msg);
        free(response);
        return NULL;
    } else {
        // If the response does not contain an error, get the JWT token and update the JWT status
        char *token = get_jwt_token(response, jwt_is_set);
        free(response);
        return token;
    }
}

void get_books(char *url, char **cookies, int cookies_count, char *jwt)
{
    // Function to get the user's books

    // Connection
    char *response = connect_to_server(2, url, NULL, NULL, 0, cookies, cookies_count, jwt);

    if (strstr(response, "error") != NULL) {
        // Error
        char *err_msg = extract_error_message(response);
        printf("%s\n", err_msg);
        free(err_msg);
        free(response);
    } else {
        // If the response does not contain an error, list the books
        list_books(response, '[');
        free(response);
    }
}

void get_book(char *url, char **cookies, int cookies_count, char *jwt)
{
    // Function to get information about a specific book

    // Get the book id from user
    char *book_id = calloc(LINELEN, sizeof(char));
    printf("id=");
    fgets(book_id, LINELEN, stdin);
    book_id[strlen(book_id) - 1] = '\0';

    // Check if the book ID is valid
    if (check_user_and_pass(book_id, book_id) == 0) {
        printf("Invalid book id\n");
        free(book_id);
        return;
    }

    char *new_url = book_url(url, book_id);

    // Connection
    char *response = connect_to_server(2, new_url, NULL, NULL, 0, cookies, cookies_count, jwt);

    if (strstr(response, "error") != NULL) {
        // Error
        char *err_msg = extract_error_message(response);
        printf("%s\n", err_msg);
        free(err_msg);
        free(response);
    } else {
        // If the response does not contain an error, list the book
        list_books(response, '{');
        free(response);
    }

    free(new_url);
    free(book_id);
}

void add_book(char *url, char **cookies, int cookies_count, char *jwt)
{
    // Function to add a book

    // Get book information from user input
    char **infos = book_infos();

    // If the information is invalid, return
    if (infos == NULL) {
        return;
    }

    char **body_data = calloc(5, sizeof(char *));

    // Create JSON strings for each book information
    body_data[0] = calloc(LINELEN, sizeof(char));
    sprintf(body_data[0], "\"title\": \"%s\"", infos[0]);

    body_data[1] = calloc(LINELEN, sizeof(char));
    sprintf(body_data[1], "\"author\": \"%s\"", infos[1]);

    body_data[2] = calloc(LINELEN, sizeof(char));
    sprintf(body_data[2], "\"genre\": \"%s\"", infos[2]);

    body_data[3] = calloc(LINELEN, sizeof(char));
    sprintf(body_data[3], "\"page_count\": \"%s\"", infos[3]);

    body_data[4] = calloc(LINELEN, sizeof(char));
    sprintf(body_data[4], "\"publisher\": \"%s\"", infos[4]);

    // Connection
    char *response = connect_to_server(1, url, "application/json", body_data, 5, cookies, cookies_count, jwt);
    
    if (strstr(response, "error") != NULL) {
        // Error
        char *err_msg = extract_error_message(response);
        printf("%s\n", err_msg);
        free(err_msg);
        free(response);
    } else {
        // If the response does not contain an error, print success message
        printf("Successfully added book\n");
        free(response);
    }

    free_array_of_array(body_data, 5);
}

void delete_book(char *url, char **cookies, int cookies_count, char *jwt)
{
    // Function to delete a book

    // Get the book ID from user input
    char *book_id = calloc(LINELEN, sizeof(char));
    printf("id=");
    fgets(book_id, LINELEN, stdin);
    book_id[strlen(book_id) - 1] = '\0';

    // Book id is invalid
    if (check_user_and_pass(book_id, book_id) == 0) {
        printf("Invalid book id\n");
        return;
    }

    char *new_url = book_url(url, book_id);

    // Connection
    char *response = connect_to_server(3, new_url, NULL, NULL, 0, cookies, cookies_count, jwt);
    
    if (strstr(response, "error") != NULL) {
        // Error
        char *err_msg = extract_error_message(response);
        printf("%s\n", err_msg);
        free(err_msg);
        free(response);
    } else {
        // Success
        printf("Successfully deleted book\n");
        free(response);
    }

    free(new_url);
}

void logout(char *url, char **cookies, int cookies_count, char *jwt)
{
    // Function to logout

    // Connection
    char *response = connect_to_server(2, url, NULL, NULL, 0, cookies, cookies_count, jwt);

    if (strstr(response, "error") != NULL) {
        // Error
        char *err_msg = extract_error_message(response);
        printf("%s\n", err_msg);
        free(err_msg);
        free(response);
    } else {
        // If the response does not contain an error, print success message
        printf("Successfully logged out\n");
        free(response);
    }

}

void parse_input(int is_logged_in, char **cookies, char *jwt, int jwt_is_set) {
    while (1) {
        char command[MAX_COMMAND_SIZE];
        fgets(command, MAX_COMMAND_SIZE, stdin);
        command[strlen(command) - 1] = '\0';

        if (!strcmp(command, "register")) {
            handle_register(is_logged_in);
        } else if (!strcmp(command, "login")) {
            handle_login(&is_logged_in, &cookies);
        } else if (!strcmp(command, "enter_library")) {
            handle_enter_library(is_logged_in, &cookies, &jwt, &jwt_is_set);
        } else if (!strcmp(command, "get_books")) {
            handle_get_books(is_logged_in, jwt_is_set, cookies, jwt);
        } else if (!strcmp(command, "get_book")) {
            handle_get_book(is_logged_in, jwt_is_set, cookies, jwt);
        } else if (!strcmp(command, "add_book")) {
            handle_add_book(is_logged_in, jwt_is_set, cookies, jwt);
        } else if (!strcmp(command, "delete_book")) {
            handle_delete_book(is_logged_in, jwt_is_set, cookies, jwt);
        } else if (!strcmp(command, "logout")) {
            handle_logout(&is_logged_in, &jwt_is_set, cookies, &jwt);
        } else if (!strcmp(command, "exit")) {
            exit(0);
        } else {
            printf("Unknown command! Known commands are: register, login, enter_library, get_book, get_books, add_book, delete_book, logout, and exit!\n");
        }
    }
}

int main(int argc, char *argv[]) {
    int is_logged_in = 0;
    char **cookies = calloc(10, sizeof(char *));
    char *jwt = NULL;
    int jwt_is_set = 0;

    parse_input(is_logged_in, cookies, jwt, jwt_is_set);

    free(cookies[0]);
    free(cookies);
    free(jwt);

    return 0;
}

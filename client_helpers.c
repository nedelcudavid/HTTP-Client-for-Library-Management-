#include "client_helpers.h"
#include "client.h"

int only_letters(char *field) {
    // check if field contains only letters
    for (int i = 0; i < strlen(field) - 1; i++) {
        if ((field[i] < 'A' || field[i] > 'Z') && (field[i] < 'a' || field[i] > 'z') && field[i] != ' ') {
            return 0;
        }
    }
    return 1;
}

int is_number(char *number) {
    // check if contains only digits
    for (int i = 0; i < strlen(number) - 1; i++) {
        if (number[i] < '0' || number[i] > '9') {
            return 0;
        }
    }
    return 1;
}


int check_user_and_pass(char *user, char *pass) {
    // check if the user and pass have any space characters
    if (strchr(user, ' ') == NULL && strchr(pass, ' ') == NULL)
    {
        return 1;
    }

    return 0;
}

char *book_url(char *url, char *book_id) {
    // Create the new URL by appending the book ID to the original URL
    char *new_url = calloc(200, sizeof(char));
    strcat(new_url, url);
    strcat(new_url, "/");
    strcat(new_url, book_id);   
    return new_url;
}

char *connect_to_server(int type, char *url, char *content, char **body_data, int data_count, 
    char **cookies, int cookies_count, char *jwt) {
    // Connect to server
    int sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
    char *message;
    if (type == 1) {
        message = compute_post_request(HOST, url, content, body_data, data_count, cookies, cookies_count, jwt);
    } else if (type == 2) {
        message = compute_get_request(HOST, url, NULL, cookies, cookies_count, jwt);
    } else {
        message = compute_delete_request(HOST, url, NULL, cookies, cookies_count, jwt);
    }
    send_to_server(sockfd, message);
    char *response = receive_from_server(sockfd);
    close_connection(sockfd);
    free(message);
    return response;
}

void get_input(char *print1, char *print2, char *field1, char *field2) {
    printf("%s", print1);
    fgets(field1, MAX_COMMAND_SIZE, stdin);
    field1[strlen(field1) - 1] = '\0';

    printf("%s", print2);
    fgets(field2, MAX_COMMAND_SIZE, stdin);
    field2[strlen(field2) - 1] = '\0';
}

char **book_infos() {
    char **infos = calloc(5, sizeof(char *));

    // Get the title
    printf("title=");
    char title[MAX_COMMAND_SIZE];
    fgets(title, MAX_COMMAND_SIZE, stdin);
    title[strlen(title) - 1] = '\0';
    infos[0] = title; // Store the title in the array

    // Get the author
    printf("author=");
    char author[MAX_COMMAND_SIZE];
    fgets(author, MAX_COMMAND_SIZE, stdin);
    author[strlen(author) - 1] = '\0';
    infos[1] = author; // Store the author in the array

    // Get the genre
    printf("genre=");
    char genre[MAX_COMMAND_SIZE];
    fgets(genre, MAX_COMMAND_SIZE, stdin);
    genre[strlen(genre) - 1] = '\0';
    infos[2] = genre; // Store the genre in the array

    // Get the page count
    printf("page_count=");
    char page_count[MAX_COMMAND_SIZE];
    fgets(page_count, MAX_COMMAND_SIZE, stdin);
    if(is_number(page_count) == 0) {
        printf("Invalid number of pages!\n");
        return NULL; // the page count is not a valid number
    }
    page_count[strlen(page_count) - 1] = '\0';
    infos[3] = page_count; // Store the page count in the array

    // Get the publisher
    printf("publisher=");
    char publisher[MAX_COMMAND_SIZE];
    fgets(publisher, MAX_COMMAND_SIZE, stdin);
    publisher[strlen(publisher) - 1] = '\0';
    infos[4] = publisher; // Store the publisher in the array
    
    return infos;
}

char **create_body_data(char *username, char *password) {
    char **body_data = calloc(2, sizeof(char *));
    body_data[0] = create_body_data_entry("username", username);
    body_data[1] = create_body_data_entry("password", password);
    return body_data;
}

char *create_body_data_entry(char *key, char *value) {
    int entry_size = strlen(key) + strlen(value) + 6;
    char *entry = calloc(entry_size, sizeof(char));
    sprintf(entry, "\"%s\": \"%s\"", key, value);
    return entry;
}

char *extract_error_message(char *response) {
    char *err_start = strstr(response, "error") + 8;
    char *err_end = strstr(response, "\"}");
    int len = err_end - err_start;
    char *err_msg = calloc(len + 1, sizeof(char));
    strncpy(err_msg, err_start, len);
    return err_msg;
}

void free_array_of_array(char **array, int n) {
    for (int i = 0; i < n; i++) {
        free(array[i]);
    }
    free(array);
}

char *get_cookie_login(char *response, int *is_logged_in) {
    *is_logged_in = 1;
    char *cookie_start = strstr(response, "connect.sid");
    char *cookie_end = strstr(cookie_start, ";");
    char *cookie = calloc(200, sizeof(char));
    strncpy(cookie, cookie_start, cookie_end - cookie_start);
    printf("User logged in successfully!\n");
    return cookie;
}

char *get_jwt_token(char *response, int *jwt_is_set) {
    *jwt_is_set = 1;
    char *token_end = strstr(response, "\"}");
    *(token_end) = '\0';
    char *token_start = strstr(response, "{\"token\":");
    token_start += 10;
    char *token = calloc(500, sizeof(char));
    strncpy(token, token_start, 500);
    printf("Successfully entered the library\n");
    return token;
}

void list_books(char *response, char c) {
    // Function to list books

    // Find the start and end positions of the book section in the response
    char *books_start = strchr(response, c);
    char *books_end = strchr(response, c + 2);
    books_end += 1;
    *books_end = '\0';
    int len = books_end - books_start;
    char *books = calloc(len + 1, sizeof(char));
    strncpy(books, books_start, len);

    // Parse the books string as JSON
    JSON_Value *value;
    value = json_parse_string(books);
    // Serialize the JSON value to a pretty formatted string
    char *pretty = json_serialize_to_string_pretty(value);
    printf("%s\n", pretty);

    json_free_serialized_string(pretty);
    json_value_free(value);
    free(books);
}

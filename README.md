# HTTP-Client for Library Management System
## Copyright 2023 Nedelcu Andrei-David
___________________________________________________________________________________________


### General Description
The code implements a program that allows you to perform various actions, such as POST, GET, and DELETE, CRUD actions.
In addition from the class skeleton, I added a field named jwt that contains the token required for authentication with the library.

### Implementation
The program includes a command parser that handles different actions,
including register, login, enter_library, get_books, get_book, add_book,
delete_book, logout, and exit.

Each action is implemented as a separate function, and the general structure
of these functions is similar. Here is an overview of their flow:

* checking the additional parameters required for the action, such as an ID
for get_book or delete_book, or a title for add_book

* validating optional inputs, ensuring that there are no unnecessary spaces or
that the input contains only valid characters, such as in the case of an author’s name

* selecting the corresponding function from request.c based on the desired action
(compute_get_message, post_message, or delete_message) + choosing the function parameters accordingly

* sending the constructed message to the server and receiving the response

* if the response contains an error, displaying the error message; otherwise,
displaying a success message

The code follows this general pattern for each action, ensuring consistent
handling of requests and responses.
For a nicer display of books, the Parson library was used.

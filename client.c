#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <sys/wait.h>
#include "helper_funct.h"

#define PORT 2024
#define DEFAULT_ADDR "127.0.0.1"

int isLoggedIn = 0;
int isAdmin = 0;
char activeUsername[11];

/// Reads the command from the console.
void ReadCommand(char * msg, unsigned len)
{
    bzero(msg, len);
    printf(">> ");
    fflush(stdout);
    int bytesRead = read(0, msg, len - 1);
    if (bytesRead == -1) {
        printf("[Client] Error at reading from console!\n");
        exit(-1);
    }
    if (bytesRead == 0)
    {
        printf("[Client] Warning! Read 0 bytes from console!\n");
        exit(-1);
    }
    // printf("[Client] Read %d bytes from console.\n", bytesRead);

    // Removing the new line character
    for (int i = 0; i < len; ++i)
        if (msg[i] == '\n')
            msg[i] = 0;
    msg[len - 1] = 0;

    // Clearing the input buffer
    if (bytesRead == len - 1)
        ClearInput();
}

/// Send a request to the specified server.
void SendRequest(int server, void * msg, unsigned len)
{
    int bytesWritten = write(server, msg, len);
    if (bytesWritten == -1) {
        perror("[Client] Error at writing to server!\n");
        exit(-1);
    }
    if (bytesWritten == 0) {
        printf("[Client] Warning! Written 0 bytes to server!\n");
        exit(-1);
    }
    // printf("DEBUG Message sent successfuly! Bytes written : %d, message = '%s'.\n", bytesWritten, (char*)msg);
}

/// Reads from the server in a specified buffer (msg).
void ReadFromServer(int server, void * msg, unsigned len)
{
    bzero(msg, len);
    int bytesRead = read(server, msg, len);
    if (bytesRead == -1) {
        perror("[Client] Error at reading from server!\n");
        exit(-1);
    }
    if (bytesRead == 0) {
        printf("[Client] Warning! Read 0 bytes from server!\n");
        exit(-1);
    }
    // printf("[Client] Read %d from server.\n", bytesRead);
}

/// Interprets the server's response and prints out a message to the client.
void InterpretResponse(int server, char * response, unsigned len)
{
    if (strncmp(response, "register:success", strlen("register:success")) == 0) {
        printf("\n----------------------------------------------------------------------------------------------------------------------\n");
        printf("Registration complete! You can now log in with your account!\n");
        printf("\n----------------------------------------------------------------------------------------------------------------------\n");
        return;
    }
    
    if (strncmp(response, "register:failed", strlen("register:failed")) == 0) {
        printf("\n----------------------------------------------------------------------------------------------------------------------\n");
        printf("Registration was unsuccessful. The username or account already exist!\n");
        printf("\n----------------------------------------------------------------------------------------------------------------------\n");
        return;
    }

    if (strncmp(response, "register:error", strlen("register:error")) == 0) {
        printf("\n----------------------------------------------------------------------------------------------------------------------\n");
        printf("Due to an error, the registration could not be completed. Try again later!\n");
        printf("\n----------------------------------------------------------------------------------------------------------------------\n");
        return;
    }



    if (strncmp(response, "login:success", strlen("login:success")) == 0) {
        printf("\n----------------------------------------------------------------------------------------------------------------------\n");
        printf("You are now logged in! Welcome!\n");
        printf("\n----------------------------------------------------------------------------------------------------------------------\n");
        isLoggedIn = 1;
        return; 
    }
    
    if (strncmp(response, "login:failed", strlen("login:failed")) == 0) {
        printf("\n----------------------------------------------------------------------------------------------------------------------\n");
        printf("The username or password is incorrect!\n");
        printf("\n----------------------------------------------------------------------------------------------------------------------\n");
        bzero(activeUsername, sizeof(activeUsername));
        isAdmin = 0;
        return; 
    }
    
    if (strncmp(response, "login:error", strlen("login:error")) == 0) {
        printf("\n----------------------------------------------------------------------------------------------------------------------\n");
        printf("Due to an error, the login process failed.\n Try again later!\n");
        printf("\n----------------------------------------------------------------------------------------------------------------------\n");
        bzero(activeUsername, sizeof(activeUsername));
        isAdmin = 0;
        return;
    }



    if (strncmp(response, "surf:success", strlen("surf:success")) == 0) {
        struct PostData post;
        ReadFromServer(server, &post, sizeof(struct PostData));
        printf("\n----------------------------------------------------------------------------------------------------------------------\n");
        printf("Posted by: %s\n", post.owner);
        if (post.groupID == 0) {
            printf("Shared with: public\n");
        }
        else if (post.groupID == 1) {
            printf("Shared with: family\n");
        }
        else if (post.groupID == 2) {
            printf("Shared with: friends\n");
        }
        printf("Date of posting: %s\n", ctime(&post.data));
        printf("%s\n", post.txt);
        printf("\n----------------------------------------------------------------------------------------------------------------------\n");
        return;
    }
    
    if (strncmp(response, "surf:error", strlen("surf:error")) == 0) {
        printf("\n----------------------------------------------------------------------------------------------------------------------\n");
        printf("An error occured while trying to get a post.\n");
        printf("\n----------------------------------------------------------------------------------------------------------------------\n");
        return;
    }



    if (strncmp(response, "post:success", strlen("post:success")) == 0) {
        printf("\n----------------------------------------------------------------------------------------------------------------------\n");
        printf("Your post was successfully published!\n");
        printf("\n----------------------------------------------------------------------------------------------------------------------\n");
        return;
    }

    if (strncmp(response, "post:error", strlen("post:error")) == 0) {
        printf("\n----------------------------------------------------------------------------------------------------------------------\n");
        printf("An error occured and the post was not published.\n Please try again later!\n");
        printf("\n----------------------------------------------------------------------------------------------------------------------\n");
        return;
    }



    if (strncmp(response, "message:success", strlen("message:success")) == 0) {
        printf("\n----------------------------------------------------------------------------------------------------------------------\n");
        printf("Your message was sent successfuly!\n");
        printf("\n----------------------------------------------------------------------------------------------------------------------\n");
        return;
    }

    if (strncmp(response, "message:failed", strlen("message:failed")) == 0) {
        printf("\n----------------------------------------------------------------------------------------------------------------------\n");
        printf("The user does not exist. Maybe you typed it wrong?\n");
        printf("\n----------------------------------------------------------------------------------------------------------------------\n");
        return;
    }

    if (strncmp(response, "message:error", strlen("message:error")) == 0) {
        printf("\n----------------------------------------------------------------------------------------------------------------------\n");
        printf("An error occured while trying to sent your message.\n");
        printf("\n----------------------------------------------------------------------------------------------------------------------\n");
        return;
    }



    if (strncmp(response, "add:success", strlen("add:success")) == 0) {
        printf("\n----------------------------------------------------------------------------------------------------------------------\n");
        printf("The user was added to the group!\n");
        printf("\n----------------------------------------------------------------------------------------------------------------------\n");
        return;        
    }

    if (strncmp(response, "add:failed", strlen("add:failed")) == 0) {
        printf("\n----------------------------------------------------------------------------------------------------------------------\n");
        printf("The user does not exists!\n");
        printf("\n----------------------------------------------------------------------------------------------------------------------\n");
        return;        
    }

    if (strncmp(response, "add:exists", strlen("add:exists")) == 0) {
        printf("\n----------------------------------------------------------------------------------------------------------------------\n");
        printf("The user already exists in the specified grup!\n");
        printf("\n----------------------------------------------------------------------------------------------------------------------\n");
        return; 
    }

    if (strncmp(response, "add:error", strlen("add:error")) == 0) {
        printf("\n----------------------------------------------------------------------------------------------------------------------\n");
        printf("Due to an error, the user was not added to the specified group. \nTry again later!\n");
        printf("\n----------------------------------------------------------------------------------------------------------------------\n");
        return;
    }



    if (strncmp(response, "read:success", strlen("read:success")) == 0) {
        printf("\n----------------------------------------------------------------------------------------------------------------------\n");
        struct MessageData m;
        ReadFromServer(server, &m, sizeof(struct MessageData));
        printf("Sent by: %s\n", m.from);
        printf("To: %s\n", m.to);
        printf("Date: %s\n\n", ctime(&m.date));
        printf("%s\n", m.txt);
        printf("\n----------------------------------------------------------------------------------------------------------------------\n");
        return;  
    }

    if (strncmp(response, "read:empty", strlen("read:empty")) == 0) {
        printf("\n----------------------------------------------------------------------------------------------------------------------\n");
        printf("You have no new messages!\n");
        printf("\n----------------------------------------------------------------------------------------------------------------------\n");
        return;  
    }

    if (strncmp(response, "read:error", strlen("read:error")) == 0) {
        printf("\n----------------------------------------------------------------------------------------------------------------------\n");
        printf("Due to an error, your message could not be received!\n");
        printf("\n----------------------------------------------------------------------------------------------------------------------\n");
        return;  
    }


    if (strncmp(response, "masterkey:success", strlen("masterkey:success")) == 0) {
        printf("\n----------------------------------------------------------------------------------------------------------------------\n");
        printf("Masterkey changed successfully!\n");
        printf("\n----------------------------------------------------------------------------------------------------------------------\n");
        return;
    }

    if (strncmp(response, "masterkey:failed", strlen("masterkey:failed")) == 0) {
        printf("\n----------------------------------------------------------------------------------------------------------------------\n");
        printf("The old Master Key does not match with the current Master Key!\n");
        printf("This could happen if two admins try to change the key at the\n");
        printf("same time.\n");
        printf("\n----------------------------------------------------------------------------------------------------------------------\n");
        return;
    }

    if (strncmp(response, "masterkey:error", strlen("masterkey:error")) == 0) {
        printf("\n----------------------------------------------------------------------------------------------------------------------\n");
        printf("Due to an error, the process failed. Try again later!\n");
        printf("\n----------------------------------------------------------------------------------------------------------------------\n");
        return;
    }

}


/// User registration interface
void RegUser(char * req)
{
    printf("\n----------------------------------------------------------------------------------------------------------------------\n");
    printf("Please insert an username (up to 10 characters) and a password (up to 10 characters).\n\n");
    char username[11];
    char password[11];

    printf("Username: \n");
    ReadCommand(username, sizeof(username));


    printf("Password: \n");
    ReadCommand(password, sizeof(password));
    
    printf("\nHere are your credentials:\nUsername: '%s'\nPassword: '%s'\n\n", username, password);

    bzero(req, sizeof(req));
    strcat(req, "register:user:");
    strcat(req, username);
    strcat(req, ":");
    strcat(req, password);
    return;
}

/// Login interface
void Login(char * req)
{
    printf("\n----------------------------------------------------------------------------------------------------------------------\n");
    
    char ch[2];
    printf("As ADMIN or USER? [A/U]\n");
    ReadCommand(ch, sizeof(ch));

    printf("Username: \n");
    char username[11];
    ReadCommand(username, sizeof(username));

    printf("Password: \n");
    char password[11];
    ReadCommand(password, sizeof(password));

    bzero(req, sizeof(req));
    if (ch[0] == 'A' || ch[0] == 'a')
    {
        strcat(req, "login:admin:");
        isAdmin = 1;
    }
    if (ch[0] == 'U' || ch[0] == 'u')
        strcat(req, "login:user:");
    strcat(req, username);
    strcat(req, ":");
    strcat(req, password);
    strcpy(activeUsername, username);
}


/// Parses the command given and returns:
///
/// 0 - the command was NULL or an invalid one
/// 1 - the command needs to write a request to the server
/// 2 - the command doesn't need to write a request (for the 'help' command)
int ParseCommand(int server, char * cmd, char * request)
{
    if (cmd == NULL || strlen(cmd) == 0) return 0;

    if (strncmp(cmd, "help", strlen("help")) == 0) {
        printf("\n----------------------------------------------------------------------------------------------------------------------\n");
        printf("Here's a list of functions to get you started:\n");
        printf("-> register\n-> login\n-> surf \n-> help\n-> quit\n");
        printf("----------------------------------------------------------------------------------------------------------------------\n\n");
        return 2;    
    } 
    
    if (strncmp(cmd, "register", strlen("register")) == 0) {
        printf("\n----------------------------------------------------------------------------------------------------------------------\n");
        printf("Would you like to register as an 'admin' or a 'user'?\n");
        printf("[Write the type down below]\n\n");

        read_command:
        ReadCommand(cmd, sizeof(cmd));
        
        if (strcmp(cmd, "admin") == 0) {
            // RegAdmin(request);
            // printf("\n----------------------------------------------------------------------------------------------------------------------\n");
            
            int masterKey = -1;
            int masterKeyRead;
            char mk[10];
            int remainingChances = 3;

            bzero(request, strlen(request));
            strcat(request, "masterkey:request");
            SendRequest(server, request, strlen(request));

            char response[100];
            bzero(response, sizeof(response));
            ReadFromServer(server, response, sizeof(response));
            if (strncmp(response, "masterkey:request:error", strlen("masterkey:request:error")) == 0) {
                printf("Due to an error, the registration could not be completed!\nTry again later!\n");
                printf("\n----------------------------------------------------------------------------------------------------------------------\n");
                return 3;
            }

            ReadFromServer(server, &masterKey, sizeof(masterKey));
            
            printf("Before you set up an 'admin' account, please insert the Master Key!\n\n");
            mk_read:

            ReadCommand(mk, sizeof(mk));
            masterKeyRead = atoi(mk);

            if (masterKeyRead == masterKey) {
                printf("\n----------------------------------------------------------------------------------------------------------------------\n");
                printf("Please insert an username (up to 10 characters) and a password (up to 10 characters).\n\n");
                char username[11];
                char password[11];

                printf("Username: \n");
                ReadCommand(username, sizeof(username));

                printf("Password: \n");
                ReadCommand(password, sizeof(password));
                
                printf("Here are your credentials:\nUsername: '%s'\nPassword: '%s'\n\n", username, password);

                bzero(request, sizeof(request));
                strcat(request, "register:admin:");
                strcat(request, username);
                strcat(request, ":");
                strcat(request, password);
            } 
            else {
                remainingChances--;
                if (remainingChances == 0) {
                    printf("You inserted the wrong Master Key too many times. Shuting down the connection...\n");
                    exit(0);
                }
                printf("You inserted the wrong Master Key (%d)! %d chances remaining!\n", masterKeyRead, remainingChances);
                goto mk_read;
            }
            SendRequest(server, request, strlen(request));
            return 1;
        } 
        else if (strcmp(cmd, "user") == 0) {
            RegUser(request);
            SendRequest(server, request, strlen(request));
            return 1;
        } 
        else {
            printf("Please insert a valid type of registration.\n");
            goto read_command;
        }
    }
    
    if (strncmp(cmd, "login", strlen("login")) == 0) {
        Login(request);
        SendRequest(server, request, strlen(request));
        
        // ReadFromServer(server, response, sizeof(response));
        // InterpretResponse(server, response, sizeof(response));

        return 1;
    }
    
    if (strncmp(cmd, "surf", strlen("surf")) == 0) {
        bzero(request, strlen(request));
        strcat(request, "surf:nologin");
        SendRequest(server, request, strlen(request));

        // ReadFromServer(server, response, sizeof(response));
        // InterpretResponse(server, response, sizeof(response));
        return 1;
    }
    
    if (strncmp(cmd, "quit", strlen("quit")) == 0) {
        printf("\n----------------------------------------------------------------------------------------------------------------------\n");
        printf("Thank you for using WON! Have a nice day! :)\n");
        printf("----------------------------------------------------------------------------------------------------------------------\n");
        
        bzero(request, strlen(request));
        strcat(request, "quit");
        SendRequest(server, request, strlen(request));
        exit(0);
    }
    
    return 0;
}


/// Parses the command while logged in, either as an admin or a user, and returns:
///
/// 0 - the command is invalid
/// 1 - the command needs to make a request
/// 2 - the command doesn't need to make a request
int ParseCommandLoggedIn(int server, char * cmd, char * request) {

    if (cmd == NULL || strlen(cmd) == 0) return 0;
    
    if (strncmp(cmd, "help", strlen("help")) == 0) {
        printf("\n----------------------------------------------------------------------------------------------------------------------\n");
        printf("Here is what you can do:\n");
        if (isAdmin == 0)
            printf("-> post\n-> message [USER_1] [...]\n-> surf\n-> add [USER] [GROUP]\n-> read\n-> help\n-> quit\n");
        else
            printf("-> post\n-> message [USER_1] [...]\n-> surf\n-> add [USER] [GROUP]\n-> read\n-> masterkey\n-> help\n-> quit\n");
        
        printf("----------------------------------------------------------------------------------------------------------------------\n\n");
        return 2;    
    }

    if (strncmp(cmd, "post", strlen("post")) == 0) {
        printf("\n----------------------------------------------------------------------------------------------------------------------\n");
        printf("Select to whom to share your post:\n");
        printf("[0] - public\n[1] - family\n[2] - friends\n\n");
        
        int groupID = 0;
        
        while (1) {
            printf(">> ");
            fflush(stdout);
            scanf("%d", &groupID);
            if (groupID < 0 || groupID > 2) {
                printf("Please enter a valid group.\n");
            }
            else
            {
                break;
            }
        }

        struct PostData post;
        bzero(&post, sizeof(struct PostData));
        InitPostData(&post, activeUsername, groupID);

        printf("\nNow enter the text you wish to post. Keep in mind that:\n");
        printf("-> the post's limit is 500 characters\n-> to submit, you must press [ENTER]\n\n");
        
        ReadCommand(post.txt, sizeof(post.txt));

        bzero(request, strlen(request));
        strcat(request, "post");
        
        
        SendRequest(server, request, strlen(request));
        SendRequest(server, &post, sizeof(struct PostData));

        printf("----------------------------------------------------------------------------------------------------------------------\n\n");
        return 1;    
    }
    
    if (strncmp(cmd, "message ", strlen("message ")) == 0) {
        printf("\n----------------------------------------------------------------------------------------------------------------------\n");
    
        char to[11];
        bzero(to, sizeof(to));
        char * p;
        struct MessageData message;

        InitMessageData(&message, activeUsername, to);
        printf("Input your message (maximum number of characters is 200, press [ENTER] to send):\n");
        ReadCommand(message.txt, sizeof(message.txt));
        printf("\n----------------------------------------------------------------------------------------------------------------------\n");

        p = strtok(cmd + strlen("message "), " \n");
        while (p)
        {
            bzero(to, sizeof(to));
            strncat(to, p, sizeof(to));

            
            bzero(request, strlen(request));
            strcat(request, "message:");
            strcat(request, to);

            strcat(message.to, to);

            SendRequest(server, request, strlen(request));
            SendRequest(server, &message, sizeof(struct MessageData));

            char response[100];
            bzero(response, sizeof(response));
            int responseSize;
            ReadFromServer(server, &responseSize, sizeof(responseSize));
            ReadFromServer(server, response, sizeof(response));
            InterpretResponse(server, response, sizeof(response));

            p = strtok(NULL, " \n");
        }
        return 2;
    }
    
    if (strncmp(cmd, "surf", strlen("surf")) == 0) {
        
        bzero(request, strlen(request));
        strcat(request, "surf:");
        strcat(request, activeUsername);
        SendRequest(server, request, strlen(request));
        return 1;
    }

    if (strncmp(cmd, "add ", strlen("add ")) == 0) {
        char username[11];
        char group[10];

        bzero(group, sizeof(group));
        bzero(username, sizeof(username));

        char * p;
        p = strtok(cmd + strlen("add "), " \n");
        if (!p) return 0;
        strncat(username, p, sizeof(username));
        
        if (strcmp(username, activeUsername) == 0) {
            printf("Sorry, but you cannot add yourself to a group of yours!\n");
            printf("\n----------------------------------------------------------------------------------------------------------------------\n");
            return 2;
        }

        p = strtok(NULL, " \n");
        if (!p) return 0;
        strncat(group, p, sizeof(group));

        bzero(request, strlen(request));
        strcat(request, "add:");
        strcat(request, activeUsername);
        strcat(request, ":");
        strcat(request, username);
        strcat(request, ":");
        strcat(request, group);
        
        SendRequest(server, request, strlen(request));
        return 1;
    }

    if (strncmp(cmd, "read", strlen("read")) == 0) {
        bzero(request, strlen(request));
        strcat(request, "read:");
        strcat(request, activeUsername);
        SendRequest(server, request, strlen(request));
        return 1;
    }

    if (strncmp(cmd, "quit", strlen("quit")) == 0) {
        printf("\n----------------------------------------------------------------------------------------------------------------------\n");
        printf("Thank you for using WON! Have a nice day! :)\n");
        printf("----------------------------------------------------------------------------------------------------------------------\n");
        strcat(request, "quit");
        SendRequest(server, request, strlen(request));
        exit(0);
    }

    if (isAdmin == 1 && strncmp(cmd, "masterkey", strlen("masterkey")) == 0) {
        char p[10];
        bzero(p, sizeof(p));

        struct MasterKey mk;
        printf("Enter the old Master Key:\n");
        ReadCommand(p, sizeof(p));
        mk.oldKey = atoi(p);

        printf("\nEnter the new Master Key: \n");
        ReadCommand(p, sizeof(p));
        mk.newKey = atoi(p);

        // printf("DEBUG old = '%d', new = '%d'\n", mk.oldKey, mk.newKey);

        bzero(request, strlen(request));
        strcat(request, "masterkey:change");
        SendRequest(server, request, strlen(request));
        SendRequest(server, &mk, sizeof(struct MasterKey));

        return 1;
    }

    return 0; 
}

/// Initial interface after client start-up
void InitialMessage()
{
    // W. O. N. = World Online Network
    if (fork() == 0) {
        execlp("clear", "clear", NULL);
    }
    wait(NULL);
    printf("\n\n");
    printf("---------------------------------------------------- W E L C O M E ---------------------------------------------------\n");
    printf("--------------------------------------------------------- T O --------------------------------------------------------\n");
    printf("------------------------------------------------------- W. O. N. -----------------------------------------------------\n\n");
    printf("Here's a list of functions to get you started:\n");
    printf("-> register\n-> login\n-> surf \n-> help\n-> quit\n");
    printf("----------------------------------------------------------------------------------------------------------------------\n\n");
    
}

int main(int argc, char * argv[])
{
    struct sockaddr_in server;

    int sd;

    if (-1 == (sd = socket(AF_INET, SOCK_STREAM, 0))) {
        perror("[Client] Error at socket()!\n");
        exit(-1);
    }

    server.sin_family = AF_INET;
    if (argc == 2)
        server.sin_addr.s_addr = inet_addr(argv[1]);
    else
        server.sin_addr.s_addr = inet_addr(DEFAULT_ADDR);
    server.sin_port = htons(PORT);

    if (-1 == connect(sd, (struct sockaddr *) & server, sizeof(server))) {
        perror("[Client] Error at connect()!\n");
        exit(-1);
    }

    printf("[Client] Connection established!\n");
    
    char msg[100];
    char request[100];
    int res;

    InitialMessage();
    
    // Read commands until a valid command was entered.
    while (1) {

        ReadCommand(msg, sizeof(msg));

        if (isLoggedIn == 0)
            res = ParseCommand(sd, msg, request);
        else
            res = ParseCommandLoggedIn(sd, msg, request);
        
        if (res == 0) {
            printf("You entered an invalid command! If you forgot which commands you can use, type 'help'.\n");
        }
        if (res == 1) {

            int responseSize;
            ReadFromServer(sd, &responseSize, sizeof(responseSize));
            
            char * response = (char*)malloc(responseSize + 1);
            
            bzero(response, responseSize + 1);
            // printf("DEBUG response before ReadFromServer = '%s'\n", response);
            ReadFromServer(sd, response, responseSize);
            // printf("DEBUG response read = '%s', size = '%d', strlen(response) = '%d'\n", response, responseSize, (int)strlen(response));
            InterpretResponse(sd, response, sizeof(response));
            free(response);
        }
    }
    return 0;
}
    
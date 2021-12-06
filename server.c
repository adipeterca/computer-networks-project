#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include "helper_funct.h"

#define PORT 2024
#define MAX_CLIENTS 10
#define LOGIN_DB_ADMIN "loginadmin"
#define LOGIN_DB_USER "loginuser"
#define POSTS_DB "posts"
#define MASTERKEY_FILE "masterkey"

/// Reads the request from the specified client.
void ReadRequest(int client, void * msg, unsigned len)
{
    bzero(msg, len);
    int bytesRead = read(client, msg, len);

    if (bytesRead == -1) {
        perror("[Server] Error at reading from client!");
        if (-1 == close(client)) {
            perror("[Server] Error at closing the client!\n");
            printf("[Server] Connection terminated unsuccessfuly!\n");
            exit(-1);
        }
        printf("[Server] Connection terminated successfuly!");
    }
    if (bytesRead == 0) {
        printf("[Server] Client has disconnected!");
        exit(-1);
    }
    // printf("[Server] Read a request of size %d. The request = '%s'\n", bytesRead, (char*)msg);
}

/// Writes a response to the specified client and deals with any issues that may arrise.
void WriteResponse(int client, void * response, unsigned len)
{
    if (len == 0)
    {
        printf("[Server] Warning! Received a response of size 0!\n");
        exit(-1);
    }

    int bytesWritten = write(client, response, len);
    if (bytesWritten == -1) {
        perror("[Server] Error at writing to client!");
        if (-1 == close(client)) {
            perror("[Server] Error at closing the client!");
            printf("[Server] Connection terminated unsuccessfuly!\n");
            exit(-1);
        }
        printf("[Server] Connection terminated successfuly!\n");
    }
    if (bytesWritten == 0)
    {
        printf("[Server] Warning! Written a response of size 0!\n");
        exit(-1);
    }
    // printf("[Server] Written a response of size %d. The response = '%s'\n", bytesWritten, (char*)response);
}

/// Parses the given request accordingly
void ParseRequest(int client, char * msg, char * response)
{
    bzero(response, sizeof(char[100]));

    if (strncmp(msg, "register:admin:", strlen("register:admin:")) == 0) {
        int res = UsernameExists(LOGIN_DB_ADMIN, msg + strlen("register:admin:"));
        if (res == 0)
            res = UsernameExists(LOGIN_DB_USER, msg + strlen("register:admin:"));
        if (res == 1) {
            // This user already exists in the admin or user data base. DO NOT REGISTER
            strcat(response, "register:failed");
            int size = strlen(response);
            WriteResponse(client, &size, sizeof(size));
            WriteResponse(client, response, strlen(response));
            return;
        }
        else if (res == 0) {
            // The user does not exists, it should be added.
            res = WriteToFile(LOGIN_DB_ADMIN, msg + strlen("register:admin:"));
            
            char fileName[35];
            char username[11];
            bzero(username, sizeof(username));
            strncat(username, msg + strlen("register:admin:"), 11);
            int i = 10, count = 0, fd;
            while (username[i] != ':')
                username[i--] = 0;
            username[i] = 0;

            bzero(fileName, sizeof(fileName));
            strcat(fileName, "./users/");
            strcat(fileName, "inbox_");
            strcat(fileName, username);
            creat(fileName, 0666);
            fd = open(fileName, O_RDWR);
            write(fd, &count, sizeof(count));

            bzero(fileName, sizeof(fileName));
            strcat(fileName, "./users/");
            strcat(fileName, "feed_");
            strcat(fileName, username);
            creat(fileName, 0666);
            fd = open(fileName, O_RDWR);
            write(fd, &count, sizeof(count));

            bzero(fileName, sizeof(fileName));
            strcat(fileName, "./users/");
            strcat(fileName, "friends_");
            strcat(fileName, username);
            strcat(fileName, "_family");
            creat(fileName, 0666);
            fd = open(fileName, O_RDWR);
            write(fd, &count, sizeof(count));

            bzero(fileName, sizeof(fileName));
            strcat(fileName, "./users/");
            strcat(fileName, "friends_");
            strcat(fileName, username);
            strcat(fileName, "_friends");
            creat(fileName, 0666);
            fd = open(fileName, O_RDWR);
            write(fd, &count, sizeof(count));

            strcat(response, "register:success");
            int size = strlen(response);
            WriteResponse(client, &size, sizeof(size));
            WriteResponse(client, response, strlen(response));
            return;
        }
        else {
            printf("[Server] An error occured while trying to register the admin.\n");
            strcat(response, "register:error");
            int size = strlen(response);
            WriteResponse(client, &size, sizeof(size));
            WriteResponse(client, response, strlen(response));
            return;
        }
    }

    if (strncmp(msg, "register:user:", strlen("register:user:")) == 0) {
        int res = UsernameExists(LOGIN_DB_USER, msg + strlen("register:user:"));
        if (res == 0)
            res = UsernameExists(LOGIN_DB_ADMIN, msg + strlen("register:user:"));

        if (res == 1) {
            // The user already exists
            strcat(response, "register:failed");
            int size = strlen(response);
            WriteResponse(client, &size, sizeof(size));
            WriteResponse(client, response, strlen(response));
            return;
        }
        else if (res == 0) {
            // The user does not exist
            res = WriteToFile(LOGIN_DB_USER, msg + strlen("register:user:"));

            char fileName[35];
            char username[11];
            bzero(username, sizeof(username));
            strncat(username, msg + strlen("register:user:"), 11);
            int i = 10, count = 0, fd;
            while (username[i] != ':')
                username[i--] = 0;
            username[i] = 0;

            bzero(fileName, sizeof(fileName));
            strcat(fileName, "./users/");
            strcat(fileName, "inbox_");
            strcat(fileName, username);
            creat(fileName, 0666);
            fd = open(fileName, O_RDWR);
            write(fd, &count, sizeof(count));

            bzero(fileName, sizeof(fileName));
            strcat(fileName, "./users/");
            strcat(fileName, "feed_");
            strcat(fileName, username);
            creat(fileName, 0666);
            fd = open(fileName, O_RDWR);
            write(fd, &count, sizeof(count));

            bzero(fileName, sizeof(fileName));
            strcat(fileName, "./users/");
            strcat(fileName, "friends_");
            strcat(fileName, username);
            strcat(fileName, "_family");
            creat(fileName, 0666);
            fd = open(fileName, O_RDWR);
            write(fd, &count, sizeof(count));

            bzero(fileName, sizeof(fileName));
            strcat(fileName, "./users/");
            strcat(fileName, "friends_");
            strcat(fileName, username);
            strcat(fileName, "_friends");
            creat(fileName, 0666);
            fd = open(fileName, O_RDWR);
            write(fd, &count, sizeof(count));

            strcat(response, "register:success");
            int size = strlen(response);
            WriteResponse(client, &size, sizeof(size));
            WriteResponse(client, response, strlen(response));
            return;
        }
        else {
            printf("[Server] An error occured while trying to register the user.\n");
            strcat(response, "register:error");
            int size = strlen(response);
            WriteResponse(client, &size, sizeof(size));
            WriteResponse(client, response, strlen(response));
            return;
        }
    }

    if (strncmp(msg, "login:admin:", strlen("login:admin:")) == 0) {

        int res = IsRegistered(LOGIN_DB_ADMIN, msg + strlen("login:admin:"));

        // It exists 
        if (res == 1) {
            strcat(response, "login:success");
            int size = strlen(response);
            WriteResponse(client, &size, sizeof(size));
            WriteResponse(client, response, strlen(response));
            return;
        }
        else if (res == 0) {
            // does not exist
            strcat(response, "login:failed");
            int size = strlen(response);
            WriteResponse(client, &size, sizeof(size));
            WriteResponse(client, response, strlen(response));
            return;
        }
        else {
            strcat(response, "login:error");
            int size = strlen(response);
            WriteResponse(client, &size, sizeof(size));
            WriteResponse(client, response, strlen(response));
            return;
            // error
        }
            
    }

    if (strncmp(msg, "login:user:", strlen("login:user:")) == 0) {

        int res = IsRegistered(LOGIN_DB_USER, msg + strlen("login:user:"));

        if (res == 1) {
            strcat(response, "login:success");
            int size = strlen(response);
            WriteResponse(client, &size, sizeof(size));
            WriteResponse(client, response, strlen(response));
            return;
        }
        else if (res == 0) {
            // does not exist
            strcat(response, "login:failed");
            int size = strlen(response);
            WriteResponse(client, &size, sizeof(size));
            WriteResponse(client, response, strlen(response));
            return;
        }
        else {
            strcat(response, "login:error");
            int size = strlen(response);
            WriteResponse(client, &size, sizeof(size));
            WriteResponse(client, response, strlen(response));
            return;
            // error
        }

    }

    if (strncmp(msg, "surf:", strlen("surf:")) == 0) {
        // The user is not logged in
        if (strncmp(msg, "surf:nologin", strlen("surf:nologin")) == 0)
        {
            struct PostData post;
            bzero(&post, sizeof(struct PostData));

            int res = GetRandomPost(&post, POSTS_DB);

            if (res == 0) {
                strcat(response, "surf:error");
                int size = strlen(response);
            WriteResponse(client, &size, sizeof(size));
                WriteResponse(client, response, strlen(response));
                return;
            }
            else {
                strcat(response, "surf:success");
                // printf("DEBUG Random post: '%s'\n", post.txt);
                int size = strlen(response);
                WriteResponse(client, &size, sizeof(size));
                WriteResponse(client, response, strlen(response));
                WriteResponse(client, &post, sizeof(struct PostData));
                return;
            }
        }
        else
        {
            struct PostData post;
            bzero(&post, sizeof(struct PostData));

            int res = 0;
            srand(time(0) * strlen(msg));
            do 
            {
                if (rand() % 2 == 1)
                    res = GetRandomPostFromFeed(&post, msg + strlen("surf:"));
                if (res == 0)
                    res = GetRandomPost(&post, POSTS_DB);
            } while (res == 0 || post.txt == NULL);
            
            if (res == 0) {
                strcat(response, "surf:error");
                int size = strlen(response);
                WriteResponse(client, &size, sizeof(size));
                WriteResponse(client, response, strlen(response));
                return;
            }
            else {
                strcat(response, "surf:success");
                // printf("DEBUG Random post: '%s'\n", post.txt);
                // printf("DEBUG Owner: '%s'\n", post.owner);
                int size = strlen(response);
                WriteResponse(client, &size, sizeof(size));
                WriteResponse(client, response, strlen(response));
                WriteResponse(client, &post, sizeof(struct PostData));
                return;
            }
        }
        return; 
    }

    if (strncmp(msg, "post", strlen("post")) == 0) {
        
        struct PostData d;
        bzero(&d, sizeof(d));
        ReadRequest(client, &d, sizeof(d));

        int res;
        if (d.groupID == 0)
            res = WritePost(&d, POSTS_DB);
        else
            res = WritePostToGroup(&d);
        
        if (res == 0) {
            strcat(response, "post:error");
            int size = strlen(response);
            WriteResponse(client, &size, sizeof(size));
            WriteResponse(client, response, strlen(response));
            return;
        }
        else {
            strcat(response, "post:success");
            int size = strlen(response);
            WriteResponse(client, &size, sizeof(size));
            WriteResponse(client, response, strlen(response));
            return;
        }
    }

    if (strncmp(msg, "quit", strlen("quit")) == 0) {
        exit(0);
    }

    if (strncmp(msg, "add:", strlen("add:")) == 0) {
        char username[11];
        char usernameToBeAdded[11];
        char group[11];

        char filename[50];
        bzero(username, sizeof(username));
        bzero(group, sizeof(group));
        bzero(usernameToBeAdded, sizeof(usernameToBeAdded));
        bzero(filename, sizeof(filename));

        int i = 4;
        int k = 0;
        while (msg[i] != ':')
            username[k++] = msg[i++];
        i++;
        k = 0;
        while (msg[i] != ':')
            usernameToBeAdded[k++] = msg[i++];
        
        i++;
        k = 0;
        while (i < strlen(msg))
            group[k++] = msg[i++];
        
        // printf("DEBUG username = '%s' usernameToBeAdded = '%s' group = '%s'\n", username, usernameToBeAdded, group);

        strcat(filename, "./users/friends_");
        strcat(filename, username);
        strcat(filename, "_");
        // printf("filename before group = '%s'\n", filename);
        strcat(filename, group);
        // printf("filename after group = '%s', group = '%s'\n", filename, group);

        char filenameToCheck[50];
        bzero(filenameToCheck, sizeof(filenameToCheck));
        strcat(filenameToCheck, "./users/inbox_");
        strcat(filenameToCheck, usernameToBeAdded);
        if (access(filenameToCheck, F_OK) != 0) {
            strcat(response, "add:failed");
            int size = strlen(response);
            WriteResponse(client, &size, sizeof(size));
            WriteResponse(client, response, strlen(response));
            return;
        }

        int fd = open(filename, O_RDWR);
        if (fd == -1) {
            perror("[Server] Error at open() for adding USER to GROUP!");
            printf("FILE = '%s'\n", filename);
            strcat(response, "add:error");
            int size = strlen(response);
            WriteResponse(client, &size, sizeof(size));
            WriteResponse(client, response, strlen(response));
            return;
        }

        int friendsCount;
        if (-1 == read(fd, &friendsCount, sizeof(friendsCount))) {
            perror("[Server] Error at read() from FRIENDS for friendsCount!");
            strcat(response, "add:error");
            int size = strlen(response);
            WriteResponse(client, &size, sizeof(size));
            WriteResponse(client, response, strlen(response));
            return;
        }

        char str[11];
        int lenStr;
        for (int i = 1; i <= friendsCount; ++i)
        {
            if (-1 == read(fd, &lenStr, sizeof(lenStr))) {
                perror("[Server] Error at read() from FRIENDS for lenStr!");
                strcat(response, "add:error");
                int size = strlen(response);
                WriteResponse(client, &size, sizeof(size));
                WriteResponse(client, response, strlen(response));
                return;
            }
            bzero(str, sizeof(str));
            if (-1 == read(fd, str, lenStr)) {
                perror("[Server] Error at read() from FRIENDS for str!");
                strcat(response, "add:error");
                int size = strlen(response);
                WriteResponse(client, &size, sizeof(size));
                WriteResponse(client, response, strlen(response));
                return;
            }
            if (strcmp(str, usernameToBeAdded) == 0) {
                strcat(response, "add:exists");
                int size = strlen(response);
                WriteResponse(client, &size, sizeof(size));
                WriteResponse(client, response, strlen(response));
                return;
            }
        }

        friendsCount++;

        if (-1 == lseek(fd, 0, SEEK_END)) {
            perror("[Server] Error at lseek() from FRIENDS for SEEK_END!");
            strcat(response, "add:error");
            int size = strlen(response);
            WriteResponse(client, &size, sizeof(size));
            WriteResponse(client, response, strlen(response));
            return;
        }

        int len = strlen(usernameToBeAdded);
        if (-1 == write(fd, &len, sizeof(len))) {
            perror("[Server] Error at write() from FRIENDS for len!");
            strcat(response, "add:error");
            int size = strlen(response);
            WriteResponse(client, &size, sizeof(size));
            WriteResponse(client, response, strlen(response));
            return;
        }

        if (-1 == write(fd, &usernameToBeAdded, len)) {
            perror("[Server] Error at write() from FRIENDS for usernameToBeAdded!");
            strcat(response, "add:error");
            int size = strlen(response);
            WriteResponse(client, &size, sizeof(size));
            WriteResponse(client, response, strlen(response));
            return;
        }

        if (-1 == lseek(fd, 0, SEEK_SET)) {
            perror("[Server] Error at lseek() from FRIENDS for updating friendsCount!");
            strcat(response, "add:error");
            int size = strlen(response);
            WriteResponse(client, &size, sizeof(size));
            WriteResponse(client, response, strlen(response));
            return;
        }

        if (-1 == write(fd, &friendsCount, sizeof(friendsCount))) {
            perror("[Server] Error at write() from FRIENDS for friendsCount!");
            strcat(response, "add:error");
            int size = strlen(response);
            WriteResponse(client, &size, sizeof(size));
            WriteResponse(client, response, strlen(response));
            return;
        }

        strcat(response, "add:success");
        int size = strlen(response);
        WriteResponse(client, &size, sizeof(size));
        WriteResponse(client, response, strlen(response));
        return;
    }

    if (strncmp(msg, "message:", strlen("message:")) == 0) {
        char filename[30];
        bzero(filename, sizeof(filename));

        strcat(filename, "./users/inbox_");
        strcat(filename, msg + strlen("message:"));

        if (access(filename, F_OK) != 0) {
            strcat(response, "message:failed");
            int size = strlen(response);
            WriteResponse(client, &size, sizeof(size));
            WriteResponse(client, response, strlen(response));
            return;
        }

        int fd = open(filename, O_RDWR);
        if (fd == -1) {
            perror("[Server] Error at open() for MESSAGE!");
            strcat(response, "message:error");
            int size = strlen(response);
            WriteResponse(client, &size, sizeof(size));
            WriteResponse(client, response, strlen(response));
            return;
        }

        int messageCount;
        if (-1 == read(fd, &messageCount, sizeof(messageCount))) {
            perror("[Server] Error at read() from INBOX for messageCount!");
            strcat(response, "message:error");
            int size = strlen(response);
            WriteResponse(client, &size, sizeof(size));
            WriteResponse(client, response, strlen(response));
            return;
        }

        messageCount++;
        if (-1 == lseek(fd, 0, SEEK_SET)) {
            perror("[Server] Error at lseek() from INBOX for updating messageCount!");
            strcat(response, "message:error");
            int size = strlen(response);
            WriteResponse(client, &size, sizeof(size));
            WriteResponse(client, response, strlen(response));
            return;
        }

        if (-1 == write(fd, &messageCount, sizeof(messageCount))) {
            perror("[Server] Error at write() from INBOX for updating messageCount!");
            strcat(response, "message:error");
            int size = strlen(response);
            WriteResponse(client, &size, sizeof(size));
            WriteResponse(client, response, strlen(response));
            return;
        }

        if (-1 == lseek(fd, 0, SEEK_END)) {
            perror("[Server] Error at lseek() from INBOX for writing message!");
            strcat(response, "message:error");
            int size = strlen(response);
            WriteResponse(client, &size, sizeof(size));
            WriteResponse(client, response, strlen(response));
            return;
        }

        struct MessageData m;
        bzero(&m, sizeof(struct MessageData));
        ReadRequest(client, &m, sizeof(struct MessageData));

        if (-1 == write(fd, &m, sizeof(struct MessageData))) {
            perror("[Server] Error at write() from INBOX for writing message!");
            strcat(response, "message:error");
            int size = strlen(response);
            WriteResponse(client, &size, sizeof(size));
            WriteResponse(client, response, strlen(response));
            return;
        }

        strcat(response, "message:success");
        int size = strlen(response);
        WriteResponse(client, &size, sizeof(size));
        WriteResponse(client, response, strlen(response));
        return;
    }

    if (strncmp(msg, "read:", strlen("read:")) == 0) {

        struct MessageData message;
        bzero(&message, sizeof(struct MessageData));

        char filename[30];
        char username[11];
        bzero(filename, sizeof(filename));
        bzero(username, sizeof(username));

        strcat(username, msg + strlen("read:"));
        strcat(filename, "./users/inbox_");
        strcat(filename, username);

        int fd = open(filename, O_RDWR);
        if (fd == -1) {
            perror("[Server] Error at open() for INBOX!");
            strcat(response, "read:error");
            int size = strlen(response);
            WriteResponse(client, &size, sizeof(size));
            WriteResponse(client, response, strlen(response));
            return;
        }

        int messageCount;
        if (-1 == read(fd, &messageCount, sizeof(messageCount))) {
            perror("[Server] Error at read() from INBOX for messageCount!");
            strcat(response, "read:error");
            int size = strlen(response);
            WriteResponse(client, &size, sizeof(size));
            WriteResponse(client, response, strlen(response));
            return;
        }

        if (messageCount == 0)
        {
            strcat(response, "read:empty");
            int size = strlen(response);
            WriteResponse(client, &size, sizeof(size));
            WriteResponse(client, response, strlen(response));
            return;
        }

        messageCount--;
        if (-1 == lseek(fd, 0, SEEK_SET)) {
            perror("[Server] Error at lseek() from INBOX for messageCount!");
            strcat(response, "read:error");
            int size = strlen(response);
            WriteResponse(client, &size, sizeof(size));
            WriteResponse(client, response, strlen(response));
            return;
        }

        if (-1 == write(fd, &messageCount, sizeof(messageCount))) {
            perror("[Server] Error at write() from INBOX for messageCount!");
            strcat(response, "read:error");
            int size = strlen(response);
            WriteResponse(client, &size, sizeof(size));
            WriteResponse(client, response, strlen(response));
            return;
        }

        if (-1 == lseek(fd, sizeof(struct MessageData) * messageCount + sizeof(int), SEEK_SET)) {
            perror("[Server] Error at lseek() from INBOX for message!");
            strcat(response, "read:error");
            int size = strlen(response);
            WriteResponse(client, &size, sizeof(size));
            WriteResponse(client, response, strlen(response));
            return;
        }

        if (-1 == read(fd, &message, sizeof(struct MessageData))) {
            perror("[Server] Error at read() from INBOX for message!");
            strcat(response, "read:error");
            int size = strlen(response);
            WriteResponse(client, &size, sizeof(size));
            WriteResponse(client, response, strlen(response));
            return;
        }
        strcat(response, "read:success");
        int size = strlen(response);
        WriteResponse(client, &size, sizeof(size));
        WriteResponse(client, response, strlen(response));
        WriteResponse(client, &message, sizeof(struct MessageData));
        return;
    }

    if (strncmp(msg, "masterkey:change", strlen("masterkey:change")) == 0) {
        
        struct flock fl;
        fl.l_type = F_WRLCK;
        fl.l_whence = SEEK_SET;
        fl.l_start = 0;
        fl.l_len = 0;
        
        int fd = open(MASTERKEY_FILE, O_RDWR);
        
        if (-1 == fd) {
            perror("[Server] Error at open() MASTERKEY_FILE!");
            strcat(response, "masterkey:error");
            int size = strlen(response);
            WriteResponse(client, &size, sizeof(size));
            WriteResponse(client, response, strlen(response));
            return;
        }
        
        if (-1 == fcntl(fd, F_SETLKW, &fl)) {
            perror("[Server] Error at fcntl() for MASTERKEY_FILE!");
            strcat(response, "masterkey:error");
            int size = strlen(response);
            WriteResponse(client, &size, sizeof(size));
            WriteResponse(client, response, strlen(response));
            return;
        }

        struct MasterKey mk;
        ReadRequest(client, &mk, sizeof(struct MasterKey));
        
        int oldKey;
        if (-1 == read(fd, &oldKey, sizeof(oldKey))) {
            perror("[Server] Error at read() from MASTERKEY_FILE for oldKey!");
            strcat(response, "masterkey:error");
            int size = strlen(response);
            WriteResponse(client, &size, sizeof(size));
            WriteResponse(client, response, strlen(response));
            return;
        }
        
        if (mk.oldKey == oldKey) {
            // The key corresponds to the old one, so it can be changed
            if (-1 == lseek(fd, 0, SEEK_SET)) {
                perror("[Server] Error at lseek() from MASTERKEY_FILE!");
                strcat(response, "masterkey:error");
                int size = strlen(response);
                WriteResponse(client, &size, sizeof(size));
                WriteResponse(client, response, strlen(response));
                return;
            }
            if (-1 == write(fd, &mk.newKey, sizeof(mk.newKey))) {
                perror("[Server] Error at write() from MASTERKEY_FILE for newKey!");
                strcat(response, "masterkey:error");
                int size = strlen(response);
                WriteResponse(client, &size, sizeof(size));
                WriteResponse(client, response, strlen(response));
                return;
            }

            strcat(response, "masterkey:success");
            int size = strlen(response);
            WriteResponse(client, &size, sizeof(size));
            WriteResponse(client, response, strlen(response));
            
            return;
        }
        else
        {
            // The old keys dont match
            strcat(response, "masterkey:failed");
            int size = strlen(response);
            WriteResponse(client, &size, sizeof(size));
            WriteResponse(client, response, strlen(response));
            
            return;
        }
        
    }

    if (strncmp(msg, "masterkey:request", strlen("masterkey:request")) == 0) {
        int masterkey;
        char response[100];
        bzero(response, sizeof(response));

        struct flock fl;
        fl.l_type = F_RDLCK;
        fl.l_whence = SEEK_SET;
        fl.l_start = 0;
        fl.l_len = 0;

        int fd = open(MASTERKEY_FILE, O_RDONLY);
        if (fd == -1) {
            perror("[Server] Error at open() for masterkey request!");
            strcat(response, "masterkey:request:error");
            WriteResponse(client, response, sizeof(response));
            return;
        }

        if (-1 == fcntl(fd, F_SETLKW, &fl)) {
            perror("[Server] Error at fcntl() for masterkey request!");
            strcat(response, "masterkey:request:error");
            WriteResponse(client, response, sizeof(response));
            return;
        }

        if (-1 == read(fd, &masterkey, sizeof(masterkey))) {
            perror("[Server] Error at read() for masterkey request!");
            strcat(response, "masterkey:request:error");
            WriteResponse(client, response, sizeof(response));
            return;
        }

        strcat(response, "masterkey:request:success");
        WriteResponse(client, response, sizeof(response));
        WriteResponse(client, &masterkey, sizeof(masterkey));
        return;
    }
}

int main()
{
    struct sockaddr_in server;
    struct sockaddr_in from;
    int sd;

    if (-1 == (sd = socket(AF_INET, SOCK_STREAM, 0))) {
        perror("[Server] Error at socket()!\n");
        exit(-1);
    }

    // Initialise the data structures
    bzero(&server, sizeof(server));
    bzero(&from, sizeof(from));
    
    // Setting up the server
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(PORT);   

    if (-1 == (bind(sd, (struct sockaddr *) &server, sizeof(struct sockaddr)))) {
        perror("[Server] Error at bind()!\n");
        exit(-1);
    }

    if (-1 == listen(sd, MAX_CLIENTS)) {
        perror("[Server] Error at listen()!\n");
        exit(-1);
    }
    
    printf("Server started with success!\n");

    while (1)
    {
        int client;     // file descriptor used to read/write from the client
        int length = sizeof(from);
        printf("[Server] Listening to PORT %d\n", PORT);

        client = accept(sd, (struct sockaddr *) & from, & length);
        if (client == -1) {
            perror("[Server] Error at accept()!\n");
            continue;
        }

        pid_t pid;
        if (-1 == (pid = fork())) {
            perror("[Server] Error at fork()!\n");
            continue;
        }

        // If a connection was made, the child serves the client
        if (pid == 0)
        {
            char msg[100];
            char response[100];

            // Serve the client until he quits
            while (1)
            {
                ReadRequest(client, msg, sizeof(char[100]));
                // printf("Debug: '%s'\n", response);
                ParseRequest(client, msg, response);           
            }
        }
    }
    return 0;
}

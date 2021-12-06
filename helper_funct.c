#include "helper_funct.h"

void ClearInput()
{
    int c;
    do
    {
        c = getchar();
    } while (c != EOF && c != '\n');

}

char * StrToLwr(char * str)
{
    if (str == NULL) return NULL;

    char * lwr = malloc(sizeof(str));
    for (int i = 0; i < sizeof(str); ++i)
        if (str[i] >= 'A' && str[i] <= 'Z')
            lwr[i] = str[i] - 'A' + 'a';
        else
            lwr[i] = str[i];
    return lwr;
}

int IsRegistered(char * file, char * str)
{
    int fd = open(file, O_RDONLY);
    if (fd == -1) {
        perror("[Server] Error at open()!");
        return -1;
    }

    int loginCount;
    int size;
    char txt[21];
    int bytesRead;
    bytesRead = read(fd, &loginCount, sizeof(loginCount));
    if (bytesRead == -1) {
        perror("[Server] Error at read() for loginCount!");
        return -1;
    }
    if (bytesRead == 0) {
        printf("[Server] Read 0 bytes from file %s.\n", file);
    }

    // printf("DEBUG loginCount = %d, bytesRead = %d\n", loginCount, bytesRead);

    while (loginCount)
    {
        bzero(txt, sizeof(txt));
        size = -1;
        loginCount--;

        bytesRead = read(fd, &size, sizeof(size));
        if (bytesRead == -1) {
            perror("[Server] Error at read() for size!");
            return -1;
        }
        if (bytesRead == 0) {
            printf("[Server] Read 0 bytes from file %s.\n", file);
        }
        
        // printf("DEBUG size = %d, bytesRead = %d\n", size, bytesRead);

        bytesRead = read(fd, txt, size);
        if (bytesRead == -1) {
            perror("[Server] Error at read() for txt!");
            return -1;
        }
        if (bytesRead == 0) {
            printf("[Server] Read 0 bytes from file %s.\n", file);
        }

        printf("txt = '%s'\n", txt);
        printf("str = '%s'\n", str);

        if (strcmp(str, txt) == 0)
            return 1;
    }
    return 0;
}

int UsernameExists(char * file, char * str)
{
    int fd = open(file, O_RDONLY);
    if (fd == -1) {
        perror("[Server] Error at open()!");
        return -1;
    }

    int usernameLength = 0;
    while (str[usernameLength] != ':')
        usernameLength++;

    int loginCount;
    int size;
    char txt[21];
    int bytesRead;
    bytesRead = read(fd, &loginCount, sizeof(loginCount));
    if (bytesRead == -1) {
        perror("[Server] Error at read() for loginCount!");
        return -1;
    }
    if (bytesRead == 0) {
        printf("[Server] Read 0 bytes from file %s.\n", file);
    }

    // printf("DEBUG loginCount = %d, bytesRead = %d\n", loginCount, bytesRead);

    while (loginCount)
    {
        bzero(txt, sizeof(txt));
        size = -1;
        loginCount--;

        bytesRead = read(fd, &size, sizeof(size));
        if (bytesRead == -1) {
            perror("[Server] Error at read() for size!");
            return -1;
        }
        if (bytesRead == 0) {
            printf("[Server] Read 0 bytes from file %s.\n", file);
        }
        
        // printf("DEBUG size = %d, bytesRead = %d\n", size, bytesRead);

        bytesRead = read(fd, txt, size);
        if (bytesRead == -1) {
            perror("[Server] Error at read() for txt!");
            return -1;
        }
        if (bytesRead == 0) {
            printf("[Server] Read 0 bytes from file %s.\n", file);
        }

        // printf("txt = '%s'\n", txt);
        // printf("str = '%s'\n", str);
        
        int txtUsernameLenght = 0;
        while (txt[txtUsernameLenght] != ':')
            txtUsernameLenght++;
        
        if (usernameLength == txtUsernameLenght && (strncmp(str, txt, usernameLength) == 0))
            return 1;
    }
    return 0;
}

int WriteToFile(char * file, char * msg)
{
    int fd = open(file, O_RDWR);
    if (fd == -1) {
        perror("[Server] Error at open()!");
        return -1;
    }
    int loginCount;
    int bytesRead;
    int bytesWritten;
    bytesRead = read(fd, &loginCount, sizeof(loginCount));

    if (bytesRead == -1) {
        perror("[Server] Error at read() for loginCount!");
        return -1;
    }
    if (bytesRead == 0) {
        printf("[Server] Warning! Read 0 bytes for loginCount!\n");
        return -1;
    }

    // daca nu pun msg[99] = 0, s ar putea sa citeasca aiurea
    if (-1 == lseek(fd, 0, SEEK_END)) {
        perror("[Server] Error at lseek()!");
        return -1;
    }

    int length = strlen(msg);

    bytesWritten = write(fd, &length, sizeof(length));
    
    if (bytesWritten == -1) {
        perror("[Server] Error at write() for length!");
        return -1;
    }
    if (bytesWritten == 0) {
        printf("[Server] Warning! Written 0 bytes for length!\n");
        return -1;
    }

    bytesWritten = write(fd, msg, length);
    if (bytesWritten == -1) {
        perror("[Server] Error at write() for msg!");
        return -1;
    }
    if (bytesWritten == 0) {
        printf("[Server] Warning! Written 0 bytes for msg!\n");
        return -1;
    }
    
    if (lseek(fd, 0, SEEK_SET) == -1) {
        perror("[Server] Error at lseek() for loginCount!");
        return -1;
    }

    loginCount++;
    bytesWritten = write(fd, &loginCount, sizeof(loginCount));
        if (bytesWritten == -1) {
        perror("[Server] Error at write() for loginCount!");
        return -1;
    }
    if (bytesWritten == 0) {
        printf("[Server] Warning! Written 0 bytes for loginCount!\n");
        return -1;
    }
    close(fd);
    return 0;
}

void InitPostData(struct PostData * d, char * owner, unsigned group)
{
    strncpy(d->owner, owner, sizeof(owner));
    d->groupID = group;
    time(&d->data);
    bzero(d->txt, sizeof(d->txt));
}

int WritePost(struct PostData * d, char * file)
{
    int fd = open(file, O_RDWR);
    if (fd == -1) {
        perror("[Server] Error at open() for writing post to database!");
        return 0;
    }

    int postCount = 0;
    int bytesRead, bytesWritten;
    bytesRead = read(fd, &postCount, sizeof(postCount));

    if (bytesRead == -1) {
        perror("[Server] Error at read() for post count!");
        return 0;
    }
    if (bytesRead == 0) {
        postCount = 0;
    }
    if (lseek(fd, 0, SEEK_SET) == -1) {
        perror("[Server] Error at lseek() after reading post count!");
        return 0;
    }
    postCount++;
    bytesWritten = write(fd, &postCount, sizeof(postCount));
    if (bytesWritten == -1) {
        perror("[Server] Error at writing post count!");
        return 0;
    }

    if (lseek(fd, 0, SEEK_END) == -1) {
        perror("[Server] Error at lseek() for SEEK_END!");
        return 0;
    }

    bytesWritten = write(fd, d, sizeof(struct PostData));
    if (bytesWritten == -1) {
        perror("[Server] Error at writing post to database!");
        return 0;
    }
    if (bytesWritten == 0) {
        printf("[Server] Warning: written 0 bytes to database for a post.\n");
        return 0;
    }
    return 1;
}

int WritePostToGroup(struct PostData * d)
{
    // Post it for the family group
    if (d->groupID == 1)
    {
        char filename[50];
        bzero(filename, sizeof(filename));

        strcat(filename, "./users/friends_");
        strcat(filename, d->owner);
        strcat(filename, "_family");

        int fd = open(filename, O_RDONLY);
        if (fd == -1) {
            perror("[Server] Error at open() for reading FAMILY GROUP!");
            return 0;
        }

        int familyCount = 0;
        if (-1 == read(fd, &familyCount, sizeof(familyCount))) {
            perror("[Server] Error at read() from FAMILY GROUP for familyCount!");
            return 0;
        }

        char writeto[50];
        char username[11];
        int len;

        while (familyCount)
        {
            familyCount--;
            bzero(writeto, sizeof(writeto));
            bzero(username, sizeof(username));
            
            if (-1 == read(fd, &len, sizeof(len))) {
                perror("[Server] Error at read() from FAMILY GROUP for len!");
                return 0;
            }
            if (-1 == read(fd, username, len)) {
                perror("[Server] Error at read() from FAMILY GROUP for username!");
                return 0;
            }

            strcat(writeto, "./users/feed_");
            strcat(writeto, username);

            int writetoFD = open(writeto, O_RDWR);
            if (writetoFD == -1) {
                perror("[Server] Error at open() from FAMILY GROUP for writing post!");
                return 0;
            }

            int postCount = 0;
            if (-1 == read(writetoFD, &postCount, sizeof(postCount))) {
                perror("[Server] Error at read() from FAMILY GROUP for postCount!");
                return 0;
            }

            if (-1 == lseek(writetoFD, 0, SEEK_END)) {
                perror("[Server] Error at lseek() from FAMILY GROUP for writing post!");
                return 0;
            }

            if (-1 == write(writetoFD, d, sizeof(struct PostData))) {
                perror("[Server] Error at write() from FAMILY GROUP for writing post!");
                return 0;
            }

            postCount++;
            if (-1 == lseek(writetoFD, 0, SEEK_SET)) {
                perror("[Server] Error at lseek() from FAMILY GROUP for updating postCount!");
                return 0;
            }

            if (-1 == write(writetoFD, &postCount, sizeof(postCount))) {
                perror("[Server] Error at write() from FAMILY GROUP for updating postCount!");
                return 0;
            }

            if (-1 == close(writetoFD)) {
                perror("[Server] Error at close() from FAMILY GROUP!");
                return 0;
            }
        }
        return 1;
    }

    // Post it for the friends group
    if (d->groupID == 2)
    {
        char filename[30];
        bzero(filename, sizeof(filename));

        strcat(filename, "./users/friends_");
        strcat(filename, d->owner);
        strcat(filename, "_friends");

        int fd = open(filename, O_RDONLY);
        if (fd == -1) {
            perror("[Server] Error at open() for reading FRIENDS GROUP!");
            return 0;
        }

        int familyCount = 0;
        if (-1 == read(fd, &familyCount, sizeof(familyCount))) {
            perror("[Server] Error at read() from FRIENDS GROUP for familyCount!");
            return 0;
        }

        char writeto[30];
        char username[11];
        int len;

        while (familyCount)
        {
            familyCount--;
            bzero(writeto, sizeof(writeto));
            bzero(username, sizeof(username));
            
            if (-1 == read(fd, &len, sizeof(len))) {
                perror("[Server] Error at read() from FRIENDS GROUP for len!");
                return 0;
            }
            if (-1 == read(fd, username, len)) {
                perror("[Server] Error at read() from FRIENDS GROUP for username!");
                return 0;
            }

            strcat(writeto, "./users/feed_");
            strcat(writeto, username);

            int writetoFD = open(writeto, O_RDWR);
            if (writetoFD == -1) {
                perror("[Server] Error at open() from FRIENDS GROUP for writing post!");
                return 0;
            }

            int postCount = 0;
            if (-1 == read(writetoFD, &postCount, sizeof(postCount))) {
                perror("[Server] Error at read() from FRIENDS GROUP for postCount!");
                return 0;
            }

            if (-1 == lseek(writetoFD, 0, SEEK_END)) {
                perror("[Server] Error at lseek() from FRIENDS GROUP for writing post!");
                return 0;
            }

            if (-1 == write(writetoFD, d, sizeof(struct PostData))) {
                perror("[Server] Error at write() from FRIENDS GROUP for writing post!");
                return 0;
            }

            postCount++;
            if (-1 == lseek(writetoFD, 0, SEEK_SET)) {
                perror("[Server] Error at lseek() from FRIENDS GROUP for updating postCount!");
                return 0;
            }

            if (-1 == write(writetoFD, &postCount, sizeof(postCount))) {
                perror("[Server] Error at write() from FRIENDS GROUP for updating postCount!");
                return 0;
            }

            if (-1 == close(writetoFD)) {
                perror("[Server] Error at close() from FRIENDS GROUP!");
                return 0;
            }
        }
        return 1;
    }

    // Invalid group
    return 0;
}

int GetRandomPost(struct PostData * d, char * file)
{
    int postCount = 0;
    int fd = open(file, O_RDONLY);

    if (-1 == fd) {
        perror("[Server] Error at open() for random post!");
        return 0;
    }

    if (-1 == read(fd, &postCount, sizeof(postCount))) {
        perror("[Server] Error at read() for post count!");
        return 0;
    }
    printf("DEBUG selected %d post to sent to client\n", postCount);
    if (postCount == 0)
    {
        // the program should not enter here, EVER
        printf("[Server] Read 0 for postCount, meaning that there are no public posts.\n");
        return 0;
    }

    srand(time(0) * postCount);
    int index = rand() % postCount + 1;

    while (index)
    {
        if (-1 == read(fd, d, sizeof(struct PostData))) {
            perror("[Server] Error at read() for PostData!");
            return 0;
        }
        index--;
    }
    return 1;
}

int GetRandomPostFromFeed(struct PostData * d, char * username)
{
    char filename[30];
    bzero(filename, sizeof(filename));

    strcat(filename, "./users/feed_");
    strcat(filename, username);

    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("[Server] Error at open() for FEED!");
        return 0;
    }

    int postCount = 0;
    if (-1 == read(fd, &postCount, sizeof(postCount))) {
        perror("[Server] Error at read() from FEED for postCount!");
        return 0;
    }

    if (postCount == 0)
        return 0;
    //printf("DEBUG selected %d postCount to sent to client\n", postCount);
    int randomPost;

    srand(time(0) * strlen(username));
    randomPost = rand() % postCount + 1;
    //printf("DEBUG selected %d randomPost to sent to client\n", randomPost);
    //printf("cursor pos = %d\n", (int)lseek(fd, 0, SEEK_CUR));
    while (randomPost)
    {
        randomPost--;
        if (-1 == read(fd, d, sizeof(struct PostData))) {
            perror("[Server] Error at read() from FEED for PostData!");
            return 0;
        }
        //printf("cursor pos = %d\n", (int)lseek(fd, 0, SEEK_CUR));
    }
    return 1;
}

void InitMessageData(struct MessageData * m, char * from, char * to)
{
    bzero(m, sizeof(struct MessageData));
    strcat(m->from, from);
    strcat(m->to, to);
    time(&m->date);
}




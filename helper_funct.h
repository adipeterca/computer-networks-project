#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

/// 'Eats' the remaining characters in the input buffer
void ClearInput();

/// Converts a string to lower case only
char * StrToLwr(char *);

/// Checks whether or not a user (str) is registered in the given database (file)
/// Returns 1 if it is found, else 0.
int IsRegistered(char * file, char * str);

/// Checks whether or not another user with the same name already exists.
/// Returns 1 if it exists, else 0.
int UsernameExists(char * file, char * str);

int WriteToFile(char * file, char * msg);

/// A struct representing a post
struct PostData {
    char owner[11];
    unsigned groupID;
    time_t data;
    char txt[500];
};

void InitPostData(struct PostData * d, char * owner, unsigned group);

/// Write the post given to the public database
int WritePost(struct PostData * d, char * file);

int WritePostToGroup(struct PostData * d);

int GetRandomPost(struct PostData * d, char * file);

int GetRandomPostFromFeed(struct PostData * d, char * username);

/// A struct representing a message
struct MessageData {
    char from[11];
    char to[11];
    time_t date;
    char txt[200];
};

void InitMessageData(struct MessageData * m, char * from, char * to);

struct MasterKey {
    int oldKey;
    int newKey;
};
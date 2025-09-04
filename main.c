#include <stdio.h>
#include<stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <conio.h> 

struct Node
{
    struct Node* link[26];
    bool flag;
};

//createnode() function

struct Node* createNode(){
    struct Node* node = (struct Node*)malloc(sizeof(struct Node));
    node->flag=false;
    for (int i = 0; i < 26; i++)
    {
        node->link[i]=NULL;
    }
    return node;
}

//insert 
void insert(struct Node* root, const char* word) {
    struct Node* curr = root;
    for (int i = 0; word[i] != '\0'; i++) {
        int index = word[i] - 'a';
        if (index < 0 || index > 25) continue;
        if (curr->link[index] == NULL) {
            curr->link[index] = createNode();
        }
        curr = curr->link[index];
    }
    curr->flag = true;
}

//collect Words() 
void collectWords(struct Node* node, char* prefix, int level, char suggestions[20][100], int* count) {
    if (*count >= 20) return;
    if (node->flag) {
        prefix[level] = '\0';
        strcpy(suggestions[*count], prefix);
        (*count)++;
    }
    for (int i = 0; i < 26; i++) {
        if (node->link[i]) {
            prefix[level] = 'a' + i;
            collectWords(node->link[i], prefix, level + 1, suggestions, count);
        }
    }
}
//auto complete feature
int autocomplete(struct Node* root, const char* prefix, char suggestions[20][100]) {
    struct Node* curr = root;
    for (int i = 0; prefix[i] != '\0'; i++) {
        int index = prefix[i] - 'a';
        if (index < 0 || index > 25 || curr->link[index] == NULL) {
            return 0;
        }
        curr = curr->link[index];
    }
    char buffer[100];
    strcpy(buffer, prefix);
    int count = 0;
    collectWords(curr, buffer, strlen(prefix), suggestions, &count);
    return count;
}

// load dictionary 

void loadDictionary(struct Node* root, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Could not open %s\n", filename);
        return;
    }
    char word[100];
    while (fscanf(file, "%s", word) != EOF) {
        insert(root, word);
    }
    fclose(file);
    printf("Dictionary loaded from %s\n", filename);
}
//loadHistory()

void loadHistory(struct Node* root, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("No history file found, starting fresh.\n");
        return;
    }
    char line[500];
    while (fgets(line, sizeof(line), file)) {
        char* token = strtok(line, " \n");
        while (token) {
            insert(root, token);
            token = strtok(NULL, " \n");
        }
    }
    fclose(file);
    printf("History loaded from %s\n", filename);
}

//save history 
void saveHistory(const char* sentence, const char* filename) {
    FILE* file = fopen(filename, "a");
    if (!file) return;
    fprintf(file, "%s\n", sentence);
    fclose(file);
}

//live feature 

void liveSentenceAutocomplete(struct Node* root) {
    char sentence[500] = "";
    char word[100] = "";
    int pos = 0;

    printf("\nStart typing your sentence:\n");

    int selected = 0;

    while (1) {
        char ch = _getch();

        if (ch == 27) break;

        if (ch == 13) {
            if (strlen(word) > 0) {
                char suggestions[20][100];
                int count = autocomplete(root, word, suggestions);
                if (count > 0 && selected < count) {
                    strcpy(word, suggestions[selected]);
                }
                strcat(sentence, word);
                word[0] = '\0';
                pos = 0;
            }
            saveHistory(sentence, "history.txt");
            printf("\nSaved sentence: %s\n", sentence);
            break;
        }

        if (ch == ' ') {
            char suggestions[20][100];
            int count = autocomplete(root, word, suggestions);
            if (count > 0 && selected < count) {
                strcpy(word, suggestions[selected]);
            }
            strcat(sentence, word);
            strcat(sentence, " ");
            word[0] = '\0';
            pos = 0;
            selected = 0;
            system("cls");
            printf("Sentence: %s\n", sentence);
            continue;
        }

        if (ch == 8) {
            if (pos > 0) {
                word[--pos] = '\0';
            }
        } else if (ch == -32) {
            int arrow = _getch();
            if (arrow == 72 && selected > 0) selected--;
            if (arrow == 80) selected++;
        } else {
            word[pos++] = ch;
            word[pos] = '\0';
            selected = 0;
        }

        system("cls");
        printf("Sentence: %s%s\n", sentence, word);

        char suggestions[20][100];
        int count = autocomplete(root, word, suggestions);

        if (count > 0) {
            if (selected >= count) selected = count - 1;
            printf("\nSuggestions:\n");
            for (int i = 0; i < count; i++) {
                if (i == selected) {
                    printf("> %s\n", suggestions[i]);
                } else {
                    printf("  %s\n", suggestions[i]);
                }
            }
        } else {
            printf("\n(No suggestions)\n");
        }
    }
}

int main() {
    struct Node* root = createNode();
    loadDictionary(root, "dictionary.txt");
    loadHistory(root, "history.txt");
    liveSentenceAutocomplete(root);
    return 0;
}

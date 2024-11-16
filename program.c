#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define TABLE_SIZE 100
#define MAX_PRODUCTS 100

// User Management Structures
typedef struct User
{
    char username[50];
    char email[50];
    char password[50];
} User;

typedef struct UserHashTable
{
    User *table[TABLE_SIZE];
} UserHashTable;

unsigned int userHash(char *str, char *email, time_t timestamp)
{
    unsigned int hash = 0;
    while (*str)
    {
        hash = (hash << 5) + *str++;
    }
    while (*email)
    {
        hash = (hash << 5) + *email++;
    }
    hash += (unsigned int)timestamp;
    return hash % TABLE_SIZE;
}

unsigned int addUser(UserHashTable *ht, User *user)
{
    time_t currentTime = time(NULL);
    unsigned int index = userHash(user->username, user->email, currentTime);
    ht->table[index] = user;
    return index;
}

User *getUser(UserHashTable *ht, char *username)
{
    for (int i = 0; i < TABLE_SIZE; i++)
    {
        if (ht->table[i] != NULL && strcmp(ht->table[i]->username, username) == 0)
        {
            return ht->table[i];
        }
    }
    return NULL;
}

int loginUser(UserHashTable *ht, char *username, char *password)
{
    User *user = getUser(ht, username);
    if (user && strcmp(user->password, password) == 0)
    {
        return 1;
    }
    return 0;
}

// Product Management Structures
typedef struct Product
{
    char productId[50];
    char name[50];
    char category[50];
    int inventory;
} Product;

typedef struct ProductHashTable
{
    Product *table[TABLE_SIZE];
} ProductHashTable;

unsigned int productHash(char *str)
{
    unsigned int hash = 0;
    while (*str)
    {
        hash = (hash << 5) + *str++;
    }
    return hash % TABLE_SIZE;
}

void addProduct(ProductHashTable *ht, Product *product)
{
    unsigned int index = productHash(product->productId);
    ht->table[index] = product;
}

Product *getProduct(ProductHashTable *ht, char *productId)
{
    unsigned int index = productHash(productId);
    return ht->table[index];
}

// Graph Node for Browsing and Purchase History
typedef struct GraphNode
{
    char productId[50];
    struct GraphNode *next;
} GraphNode;

typedef struct Graph
{
    int numVertices;
    GraphNode **adjLists;
} Graph;

Graph *createGraph(int vertices)
{
    Graph *graph = malloc(sizeof(Graph));
    graph->numVertices = vertices;
    graph->adjLists = malloc(vertices * sizeof(GraphNode *));
    for (int i = 0; i < vertices; i++)
    {
        graph->adjLists[i] = NULL;
    }
    return graph;
}

int hasEdge(Graph *graph, int src, char *productId)
{
    GraphNode *adjList = graph->adjLists[src];
    while (adjList != NULL)
    {
        if (strcmp(adjList->productId, productId) == 0)
        {
            return 1;
        }
        adjList = adjList->next;
    }
    return 0;
}

void addEdge(Graph *graph, int src, int dest, char *productId)
{
    if (!hasEdge(graph, src, productId))
    {
        GraphNode *newNode = malloc(sizeof(GraphNode));
        strcpy(newNode->productId, productId);
        newNode->next = graph->adjLists[src];
        graph->adjLists[src] = newNode;
    }
}

// Recommendation Engine
void generateRecommendations(Graph *graph, int userIndex, ProductHashTable *productTable)
{
    printf("Recommended products for user %d:\n", userIndex);
    GraphNode *adjList = graph->adjLists[userIndex];
    while (adjList != NULL)
    {
        Product *product = getProduct(productTable, adjList->productId);
        if (product != NULL)
        {
            printf("Product ID: %s, Name: %s, Category: %s, Inventory: %d\n",
                   product->productId,
                   product->name,
                   product->category,
                   product->inventory);
        }
        adjList = adjList->next;
    }
}

void suggestProducts(ProductHashTable *productTable, char *query)
{
    printf("Suggestions for '%s':\n", query);
    for (int i = 0; i < TABLE_SIZE; i++)
    {
        if (productTable->table[i] != NULL && strstr(productTable->table[i]->name, query) != NULL)
        {
            printf("Product ID: %s, Name: %s, Category: %s, Inventory: %d\n",
                   productTable->table[i]->productId,
                   productTable->table[i]->name,
                   productTable->table[i]->category,
                   productTable->table[i]->inventory);
        }
    }
}

// Main Function to Demonstrate Usage
int main()
{
    UserHashTable userTable;
    memset(&userTable, 0, sizeof(userTable));

    ProductHashTable productTable;
    memset(&productTable, 0, sizeof(productTable));

    Graph *graph = createGraph(MAX_PRODUCTS);

    char command[50];
    while (1)
    {
        printf("Enter command (register, login, add_product, browse, purchase, recommend, suggest, exit): ");
        scanf("%s", command);

        if (strcmp(command, "register") == 0)
        {
            User user;
            printf("Enter username: ");
            scanf("%s", user.username);
            printf("Enter email: ");
            scanf("%s", user.email);
            printf("Enter password: ");
            scanf("%s", user.password);
            unsigned int userIndex = addUser(&userTable, &user);
            printf("User registered successfully. User index: %u\n", userIndex);
        }
        else if (strcmp(command, "login") == 0)
        {
            char username[50], password[50];
            printf("Enter username: ");
            scanf("%s", username);
            printf("Enter password: ");
            scanf("%s", password);
            if (loginUser(&userTable, username, password))
            {
                printf("Login successful.\n");
            }
            else
            {
                printf("Invalid username or password.\n");
            }
        }
        else if (strcmp(command, "add_product") == 0)
        {
            Product product;
            printf("Enter product ID: ");
            scanf("%s", product.productId);
            printf("Enter product name: ");
            scanf("%s", product.name);
            printf("Enter product category: ");
            scanf("%s", product.category);
            printf("Enter product inventory: ");
            scanf("%d", &product.inventory);
            addProduct(&productTable, &product);
            printf("Product added successfully.\n");
        }
        else if (strcmp(command, "browse") == 0)
        {
            char productId[50];
            int userIndex;
            printf("Enter user index: ");
            scanf("%d", &userIndex);
            printf("Enter product ID to browse: ");
            scanf("%s", productId);
            addEdge(graph, userIndex, userIndex, productId);
            printf("Browsing history recorded.\n");
        }
        else if (strcmp(command, "purchase") == 0)
        {
            char productId[50];
            int userIndex;
            printf("Enter user index: ");
            scanf("%d", &userIndex);
            printf("Enter product ID to purchase: ");
            scanf("%s", productId);
            addEdge(graph, userIndex, userIndex, productId);
            printf("Purchase recorded.\n");
        }
        else if (strcmp(command, "recommend") == 0)
        {
            int userIndex;
            printf("Enter user index: ");
            scanf("%d", &userIndex);
            generateRecommendations(graph, userIndex, &productTable);
        }
        else if (strcmp(command, "suggest") == 0)
        {
            char query[50];
            printf("Enter product name query: ");
            scanf("%s", query);
            suggestProducts(&productTable, query);
        }
        else if (strcmp(command, "exit") == 0)
        {
            break;
        }
        else
        {
            printf("Invalid command.\n");
        }
    }

    return 0;
}
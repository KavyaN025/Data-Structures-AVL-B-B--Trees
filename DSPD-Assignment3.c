#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_NAME_LENGTH 50
#define MAX_MEMBERS 4
#define MAX_KEYS 4
#define MIN_KEYS 3
#define DATE_LENGTH 11
#define MAX_USERS 1000
#define MAX_FAMILIES 100
#define MAX_EXPENSES 1000
#define MAX_CHILDREN 5
#define MAX_CATEGORY 5
//#define MAX_KEYS (MAX_CHILDREN-1) // Max keys in a B-Tree Node

//Structure for the AVL-Tree Node (Users) 
typedef struct UserNode {
    int user_id; //Unique id:1-1000
    char user_name[MAX_NAME_LENGTH];
    float income;
    struct UserNode *left;
    struct UserNode *right;
    int height; //for checking the height of the every updated subtree
}UserNode;

//creating an enum for the expense category
typedef enum
{
    RENT=1,
    UTILITY,
    GROCERY,
    STATIONARY,
    LEISURE
} ExpenseCategory;

//Structures for  the B+ Tree Nodes (Expenses)
typedef struct Expense{
    int expense_id;
    int user_id;
    ExpenseCategory category;
    float amount;
    char date[11]; //Format: YYYY-MM-DD
}Expense;


typedef struct ExpenseNode
{
    int num_keys;
    int keys[MAX_KEYS];
    Expense expenses[MAX_KEYS];
    struct ExpenseNode *children[MAX_CHILDREN];

    struct ExpenseNode *next; //for doubly linked list at leaf level
    struct ExpenseNode *prev;
    int is_leaf;
} ExpenseNode;

// Structure for Family
typedef struct Family {
    int family_id;
    char family_name[MAX_NAME_LENGTH];
    int member_count;
    UserNode* members[MAX_MEMBERS];  // Array of pointers to UserNode
    float total_income;
    float total_monthly_expense;
} Family;

// Structure for a B-tree node for families
typedef struct FamilyNode 
{
    int num_keys;  // Current number of keys
    int keys[MAX_KEYS];  // Family IDs
    Family* families[MAX_KEYS];  // Pointers to family records
    struct FamilyNode* children[MAX_CHILDREN];  // Pointers to children
    int is_leaf;  // Is this a leaf node?
} FamilyNode;

// B-tree structure for family management
typedef struct FamilyTree {
    FamilyNode* root;
    int count;  // Count of families in the tree
} FamilyTree;

//Function prototypes for Users using AVL Trees
UserNode *createUserNode(int user_id, char* user_name,float income);
void writeUserToFile(const char* filename, int user_id, char* user_name, float income);
int getHeight(UserNode *node);
int getBalanceFactor(UserNode* node);
struct UserNode *rightRotate(UserNode *y);
struct UserNode* leftRotate(UserNode* x);
struct UserNode* insertUser(UserNode* root, int user_id, char* user_name, float income);
int searchUser(UserNode *root,int user_id);
struct UserNode* loadUsersFromFile(const char* filename,UserNode* root);
void printUserTable(UserNode* root);
UserNode* updateUser(UserNode* root, int user_id, char* new_name, float new_income);
UserNode* deleteUserNode(UserNode* root, int user_id);
void bulkInsert(ExpenseNode** root, Expense* expenses, int count);
void removeUserFromFamilies(FamilyTree* tree, int user_id);
// Add this function prototype before removeUserFromFamilies
void traverseAndUpdateFamilies(FamilyNode* node, int user_id, int* familiesToDelete, int* deleteCount);

// Function prototypes for Family using B Trees
Family* createFamilyN(int family_id, const char* family_name);
FamilyNode* createFamilyNode();
FamilyTree* createFamilyTree();
int isUserInAnyFamily(FamilyNode* node, int user_id);
float calculateTotalMonthlyExpense(ExpenseNode* expenseRoot, Family* family);
Family* searchFamily(FamilyNode* node, int family_id);
int findPosition(ExpenseNode *node, int key);
void splitChild(FamilyNode* parent, int index, FamilyNode* child);
void insertNonFull(FamilyNode* node, int family_id, Family* family);
void insertFamily(FamilyTree* tree, int family_id, Family* family);
void writeFamiliesRecursive(FamilyNode* node, FILE* file);
FamilyTree* loadFamiliesFromFile(const char* filename, UserNode* userRoot);
void printFamiliesInNode(FamilyNode* node);
void printFamiliesTable(FamilyTree* tree);
void collectExpensesInIDRange(ExpenseNode* node, int start_id, int end_id, int user_id, Expense** expenses, int* count);
void mergeFamilyNodes(FamilyTree* tree, FamilyNode* parent, int index);
FamilyNode* findParentNode(FamilyNode* root, FamilyNode* child);
void balanceFamilyTree(FamilyTree* tree, FamilyNode* node);
int findFamilyNode(FamilyNode* root, int family_id, FamilyNode** nodeOut, int* indexOut);
int deleteFamilyFromTree(FamilyTree* familyTree, int family_id);
void Update_or_delete_individual_Family_details(UserNode** userRoot, FamilyTree* familyTree, ExpenseNode* expenseRoot, int id, int is_family, int operation);
void TraverseAndUpdateFamilies(FamilyNode* node, ExpenseNode* expenses, int user_id);


//Function prototypes for Expenses using B + Trees
ExpenseNode *createLeafNode();
int SearchExpenseID(ExpenseNode *root,int expense_id);
int CountExpenses(ExpenseNode *root);
ExpenseNode *InsertExpense(ExpenseNode *node,Expense newExpense,int *pNewKey,ExpenseNode **pNewChild, int *pDuplicate);
void writeExpensesToFile(ExpenseNode *root,const char *filename);
void readExpensesFromFile(ExpenseNode **root,const char *filename);
const char* getCategoryName(ExpenseCategory category);
void printExpensesTable(ExpenseNode* root);
int isDateInRange(const char* date, const char* start_date, const char* end_date);
void printExpenseDetails(Expense expense);
void collectExpensesInDateRange(ExpenseNode* node, const char* start_date, const char* end_date, Expense** expenses, int* count);
Expense* FindExpenseByID(ExpenseNode* root, int expense_id);
void DeleteExpense(ExpenseNode** root, int expense_id);
void UpdateFamilyExpenses(FamilyTree* tree, ExpenseNode* expenses, int user_id);
void DeleteExpense(ExpenseNode** root, int expense_id);
void UpdateFamilyExpenses(FamilyTree* tree, ExpenseNode* expenses, int user_id);
// Expense management
void Update_delete_expense(ExpenseNode** expenseRoot, FamilyTree* familyTree, UserNode* userRoot, const char* expensesFile, const char* familiesFile);
Expense* FindExpenseByID(ExpenseNode* root, int expense_id);
void DeleteExpense(ExpenseNode** root, int expense_id);
void UpdateFamilyExpenses(FamilyTree* tree, ExpenseNode* expenses, int user_id);
int compareExpenses(const void* a, const void* b);

//Final Functions
struct UserNode* AddUser(UserNode* root, const char* filename);
void AddExpense(ExpenseNode** root, const char* filename);
void get_total_expense(FamilyTree* familyTree, ExpenseNode* expenseRoot, int family_id, int month, int year);
void get_highest_expense_day(FamilyTree* familyTree, ExpenseNode* expenseRoot, int family_id);
void get_individual_expense(UserNode* userRoot, ExpenseNode* expenseRoot, int user_id, int month, int year);
void get_expense_in_period(ExpenseNode* expenseRoot, const char* start_date, const char* end_date);
void get_expense_in_range(UserNode* userRoot, ExpenseNode* expenseRoot, int expenseID_1, int expenseID_2, int individualID);


void writeUsersRecursive(UserNode* node, FILE* file);
void saveUsersToFile(UserNode* root, const char* filename);
void writeFamiliesRecursiveToFile(FamilyNode* node, FILE* file);
void saveFamiliesToFile(FamilyTree* tree, const char* filename,const char* tempFilename);


//Function to create a new user node
UserNode *createUserNode(int user_id, char* user_name,float income)
{
    UserNode *newNode =(UserNode *)malloc(sizeof(UserNode));
    newNode->user_id=user_id;
    strcpy(newNode->user_name,user_name);
    newNode->income=income;
    newNode->left=newNode->right=NULL;
    newNode->height=1;
    return newNode;
}


int compareExpenses(const void* a, const void* b) {
    const Expense* exp1 = *(const Expense**)a;
    const Expense* exp2 = *(const Expense**)b;
    return exp1->expense_id - exp2->expense_id; // Sort by ID ascending
}


//Function to count total expenses in the database
int CountExpenses(ExpenseNode *root)
{
    if(!root) return 0;
    int count=0;

    //Find the leftmost leaf node
    ExpenseNode *node=root;
    while(node && !node->is_leaf)
    {
        node=node->children[0];
    }

    //Count expenses in all leaf nodes
    while(node)
    {
        count+=node->num_keys;
        node=node->next;
    }
    return count;
}


//Function to Save Expenses to File
void writeExpensesToFile(ExpenseNode *root,const char *filename)
{
    if(!root) return;
    FILE *file = fopen(filename, "w");
    if(!file)
    {
        printf("Error opening file!\n");
        return;
    }

    ExpenseNode *node = root;
    while(node && !node->is_leaf)
    {
        node = node->children[0];
    }

    while(node)
    {
        for(int i=0; i<node->num_keys;i++)
        {
            fprintf(file, "%d %d %d %.2f %s\n",node->expenses[i].expense_id,node->expenses[i].user_id,node->expenses[i].category,node->expenses[i].amount,node->expenses[i].date);
        }
        node = node->next;
    }
    fclose(file);
}

//Function to Read Expenses from File
void readExpensesFromFile(ExpenseNode **root,const char *filename)
{
    FILE *file = fopen(filename,"r");
    if(!file) {
        printf("Could not open file %s\n", filename);
        return;
    }
    if (*root == NULL) {
        *root = createLeafNode();
        (*root)->is_leaf = 1;
    }
    Expense tempExpense;
    int tempNewKey;
    ExpenseNode *tempNewChild = NULL;
    int duplicate = 0;
    int count = 0;
    int duplicateCount = 0;

    if(*root == NULL)
    {
        *root = createLeafNode();
    }

    char line[256]; // Buffer for reading lines
    
    // Read file line by line to avoid getting stuck in any loop
    while(fgets(line, sizeof(line), file) != NULL)
    {
        // Check if we have reached the maximum number of expenses
        if(count >= MAX_EXPENSES)
        {
            printf("Warning: Maximum number of expenses (%d) reached. Skipping remaining expenses.\n", MAX_EXPENSES);
            break;        
        }

        // Parse the line
        if(sscanf(line, "%d %d %d %f %s", 
                 &tempExpense.expense_id, 
                 &tempExpense.user_id, 
                 (int*)&tempExpense.category, 
                 &tempExpense.amount, 
                 tempExpense.date) != 5) {
            printf("Warning: Invalid format in line. Skipping.\n");
            continue;
        }

        duplicate = 0;
        ExpenseNode* tempRoot = *root;
        ExpenseNode* splitRoot = InsertExpense(tempRoot, tempExpense, &tempNewKey, &tempNewChild, &duplicate);
        
        if (duplicate) {
            duplicateCount++;
            // Only print the first few duplicate warnings to avoid flooding console
            if (duplicateCount <= 5) {
                printf("Warning: Duplicate expense ID %d found in file. Skipping.\n", tempExpense.expense_id);
            } else if (duplicateCount == 6) {
                printf("Additional duplicate entries found. Suppressing further warnings.\n");
            }
            continue;
        }
        
        if (splitRoot) {
            ExpenseNode* newRoot = createLeafNode();
            newRoot->is_leaf = 0;
            newRoot->keys[0] = tempNewKey;
            newRoot->children[0] = tempRoot;
            newRoot->children[1] = tempNewChild;
            newRoot->num_keys = 1;
            *root = newRoot;
        }

        count++;
    }
    
    printf("Loaded %d expenses from file. Found %d duplicate entries.\n", count, duplicateCount);
    fclose(file);
}



//Function to get category name from enum
const char* getCategoryName(ExpenseCategory category) {
    switch(category) {
        case RENT: return "RENT";
        case UTILITY: return "UTILITY";
        case GROCERY: return "GROCERY";
        case STATIONARY: return "STATIONARY";
        case LEISURE: return "LEISURE";
        default: return "UNKNOWN";
    }
}

// Improved search function that prints debug information
void PrintTreeStructure(ExpenseNode *node, int level) {
    if (!node) return;

    printf("Level %d: ", level);
    if (node->is_leaf) {
        printf("Leaf [");
    } else {
        printf("Internal [");
    }
    
    for (int i = 0; i < node->num_keys; i++) {
        printf("%d", node->keys[i]);
        if (i < node->num_keys - 1) printf(", ");
    }
    printf("]\n");

    if (!node->is_leaf) {
        for (int i = 0; i <= node->num_keys; i++) {
            PrintTreeStructure(node->children[i], level + 1);
        }
    }
}


// Function to Print Expenses in a Tabular Format
void printExpensesTable(ExpenseNode* root) {
    if (!root) {
        printf("No expenses to display.\n");
        return;
    }
    
    // Find the leftmost leaf node (first node with expenses)
    ExpenseNode* node = root;
    while (node && !node->is_leaf) {
        node = node->children[0];
    }
    
    // Print table header
    printf("\n+--------+--------+---------------+----------+------------+\n");
    printf("| Exp ID | User ID |   Category    |  Amount  |    Date    |\n");
    printf("+--------+--------+---------------+----------+------------+\n");
    
    // Print all expenses in the linked list of leaf nodes
    int count = 0;
    while (node) {
        for (int i = 0; i < node->num_keys; i++) {
            printf("| %6d | %6d | %-13s | %8.2f | %-10s |\n", 
                   node->expenses[i].expense_id, 
                   node->expenses[i].user_id,
                   getCategoryName(node->expenses[i].category),
                   node->expenses[i].amount,
                   node->expenses[i].date);
            count++;
        }
        node = node->next;
    }
    
    // Print table footer
    printf("+--------+--------+---------------+----------+------------+\n");
    printf("Total expenses: %d\n\n", count);
}

//2.Function to add a new expense with user input
void AddExpense(ExpenseNode** root, const char* filename) {
    char choice;
    
    printf("\nDo you want to add a new expense? (y/n): ");
    scanf(" %c", &choice);
    
    while (choice == 'y' || choice == 'Y') {
        // Check if we've reached the maximum number of expenses
        if (CountExpenses(*root) >= MAX_EXPENSES) {
            printf("Maximum number of expenses (%d) reached. Cannot add more.\n", MAX_EXPENSES);
            return;
        }
        
        Expense newExpense;
        int category;
        
        // Ask for expense ID instead of generating it
        printf("\nEnter Expense ID: ");
        scanf("%d", &newExpense.expense_id);
        
        printf("Enter User ID: ");
        scanf("%d", &newExpense.user_id);
        
        printf("Enter Category (1:RENT, 2:UTILITY, 3:GROCERY, 4:STATIONARY, 5:LEISURE): ");
        scanf("%d", &category);
        
        // Validate category input
        if (category < 1 || category > 5) {
            printf("Invalid category. Using GROCERY as default.\n");
            newExpense.category = GROCERY;
        } else {
            newExpense.category = (ExpenseCategory)category;
        }
        
        printf("Enter Amount: ");
        scanf("%f", &newExpense.amount);
        
        printf("Enter Date (YYYY-MM-DD): ");
        scanf("%s", newExpense.date);
        
        // Insert the new expense
        int tempNewKey;
        ExpenseNode* tempNewChild = NULL;
        int duplicate = 0;
        
        ExpenseNode* splitRoot = InsertExpense(*root, newExpense, &tempNewKey, &tempNewChild, &duplicate);
        
        if (duplicate) {
            printf("Warning: Expense ID %d already exists. Please use a different ID.\n", newExpense.expense_id);
        } else {
            if (splitRoot) {
                ExpenseNode* newRoot = createLeafNode();
                newRoot->is_leaf = 0;
                newRoot->keys[0] = tempNewKey;
                newRoot->children[0] = *root;
                newRoot->children[1] = tempNewChild;
                newRoot->num_keys = 1;
                *root = newRoot;
            }
            
            printf("Added new expense (ID: %d).\n", newExpense.expense_id);
            
            // Save expenses to file
            writeExpensesToFile(*root, filename);
            printf("Expenses saved to %s\n", filename);
            
            // Print updated expenses
            printf("Updated expenses:\n");
            printExpensesTable(*root);
        }
        
        printf("\nDo you want to add another expense? (y/n): ");
        scanf(" %c", &choice);
    }
}

// Function to write a user to the file
void writeUserToFile(const char* filename, int user_id, char* user_name, float income) {
    FILE* file = fopen(filename, "a");
    if (!file) {
        printf("Error: Unable to open file %s for writing\n", filename);
        return;
    }
    fprintf(file, "%d, %s, %.2f\n", user_id, user_name, income);
    fclose(file);
}

// Function to get height
int getHeight(UserNode *node)
{
    if(!node) return 0;
    return node->height;
}

//Function to get balance factor
int getBalanceFactor(UserNode* node)
{
    if(!node) return 0;
    return getHeight(node->left)-getHeight(node->right);
}

//Right Rotate function
struct UserNode *rightRotate(UserNode *y)
{
    struct UserNode *x=y->left;
    struct UserNode *T2=x->right;
    x->right=y;
    y->left=T2;
    y->height= 1 + (getHeight(y->left) > getHeight(y->right) ? getHeight(y->left) : getHeight(y->right));
    x->height= 1 + (getHeight(x->left) > getHeight(x->right) ? getHeight(x->left) : getHeight(x->right));
    return x;
}

//Left Rotate function
struct UserNode* leftRotate(UserNode* x) {
    struct UserNode* y = x->right;
    struct UserNode* T2 = y->left;
    y->left = x;
    x->right = T2;
    x->height = 1 + (getHeight(x->left) > getHeight(x->right) ? getHeight(x->left) : getHeight(x->right));
    y->height = 1 + (getHeight(y->left) > getHeight(y->right) ? getHeight(y->left) : getHeight(y->right));
    return y;
}

// Insert a node into AVL tree
struct UserNode* insertUser(UserNode* root, int user_id, char* user_name, float income) 
{
    if (!root) return createUserNode(user_id, user_name, income);

    if (user_id < root->user_id)
        root->left = insertUser(root->left, user_id, user_name, income);

    else if (user_id > root->user_id)
        root->right = insertUser(root->right, user_id, user_name, income);

    else
        return root;

    root->height = 1 + (getHeight(root->left) > getHeight(root->right) ? getHeight(root->left) : getHeight(root->right));
    int balance = getBalanceFactor(root);

    if (balance > 1 && user_id < root->left->user_id)
        return rightRotate(root);

    if (balance < -1 && user_id > root->right->user_id)
        return leftRotate(root);

    if (balance > 1 && user_id > root->left->user_id) 
    {
        root->left = leftRotate(root->left);
        return rightRotate(root);
    }
    if (balance < -1 && user_id < root->right->user_id) 
    {
        root->right = rightRotate(root->right);
        return leftRotate(root);
    }
    return root;
}

// Function to search for a user in the AVL Tree
int searchUser(UserNode *root,int user_id)
{
    if(!root) return 0;
    if(user_id == root->user_id) return 1;
    if(user_id < root->user_id)
       return searchUser(root->left,user_id);
    return searchUser(root->right,user_id);
}

// Load users from file and insert into AVL Tree
struct UserNode* loadUsersFromFile(const char* filename,UserNode* root) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error: Unable to open file %s\n", filename);
        return root;
    }
    int user_id;
    char user_name[MAX_NAME_LENGTH];
    float income;
    while (fscanf(file, "%d, %49[^,], %f", &user_id, user_name, &income) == 3) {
        root = insertUser(root, user_id, user_name, income);
    }
    fclose(file);
    return root;
}

//1.Add User function
struct UserNode* AddUser(UserNode* root, const char* filename) 
{
    int user_id;
    char user_name[MAX_NAME_LENGTH];
    float income;
    
    printf("Enter User ID: ");
    scanf("%d", &user_id);

    if(searchUser(root,user_id))
    {
        printf("Error: User ID %d already exists. Please enter a unique ID.\n",user_id);
        return root;
    }
    printf("Enter User Name: ");
    scanf("%49s", user_name);
    printf("Enter Income: ");
    scanf("%f", &income);
    
    root = insertUser(root, user_id, user_name, income);
    printf("Successfully inserted a new user!\n");
    writeUserToFile(filename, user_id, user_name, income);
    return root;
}

// In-order traversal to print sorted users
void inOrder(UserNode* root) {
    if (root) {
        inOrder(root->left);
        printf("| %-5d | %-20s | %-10.2f |\n", root->user_id, root->user_name, root->income);
        inOrder(root->right);
    }
}

//Making a table header
// Function to print a table header
void printUserTable(UserNode* root) {
    printf("\n--------------------------------------------------\n");
    printf("|ID     3| Name                | Income    |\n");
    printf("--------------------------------------------------\n");
    inOrder(root);
    printf("--------------------------------------------------\n");
}


// Implementation for createFamilyN function
Family* createFamilyN(int family_id, const char* family_name) {
    Family* family = (Family*)malloc(sizeof(Family));
    if (family) {
        family->family_id = family_id;
        strncpy(family->family_name, family_name, MAX_NAME_LENGTH - 1);
        family->family_name[MAX_NAME_LENGTH - 1] = '\0';
        family->member_count = 0;
        family->total_income = 0.0;
        family->total_monthly_expense = 0.0;
        for (int i = 0; i < MAX_MEMBERS; i++) {
            family->members[i] = NULL;
        }
    }
    return family;
}

//Function to create a new FamilyNode (B-Tree Node)
FamilyNode *createFamilyNode()
{
    FamilyNode *newNode = (FamilyNode *)malloc(sizeof(FamilyNode));
    if(!newNode)
    {
        printf("Memory allocation failed for family node\n");
        return NULL;
    }

    newNode->num_keys=0;
    newNode->is_leaf=1;

    for(int i = 0; i < MAX_KEYS;i++)
    {
        newNode->keys[i] = 0;
        newNode->families[i] =  NULL;
    }

    for(int i = 0; i < MAX_CHILDREN;i++)
    {
        newNode->children[i] = NULL;
    }

    return newNode;
}

//Function to create a new FamilyTree
FamilyTree *createFamilyTree()
{
    FamilyTree *newTree = (FamilyTree *)malloc(sizeof(FamilyTree));
    if(!newTree)
    {
        printf("Memory allocation failed for family tree\n");
        return NULL;
    }

    newTree->root = NULL;
    newTree->count = 0;

    return newTree;
}

// Helper function to check if a user already belongs to any family
int isUserInAnyFamily(FamilyNode* node, int user_id) {
    if (!node) return 0;
    
    // Check all families in current node
    for (int i = 0; i < node->num_keys; i++) {
        Family* family = node->families[i];
        for (int j = 0; j < family->member_count; j++) {
            if (family->members[j] && family->members[j]->user_id == user_id) {
                return 1; // User found in this family
            }
        }
    }
    
    // Check all children if not a leaf node
    if (!node->is_leaf) {
        for (int i = 0; i <= node->num_keys; i++) {
            if (isUserInAnyFamily(node->children[i], user_id)) {
                return 1;
            }
        }
    }
    
    return 0; // User not found in any family
}

//Function to calculate total monthly expenses for a family
float calculateTotalMonthlyExpense(ExpenseNode *expenseRoot,Family *family)
{
    if(!expenseRoot || !family) return 0.0;

    float totalExpense = 0.0;

    //Find the leftmost leaf node
    ExpenseNode *node = expenseRoot;
    while(node && !node->is_leaf)
    {
        node = node->children[0];
    }

    //Traverse all expense nodes
    while (node)
    {
        for(int i = 0;i < node->num_keys;i++)
        {
            //check if the expense belongs to any family member
            for(int j = 0;j < family->member_count;j++)
            {
                if(family->members[j] && node->expenses[i].user_id == family->members[j]->user_id)
                {
                    totalExpense += node->expenses[i].amount;
                }
            }
        }
        node = node->next;
    }

    return totalExpense;
}

//Function to search for a family by ID in a B-tree
Family *searchFamily(FamilyNode *node,int family_id)
{
    if(!node) return NULL;

    int i = 0;
    while(i < node->num_keys && family_id > node->keys[i])
    {
        i++;
    }

    if(i < node->num_keys && family_id == node->keys[i])
    {
        return node->families[i];
    }

    if(node->is_leaf)
    {
        return NULL; //Not found in leaf
    }

    return searchFamily(node->children[i],family_id); //Search in appropriate child
}

//Function to find the position where new key should be inserted

//Helper function to split a child node when it's full
void splitChild(FamilyNode *parent, int index, FamilyNode *child)
{
    //Create a new node which will store (MAX_KEYS-1)/2 keys of child
    FamilyNode *newNode = createFamilyNode();
    newNode->is_leaf = child->is_leaf;

    //Since MAX_KEYS is 4, we will have to move 2 keys to the new node
    int mid = MAX_KEYS/2;
    newNode->num_keys = mid;

    //Copy the second half of keys and families from child to newNode
    for(int i = 0;i < mid;i++)
    {
        newNode->keys[i] = child->keys[i + mid];
        newNode->families[i] = child->families[i + mid];
    }

    //If child is not a leaf, also copy the relevant children pointers
    if(!child->is_leaf)
    {
        for(int i=0;i<=mid;i++)
        {
            newNode->children[i] = child->children[i + mid];
            child->children[i + mid] = NULL; // Clear old pointers
        }
    }

    //Reduce the number of keys in child
    child->num_keys = mid;

    //Make room for a new child in parent
    for(int i = parent->num_keys; i > index;i--)
    {
        parent->children[i + 1] = parent->children[i];
    }

    //Link the new child to the parent
    parent->children[index + 1] = newNode;

    //Move the middle key of child to the parent
    for(int i = parent->num_keys - 1;i >= index;i--)  // FIXED: changed i++ to i--
    {
        parent->keys[i + 1] = parent->keys[i];
        parent->families[i + 1] = parent->families[i];
    }

    parent->keys[index] = child->keys[mid - 1];
    parent->families[index] = child->families[mid - 1];

    // Clear the moved key from child
    child->keys[mid - 1] = 0;
    child->families[mid - 1] = NULL;
    child->num_keys--;

    //Increment the number of keys in parent
    parent->num_keys++;
}


//Function to insert a non-full node
void insertNonFull(FamilyNode* node, int family_id, Family* family) {
    int i = node->num_keys - 1;

    if (node->is_leaf) {
        // Shift keys to make space
        while (i >= 0 && family_id < node->keys[i]) {
            node->keys[i+1] = node->keys[i];
            node->families[i+1] = node->families[i];
            i--;
        }
        node->keys[i+1] = family_id;
        node->families[i+1] = family;
        node->num_keys++;
        printf("Inserted family %d into leaf node\n", family_id); // Debug
    } else {
        // Find child position
        while (i >= 0 && family_id < node->keys[i]) i--;
        i++;
        
        // Split full child
        if (node->children[i]->num_keys == MAX_KEYS) {
            splitChild(node, i, node->children[i]);
            if (family_id > node->keys[i]) i++;
        }
        insertNonFull(node->children[i], family_id, family);
    }
}



// Function to insert a family into the B-tree
void insertFamily(FamilyTree* tree, int family_id, Family* family) {
    if (!tree->root) {
        tree->root = createFamilyNode();
        tree->root->keys[0] = family_id;
        tree->root->families[0] = family;
        tree->root->num_keys = 1;
        tree->count++;
        return;
    }

    FamilyNode* root = tree->root;

    // Split root if full
    if (root->num_keys == MAX_KEYS) {
        FamilyNode* newRoot = createFamilyNode();
        newRoot->is_leaf = 0;
        newRoot->children[0] = root;
        splitChild(newRoot, 0, root);
        
        // Determine which child to insert into
        int i = 0;
        if (family_id > newRoot->keys[0]) i++;
        insertNonFull(newRoot->children[i], family_id, family);
        tree->root = newRoot;
    } else {
        insertNonFull(root, family_id, family);
    }
    tree->count++;
}


// Helper function for writeFamiliesToFile (moved out of the parent function)
void writeFamiliesRecursive(FamilyNode* node, FILE* file) {
    if (!node) return;
    
    // If not leaf, process leftmost child first
    if (!node->is_leaf) {
        writeFamiliesRecursive(node->children[0], file);
    }
    
    // Process all keys in this node
    for (int i = 0; i < node->num_keys; i++) {
        Family* family = node->families[i];
        
        // Write basic family info
        fprintf(file, "%d,%s,%d,%.2f,%.2f", 
              family->family_id, family->family_name, 
              family->member_count, family->total_income,
              family->total_monthly_expense);
        
        // Write member IDs
        for (int j = 0; j < family->member_count; j++) {
            if (family->members[j]) {
                fprintf(file, ",%d", family->members[j]->user_id);
            } else {
                fprintf(file, ",0");
            }
        }
        fprintf(file, "\n");
        
        // If not leaf, process child to the right of this key
        if (!node->is_leaf) {
            writeFamiliesRecursive(node->children[i+1], file);
        }
    }
}


// Simple function to write families to a file
void writeFamiliesToFile(FamilyTree* tree, const char* filename) {
    if (!tree || !tree->root) {
        printf("No families to save.\n");
        return;
    }
    
    FILE* file = fopen(filename, "w");  // Use "w" to overwrite with all current data
    if (!file) {
        printf("Error opening file for writing families\n");
        return;
    }
    
    // Start the recursive function
    writeFamiliesRecursive(tree->root, file);
    
    fclose(file);
    printf("Families saved to file successfully.\n");
}

// Helper function to find a user by ID (used by loadFamiliesFromFile)
UserNode* findUserById(UserNode* root, int id) {
    if (!root) return NULL;
    if (root->user_id == id) return root;
    if (id < root->user_id) return findUserById(root->left, id);
    return findUserById(root->right, id);
}

void createFamily(FamilyTree* familyTree, UserNode* userRoot, ExpenseNode* expenseRoot, const char* filename) {
    if (!familyTree || !userRoot) {
        printf("Error: Invalid family tree or user database.\n");
        return;
    }
    
    char choice = 'y';
    
    while (choice == 'y' || choice == 'Y') {
        // Check if we've reached the maximum number of families
        if (familyTree->count >= MAX_FAMILIES) {
            printf("Maximum number of families (%d) reached. Cannot add more.\n", MAX_FAMILIES);
            return;
        }
        
        int family_id;
        char family_name[MAX_NAME_LENGTH];
        int member_count = 0;
        
        printf("\n===== Create New Family =====\n");
        printf("Enter Family ID: ");
        scanf("%d", &family_id);
        
        // Check if family ID already exists
        if (searchFamily(familyTree->root, family_id)) {
            printf("Error: Family ID %d already exists. Please use a unique ID.\n", family_id);
            printf("Do you want to create another family? (y/n): ");
            scanf(" %c", &choice);
            continue;
        }
        
        printf("Enter Family Name: ");
        scanf(" %49[^\n]", family_name);
        
        printf("How many members (1-4): ");
        scanf("%d", &member_count);
        
        if (member_count < 1 || member_count > MAX_MEMBERS) {
            printf("Invalid number of members. Family must have 1-%d members.\n", MAX_MEMBERS);
            printf("Do you want to create another family? (y/n): ");
            scanf(" %c", &choice);
            continue;
        }
        
        // Create the family
        Family* newFamily = createFamilyN(family_id, family_name);
        if (!newFamily) {
            printf("Error: Failed to create family. Memory allocation error.\n");
            printf("Do you want to try again? (y/n): ");
            scanf(" %c", &choice);
            continue;
        }
        
        // Add members to the family
        printf("\nEnter User IDs for family members:\n");
        newFamily->member_count = 0; // Reset to ensure accurate count
        
        // Array to track user IDs already added to this family
        int addedUserIds[MAX_MEMBERS] = {0};
        int addedCount = 0;
        
        for (int i = 0; i < member_count; i++) {
            int user_id;
            printf("Member %d User ID: ", i + 1);
            scanf("%d", &user_id);
            
            // Check if user exists 
            UserNode* user = findUserById(userRoot, user_id);
            if (!user) {
                printf("Error: User ID %d does not exist. Please enter a valid user ID.\n", user_id);
                i--; // Retry this member
                continue;
            }
            
            // Check if user already belongs to another family
            if (isUserInAnyFamily(familyTree->root, user_id)) {
                printf("Error: User ID %d already belongs to another family. Users cannot be in multiple families.\n", user_id);
                i--; // Retry this member
                continue;
            }
            
            // Check if user is already added to this family
            int isDuplicate = 0;
            for (int j = 0; j < addedCount; j++) {
                if (addedUserIds[j] == user_id) {
                    isDuplicate = 1;
                    break;
                }
            }
            
            if (isDuplicate) {
                printf("Error: User ID %d is already added to this family. Each user can only be added once.\n", user_id);
                i--; // Retry this member
                continue;
            }
            
            // Add user to family
            newFamily->members[i] = user;
            newFamily->member_count++;
            newFamily->total_income += user->income;
            
            // Track this user ID as added
            addedUserIds[addedCount++] = user_id;
        }
        
        // Calculate total monthly expense
        newFamily->total_monthly_expense = calculateTotalMonthlyExpense(expenseRoot, newFamily);
        
        // Insert family into B-tree
        insertFamily(familyTree, family_id, newFamily);
        
        printf("\nFamily created successfully!\n");
        printf("Family ID: %d\n", newFamily->family_id);
        printf("Family Name: %s\n", newFamily->family_name);
        printf("Member Count: %d\n", newFamily->member_count);
        printf("Total Income: %.2f\n", newFamily->total_income);
        printf("Total Monthly Expense: %.2f\n", newFamily->total_monthly_expense);
        
        // Save families to file
        writeFamiliesToFile(familyTree, filename);
        
        printf("\nDo you want to create another family? (y/n): ");
        scanf(" %c", &choice);
    }
}

// Simple function to load families from a file with improved error handling
FamilyTree* loadFamiliesFromFile(const char* filename, UserNode* userRoot) {
    FamilyTree* tree = createFamilyTree();
    FILE* file = fopen(filename, "r");
    if (!file) return tree;

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        // Skip empty/invalid lines
        if (strlen(line) < 10) continue;

        // Parse using sscanf for structured data
        int family_id, member_count;
        char family_name[MAX_NAME_LENGTH];
        float total_income, total_monthly_expense;
        int members[MAX_MEMBERS] = {0};

        // Parse first 5 fixed fields
        int parsed = sscanf(line, "%d,%49[^,],%d,%f,%f", 
            &family_id, family_name, &member_count, 
            &total_income, &total_monthly_expense);

        if (parsed != 5) continue;

        // Parse member IDs
        char* ptr = strchr(line, ',');
        for (int i = 0; i < 4; i++) ptr = strchr(ptr+1, ','); // Skip first 5 fields
        ptr++;

        for (int i = 0; i < member_count && i < MAX_MEMBERS; i++) {
            if (sscanf(ptr, "%d", &members[i]) != 1) break;
            ptr = strchr(ptr, ',');
            if (!ptr) break;
            ptr++;
        }

        // Create family
        Family* family = createFamilyN(family_id, family_name);
        family->total_income = total_income;
        family->total_monthly_expense = total_monthly_expense;
        family->member_count = member_count;

        // Link members
        for (int i = 0; i < member_count; i++) {
            family->members[i] = findUserById(userRoot, members[i]);
        }

        // Insert into B-tree
        insertFamily(tree, family_id, family);
    }
    fclose(file);
    return tree;
}


// Modified function to print families in a node with member information
void printFamiliesInNode(FamilyNode* node) {
    if (!node) return;
    
    // For non-leaf nodes, first print the left subtree
    if (!node->is_leaf) {
        printFamiliesInNode(node->children[0]);
    }
    
    // Print all keys in this node
    for (int i = 0; i < node->num_keys; i++) {
        Family* family = node->families[i];
        printf("| %-8d | %-18s | %-6d | %-13.2f | %-18.2f |\n", 
               family->family_id, 
               family->family_name,
               family->member_count,
               family->total_income,
               family->total_monthly_expense);
        
        // Print member information for this family
        printf("+----------+--------------------+--------+---------------+--------------------+\n");
        printf("| Members of family %-38s |\n", family->family_name);
        printf("+----------+--------------------+--------+---------------+--------------------+\n");
        printf("| User ID  | User Name          | Income        |\n");
        printf("+----------+--------------------+---------------+\n");
        
        for (int j = 0; j < family->member_count; j++) {
            if (family->members[j]) {
                printf("| %-8d | %-18s | %-13.2f |\n",
                       family->members[j]->user_id,
                       family->members[j]->user_name,
                       family->members[j]->income);
            }
        }
        printf("+----------+--------------------+---------------+\n");
        
        // For non-leaf nodes, print subtrees between keys
        if (!node->is_leaf) {
            printFamiliesInNode(node->children[i+1]);
        }
    }
}

// Updated function to print families in a tabular format
void printFamiliesTable(FamilyTree* tree) {
    if (!tree || !tree->root) {
        printf("No families to display.\n");
        return;
    }
    
    printf("\n+----------+--------------------+--------+---------------+--------------------+\n");
    printf("| Family ID | Family Name        | Members | Total Income  | Monthly Expenses   |\n");
    printf("+----------+--------------------+--------+---------------+--------------------+\n");
    
    printFamiliesInNode(tree->root);
    
    printf("Total families: %d\n\n", tree->count);
}
// Helper function to get maximum of two integers
int Max(int a, int b) {
    return (a > b) ? a : b;
}

// Function to update user details
UserNode* updateUser(UserNode* root, int user_id, char* new_name, float new_income) {
    // Find the user node
    UserNode* current = root;
    
    while (current != NULL) {
        if (user_id < current->user_id) {
            current = current->left;
        } else if (user_id > current->user_id) {
            current = current->right;
        } else {
            // User found, update details
            strcpy(current->user_name, new_name);
            current->income = new_income;
            return root;
        }
    }
    
    printf("User with ID %d not found.\n", user_id);
    return root;
}



// Function to get the total expense for a family for a specific month and year
void get_total_expense(FamilyTree* familyTree, ExpenseNode* expenseRoot, int family_id, int month, int year) {
    // Search for the family in the family tree
    FamilyNode* root = familyTree->root;
    Family* family = searchFamily(root, family_id);
    
    if (family == NULL) {
        printf("Family with ID %d not found.\n", family_id);
        return;
    }
    
    // Calculate total expenses for the family in the given month and year
    float total_expense = 0.0f;
    float individual_expenses[MAX_MEMBERS] = {0.0f}; // Track expenses for each member
    
    // Loop through all expenses in the B+ tree
    ExpenseNode* current = expenseRoot;
    
    // First, find the leftmost leaf node
    while (current != NULL && !current->is_leaf) {
        current = current->children[0];
    }
    
    // Now traverse all leaf nodes
    while (current != NULL) {
        for (int i = 0; i < current->num_keys; i++) {
            Expense expense = current->expenses[i];
            
            // Extract month and year from date (format: YYYY-MM-DD)
            int exp_year, exp_month, exp_day;
            sscanf(expense.date, "%d-%d-%d", &exp_year, &exp_month, &exp_day);
            
            // Check if expense is in the specified month and year
            if (exp_year == year && exp_month == month) {
                // Check if this expense belongs to a family member
                for (int j = 0; j < family->member_count; j++) {
                    if (expense.user_id == family->members[j]->user_id) {
                        total_expense += expense.amount;
                        individual_expenses[j] += expense.amount;
                        break;
                    }
                }
            }
        }
        current = current->next; // Move to the next leaf node
    }
    
    // Print family information
    printf("\n========== Family Expense Report ==========\n");
    printf("Family ID: %d\n", family->family_id);
    printf("Family Name: %s\n", family->family_name);
    printf("Total Members: %d\n", family->member_count);
    printf("Period: %d-%d (Month-Year)\n", month, year);
    printf("Total Family Income: Rs. %.2f\n", family->total_income);
    printf("Total Monthly Expense: Rs. %.2f\n", total_expense);
    
    // Compare expense with income and calculate difference
    float difference = family->total_income - total_expense;
    
    if (difference >= 0) {
        printf("Status: Expense is WITHIN family income\n");
        printf("Amount Saved: Rs. %.2f\n", difference);
    } else {
        printf("Status: Expense EXCEEDS family income\n");
        printf("Budget Deficit: Rs. %.2f\n", -difference);
    }
    
    // Print individual contribution to expenses
    printf("\nIndividual Members Expenses:\n");
    for (int i = 0; i < family->member_count; i++) {
        UserNode* member = family->members[i];
        printf("%d. %s (ID: %d) - Income: Rs. %.2f, Expenses: Rs. %.2f\n", 
               i+1, member->user_name, member->user_id, member->income, individual_expenses[i]);
    }
    
    printf("==========================================\n");
}

// Function to get total family expense for a specific category
void get_categorical_expense(FamilyTree* familyTree, ExpenseNode* expenseRoot, int family_id, ExpenseCategory category) {
    // Step 1: Find the family
    Family* family = searchFamily(familyTree->root, family_id);
    if (family == NULL) {
        printf("Family with ID %d not found!\n", family_id);
        return;
    }
    
    // Step 2: Initialize variables to store individual contributions
    float total_category_expense = 0.0;
    typedef struct {
        int user_id;
        char user_name[MAX_NAME_LENGTH];
        float expense_amount;
    } UserExpense;
    
    UserExpense member_expenses[MAX_MEMBERS];
    for (int i = 0; i < family->member_count; i++) {
        member_expenses[i].user_id = family->members[i]->user_id;
        strcpy(member_expenses[i].user_name, family->members[i]->user_name);
        member_expenses[i].expense_amount = 0.0;
    }
    
    // Step 3: Traverse the expense tree to find expenses of the given category for family members
    ExpenseNode* current = expenseRoot;
    
    // Find leftmost leaf node to start traversal
    while (current && !current->is_leaf) {
        current = current->children[0];
    }
    
    // Traverse leaf nodes using the B+ tree linked list structure
    while (current != NULL) {
        for (int i = 0; i < current->num_keys; i++) {
            Expense* exp = &current->expenses[i];
            
            // Check if this expense belongs to any family member and matches the category
            for (int j = 0; j < family->member_count; j++) {
                if (exp->user_id == family->members[j]->user_id && exp->category == category) {
                    member_expenses[j].expense_amount += exp->amount;
                    total_category_expense += exp->amount;
                }
            }
        }
        current = current->next; // Move to next leaf node
    }
    
    // Step 4: Sort individual contributions in descending order
    for (int i = 0; i < family->member_count - 1; i++) {
        for (int j = 0; j < family->member_count - i - 1; j++) {
            if (member_expenses[j].expense_amount < member_expenses[j + 1].expense_amount) {
                UserExpense temp = member_expenses[j];
                member_expenses[j] = member_expenses[j + 1];
                member_expenses[j + 1] = temp;
            }
        }
    }
    
    // Step 5: Print the results
    printf("\n==== Categorical Expense Report ====\n");
    printf("Family ID: %d\n", family_id);
    printf("Family Name: %s\n", family->family_name);
    printf("Category: %s\n", getCategoryName(category));
    printf("Total %s Expense: Rs. %.2f\n\n", getCategoryName(category), total_category_expense);
    
    printf("Individual Contributions (Sorted):\n");
    printf("-----------------------------------\n");
    printf("%-5s %-20s %-15s %-10s\n", "ID", "Name", "Amount", "Percentage");
    printf("-----------------------------------\n");
    
    for (int i = 0; i < family->member_count; i++) {
        if (member_expenses[i].expense_amount > 0) {
            float percentage = (member_expenses[i].expense_amount / total_category_expense) * 100;
            printf("%-5d %-20s Rs. %-10.2f %.2f%%\n", 
                   member_expenses[i].user_id, 
                   member_expenses[i].user_name, 
                   member_expenses[i].expense_amount, 
                   percentage);
        } else {
            printf("%-5d %-20s Rs. %-10.2f 0.00%%\n", 
                   member_expenses[i].user_id, 
                   member_expenses[i].user_name, 
                   0.0);
        }
    }
    printf("-----------------------------------\n");
    
    // Step 6: Compare with family income to determine if this category expense is within budget
    float category_percentage = (total_category_expense / family->total_income) * 100;
    printf("\nCategory expense as percentage of family income: %.2f%%\n", category_percentage);
    
    if (category_percentage > 30) {
        printf("Warning: Expenses in this category exceed 30%% of family income!\n");
    }
}

// Function to get the day with the highest expense for a family
void get_highest_expense_day(FamilyTree* familyTree, ExpenseNode* expenseRoot, int family_id) 
{
    // Step 1: Find the specified family
    Family* family = searchFamily(familyTree->root, family_id);
    if (family == NULL) {
        printf("Family with ID %d not found!\n", family_id);
        return;
    }
    
    // Step 2: Create a structure to track expenses by date
    typedef struct 
    {
        char date[DATE_LENGTH];
        float total_amount;
    } DateExpense;
    
    // Set maximum number of unique dates we can track
    // Assuming we won't have more than 100 unique dates in our dataset
    #define MAX_DATES 100
    DateExpense dateExpenses[MAX_DATES];
    int date_count = 0;
    
    // Step 3: Initialize the date expense array
    for (int i = 0; i < MAX_DATES; i++) {
        strcpy(dateExpenses[i].date, "");
        dateExpenses[i].total_amount = 0.0;
    }
    
    // Step 4: Traverse the expense tree to find all expenses for family members
    ExpenseNode* current = expenseRoot;
    
    // Find leftmost leaf node to start traversal
    while (current && !current->is_leaf) {
        current = current->children[0];
    }
    
    // Traverse leaf nodes using the B+ tree linked list structure
    while (current != NULL) {
        for (int i = 0; i < current->num_keys; i++) {
            Expense* exp = &current->expenses[i];
            
            // Check if this expense belongs to any family member
            for (int j = 0; j < family->member_count; j++) {
                if (exp->user_id == family->members[j]->user_id) {
                    // Found an expense by a family member
                    
                    // Check if we already have this date in our array
                    int date_index = -1;
                    for (int k = 0; k < date_count; k++) {
                        if (strcmp(dateExpenses[k].date, exp->date) == 0) {
                            date_index = k;
                            break;
                        }
                    }
                    
                    if (date_index == -1) {
                        // This is a new date
                        if (date_count < MAX_DATES) {
                            strcpy(dateExpenses[date_count].date, exp->date);
                            dateExpenses[date_count].total_amount = exp->amount;
                            date_count++;
                        } else {
                            printf("Warning: Maximum unique dates exceeded!\n");
                        }
                    } else {
                        // Add to existing date
                        dateExpenses[date_index].total_amount += exp->amount;
                    }
                    
                    // We found an expense for this family member, 
                    // no need to check other members for this expense
                    break;
                }
            }
        }
        current = current->next; // Move to next leaf node
    }
    
    // Step 5: Find the date with the highest expense
    if (date_count == 0) {
        printf("No expenses found for this family!\n");
        return;
    }
    
    int highest_index = 0;
    for (int i = 1; i < date_count; i++) {
        if (dateExpenses[i].total_amount > dateExpenses[highest_index].total_amount) {
            highest_index = i;
        }
    }
    
    // Step 6: Print the results
    printf("\n===== Highest Expense Day Report =====\n");
    printf("Family ID: %d\n", family_id);
    printf("Family Name: %s\n", family->family_name);
    printf("\nDate with highest expense: %s\n", dateExpenses[highest_index].date);
    printf("Total amount spent: Rs. %.2f\n", dateExpenses[highest_index].total_amount);
    
    // Step 7: Print additional information about all recorded dates
    printf("\nAll Expense Days (in descending order of expense):\n");
    printf("---------------------------------------------------\n");
    printf("%-15s %-20s\n", "Date", "Total Amount");
    printf("---------------------------------------------------\n");
    
    // Sort dates by expense amount (descending)
    for (int i = 0; i < date_count - 1; i++) {
        for (int j = 0; j < date_count - i - 1; j++) {
            if (dateExpenses[j].total_amount < dateExpenses[j + 1].total_amount) {
                DateExpense temp = dateExpenses[j];
                dateExpenses[j] = dateExpenses[j + 1];
                dateExpenses[j + 1] = temp;
            }
        }
    }
    
    // Print sorted dates
    for (int i = 0; i < date_count; i++) {
        printf("%-15s Rs. %-20.2f\n", dateExpenses[i].date, dateExpenses[i].total_amount);
    }
    printf("---------------------------------------------------\n");
    
    // Step 8: Calculate some additional statistics
    float average_daily_expense = 0.0;
    float total_all_days = 0.0;
    
    for (int i = 0; i < date_count; i++) {
        total_all_days += dateExpenses[i].total_amount;
    }
    
    average_daily_expense = total_all_days / date_count;
    
    printf("\nTotal expenses across all days: Rs. %.2f\n", total_all_days);
    printf("Average daily expense: Rs. %.2f\n", average_daily_expense);
    printf("Highest day expense is %.2f%% of the family's monthly income\n", 
           (dateExpenses[highest_index].total_amount / family->total_income) * 100);
}

// Function to get individual expense for a specified user ID
void get_individual_expense(UserNode* userRoot, ExpenseNode* expenseRoot, int user_id, int month, int year) {
    // First check if the user exists
    if (!searchUser(userRoot, user_id)) {
        printf("Error: User with ID %d not found!\n", user_id);
        return;
    }
    
    // Array to store expenses by category
    float category_expenses[5] = {0}; // Indexed by ExpenseCategory enum (RENT=1, etc.)
    float total_expense = 0;
    
    // Traverse the B+ tree to find all expenses for this user
    ExpenseNode* current = expenseRoot;
    
    // First find a leaf node
    while (current && !current->is_leaf) {
        int i;
        for (i = 0; i < current->num_keys; i++) {
            if (user_id < current->keys[i]) break;
        }
        current = current->children[i];
    }
    
    // Traverse all leaf nodes
    while (current != NULL) {
        for (int i = 0; i < current->num_keys; i++) {
            Expense* exp = &current->expenses[i];
            
            // Check if this expense belongs to the specified user
            if (exp->user_id == user_id) {
                // Parse the date to check month and year
                int exp_year, exp_month, exp_day;
                sscanf(exp->date, "%d-%d-%d", &exp_year, &exp_month, &exp_day);
                
                // Check if this expense is in the specified month and year
                if (exp_month == month && exp_year == year) {
                    // Add to the appropriate category total
                    if (exp->category >= RENT && exp->category <= LEISURE) {
                        category_expenses[exp->category - 1] += exp->amount;
                        total_expense += exp->amount;
                    }
                }
            }
        }
        current = current->next; // Move to the next leaf node
    }
    
    // Print the total expense
    printf("Total expense for User ID %d in %d-%d: $%.2f\n", user_id, year, month, total_expense);
    
    // Create an array of category-expense pairs for sorting
    typedef struct {
        ExpenseCategory category;
        float amount;
    } CategoryExpense;
    
    CategoryExpense sorted_expenses[5];
    for (int i = 0; i < 5; i++) {
        sorted_expenses[i].category = i + 1; // Categories are 1-indexed
        sorted_expenses[i].amount = category_expenses[i];
    }
    
    // Sort in descending order by amount
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4 - i; j++) {
            if (sorted_expenses[j].amount < sorted_expenses[j + 1].amount) {
                CategoryExpense temp = sorted_expenses[j];
                sorted_expenses[j] = sorted_expenses[j + 1];
                sorted_expenses[j + 1] = temp;
            }
        }
    }
    
    // Print category-wise expenses in descending order
    printf("Category-wise expenses (descending order):\n");
    for (int i = 0; i < 5; i++) {
        if (sorted_expenses[i].amount > 0) {
            printf("%s: $%.2f\n", getCategoryName(sorted_expenses[i].category), sorted_expenses[i].amount);
        }
    }
}

// Function to check if a date falls within a given range
int isDateInRange(const char* date, const char* start_date, const char* end_date) 
{
    // Using string comparison for comparing the dates

    if (strcmp(date, start_date) >= 0 && strcmp(date, end_date) <= 0) 
    {
        return 1; // Date is within range
    }
    return 0; // Date is outside range
}

// Function to print an individual expense
void printExpenseDetails(Expense expense) 
{
    printf("| %-10d | %-10d | %-12s | %-10.2f | %-12s |\n", 
           expense.expense_id, 
           expense.user_id, 
           getCategoryName(expense.category), 
           expense.amount, 
           expense.date);
}

// Helper function to collect expenses within date range from B+ tree
//A wrapper function
// Helper function to collect expenses within date range from B+ tree
void collectExpensesInDateRange(ExpenseNode* node, const char* start_date, const char* end_date, Expense** expenses, int* count) {
    if (node == NULL) return;
    
    // First, navigate to leaf level using standard B+ tree traversal
    if (!node->is_leaf) {
        // For non-leaf nodes, just traverse down to leaves
        for (int i = 0; i <= node->num_keys; i++) {
            if (node->children[i] != NULL) {
                collectExpensesInDateRange(node->children[i], start_date, end_date, expenses, count);
            }
        }
        return; 
    }
    
    // For leaf nodes, process expenses in this node 
    for (int i = 0; i < node->num_keys; i++) {
        if (isDateInRange(node->expenses[i].date, start_date, end_date)) {
            expenses[*count] = &(node->expenses[i]);
            (*count)++;
        }
    }
}

// Function to get all expenses within a given date range
void get_expense_in_period(ExpenseNode* expenseRoot, const char* start_date, const char* end_date) {
    if (expenseRoot == NULL) {
        printf("No expenses found in the database.\n");
        return;
    }
    
    // Count total expenses first to allocate array
    int totalExpenses = CountExpenses(expenseRoot);
    if (totalExpenses == 0) {
        printf("No expenses found in the database.\n");
        return;
    }
    
    // Create an array to store pointers to expenses within the date range
    Expense* filteredExpenses[MAX_EXPENSES];
    int count = 0;
    
    // Collect all expenses within the date range
    collectExpensesInDateRange(expenseRoot, start_date, end_date, filteredExpenses, &count);
    
    if (count == 0) 
    {
        printf("No expenses found between %s and %s.\n", start_date, end_date);
        return;
    }
    
    // Print header
    printf("\n=== Expenses from %s to %s ===\n", start_date, end_date);
    printf("+------------+------------+--------------+------------+--------------+\n");
    printf("| Expense ID | User ID    | Category     | Amount     | Date         |\n");
    printf("+------------+------------+--------------+------------+--------------+\n");
    
    // Print all expenses in the date range
    for (int i = 0; i < count; i++) 
    {
        printExpenseDetails(*(filteredExpenses[i]));
    }
    
    printf("+------------+------------+--------------+------------+--------------+\n");
    
    // Calculate total amount
    float totalAmount = 0;
    for (int i = 0; i < count; i++) {
        totalAmount += filteredExpenses[i]->amount;
    }
    
    printf("Total expenses in this period: %.2f\n", totalAmount);
    printf("Number of expense entries: %d\n", count);
}

void collectExpensesInIDRange(ExpenseNode* root, int start_id, int end_id, int user_id, Expense** expenses, int* count) {
    if (root == NULL) return;
    
    // Find leaf node containing start_id
    ExpenseNode* current = root;
    while (!current->is_leaf) {
        int i = 0;
        while (i < current->num_keys && start_id > current->keys[i]) {
            i++;
        }
        current = current->children[i];
    }
    
    // Track already processed expense IDs to avoid duplicates
    int processedIDs[MAX_EXPENSES] = {0};
    int processedCount = 0;
    
    int found=0;
    // Now current points to the leaf node where we should start collecting
    while (current != NULL) {
        for (int i = 0; i < current->num_keys && found == 0; i++) {
            if (current->expenses[i].user_id == user_id && current->expenses[i].expense_id >= start_id && current->expenses[i].expense_id <= end_id) {
                
                // Check if we've already processed this expense ID
                bool isDuplicate = false;
                for (int j = 0; j < processedCount; j++) {
                    if (processedIDs[j] == current->expenses[i].expense_id) {
                        isDuplicate = true;
                        found=1;
                    }
                }
                
                if (!isDuplicate) {
                    expenses[*count] = &(current->expenses[i]);
                    (*count)++;
                    processedIDs[processedCount++] = current->expenses[i].expense_id;
                }
            }
        }
        
        // Move to next leaf if the highest key in current leaf is <= end_id
        if (current->num_keys > 0 && current->expenses[current->num_keys - 1].expense_id <= end_id) {
            current = current->next;
        } else {
            current = NULL;
        }
    }
}

// Function to get expenses within a range of expense IDs for a specific individual
void get_expense_in_range(UserNode* userRoot, ExpenseNode* expenseRoot, int expenseID_1, int expenseID_2, int individualID) {
    // Validate input parameters
    if (expenseID_1 > expenseID_2) 
    {
        int temp = expenseID_1;
        expenseID_1 = expenseID_2;
        expenseID_2 = temp;
    }
    
    if (expenseRoot == NULL) {
        printf("No expenses found in the database.\n");
        return;
    }
    
    // Check if user exists
    if (!searchUser(userRoot, individualID)) {
        printf("User with ID %d not found.\n", individualID);
        return;
    }
    
    // Create an array to store pointers to expenses within the ID range
    Expense* filteredExpenses[MAX_EXPENSES];
    int count = 0;
    
    // Collect all expenses within the expense ID range for the given individual
    collectExpensesInIDRange(expenseRoot, expenseID_1, expenseID_2, individualID, filteredExpenses, &count);
    
    if (count == 0) {
        printf("No expenses found for user ID %d between expense IDs %d and %d.\n", 
               individualID, expenseID_1, expenseID_2);
        return;
    }
    
    // Print header
    printf("\n=== Expenses for User ID %d between Expense IDs %d and %d ===\n", 
           individualID, expenseID_1, expenseID_2);
    printf("+------------+------------+--------------+------------+--------------+\n");
    printf("| Expense ID | User ID    | Category     | Amount     | Date         |\n");
    printf("+------------+------------+--------------+------------+--------------+\n");
    
    // Print all expenses in the ID range
    for (int i = 0; i < count; i++) {
        printExpenseDetails(*(filteredExpenses[i]));
    }
    
    printf("+------------+------------+--------------+------------+--------------+\n");
    
    // Calculate total amount and category breakdown
    float totalAmount = 0;
    float categoryAmount[MAX_CATEGORY + 1] = {0}; // +1 because categories start from 1
    
    for (int i = 0; i < count; i++) {
        totalAmount += filteredExpenses[i]->amount;
        categoryAmount[filteredExpenses[i]->category] += filteredExpenses[i]->amount;
    }
    
    printf("Total expenses in this range: %.2f\n", totalAmount);
    printf("Number of expense entries: %d\n\n", count);
    
    // Print category breakdown
    printf("=== Category Breakdown ===\n");
    for (int i = 1; i <= MAX_CATEGORY; i++) {
        if (categoryAmount[i] > 0) {
            printf("%s: %.2f (%.1f%%)\n", 
                   getCategoryName(i), 
                   categoryAmount[i], 
                   (categoryAmount[i] / totalAmount) * 100);
        }
    }
}

// Function to update or delete individual or family details
void Update_or_delete_individual_Family_details(UserNode** userRoot, FamilyTree* familyTree, ExpenseNode* expenseRoot, int id, int is_family, int operation) {
    // operation: 1 = update, 2 = delete
    // is_family: 1 = family, 0 = individual
    
    if (operation == 1) { // Update
        if (is_family) { // Update family
            // Search for the family
            Family* family = searchFamily(familyTree->root, id);
            
            if (family) {
                char new_name[MAX_NAME_LENGTH];
                printf("Enter new family name (or press enter to keep current): ");
                getchar(); // Clear input buffer
                fgets(new_name, MAX_NAME_LENGTH, stdin);
                
                // Remove trailing newline if exists
                if (new_name[0] != '\n') {
                    new_name[strcspn(new_name, "\n")] = 0;
                    strcpy(family->family_name, new_name);
                }
                
                // Recalculate total income
                family->total_income = 0;
                for (int i = 0; i < family->member_count; i++) {
                    family->total_income += family->members[i]->income;
                }
                
                // Recalculate total monthly expense
                family->total_monthly_expense = calculateTotalMonthlyExpense(expenseRoot, family);
                
                printf("Family updated successfully.\n");
            } else {
                printf("Family with ID %d not found.\n", id);
            }
        } else { // Update individual
            char new_name[MAX_NAME_LENGTH];
            float new_income;
            
            // Search for the user
            if (searchUser(*userRoot, id)) {
                printf("Enter new name (or press enter to keep current): ");
                getchar(); // Clear input buffer
                fgets(new_name, MAX_NAME_LENGTH, stdin);
                
                // Remove trailing newline if exists
                if (new_name[0] != '\n') {
                    new_name[strcspn(new_name, "\n")] = 0;
                } else {
                    // Keep current name
                    new_name[0] = '\0';
                }
                
                printf("Enter new income (or -1 to keep current): ");
                scanf("%f", &new_income);
                
                // Update user
                *userRoot = updateUser(*userRoot, id, new_name, new_income);
                
                // Find the family this user belongs to and update it
                FamilyNode* currentNode = familyTree->root;
                while (currentNode) {
                    for (int i = 0; i < currentNode->num_keys; i++) {
                        Family* family = currentNode->families[i];
                        for (int j = 0; j < family->member_count; j++) {
                            if (family->members[j]->user_id == id) {
                                // Update family's total income
                                family->total_income = 0;
                                for (int k = 0; k < family->member_count; k++) {
                                    family->total_income += family->members[k]->income;
                                }
                                
                                // Update family's total monthly expense
                                family->total_monthly_expense = calculateTotalMonthlyExpense(expenseRoot, family);
                                
                                printf("User and associated family updated successfully.\n");
                                return;
                            }
                        }
                    }
                    
                    // B-tree traversal to find the family
                    if (currentNode->is_leaf) {
                        break;
                    }
                    
                    // If we're still looking, we need to search children
                    // This is a simplified approach and might need refinement
                    currentNode = currentNode->children[0]; // Simplified approach
                }
                
                printf("User updated successfully, but not found in any family.\n");
            } else {
                printf("User with ID %d not found.\n", id);
            }
        }
    } else if (operation == 2) { // Delete
        if (is_family) { // Delete family
            // First, check if the family exists
            Family* family = searchFamily(familyTree->root, id);
            if (!family) {
                printf("Family with ID %d not found.\n", id);
                return;
            }
            
            // Delete the family from the B-tree
            if (deleteFamilyFromTree(familyTree, id)) {
                printf("Family with ID %d deleted successfully.\n", id);
            } else {
                printf("Failed to delete family with ID %d.\n", id);
            }
        } else { // Delete individual
            // First, check if the user exists
            if (!searchUser(*userRoot, id)) {
                printf("User with ID %d not found.\n", id);
                return;
            }
            
            // Find the family this user belongs to
            FamilyNode* currentNode = familyTree->root;
            Family* targetFamily = NULL;
            int j_index = -1;
            
            // Find the family and the index of this user in that family
            while (currentNode && !targetFamily) {
                for (int i = 0; i < currentNode->num_keys; i++) {
                    Family* family = currentNode->families[i];
                    for (int j = 0; j < family->member_count; j++) {
                        if (family->members[j]->user_id == id) {
                            targetFamily = family;
                            j_index = j;
                            break;
                        }
                    }
                    if (targetFamily) break;
                }
                
                if (targetFamily) break;
                
                // If we're still looking, we need to search children
                if (!currentNode->is_leaf) {
                    // Simple approach for traversal, can be improved
                    currentNode = currentNode->children[0];
                } else {
                    break;
                }
            }
            
            // Now handle the deletion based on whether the user is in a family
            if (targetFamily) {
                if (targetFamily->member_count == 1) {
                    // User is the only member of the family, delete the family too
                    printf("User is the only member of family %d. Deleting both user and family.\n", targetFamily->family_id);
                    
                    // Delete the user
                    *userRoot = deleteUserNode(*userRoot, id);
                    
                    // Delete the family
                    deleteFamilyFromTree(familyTree, targetFamily->family_id);
                } else {
                    // Remove user from the family
                    for (int k = j_index; k < targetFamily->member_count - 1; k++) {
                        targetFamily->members[k] = targetFamily->members[k + 1];
                    }
                    targetFamily->member_count--;
                    
                    // Update family's total income
                    targetFamily->total_income = 0;
                    for (int k = 0; k < targetFamily->member_count; k++) {
                        targetFamily->total_income += targetFamily->members[k]->income;
                    }
                    
                    // Update family's total monthly expense
                    targetFamily->total_monthly_expense = calculateTotalMonthlyExpense(expenseRoot, targetFamily);
                    
                    // Delete the user
                    *userRoot = deleteUserNode(*userRoot, id);
                    
                    printf("User deleted and family updated successfully.\n");
                }
            } else {
                // User is not in any family, just delete the user
                *userRoot = deleteUserNode(*userRoot, id);
                printf("User deleted successfully.\n");
            }
        }
    } else {
        printf("Invalid operation. Use 1 for update, 2 for delete.\n");
    }
}

// Helper function to delete a family from the B-Tree
int deleteFamilyFromTree(FamilyTree* familyTree, int family_id) {
    if (!familyTree || !familyTree->root) {
        return 0;
    }
    
    // Find the family node and index
    FamilyNode* node = NULL;
    int keyIndex = -1;
    
    // Implementation for finding node and keyIndex
    if (!findFamilyNode(familyTree->root, family_id, &node, &keyIndex)) {
        return 0;  // Family not found
    }
    
    // If the family is in a leaf node
    if (node->is_leaf) {
        // Remove the key and shift other keys
        for (int i = keyIndex; i < node->num_keys - 1; i++) {
            node->keys[i] = node->keys[i + 1];
            node->families[i] = node->families[i + 1];
        }
        node->num_keys--;
        
        // Balance the tree if needed
        if (node == familyTree->root && node->num_keys == 0) {
            free(node);
            familyTree->root = NULL;
        } else if (node->num_keys < MIN_KEYS) {
            balanceFamilyTree(familyTree, node);
        }
        
        familyTree->count--;
        return 1;
    }
    
    // If the family is in an internal node
    // This is a complex case requiring finding the predecessor or successor
    // For simplicity, we'll find the predecessor (rightmost key in left subtree)
    FamilyNode* predecessorNode = node->children[keyIndex];
    while (!predecessorNode->is_leaf) {
        predecessorNode = predecessorNode->children[predecessorNode->num_keys];
    }
    
    // Replace the key with predecessor's rightmost key
    int predecessorIndex = predecessorNode->num_keys - 1;
    node->keys[keyIndex] = predecessorNode->keys[predecessorIndex];
    node->families[keyIndex] = predecessorNode->families[predecessorIndex];
    
    // Remove the predecessor
    predecessorNode->num_keys--;
    
    // Balance the tree if needed
    if (predecessorNode->num_keys < MIN_KEYS) {
        balanceFamilyTree(familyTree, predecessorNode);
    }
    
    familyTree->count--;
    return 1;
}

int deleteFamily(FamilyTree* familyTree, int family_id, const char* filename) {
    // Step 1: Delete family from in-memory tree
    int result = deleteFamilyFromTree(familyTree, family_id);
    if (result == 0) {
        printf("Family with ID %d not found.\n", family_id);
        return 0; // Family not found
    }

    // Step 2: Save updated tree to file
    saveFamiliesToFile(familyTree, filename, "temp_file.txt");

    printf("Family with ID %d deleted successfully.\n", family_id);
    return 1; // Successful deletion
}


// Helper function to find a family node and its key index
int findFamilyNode(FamilyNode* root, int family_id, FamilyNode** nodeOut, int* indexOut) {
    if (!root) {
        return 0;
    }
    
    // Try to find the key in this node
    int i = 0;
    while (i < root->num_keys && family_id > root->keys[i]) {
        i++;
    }
    
    // If found, return this node and index
    if (i < root->num_keys && family_id == root->keys[i]) {
        *nodeOut = root;
        *indexOut = i;
        return 1;
    }
    
    // If this is a leaf, the key doesn't exist
    if (root->is_leaf) {
        return 0;
    }
    
    // Otherwise, recurse into the appropriate child
    return findFamilyNode(root->children[i], family_id, nodeOut, indexOut);
}

// Helper function to balance the B-Tree after deletion
void balanceFamilyTree(FamilyTree* tree, FamilyNode* node) {
    // This is a simplified approach - a complete implementation would require
    // identifying the parent node and handling various rebalancing cases
    
    // If node is root, no need to balance
    if (node == tree->root) {
        return;
    }
    
    // Find the parent node
    FamilyNode* parent = findParentNode(tree->root, node);
    if (!parent) {
        return;  // Should not happen
    }
    
    // Find the node's index in the parent's children array
    int nodeIndex = 0;
    while (nodeIndex <= parent->num_keys && parent->children[nodeIndex] != node) {
        nodeIndex++;
    }
    
    // If right sibling exists and has extra keys
    if (nodeIndex < parent->num_keys && parent->children[nodeIndex + 1]->num_keys > MIN_KEYS) {
        // Borrow from right sibling
        FamilyNode* rightSibling = parent->children[nodeIndex + 1];
        
        // Add the parent's key to the current node
        node->keys[node->num_keys] = parent->keys[nodeIndex];
        node->families[node->num_keys] = parent->families[nodeIndex];
        node->num_keys++;
        
        // Move the first key from right sibling to parent
        parent->keys[nodeIndex] = rightSibling->keys[0];
        parent->families[nodeIndex] = rightSibling->families[0];
        
        // Shift all keys in right sibling
        for (int i = 0; i < rightSibling->num_keys - 1; i++) {
            rightSibling->keys[i] = rightSibling->keys[i + 1];
            rightSibling->families[i] = rightSibling->families[i + 1];
        }
        
        // If not a leaf, move the child pointer too
        if (!node->is_leaf) {
            node->children[node->num_keys] = rightSibling->children[0];
            for (int i = 0; i < rightSibling->num_keys; i++) {
                rightSibling->children[i] = rightSibling->children[i + 1];
            }
        }
        
        rightSibling->num_keys--;
        return;
    }
    
    // If left sibling exists and has extra keys
    if (nodeIndex > 0 && parent->children[nodeIndex - 1]->num_keys > MIN_KEYS) {
        // Borrow from left sibling
        FamilyNode* leftSibling = parent->children[nodeIndex - 1];
        
        // Shift all keys in current node to make room
        for (int i = node->num_keys; i > 0; i--) {
            node->keys[i] = node->keys[i - 1];
            node->families[i] = node->families[i - 1];
        }
        
        // If not a leaf, shift child pointers too
        if (!node->is_leaf) {
            for (int i = node->num_keys + 1; i > 0; i--) {
                node->children[i] = node->children[i - 1];
            }
        }
        
        // Add the parent's key to the current node
        node->keys[0] = parent->keys[nodeIndex - 1];
        node->families[0] = parent->families[nodeIndex - 1];
        node->num_keys++;
        
        // Move the last key from left sibling to parent
        parent->keys[nodeIndex - 1] = leftSibling->keys[leftSibling->num_keys - 1];
        parent->families[nodeIndex - 1] = leftSibling->families[leftSibling->num_keys - 1];
        
        // If not a leaf, move the child pointer too
        if (!node->is_leaf) {
            node->children[0] = leftSibling->children[leftSibling->num_keys];
        }
        
        leftSibling->num_keys--;
        return;
    }
    
    // If both siblings have minimum keys, merge with one of them
    if (nodeIndex < parent->num_keys) {
        // Merge with right sibling
        mergeFamilyNodes(tree, parent, nodeIndex);
    } else {
        // Merge with left sibling
        mergeFamilyNodes(tree, parent, nodeIndex - 1);
    }
}

// Helper function to find parent node
FamilyNode* findParentNode(FamilyNode* root, FamilyNode* child) {
    if (!root || root == child) {
        return NULL;
    }
    
    // Check if any of the children matches
    for (int i = 0; i <= root->num_keys; i++) {
        if (root->children[i] == child) {
            return root;
        }
    }
    
    // If not found, recurse into non-leaf children
    if (!root->is_leaf) {
        for (int i = 0; i <= root->num_keys; i++) {
            FamilyNode* parent = findParentNode(root->children[i], child);
            if (parent) {
                return parent;
            }
        }
    }
    
    return NULL;
}

// Helper function to merge two family nodes
void mergeFamilyNodes(FamilyTree* tree, FamilyNode* parent, int index) {
    FamilyNode* child = parent->children[index];
    FamilyNode* sibling = parent->children[index + 1];
    
    // Add key from parent to child
    child->keys[child->num_keys] = parent->keys[index];
    child->families[child->num_keys] = parent->families[index];
    child->num_keys++;
    
    // Copy all keys from sibling to child
    for (int i = 0; i < sibling->num_keys; i++) {
        child->keys[child->num_keys] = sibling->keys[i];
        child->families[child->num_keys] = sibling->families[i];
        child->num_keys++;
    }
    
    // If not leaf nodes, copy child pointers too
    if (!child->is_leaf) {
        for (int i = 0; i <= sibling->num_keys; i++) {
            child->children[child->num_keys - sibling->num_keys + i] = sibling->children[i];
        }
    }
    
    // Remove key from parent and adjust pointers
    for (int i = index; i < parent->num_keys - 1; i++) {
        parent->keys[i] = parent->keys[i + 1];
        parent->families[i] = parent->families[i + 1];
        parent->children[i + 1] = parent->children[i + 2];
    }
    parent->num_keys--;
    
    // Free the sibling node
    free(sibling);
    
    // If parent becomes empty, make child the new root
    if (parent == tree->root && parent->num_keys == 0) {
        tree->root = child;
        free(parent);
    } else if (parent->num_keys < MIN_KEYS) {
        // Balance the parent if it has too few keys
        balanceFamilyTree(tree, parent);
    }
}


// Helper function to traverse the tree and write users to file
void writeUsersRecursive(UserNode* node, FILE* file) {
    if (node != NULL) {
        // Inorder traversal to maintain sorted order
        writeUsersRecursive(node->left, file);
        
        // Write the current user's data
        fprintf(file, "%d,%s,%.2f\n", node->user_id, node->user_name, node->income);
        
        writeUsersRecursive(node->right, file);
    }
}

// Save all users from the AVL tree to a file
void saveUsersToFile(UserNode* root, const char* filename) {
    if (root == NULL || filename == NULL) {
        return;
    }
    
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error opening file %s for writing\n", filename);
        return;
    }
    
    // Call the helper function
    writeUsersRecursive(root, file);
    
    fclose(file);
    printf("Users saved to %s successfully.\n", filename);
}

// Helper function to write families recursively to file



// Full B+ tree deletion implementation
void DeleteExpense(ExpenseNode** root, int expense_id) {
    if (!*root) return;
    ExpenseNode* node = *root;
    while (!node->is_leaf) {
        int pos = 0;
        while (pos < node->num_keys && expense_id > node->keys[pos]) pos++;
        node = node->children[pos];
    }
    int pos = -1;
    for (int i = 0; i < node->num_keys; i++) {
        if (node->keys[i] == expense_id) pos = i;
    }
    if (pos == -1) return;
    // Shift elements left
    for (int i = pos; i < node->num_keys-1; i++) {
        node->keys[i] = node->keys[i+1];
        node->expenses[i] = node->expenses[i+1];
    }
    node->num_keys--;
}


// Add this function definition to your code

// Helper to recalculate expenses for families containing a user
void UpdateFamilyExpenses(FamilyTree* tree, ExpenseNode* expenses, int user_id) {
    // Traverse all families (B-tree traversal)
    
    TraverseAndUpdateFamilies(tree->root, expenses, user_id);
}

void TraverseAndUpdateFamilies(FamilyNode* node, ExpenseNode* expenses, int user_id) {
    if (!node) return;
    
    for (int i = 0; i < node->num_keys; i++) {
        Family* fam = node->families[i];
        for (int j = 0; j < fam->member_count; j++) {
            if (fam->members[j]->user_id == user_id) {
                fam->total_monthly_expense = calculateTotalMonthlyExpense(expenses, fam);
                break;
            }
        }
    }
    
    // Recurse for non-leaf nodes
    if (!node->is_leaf) {
        for (int i = 0; i <= node->num_keys; i++) {
            TraverseAndUpdateFamilies(node->children[i], expenses, user_id);
        }
    }
}

ExpenseNode *InsertExpense(ExpenseNode *node,Expense newExpense,int *pNewKey,ExpenseNode **pNewChild, int *pDuplicate)
{
    if(node == NULL) return NULL;

    //Check for dupliacte expense_id
    if(SearchExpenseID(node,newExpense.expense_id))
    {
        *pDuplicate=1;
        return NULL;
    }

    if(node->is_leaf)
    {
        int pos=0;
        while(pos < node->num_keys && node->expenses[pos].expense_id < newExpense.expense_id)
        {
            pos++;
        }

        //Check for dupliacte at the correct position
        if(pos < node->num_keys && node->expenses[pos].expense_id < newExpense.expense_id)
        {
            *pDuplicate=1;
            return NULL;
        }

        if(node->num_keys < MAX_KEYS)
        {
            for(int i = node->num_keys;i>pos;i--)
            {
                node->expenses[i] = node->expenses[i-1];
                node->keys[i] = node->keys[i-1];
            }
            node->expenses[pos] = newExpense;
            node->keys[pos] = newExpense.expense_id;
            node->num_keys++;
            return NULL;
        }

        else
        {
            ExpenseNode *splitNode = createLeafNode();
            int mid= MAX_KEYS/2;
            for(int i = mid,j=0; i < MAX_KEYS;i++,j++)
            {
                splitNode->expenses[j]=node->expenses[i];
                splitNode->keys[j] = node->keys[i];
            }

            splitNode->num_keys = MAX_KEYS-mid;
            node->num_keys= mid;

            if(pos < mid)
            {
                for(int i = node->num_keys; i>pos;i--)
                {
                    node->expenses[i] = node->expenses[i-1];
                    node->keys[i] = node->keys[i-1];
                }

                node->expenses[pos] = newExpense;
                node->keys[pos] = newExpense.expense_id;
                node->num_keys++;
            }
            else
            {
                int newpos=pos - mid;
                for(int i = splitNode->num_keys;i>newpos;i--)
                {
                    splitNode->expenses[i] = splitNode->expenses[i-1];
                    splitNode->keys[i] = splitNode->keys[i-1];
                }
                splitNode->expenses[newpos] = newExpense;
                splitNode->keys[newpos] = newExpense.expense_id;
                splitNode->num_keys++;
            }

            splitNode->next = node->next;
            node->next = splitNode;
            splitNode->prev=node;
            if(splitNode->next) splitNode->next->prev = splitNode;

            *pNewKey = splitNode->keys[0];
            *pNewChild = splitNode;
            return splitNode;
        }
    }

    int pos = 0;
    while(pos < node->num_keys && node->keys[pos] < newExpense.expense_id)
    {
        pos++;
    }

    int tempNewKey;
    ExpenseNode *tempNewChild = NULL;
    ExpenseNode *splitNode = InsertExpense(node->children[pos],newExpense,&tempNewKey,&tempNewChild,pDuplicate);

    if(*pDuplicate)
    {
        return NULL;
    }

    if(splitNode)
    {
        if(node->num_keys < MAX_KEYS)
        {
            for(int i=node->num_keys; i>pos;i--)
            {
                node->keys[i]= node->keys[i-1];
                node->children[i+1] = node->children[i];
            }
            node->keys[pos] = tempNewKey;
            node->children[pos + 1] = tempNewChild;
            node->num_keys++;
            return NULL;
        }
        else
        {
            ExpenseNode *newNode =  createLeafNode();
            newNode->is_leaf = 0; //This is an internal node
            int mid = MAX_KEYS/2;

            for(int i = mid+1,j=0;i<MAX_KEYS;i++,j++)
            {
                newNode->keys[j] = node->keys[i];
                newNode->children[j] = node->children[i];
            }

            newNode->children[MAX_KEYS - mid -1] = node->children[MAX_KEYS];
            newNode->num_keys = MAX_KEYS - mid - 1;
            node->num_keys=mid;

            if(pos <= mid)
            {
                for (int i = node->num_keys; i > pos; i--) {
                    node->keys[i] = node->keys[i - 1];
                    node->children[i + 1] = node->children[i];
                }
                node->keys[pos] = tempNewKey;
                node->children[pos + 1] = tempNewChild;
                node->num_keys++;
            } 
            else 
            {
                int newPos = pos - mid - 1;
                for (int i = newNode->num_keys; i > newPos; i--) {
                    newNode->keys[i] = newNode->keys[i - 1];
                    newNode->children[i + 1] = newNode->children[i];
                }
                newNode->keys[newPos] = tempNewKey;
                newNode->children[newPos + 1] = tempNewChild;
                newNode->num_keys++;
            }

             *pNewKey = node->keys[mid];
             *pNewChild = newNode;
              return newNode;
            }
        }
        return NULL;
}

// Modified Update_delete_expense function



void PrintExpenseTree(ExpenseNode *node, int level) {
    if (node == NULL) return;
    
    printf("Level %d: ", level);
    if (node->is_leaf) {
        printf("Leaf [");
        for (int i = 0; i < node->num_keys; i++) {
            printf("%d", node->keys[i]);
            if (i < node->num_keys-1) printf(", ");
        }
        printf("]\n");
    } else {
        printf("Internal [");
        for (int i = 0; i < node->num_keys; i++) {
            printf("%d", node->keys[i]);
            if (i < node->num_keys-1) printf(", ");
        }
        printf("]\n");
        for (int i = 0; i <= node->num_keys; i++) {
            PrintExpenseTree(node->children[i], level+1);
        }
    }
}

// Helper function to create a new expense node (either leaf or internal)
ExpenseNode *createExpenseNode(int isLeaf) {
    ExpenseNode *newNode = (ExpenseNode *)malloc(sizeof(ExpenseNode));
    if (newNode == NULL) {
        printf("Memory allocation failed\n");
        exit(1);
    }
    
    // Initialize node properties
    newNode->num_keys = 0;
    newNode->is_leaf = isLeaf;
    
    // Initialize all keys and data
    for (int i = 0; i < MAX_KEYS; i++) {
        newNode->keys[i] = 0;
        if (isLeaf) {
            // Initialize expense fields for leaf nodes
            newNode->expenses[i].expense_id = 0;
            // Initialize other expense fields as needed
        }
    }
    
    // Initialize all children pointers
    for (int i = 0; i <= MAX_KEYS; i++) {
        newNode->children[i] = NULL;
    }
    
    // Initialize linked list pointers for leaf nodes
    newNode->prev = NULL;
    newNode->next = NULL;
    
    return newNode;
}

// Helper function to find the position of a key in a node
int findPosition(ExpenseNode *node, int key) {
    int pos = 0;
    while (pos < node->num_keys && key > node->keys[pos]) {
        pos++;
    }
    return pos;
}

// Helper function to search for an expense ID in a B+ tree
int SearchExpenseID(ExpenseNode *root, int expense_id) {
    if (!root) return 0;
    
    ExpenseNode *node = root;
    while (!node->is_leaf) {
        int pos = 0;
        // Corrected comparison for B+ tree traversal
        while (pos < node->num_keys && expense_id >= node->keys[pos]) pos++;
        node = node->children[pos];
    }
    
    ExpenseNode *current = root;
    
    // Traverse to the appropriate leaf node
    while (!current->is_leaf) {
        int pos = findPosition(current, expense_id);
        current = current->children[pos];
    }
    
    // Search in the leaf node
    for (int i = 0; i < current->num_keys; i++) {
        if (current->keys[i] == expense_id) {
            return 1; // Found
        }
    }
    
    return 0; // Not found
}

// Function to insert a new expense into a leaf node that has space
void insertIntoLeaf(ExpenseNode *leaf, Expense newExpense) {
    int pos = findPosition(leaf, newExpense.expense_id);
    
    // Shift keys and expenses to make room
    for (int i = leaf->num_keys; i > pos; i--) {
        leaf->keys[i] = leaf->keys[i-1];
        leaf->expenses[i] = leaf->expenses[i-1];
    }
    
    // Insert the new key and expense
    leaf->keys[pos] = newExpense.expense_id;
    leaf->expenses[pos] = newExpense;
    leaf->num_keys++;
}

// Function to split a leaf node and insert the new key
ExpenseNode *splitLeafNode(ExpenseNode *leaf, Expense newExpense, int *pNewKey) {
    // Create a new leaf node
    ExpenseNode *newLeaf = createExpenseNode(1);
    
    // Create temporary arrays to hold all keys and expenses including the new one
    int tempKeys[MAX_KEYS + 1];
    Expense tempExpenses[MAX_KEYS + 1];
    
    // Copy existing keys and expenses
    int i, j, pos;
    for (i = 0, pos = 0; i < leaf->num_keys; i++) {
        if (pos == 0 && newExpense.expense_id < leaf->keys[i]) {
            tempKeys[pos] = newExpense.expense_id;
            tempExpenses[pos] = newExpense;
            pos++;
        }
        tempKeys[pos] = leaf->keys[i];
        tempExpenses[pos] = leaf->expenses[i];
        pos++;
    }
    
    // If new expense is the largest, add it at the end
    if (pos == leaf->num_keys) {
        tempKeys[pos] = newExpense.expense_id;
        tempExpenses[pos] = newExpense;
    }
    
    // Calculate split point - middle for even distribution
    int splitPoint = (MAX_KEYS + 1) / 2;
    
    // Reset leaf node and copy first half
    leaf->num_keys = 0;
    for (i = 0; i < splitPoint; i++) {
        leaf->keys[i] = tempKeys[i];
        leaf->expenses[i] = tempExpenses[i];
        leaf->num_keys++;
    }
    
    // Copy second half to new leaf
    for (i = splitPoint, j = 0; i <= MAX_KEYS; i++, j++) {
        newLeaf->keys[j] = tempKeys[i];
        newLeaf->expenses[j] = tempExpenses[i];
        newLeaf->num_keys++;
    }
    
    // Update the leaf node linked list
    newLeaf->next = leaf->next;
    if (leaf->next != NULL) {
        leaf->next->prev = newLeaf;
    }
    leaf->next = newLeaf;
    newLeaf->prev = leaf;
    
    // The first key of the new leaf is the key to be inserted in the parent
    *pNewKey = newLeaf->keys[0];
    
    return newLeaf;
}

// Function to insert into an internal node that has space
void insertIntoInternal(ExpenseNode *node, int key, ExpenseNode *rightChild, int pos) {
    // Shift keys and children pointers to make room
    for (int i = node->num_keys; i > pos; i--) {
        node->keys[i] = node->keys[i-1];
        node->children[i+1] = node->children[i];
    }
    
    // Insert the new key and child pointer
    node->keys[pos] = key;
    node->children[pos+1] = rightChild;
    node->num_keys++;
}

// Function to split an internal node
ExpenseNode *splitInternalNode(ExpenseNode *node, int key, ExpenseNode *rightChild, int pos, int *pNewKey) {
    // Temporary arrays for keys and children
    int tempKeys[MAX_KEYS + 1];
    ExpenseNode *tempChildren[MAX_KEYS + 2];
    
    // Copy existing keys and children, and insert the new ones
    int i, j;
    
    // Copy keys before insertion position
    for (i = 0; i < pos; i++) {
        tempKeys[i] = node->keys[i];
        tempChildren[i] = node->children[i];
    }
    
    // Insert the new key and child
    tempKeys[pos] = key;
    tempChildren[pos] = node->children[pos];
    tempChildren[pos+1] = rightChild;
    
    // Copy keys after insertion position
    for (i = pos, j = pos+1; i < node->num_keys; i++, j++) {
        tempKeys[j] = node->keys[i];
        tempChildren[j+1] = node->children[i+1];
    }
    
    // Create a new internal node
    ExpenseNode *newNode = createExpenseNode(0);
    
    // Find middle key for B+ tree internal node
    int mid = MAX_KEYS / 2;
    
    // Key to be moved up (but also stays in the right node for B+ tree)
    *pNewKey = tempKeys[mid];
    
    // Reset current node and copy first half (excluding the middle key for B+ trees)
    node->num_keys = 0;
    for (i = 0; i < mid; i++) {
        node->keys[i] = tempKeys[i];
        node->children[i] = tempChildren[i];
        node->num_keys++;
    }
    node->children[mid] = tempChildren[mid];
    
    // Copy second half to new node (including a copy of the middle key for B+ trees)
    for (i = mid, j = 0; i <= MAX_KEYS; i++, j++) {
        if (i == mid) {
            // Skip the middle key in the new node for non-leaf nodes
            newNode->children[j] = tempChildren[i+1];
            continue;
        }
        newNode->keys[j] = tempKeys[i];
        newNode->children[j+1] = tempChildren[i+1];
        newNode->num_keys++;
    }
    
    return newNode;
}

// Main recursive insertion function


// Public interface for insertion
ExpenseNode *InsertExpenseRoot(ExpenseNode *root, Expense newExpense, int *pDuplicate) {
    // Handle empty tree case
    if (root == NULL) {
        *pDuplicate = 0;
        root = createExpenseNode(1);  // Create a leaf node
        root->keys[0] = newExpense.expense_id;
        root->expenses[0] = newExpense;
        root->num_keys = 1;
        return root;
    }
    
    int newKey;
    ExpenseNode *newChild = NULL;
    
    // Call the recursive insert function
    ExpenseNode *result = InsertExpense(root, newExpense, &newKey, &newChild, pDuplicate);
    
    // If duplicate found, just return the existing root
    if (*pDuplicate) {
        return root;
    }
    
    // If the root was split, create a new root
    if (result != NULL) {
        ExpenseNode *newRoot = createExpenseNode(0);  // Internal node
        newRoot->keys[0] = newKey;
        newRoot->children[0] = root;
        newRoot->children[1] = newChild;
        newRoot->num_keys = 1;
        return newRoot;
    }
    
    // No root split occurred
    return root;
}

ExpenseNode *createLeafNode() 
{
    ExpenseNode *newNode = (ExpenseNode *)malloc(sizeof(ExpenseNode));
    if (!newNode) return NULL;
    
    newNode->num_keys = 0;
    newNode->is_leaf = 1;
    newNode->next = newNode->prev = NULL;
    
    // Initialize all keys and children
    for (int i = 0; i < MAX_KEYS; i++) {
        newNode->keys[i] = 0;
        newNode->children[i] = NULL;
    }
    newNode->children[MAX_KEYS] = NULL;
    
    return newNode;
}
int ValidateExpenseTree(ExpenseNode *node, int is_root, int *min_key, int *max_key) {
    if (!node) return 1;
    
    // Check key order in this node
    for (int i = 1; i < node->num_keys; i++) {
        if (node->keys[i] <= node->keys[i-1]) {
            printf("Key order violation: keys[%d]=%d <= keys[%d]=%d\n", 
                  i, node->keys[i], i-1, node->keys[i-1]);
            return 0;
        }
    }
    
    if (node->is_leaf) {
        *min_key = node->keys[0];
        *max_key = node->keys[node->num_keys-1];
        return 1;
    }
    
    // Validate children
    int child_min, child_max;
    for (int i = 0; i <= node->num_keys; i++) {
        if (!ValidateExpenseTree(node->children[i], 0, &child_min, &child_max))
            return 0;
            
        if (i > 0 && child_min <= node->keys[i-1]) {
            printf("Left child max violation: child_min=%d, parent_key=%d\n",
                  child_min, node->keys[i-1]);
            return 0;
        }
        if (i < node->num_keys && child_max > node->keys[i]) {
            printf("Right child min violation: child_max=%d, parent_key=%d\n",
                  child_max, node->keys[i]);
            return 0;
        }
    }
    
    *min_key = node->children[0]->keys[0];
    *max_key = node->children[node->num_keys]->keys[node->children[node->num_keys]->num_keys-1];
    return 1;
}



void Update_delete_expense(ExpenseNode** expenseRoot, FamilyTree* familyTree, UserNode* userRoot, 
    const char* expensesFile, const char* familiesFile) 
{
    int expense_id;
    printf("Enter Expense ID to update/delete: ");
    scanf("%d", &expense_id);

    // Search for the expense in the B+ tree
    printf("Searching for expense ID: %d\n", expense_id);
    Expense* foundExpense = FindExpenseByID(*expenseRoot, expense_id);

    if (!foundExpense) 
    {
        printf("Error: Expense ID %d not found.\n", expense_id);
        return;
    }

    // Store original values for later comparison
    int original_user_id = foundExpense->user_id;
    Expense originalExpense = *foundExpense;  // Make a copy

    printf("Found Expense:\n");
    printf("ID: %d, User ID: %d, Category: %s, Amount: %.2f, Date: %s\n", 
    foundExpense->expense_id, foundExpense->user_id, 
    getCategoryName(foundExpense->category), 
    foundExpense->amount, foundExpense->date);

    printf("1. Update Expense\n2. Delete Expense\nChoice: ");
    int choice;
    scanf("%d", &choice);

    if (choice == 1) 
    {
        // Update logic
        printf("Current Amount: %.2f\n", foundExpense->amount);
        printf("Enter New Amount (-1 to keep current): ");
        float new_amount;
        scanf("%f", &new_amount);

        if (new_amount != -1) 
        {
            foundExpense->amount = new_amount;  // Update the amount
            printf("Amount updated successfully.\n");
        }

        // Optionally update other fields like category or date
        printf("Current Category: %s\n", getCategoryName(foundExpense->category));
        printf("Enter New Category (1-5, -1 to keep current): ");
        int new_category;
        scanf("%d", &new_category);
        if (new_category != -1 && new_category >= 1 && new_category <= 5) {
        foundExpense->category = (ExpenseCategory)new_category;
        printf("Category updated successfully.\n");
        }

        printf("Current Date: %s\n", foundExpense->date);
        printf("Enter New Date (YYYY-MM-DD, '0' to keep current): ");
        char new_date[DATE_LENGTH];
        scanf("%s", new_date);
        if (strcmp(new_date, "0") != 0) 
        {
            strcpy(foundExpense->date, new_date);
            printf("Date updated successfully.\n");
        }

        // Write all expenses back to the file to reflect the changes
        writeExpensesToFile(*expenseRoot, expensesFile);
        printf("Expense updated in memory and file.\n");

    } 
    else if (choice == 2) 
    {
        // Delete the expense from the B+ tree
        DeleteExpense(expenseRoot, expense_id);

        // Write the updated expenses to the file
        writeExpensesToFile(*expenseRoot, expensesFile);
        printf("Expense deleted successfully.\n");
    } 
    else 
    {
        printf("Invalid choice!\n");
        return;
    }

    // Update family expenses for the user associated with this expense
    UpdateFamilyExpenses(familyTree, *expenseRoot, original_user_id);

    // Save updated family information
    saveFamiliesToFile(familyTree, familiesFile, "temp.txt");

    printf("Operation completed successfully!\n");
}

Expense* FindExpenseByID(ExpenseNode* root, int expense_id) 
{
    if (root == NULL) {
        return NULL;
    }
    
    ExpenseNode* current = root;
    
    // Traverse down to the leaf level
    while (!current->is_leaf) {
        int i = 0;
        
        // Debug output
        printf("Level %d (Internal): Keys [", current->num_keys);
        for (int j = 0; j < current->num_keys; j++) {
            printf("%d", current->keys[j]);
            if (j < current->num_keys - 1) printf(", ");
        }
        printf("]\n");
        
        // Find the appropriate child to traverse
        while (i < current->num_keys && expense_id > current->keys[i]) {
            i++;
        }
        
        printf("Taking child %d\n", i);
        current = current->children[i];
    }
    
    // At leaf level, search for the expense
    printf("Level %d (Leaf): Keys [", current->num_keys);
    for (int j = 0; j < current->num_keys; j++) {
        printf("%d", current->keys[j]);
        if (j < current->num_keys - 1) printf(", ");
    }
    printf("]\n");
    
    // Linear search in the leaf node
    for (int i = 0; i < current->num_keys; i++) {
        printf("Checking position %d: %d\n", i, current->keys[i]);
        if (current->keys[i] == expense_id) {
            printf("Found expense ID %d at position %d\n", expense_id, i);
            return &(current->expenses[i]);
        }
    }
    
    // If the expense is not found in this leaf, check if there are more leaves
    while (current->next != NULL) {
        current = current->next;
        printf("Checking next leaf node: Keys [");
        for (int j = 0; j < current->num_keys; j++) {
            printf("%d", current->keys[j]);
            if (j < current->num_keys - 1) printf(", ");
        }
        printf("]\n");
        
        for (int i = 0; i < current->num_keys; i++) {
            printf("Checking position %d: %d\n", i, current->keys[i]);
            if (current->keys[i] == expense_id) {
                printf("Found expense ID %d at position %d in next leaf\n", expense_id, i);
                return &(current->expenses[i]);
            }
        }
    }
    
    printf("Expense ID %d not found in any leaf node\n", expense_id);
    return NULL;
}

void saveFamiliesToFile(FamilyTree* tree, const char* filename, const char* tempFilename) {
    FILE* tempFile = fopen(tempFilename, "w");
    if (!tempFile) {
        printf("Error creating temporary file\n");
        return;
    }

    if (tree->root) {
        writeFamiliesRecursiveToFile(tree->root, tempFile);
    }
    fclose(tempFile);

    // Atomic file replacement
    remove(filename);
    rename(tempFilename, filename);
}


UserNode* deleteUserNode(UserNode* root, int user_id) 
{
    if (!root) return root;

    if (user_id < root->user_id) {
        root->left = deleteUserNode(root->left, user_id);
    } else if (user_id > root->user_id) {
        root->right = deleteUserNode(root->right, user_id);
    } else {
        UserNode* temp = NULL;

        // Node with one child or no child
        if (!root->left || !root->right) {
            temp = root->left ? root->left : root->right;

            if (!temp) { // No child
                temp = root;
                root = NULL;
            } else { // One child: copy data, not struct
                root->user_id = temp->user_id;
                strcpy(root->user_name, temp->user_name);
                root->income = temp->income;
                root->left = temp->left;
                root->right = temp->right;
            }
            free(temp);
        } else { // Two children
            UserNode* successor = root->right;
            while (successor->left) successor = successor->left;
            
            // Copy data without struct assignment
            root->user_id = successor->user_id;
            strcpy(root->user_name, successor->user_name);
            root->income = successor->income;
            
            root->right = deleteUserNode(root->right, successor->user_id);
        }
    }

    if (!root) return root;

    // Update height and balance factor
    root->height = 1 + Max(getHeight(root->left), getHeight(root->right));
    int balance = getBalanceFactor(root);

    // Left Left
    if (balance > 1 && getBalanceFactor(root->left) >= 0)
        return rightRotate(root);
    // Left Right
    if (balance > 1 && getBalanceFactor(root->left) < 0) {
        root->left = leftRotate(root->left);
        return rightRotate(root);
    }
    // Right Right
    if (balance < -1 && getBalanceFactor(root->right) <= 0)
        return leftRotate(root);
    // Right Left
    if (balance < -1 && getBalanceFactor(root->right) > 0) {
        root->right = rightRotate(root->right);
        return leftRotate(root);
    }
    return root;
}





int userExistsInFile(const char* filename, int user_id) {
    FILE* file = fopen(filename, "r");
    if (!file) return 0;

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        int current_id;
        if (sscanf(line, "%d,", &current_id) == 1 && current_id == user_id) {
            fclose(file);
            return 1;
        }
    }
    fclose(file);
    return 0;
}





void removeUserFromFamilies(FamilyTree* tree, int user_id) {
    if (!tree || !tree->root) return;

    FamilyNode* node = tree->root;
    int familiesToDelete[MAX_FAMILIES] = {0};
    int deleteCount = 0;

    // Use proper B-tree traversal (this is simplified)
    // In a real implementation, you'd need to traverse the entire tree
    traverseAndUpdateFamilies(tree->root, user_id, familiesToDelete, &deleteCount);

    // Delete empty families
    for (int i = 0; i < deleteCount; i++) {
        deleteFamilyFromTree(tree, familiesToDelete[i]);
    }
}

// New helper function for proper tree traversal
void traverseAndUpdateFamilies(FamilyNode* node, int user_id, int* familiesToDelete, int* deleteCount) {
    if (!node) return;
    
    // Process current node
    for (int i = 0; i < node->num_keys; i++) {
        Family* family = node->families[i];
        float incomeReduction = 0.0;
        int memberRemoved = 0;
        
        for (int j = 0; j < family->member_count; j++) {
            if (family->members[j] && family->members[j]->user_id == user_id) {
                // Store income before removing
                incomeReduction = family->members[j]->income;
                
                // Shift members left
                for (int k = j; k < family->member_count-1; k++) {
                    family->members[k] = family->members[k+1];
                }
                
                // Set last member to NULL and reduce count
                family->members[family->member_count-1] = NULL;
                family->member_count--;
                
                // Adjust income
                family->total_income -= incomeReduction;
                if (family->total_income < 0) family->total_income = 0;
                
                memberRemoved = 1;
                break;  // Each user should appear only once in a family
            }
        }
        
        // Mark empty families for deletion
        if (memberRemoved && family->member_count == 0) {
            familiesToDelete[(*deleteCount)++] = family->family_id;
        }
    }
    
    // If not leaf, traverse children
    if (!node->is_leaf) {
        for (int i = 0; i <= node->num_keys; i++) {
            traverseAndUpdateFamilies(node->children[i], user_id, familiesToDelete, deleteCount);
        }
    }
}

void writeFamiliesRecursiveToFile(FamilyNode* node, FILE* file) {
    if (node == NULL) return;

    int i;
    
    // If not leaf, first traverse to leftmost leaf
    if (!node->is_leaf) {
        writeFamiliesRecursiveToFile(node->children[0], file);
    }
    
    // Process all keys in current node
    for (i = 0; i < node->num_keys; i++) {
        Family* family = node->families[i];
        
        // Skip families with zero members (just in case any weren't properly deleted)
        if (family->member_count <= 0) continue;
        
        // Write family basic info
        fprintf(file, "%d,%s,%d,%.2f,%.2f,",
              family->family_id, 
              family->family_name,
              family->member_count,
              family->total_income,
              family->total_monthly_expense);
        
        // Write member IDs (only for valid members)
        for (int j = 0; j < family->member_count; j++) {
            if (family->members[j] != NULL) {
                fprintf(file, "%d", family->members[j]->user_id);
            } else {
                fprintf(file, "0");  // Backup for any NULL pointers
            }
            
            if (j < family->member_count - 1) {
                fprintf(file, ",");
            }
        }
        fprintf(file, "\n");
        
        // If not leaf, traverse child between keys
        if (!node->is_leaf) {
            writeFamiliesRecursiveToFile(node->children[i+1], file);
        }
    }
}


int main() {
    // Initialize data structures
    UserNode* userRoot = NULL;
    ExpenseNode* expenseRoot = NULL;
    FamilyTree* familyTree = createFamilyTree();

    // File names
    const char* usersFile = "individuals.txt";
    const char* expensesFile = "expenses.txt";
    const char* familiesFile = "families.txt";
    const char* tempFile = "temp.txt";

    // Load initial data
    printf("Loading data...\n");
    userRoot = loadUsersFromFile(usersFile, userRoot);
    readExpensesFromFile(&expenseRoot, expensesFile);
    familyTree = loadFamiliesFromFile(familiesFile, userRoot);
    printf("Data loaded successfully.\n\n");

    int choice;
    while (1) {
        printf("\n===== Expense Tracking System =====\n");
        printf("1. Add New User\n");
        printf("2. Add New Expense\n");
        printf("3. Create New Family\n");
        printf("4. Display All Users\n");
        printf("5. Display All Expenses\n");
        printf("6. Display All Families\n");
        printf("7. Get Total Family Expense\n");
        printf("8. Get Categorical Expense\n");
        printf("9. Get Highest Expense Day\n");
        printf("10. Get Individual Expense Report\n");
        printf("11. Get Expenses in Date Range\n");
        printf("12. Get Expenses in ID Range\n");
        printf("13. Update/Delete Records\n");
        printf("14. Exit\n");
        printf("Enter your choice (1-14): ");
        
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input! Please enter a number.\n");
            while (getchar() != '\n'); // Clear input buffer
            continue;
        }

        switch (choice) {
            case 1: // Add New User
                userRoot = AddUser(userRoot, usersFile);
                saveUsersToFile(userRoot, usersFile);
                break;

            case 2: // Add New Expense
                AddExpense(&expenseRoot, expensesFile);
                writeExpensesToFile(expenseRoot, expensesFile);
                break;

            case 3: // Create New Family
                createFamily(familyTree, userRoot, expenseRoot, familiesFile);
                saveFamiliesToFile(familyTree, familiesFile, tempFile);
                break;

            case 4: // Display All Users
                printUserTable(userRoot);
                break;

            case 5: // Display All Expenses
                printExpensesTable(expenseRoot);
                break;

            case 6: // Display All Families
                printFamiliesTable(familyTree);
                break;

            case 7: { // Get Total Family Expense
                int family_id, month, year;
                printFamiliesTable(familyTree);
                printf("Enter Family ID: ");
                scanf("%d", &family_id);
                printf("Enter Month (1-12): ");
                scanf("%d", &month);
                printf("Enter Year: ");
                scanf("%d", &year);
                get_total_expense(familyTree, expenseRoot, family_id, month, year);
                break;
            }

            case 8: { // Get Categorical Expense
                int family_id, category;
                printFamiliesTable(familyTree);
                printf("Enter Family ID: ");
                scanf("%d", &family_id);
                printf("Enter Category (1-5):\n");
                printf("1. Rent\n2. Utility\n3. Grocery\n4. Stationary\n5. Leisure\n");
                scanf("%d", &category);
                get_categorical_expense(familyTree, expenseRoot, family_id, (ExpenseCategory)category);
                break;
            }

            case 9: { // Get Highest Expense Day
                int family_id;
                printFamiliesTable(familyTree);
                printf("Enter Family ID: ");
                scanf("%d", &family_id);
                get_highest_expense_day(familyTree, expenseRoot, family_id);
                break;
            }

            case 10: { // Get Individual Expense Report
                int user_id, month, year;
                printUserTable(userRoot);
                printf("Enter User ID: ");
                scanf("%d", &user_id);
                printf("Enter Month (1-12): ");
                scanf("%d", &month);
                printf("Enter Year: ");
                scanf("%d", &year);
                get_individual_expense(userRoot, expenseRoot, user_id, month, year);
                break;
            }

            case 11: { // Get Expenses in Date Range
                char start_date[11], end_date[11];
                printf("Enter Start Date (YYYY-MM-DD): ");
                scanf("%s", start_date);
                printf("Enter End Date (YYYY-MM-DD): ");
                scanf("%s", end_date);
                get_expense_in_period(expenseRoot, start_date, end_date);
                break;
            }

            case 12: { // Get Expenses in ID Range
                int start_id, end_id, user_id;
                printf("Enter Start Expense ID: ");
                scanf("%d", &start_id);
                printf("Enter End Expense ID: ");
                scanf("%d", &end_id);
                printUserTable(userRoot);
                printf("Enter User ID: ");
                scanf("%d", &user_id);
                get_expense_in_range(userRoot, expenseRoot, start_id, end_id, user_id);
                break;
            }

            case 13: { // Update/Delete Records
                int sub_choice;
                printf("\n1. Update User\n");
                printf("2. Delete User\n");
                printf("3. Update Family\n");
                printf("4. Delete Family\n");
                printf("5. Update Expense\n");
                printf("6. Delete Expense\n");
                printf("Enter your choice: ");
                scanf("%d", &sub_choice);

                switch (sub_choice) {
                    case 1: { // Update User
                        int user_id;
                        char new_name[MAX_NAME_LENGTH];
                        float new_income;
                        
                        printUserTable(userRoot);
                        printf("Enter User ID to update: ");
                        scanf("%d", &user_id);
                        printf("Enter new name: ");
                        scanf(" %49[^\n]", new_name);
                        printf("Enter new income: ");
                        scanf("%f", &new_income);
                        
                        userRoot = updateUser(userRoot, user_id, new_name, new_income);
                        saveUsersToFile(userRoot, usersFile);
                        break;
                    }
                    case 2: 
                    { // Delete User
                        int user_id;
                        printUserTable(userRoot);
                        printf("Enter User ID to delete: ");
                        scanf("%d", &user_id);

                        // Validate existence
                        if (!userExistsInFile(usersFile, user_id)) {
                            printf("Error: User ID %d not found\n", user_id);
                            break;
                        }

                        // Remove from families
                        removeUserFromFamilies(familyTree, user_id);
                        
                        // Delete from AVL tree
                        userRoot = deleteUserNode(userRoot, user_id);
                        
                        // Update files
                        saveUsersToFile(userRoot, usersFile);
                        saveFamiliesToFile(familyTree, familiesFile, "temp.txt");
                        
                        printf("User deleted successfully\n");
                        break;
                    }

                    case 3: { // Update Family
                        int family_id;
                        char new_name[MAX_NAME_LENGTH];
                        
                        printFamiliesTable(familyTree);
                        printf("Enter Family ID to update: ");
                        scanf("%d", &family_id);
                        printf("Enter new family name: ");
                        scanf(" %49[^\n]", new_name);
                        
                        Family* family = searchFamily(familyTree->root, family_id);
                        if (family) {
                            strcpy(family->family_name, new_name);
                            saveFamiliesToFile(familyTree, familiesFile, tempFile);
                        } else {
                            printf("Family not found!\n");
                        }
                        break;
                    }
                    case 4: {  int family_id;
                        printf("Enter Family ID to delete: ");
                        scanf("%d", &family_id);
        
                        // Call the deleteFamily function
                        int result = deleteFamily(familyTree, family_id, familiesFile);
                        if (result) {
                            printf("Family with ID %d deleted successfully.\n", family_id);
                        } else {
                            printf("Family with ID %d not found.\n", family_id);
                        }
                        break;
                    }
                    case 5: { // Update Expense
                        int expense_id;
                        printExpensesTable(expenseRoot);
                        printf("Enter Expense ID to update: ");
                        scanf("%d", &expense_id);
                        
                        Expense* exp = FindExpenseByID(expenseRoot, expense_id);
                        if (exp) {
                            printf("Enter new amount (-1 to keep current): ");
                            float new_amount;
                            scanf("%f", &new_amount);
                            if (new_amount != -1) exp->amount = new_amount;
                            
                            printf("Enter new category (1-5, -1 to keep): ");
                            int new_cat;
                            scanf("%d", &new_cat);
                            if (new_cat != -1) exp->category = (ExpenseCategory)new_cat;
                            
                            writeExpensesToFile(expenseRoot, expensesFile);
                            printf("Expense updated successfully.\n");
                        } else {
                            printf("Expense not found!\n");
                        }
                        break;
                    }
                    case 6: { // Delete Expense
                        int expense_id;
                        printExpensesTable(expenseRoot);
                        printf("Enter Expense ID to delete: ");
                        scanf("%d", &expense_id);
                        
                        Expense* exp = FindExpenseByID(expenseRoot, expense_id);
                        if (exp) {
                            int user_id = exp->user_id;
                            DeleteExpense(&expenseRoot, expense_id);
                            writeExpensesToFile(expenseRoot, expensesFile);
                            
                            // Update family expenses
                            UpdateFamilyExpenses(familyTree, expenseRoot, user_id);
                            saveFamiliesToFile(familyTree, familiesFile, tempFile);
                            printf("Expense deleted successfully.\n");
                        } else {
                            printf("Expense not found!\n");
                        }
                        break;
                    }
                    default:
                        printf("Invalid choice!\n");
                }
                break;
            }

            case 14: // Exit
                printf("\nSaving data...\n");
                saveUsersToFile(userRoot, usersFile);
                writeExpensesToFile(expenseRoot, expensesFile);
                saveFamiliesToFile(familyTree, familiesFile, tempFile);
                printf("Goodbye!\n");
                exit(0);

            default:
                printf("Invalid choice! Please try again.\n");
        }
    }

    return 0;
}

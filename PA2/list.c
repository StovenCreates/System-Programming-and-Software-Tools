#include <stdio.h>
#include <stdlib.h>

#define MAX_INTS 1000

/* A node in the linked list */
struct node {
    int data;
    struct node *next;
};

void free_list(struct node *head) {
    struct node *temp;
    while (head != NULL) {
        temp = head;
        head = head->next;
        free(temp);
    }
}

void read_file_into_array(FILE *file, int **array, int *len)
{
    *array = (int *)malloc(MAX_INTS * sizeof(int));
    if (*array == NULL) {
        printf("Error: Memory allocation failed.\n");
        exit(1);
    }

    *len = 0;
    while (fscanf(file, "%d", &(*array)[*len]) != EOF) {
        (*len)++;
        if (*len >= MAX_INTS)
            break;
    }
}

struct node *create_list(int intarray[], int len);

struct node *add_item_at_start(struct node *head, int data);

int search_array(int integers[], int numints, int element);

int search_list(struct node *head, int element);

struct node *create_sorted_list(struct node *head);

struct node *add_item_sorted(struct node *head, int data);

int copy_list_to_array(struct node *head, int *array);

void print_list(struct node *head);

void print_array(int integers[], int len);

int main(int argc, char *argv[])
{
    if (argc != 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }
    /* Open a file for reading */
    FILE *file = fopen(argv[1], "r");
    if (file == NULL) {
        printf("Error opening file!\n");
        return 1;
    }
    /* Read the numbers from the file, into an array */
    int *intarray;
    int numints;
    read_file_into_array(file, &intarray, &numints);
    fclose(file);
    /* Print the array */
    print_array(intarray, numints);
    /* Create a linked list with the integers from the array */
    struct node *head = create_list(intarray, numints);
    /* Print the linked list */
    print_list(head);
    /* Repeatedly prompt the user for a number to be searched.
     *  Search the array for the number and print out the result as shown in the
     * specs. Search the linked list for the number and print out the result as
     * shown in the specs. Stop prompting when the user enters 'q' (just the
     * character q without the single quotes).
     */
    char input[10];
    while (1) {
        printf("Enter a number to search (or 'q' to quit): ");
        scanf("%s", input);

        if (input[0] == 'q') {
            break;
        }

        int element = atoi(input);

        int index = search_array(intarray, numints, element);
        if (index != -1) {
            printf("Found %d at index %d in array.\n", element, index);
        } else {
            printf("%d not found in array.\n", element);
        }

        int position = search_list(head, element);
        if (position != -1) {
            printf("Found %d at position %d in linked list.\n", element,
                   position);
        } else {
            printf("%d not found in linked list.\n", element);
        }
    }
    /* Create a sorted list(in ascending order) from the unsorted list */
    struct node *sorted_head = create_sorted_list(head);
    /* Print the sorted list */
    print_list(sorted_head);
    /* Copy the sorted list to an array with the same sorted order */
    int sorted_array[MAX_INTS];
    int sorted_len = copy_list_to_array(sorted_head, sorted_array);
    /* Print out the sorted array */
    print_array(sorted_array, sorted_len);
    /* Print the original linked list again */
    print_list(head);
    /* Print the original array again */
    print_array(intarray, numints);

    /* Open a file for writing */
    FILE *out_file = fopen("sorted_numbers.txt", "w");
    if (out_file == NULL) {
        printf("Error opening file for writing!\n");
        return 1;
    }
    /* Write the sorted array to a file named "sorted_numbers.txt" */
    for (int i = 0; i < sorted_len; i++) {
        fprintf(out_file, "%d\n", sorted_array[i]);
    }
    fclose(out_file);
    /* Print out the number of integers written to the file */
    printf("Wrote %d numbers to sorted_numbers.txt\n", sorted_len);

    free_list(sorted_head);

    free_list(head);

    free(intarray);

    return 0;
}

struct node *create_list(int intarray[], int len)
{
    struct node *head = NULL, *tail = NULL;

    for (int i = 0; i < len; i++) {
        struct node* new_node = (struct node*)malloc(sizeof(struct node));
        if (new_node == NULL) {
            printf("Memory allocation failed.\n");
            exit(1);
        }
        new_node->data = intarray[i];
        new_node->next = NULL;

        if (head == NULL) {
            head = new_node;
            tail = new_node;
        }
        else {
            tail->next = new_node;
            tail = new_node;
        }
    }
    return head;
}

struct node *add_item_at_start(struct node *head, int data)
{
    struct node *new_node = (struct node *)malloc(sizeof(struct node));
    new_node->data = data;
    new_node->next = head;
    return new_node;
}

int search_list(struct node *head, int element)
{
    struct node *current = head;
    int position = 1;

    while (current != NULL) {
        if (current->data == element) {
            return position;
        }
        current = current->next;
        position++;
    }
    return -1;
}

int search_array(int integers[], int numints, int element)
{
    for (int i = 0; i < numints; i++) {
        if (integers[i] == element) {
            return i;
        }
    }
    return -1;
}

int copy_list_to_array(struct node *head, int *array)
{
    int count = 0;
    struct node *current = head;

    while (current != NULL) {
        array[count++] = current->data;
        current = current->next;
    }

    return count;
}

struct node *create_sorted_list(struct node *head)
{
    struct node *sorted_head = NULL;
    struct node *current = head;

    while (current != NULL) {
        sorted_head = add_item_sorted(sorted_head, current->data);
        current = current->next;
    }
    return sorted_head;
}

struct node *add_item_sorted(struct node *sorted_head, int data)
{
    struct node *new_node = (struct node *)malloc(sizeof(struct node));
    if (!new_node) {
        printf("Memory allocation failed.\n");
        exit(1);
    }
    new_node->data = data;
    new_node->next = NULL;

    if (sorted_head == NULL || sorted_head->data >= data) {
        new_node->next = sorted_head;
        return new_node;
    }

    struct node *current = sorted_head;
    while (current->next != NULL && current->next->data < data) {
        current = current->next;
    }
    new_node->next = current->next;
    current->next = new_node;

    return sorted_head;
}

void print_list(struct node *head)
{
    if (head == NULL) {
        printf("Linked List is Empty.\n");
    } else {
        struct node *temp = head;
        printf("head->");
        while (temp != NULL) {
            printf("|%d|->", temp->data);
            temp = temp->next;
        }
        printf("NULL\n");
    }
}

void print_array(int integers[], int len)
{
    int i;
    for (i = 0; i < len; i++) {
        printf("| %d ", integers[i]);
    }
    printf("|\n");
}

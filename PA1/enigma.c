#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ALPHABET_SIZE 26
#define NUM_ROTORS 8

/* Array of rotors */
const char* enigma_rotors[NUM_ROTORS+1] = {
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ", //0
    "EKMFLGDQVZNTOWYHXUSPAIBRCJ", //1
    "AJDKSIRUXBLHWTMCQGZNPYFVOE", //2
    "BDFHJLCPRTXVZNYEIWGAKMUSQO", //3
    "ESOVPZJAYQUIRHXLNFTGKDCMWB", //4
    "UKLBEPXWJVFZIYGAHCMTONDRQS", //5
    "JPGVOUMFYQBENHZRDKASXLICTW", //6
    "NZJHGRCXMYSWBOUFAIVLPEKQDT", //7
    "FKQHTLXOCBJSPDZRAMEWNIUYGV"  //8
};

/*
 * Convert a space-separated string of rotor indices into
 * an integer array of rotor indices
 *
 * @param rotor_ind_str   Space-separated string of rotor indices
 * @param num_rotors      Number of rotors provided in the string
 * @return                Integer array of rotor indices
 */
int* parse_rotor_indices(char* rotor_ind_str, int num_rotors) {
    int* rotors = malloc(num_rotors * sizeof(int));
    char* token = strtok(rotor_ind_str, " ");
    int i = 0;
    while (token != NULL && i < num_rotors) {
        rotors[i] = atoi(token);
        token = strtok(NULL, " ");
        i++;
    }
    return rotors;
}

/*
 * Create a 2D array of integers where
 * each row represents a rotor
 *
 * @param rotors          Integer array of rotor indices
 * @param num_rotors      Number of rotors provided
 * @return                2D array where each row represents a rotor
 */
int** set_up_rotors(int* rotors, int num_rotors) {
    int** rotor_config = malloc(num_rotors * sizeof(int*));

    for (int i = 0; i < num_rotors; i++) {
        rotor_config[i] = malloc(ALPHABET_SIZE * sizeof(int));

        int rotor_index = rotors[i];
        for (int j = 0; j < ALPHABET_SIZE; j++) {
            rotor_config[i][j] = enigma_rotors[rotor_index][j] - 'A';
        }
    }

    return rotor_config;
}


/*
 * Rotate each rotor to the right by the
 * given number of rotations
 *
 * @param rotor_config   2D array of rotors
 * @param rotations      Number of rotations
 * @param num_rotors     Number of rotors provided
 */
void rotate_rotors(int** rotor_config, int rotations, int num_rotors) {
    rotations %= ALPHABET_SIZE;
    for (int i = 0; i < num_rotors; i++) {
        int temp[ALPHABET_SIZE];

        for (int j = 0; j < ALPHABET_SIZE; j++) {
            temp[(j + rotations) % ALPHABET_SIZE] = rotor_config[i][j];
        }

        memcpy(rotor_config[i], temp, ALPHABET_SIZE * sizeof(int));
    }
}

/*
 * Encrypt the given message
 *
 * @param message        Message to encrypt
 * @param rotor_config   2D array of rotors
 * @param num_rotors     Number of rotors provided
 * @return               Encrypted message
 */
char* encrypt(char *message, int** rotor_config, int num_rotors) {
    int len = strlen(message);
    char* encrypted = malloc((len + 1) * sizeof(char));

    for (int i = 0; i < len; i++) {
        char ch = message[i];
        if (ch >= 'a' && ch <= 'z') {
            ch -= 32;  
        }

        if (ch >= 'A' && ch <= 'Z') {
            int index = ch - 'A';
            for (int j = 0; j < num_rotors; j++) {
                index = rotor_config[j][index];
            }
            encrypted[i] = 'A' + index;
        }
        else {
            encrypted[i] = ch;
        }
    }
    encrypted[len] = '\0';
    return encrypted;
}

/*
 * Decrypt the given message
 *
 * @param message        Message to decrypt
 * @param rotor_config   2D array of rotors
 * @param num_rotors     Number of rotors provided
 * @return               Decrypted message
 */
char* decrypt(char *message, int** rotor_config, int num_rotors) {
    int len = strlen(message);
    char* decrypted = malloc((len + 1) * sizeof(char));
    int inverse_rotors[num_rotors][ALPHABET_SIZE];

    for (int j = 0; j < num_rotors; j++) {
        for (int k = 0; k < ALPHABET_SIZE; k++) {
            inverse_rotors[j][k] = -1;
        }
        for (int k = 0; k < ALPHABET_SIZE; k++) {
            inverse_rotors[j][rotor_config[j][k]] = k;
        }
    }

    for (int i = 0; i < len; i++) {
        char ch = message[i];

        if (ch >= 'a' && ch <= 'z') {
            ch -= 32;
        }

        if (ch >= 'A' && ch <= 'Z') {
            int index = ch - 'A';
   
            for (int j = num_rotors - 1; j >= 0; j--) {
                index = inverse_rotors[j][index];
            }
            decrypted[i] = 'A' + index;
        }
        else {
            decrypted[i] = ch;
        }
    }

    decrypted[len] = '\0';
    return decrypted;
}

int** reverse_rotor_order(int** rotor_config, int num_rotors) {
    int** reversed = malloc(num_rotors * sizeof(int*));
    if (!reversed) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }

    for (int i = 0; i < num_rotors; i++) {
        reversed[i] = rotor_config[num_rotors - 1 - i]; 
    }

    return reversed;
}

/*
 * Format of command line input:
 * ./enigma e "JAVA" 3 "1 2 4" 0
 * 
 *    e    - mode (e for encrypt, d for decrypt)
 * "JAVA"  - message
 *    3    - number of rotors to use
 * "1 2 4" - indices of rotors to use
 *    0    - number of rotations of the rotors
 */
int main(int argc, char* argv[]) {
    char* mode = argv[1];
    char* message = argv[2];
    int num_rotors = atoi(argv[3]);

    if (num_rotors < 1 || num_rotors > 8) {
        fprintf(stderr, "Error: Number of rotors must be between 1 and 8.\n");
        return 1;
    }

    char* rotor_indices_str = strdup(argv[4]);
    if (!rotor_indices_str) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }

    int* rotor_indices = parse_rotor_indices(rotor_indices_str, num_rotors);
    free(rotor_indices_str);
    
    int** rotor_config = set_up_rotors(rotor_indices, num_rotors);
    free(rotor_indices); 

    int rotor_offset = 0;
    if (argc == 6) {
        rotor_offset = atoi(argv[5]);
    }

    char* result;
    if (mode[0] == 'e') {
        rotate_rotors(rotor_config, rotor_offset, num_rotors);
        result = encrypt(message, rotor_config, num_rotors);
        printf("Encrypted message: %s\n", result);
    }
    else if (mode[0] == 'd') { 
        rotor_config = reverse_rotor_order(rotor_config, num_rotors); 
        rotate_rotors(rotor_config, rotor_offset, num_rotors);
        result = decrypt(message, rotor_config, num_rotors);
        printf("Decrypted message: %s\n", result);
    }
    else {
        fprintf(stderr, "Error: Invalid mode. Use 'e' for encryption or 'd' for decryption.\n");
        return 1;
    }

    free(result);
    for (int i = 0; i < num_rotors; i++) {
        free(rotor_config[i]);
    }
    free(rotor_config);

    return 0;
}

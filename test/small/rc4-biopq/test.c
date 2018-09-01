#include <stdio.h>

#define KEY_LENGTH 6
#define TEXT_LENGTH 10

typedef struct RC4_KEY_S{
    unsigned char S[256];
}RC4_KEY;

void RC4_key(RC4_KEY *rc4_key, unsigned char *key, int *keylength){
    int i, j, temp;

    /*Initialize S*/
    for (i = 0; i < 256; i++)
	rc4_key -> S[i] = i;

    j = 0;
    for (i = 0; i < 256; i++){
	j = (j + rc4_key -> S[i] + *(key + i % *keylength)) % 256;
	/*Swap rc4_key -> S[i] and rc4_key -> S[j]*/
	temp = rc4_key -> S[i];
	rc4_key -> S[i] = rc4_key -> S[j];
	rc4_key -> S[j] = temp;
    }
}

/*Generate the key stream which length is the same as plaintext's and encrypt the plaintext and output the ciphertext.*/
void RC4(RC4_KEY *rc4_key, unsigned char *plaintext, int *plaintext_length, unsigned char *ciphertext){
    int i = 0, j = 0, n, temp;

    for (i = 0; i < *plaintext_length; i++){
	i = (i + 1) % 256;
	j = (j + rc4_key -> S[i]) % 256;
	/*Swap rc4_key -> S[i] and rc4_key -> S[j]*/
	temp = rc4_key -> S[i];
	rc4_key -> S[i] = rc4_key -> S[j];
	rc4_key -> S[j] = temp;

	n = rc4_key -> S[(rc4_key -> S[i] + rc4_key -> S[j]) % 256];

	/*Encryption*/
	*(ciphertext + i) = *(plaintext + i) ^ n;
    }
}
int main(int argc, char** argv){
    int i, x;
    int *p;
    unsigned char plaintext[TEXT_LENGTH];
    unsigned char key[KEY_LENGTH] = "cipher";
    unsigned char ciphertext[TEXT_LENGTH];
    
    RC4_KEY rc4_key;
    
    printf("------------------------------\n----------RC4 CIPHER----------\n------------------------------\n");
    printf("Please enter the plaintext(%d characters): ", TEXT_LENGTH);
    for (i = 0; i < TEXT_LENGTH; i++)
		plaintext[i]=1;
   
    /*Encryption*/
    x = KEY_LENGTH;
    p = &x;
    RC4_key(&rc4_key, &key[0], p);
    x = TEXT_LENGTH;
    RC4(&rc4_key, &plaintext[0], p, &ciphertext[0]);
    
    printf("The plaintext is: ");
    for (i = 0; i < TEXT_LENGTH; i++)
	printf("%x ", plaintext[i]);
    printf("\nThe key is: ");
    for (i = 0; i < KEY_LENGTH; i++)
	printf("%x ", key[i]);
    printf("\nThe ciphertext is: ");
    for (i = 0; i <TEXT_LENGTH; i++)
	printf("%x ", ciphertext[i]);
    printf("\n\n");
    
    return 0;
}

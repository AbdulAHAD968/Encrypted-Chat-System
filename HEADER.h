#ifndef HEADER_H
#define HEADER_H


//* ANSI COLOR SEQUENCES. *//
#define ANSI_COLOR_RESET        "\x1b[0m "
#define ANSI_COLOR_RED          "\x1b[31m"
#define ANSI_COLOR_YELLOW       "\x1b[33m"
#define ANSI_COLOR_GREEN        "\x1b[32m"
#define ANSI_COLOR_BLUE         "\033[34m"
#define ANSI_COLOR_ORANGE       "\033[38;5;208m"
#define ANSI_COLOR_PINK         "\033[38;5;205m"
#define ANSI_COLOR_PURPLE       "\033[38;5;165m"
#define ANSI_COLOR_DARK_GREEN   "\033[38;5;22m"
#define CLEAR_SCREEN            "\x1B[2J\x1B[H"
#define ANSI_COLOR_BOLD_RED   "\033[1;31m"
#define ANSI_COLOR_BOLD_GREEN   "\033[1;32m"


//* MAIN REQUIRED LIBRARIES. *//
#include <iostream>  
#include <cstring>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <unistd.h>
#include <cstdlib>
#include <arpa/inet.h>  
#include <string>
#include <fstream>
#include <cmath>
#include <cstdint>
#include <openssl/aes.h>
#include <openssl/rand.h>
#include <openssl/evp.h>


using namespace std;


int APPEND_KEY = ( rand() % 100 ) +10;


//*STANDARD AES ENCRYPTION FUNCTION. *//
string AES_encrypt(const string& plaintext, const string& key){

    EVP_CIPHER_CTX *ctx;
    unsigned char iv[EVP_MAX_IV_LENGTH];
    unsigned char ciphertext[1024];
    int len, ciphertext_len;

    // Create and initialize the context
    if(!(ctx = EVP_CIPHER_CTX_new())){
        cerr<<" ERROR CREATING CONTEXT. "<<endl;
        exit(EXIT_FAILURE);
    }

    // Generate a random IV
    if( ! RAND_bytes(iv, EVP_MAX_IV_LENGTH) ){
        cerr<<"ERROR GENERATING RANDOM IV."<<endl;
        exit(EXIT_FAILURE);
    }

    // Initialize encryption operation with AES-128-CBC mode
    if(1 != EVP_EncryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, (const unsigned char*)key.c_str(), iv)) {
        cerr << "Error initializing encryption" << endl;
        exit(EXIT_FAILURE);
    }

    // Encrypt the message
    if(1 != EVP_EncryptUpdate(ctx, ciphertext, &len, (const unsigned char*)plaintext.c_str(), plaintext.length())) {
        cerr << "Error during encryption" << endl;
        exit(EXIT_FAILURE);
    }
    ciphertext_len = len;

    // Finalize encryption
    if(1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len)) {
        cerr << "Error finalizing encryption" << endl;
        exit(EXIT_FAILURE);
    }
    ciphertext_len += len;

    // Cleanup
    EVP_CIPHER_CTX_free(ctx);

    // Return IV + ciphertext
    return string((char*)iv, EVP_MAX_IV_LENGTH) + string((char*)ciphertext, ciphertext_len);
}


//*STANDARD AES DECRYPTION FUNCTION. *//
string AES_decrypt(const string& ciphertext, const string& key){

    EVP_CIPHER_CTX *ctx;
    unsigned char iv[EVP_MAX_IV_LENGTH];
    unsigned char plaintext[1024];
    int len, plaintext_len;

    // Extract the IV from the beginning of the ciphertext
    memcpy(iv, ciphertext.c_str(), EVP_MAX_IV_LENGTH);

    // Create and initialize the context
    if(!(ctx = EVP_CIPHER_CTX_new())) {
        cerr << "Error creating context" << endl;
        exit(EXIT_FAILURE);
    }

    // Initialize decryption operation with AES-128-CBC mode
    if(1 != EVP_DecryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, (const unsigned char*)key.c_str(), iv)) {
        cerr << "Error initializing decryption" << endl;
        exit(EXIT_FAILURE);
    }

    // Decrypt the message
    if(1 != EVP_DecryptUpdate(ctx, plaintext, &len, (const unsigned char*)ciphertext.c_str() + EVP_MAX_IV_LENGTH, ciphertext.length() - EVP_MAX_IV_LENGTH)) {
        cerr << "Error during decryption" << endl;
        exit(EXIT_FAILURE);
    }
    plaintext_len = len;

    // Finalize decryption
    if(1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len)) {
        cerr << "Error finalizing decryption" << endl;
        exit(EXIT_FAILURE);
    }
    plaintext_len += len;

    // Cleanup
    EVP_CIPHER_CTX_free(ctx);

    return string((char*)plaintext, plaintext_len);
}


//*CHAT SYSTEM NAME.*//
void HASH_CHAT(){
    
    cout<<endl<<endl;
    cout<<ANSI_COLOR_BOLD_GREEN<<"\t #############################################################################################"<<endl;
    cout<<"\t #   ___   ___  ________  ______  ___   ___       ______  ___   ___  ________  _________     #"<<endl;
    cout<<"\t #  /__/\\ /__/\\/_______/\\/_____/\\/__/\\ /__/\\     /_____/\\/__/\\ /__/\\/_______/\\/________/\\    #"<<endl; 
    cout<<"\t #  \\::\\ \\\\  \\ \\::: _  \\ \\::::_\\/\\::\\ \\\\  \\ \\    \\:::__\\/\\::\\ \\\\  \\ \\::: _  \\ \\__.::.__\\/    #"<<endl; 
    cout<<"\t #   \\::\\/_\\ .\\ \\::(_)  \\ \\:\\/___/\\::\\/_\\ .\\ \\    \\:\\ \\  _\\::\\/_\\ .\\ \\::(_)  \\ \\ \\::\\ \\      #"<<endl; 
    cout<<"\t #    \\:: ___::\\ \\:: __  \\ \\_::._\\:\\:: ___::\\ \\    \\:\\ \\/_/\\:: ___::\\ \\:: __  \\ \\ \\::\\ \\     #"<<endl; 
    cout<<"\t #     \\: \\ \\\\::\\ \\:.\\ \\  \\ \\/____\\:\\: \\ \\\\::\\ \\    \\:\\_\\ \\ \\: \\ \\\\::\\ \\:.\\ \\  \\ \\ \\::\\ \\    #"<<endl; 
    cout<<"\t #      \\__\\/ \\::\\/\\__\\/\\__\\/\\_____\\/\\__\\/ \\::\\/     \\_____\\/\\__\\/ \\::\\/\\__\\/\\__\\/  \\__\\/    #"<<endl;
    cout<<ANSI_COLOR_BOLD_RED<<"\t #                                                                                 By R0BB3R #"<<endl;
    cout<<"\t #                                                           https://github.com/AbdulAHAD968 #"<<endl;
    cout<<"\t #                                         https://www.linkedin.com/in/abdul-ahad-988305286/ #"<<endl;
    cout<<"\t #############################################################################################"<<ANSI_COLOR_RESET<<endl<<endl;

}


//*ADMIN_DISPLAY_UTILITY*//
void ADMIN_PLAYS(){
    
    cout<<endl<<endl;
    cout<<ANSI_COLOR_BOLD_GREEN<<"\t  #########################################################################"<<endl;
    cout<<ANSI_COLOR_BOLD_RED<<"\t  //        __      ________  ___      ___  __   _____  ___              //"<<endl; 
    cout<<"\t  //       /  \\    |        \\|   \\    /   ||  \\ (\\    \\|   \\             //"<<endl; 
    cout<<"\t  //      /    \\   (.  ___  :)\\   \\  //   |||  ||.\\\\   \\    |            //"<<endl; 
    cout<<"\t  //     /' /\\  \\  |: \\   ) ||/\\\\  \\/.    ||:  ||: \\.   \\\\  |            //"<<endl; 
    cout<<"\t  //    //  __'  \\ (| (___\\ ||: \\.        ||.  ||.  \\    \\. |            //"<<endl; 
    cout<<"\t  //   /   /  \\\\  \\|:       :|.  \\    /:  |/\\  ||    \\    \\ |            //"<<endl; 
    cout<<"\t  //  (___/    \\___(________/|___|\\__/|___(__\\_|_\\___|\\____\\)            //"<<ANSI_COLOR_RESET<<endl; 
    cout<<ANSI_COLOR_BOLD_GREEN<<"\t  #########################################################################"<<endl<<ANSI_COLOR_RESET<<endl;

return;
}

#endif

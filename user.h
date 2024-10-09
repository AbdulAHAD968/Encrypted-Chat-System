#ifndef USER_H
#define USER_H

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <random>
#include <iomanip>
#include <openssl/sha.h>
#include <cstring>

using namespace std;


// Function to generate a random salt
string generate_salt(int length) {
    const char* chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    string salt;
    random_device rd;
    mt19937 generator(rd());
    uniform_int_distribution<> dist(0, strlen(chars) - 1);

    for (int i = 0; i < length; ++i) {
        salt += chars[dist(generator)];
    }
    return salt;
}

// Function to hash a password with salt using SHA-256
string hash_password(const string& password, const string& salt) {
    string salted_password = password + salt;
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char*)salted_password.c_str(), salted_password.size(), hash);

    stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        ss << hex << setw(2) << setfill('0') << (int)hash[i];
    }
    return ss.str();
}

// COUNT THE LINES FROM FILE
int count_lines() {
    int count = 0;
    char alpha;
    ifstream file("updated_creds.txt");
    if(!file){
        cerr << "Error opening file!" << endl;
        return 0;
    }

    while (!file.eof()) {
        file.get(alpha);
        if (alpha == '\n') {
            count++;
        }
    }

    return count;
}

static int record = 1;

// USER CLASS
class USER {
    private:
        string valid_email;
        string unique_name;
        string salt;
        string hashed_pass;

    public:
        // Constructor
        USER(string email = "", string name = "", string pass = "") {
            this->valid_email = email;
            this->unique_name = name;
            this->hashed_pass = pass;
        }

        // Read and store data from creds file
        void Read_Client_Data(ifstream& read) {
            string name, email, temp_salt, temp_hash;
            if (read >> name >> email >> temp_salt >> temp_hash) {
                this->valid_email = email;
                this->unique_name = name;
                this->salt = temp_salt;
                this->hashed_pass = temp_hash;
            }
        }

        // Print info
        void Print_Info() {
            cout << "\n\t -------------------------";
            cout << "\n\t | USER ID: " << record;
            cout << "\n\t |------------------------";
            cout << "\n\t |  USER NAME: " << this->unique_name;
            cout << "\n\t |  EMAIL: " << this->valid_email;
            cout << "\n\t |  HASHED PASSWORD: " << this->hashed_pass;
            cout << "\n\t |  SALT: " << this->salt;
            cout << "\n\t -------------------------\n";
        }

        // Verify identity
        bool Verify_Identity(string name, string pass) {
            string entered_hash = hash_password(pass, this->salt);
            return this->unique_name == name && this->hashed_pass == entered_hash;
        }

        // Validate email
        bool Valid_Email(string email) {
            return email.find('@') != string::npos && email.find('.') != string::npos;
        }

        // Strong password requirement
        bool Strong_Password(string pass) {
            int digit_count = 0, small_count = 0, capital_count = 0, special_count = 0;
            for (char& i : pass) {
                if (isdigit(i)) digit_count++;
                else if (islower(i)) small_count++;
                else if (isupper(i)) capital_count++;
                else if (ispunct(i)) special_count++;
            }
            return digit_count >= 1 && small_count >= 1 && capital_count >= 1 && special_count >= 2;
        }

        // User existence validation
        bool User_Exists(string name) {
            ifstream read("updated_creds.txt");
            string name_check, temp_email, temp_salt, temp_hash;
            while (read >> name_check >> temp_email >> temp_salt >> temp_hash) {
                if (name_check == name) return true;
            }
            read.close();
            return false;
        }

        // Register new user
        void Register_User() {
            string email, name, pass;
            cout << "\n\t -----------------------------------";
            cout << "\n\t |  ENTER YOUR USERNAME [NO SPACE]: ";
            cin >> name;

            while (User_Exists(name)) {
                cout << "\n\t |  USERNAME ALREADY EXISTS!";
                cout << "\n\t |  ENTER YOUR USERNAME [NO SPACE]: ";
                cin >> name;
            }

            cout << "\n\t |  ENTER YOUR EMAIL: ";
            cin >> email;
            while (!Valid_Email(email)) {
                cout << "\n\t |  INVALID EMAIL!";
                cout << "\n\t |  ENTER YOUR EMAIL: ";
                cin >> email;
            }

            cout << "\n\t |  ENTER YOUR PASSWORD: ";
            cin >> pass;
            while (!Strong_Password(pass)) {
                cout << "\n\t |  STRONG PASSWORD REQUIRED!";
                cout << "\n\t |  ENTER YOUR PASSWORD: ";
                cin >> pass;
            }

            cout << "\n\t -----------------------------------";
            cout << "\n\t |  CONFIRM PASSWORD: ";
            string confirm_pass;
            cin >> confirm_pass;

            if (pass == confirm_pass) {
                string salt = generate_salt(16);
                string hashed_password = hash_password(pass, salt);
                
                ofstream write("updated_creds.txt", ios::app);
                write << name << " " << email << " " << salt << " " << hashed_password << endl;
                cout << "\n\t |  REGISTRATION COMPLETED!";
                write.close();
            } else {
                cout << "\n\t |  PASSWORDS DO NOT MATCH!";
            }
        }
};

#endif


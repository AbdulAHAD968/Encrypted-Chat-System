#ifndef ADMIN_H
#define ADMIN_H


#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <random>
#include <iomanip>
#include <openssl/sha.h>
#include <cstring>
#define ANSI_COLOR_ORANGE       "\033[38;5;208m"
#define ANSI_COLOR_RESET        "\x1b[0m "
#define ANSI_COLOR_BOLD_RED   "\033[1;31m"

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


//** MAKING THE ADMIN CLASS [ADMIN FOR CLIENT-SERVER COMMUNICATION] */
class ADMIN{

    private:

        string valid_email;
        string unique_name;
        string strong_pass;
        string salt;


        // READ ADMIN DATA FROM admin_pass.txt
        void read_admin_data(){
            ifstream read("admin_pass.txt");
            
            if(!read){
                cout<<"\n\t | ERROR: COULD NOT OPEN FILE!";
                exit(1);
            }
            else{
                read>>unique_name>>valid_email>>salt>>strong_pass;
                read.close();
            }
        }

    public:

        // ADMIN DEFINED DEFAULT COSTRUCTOR.
        ADMIN(string email="" , string name="" , string pass="" , string salt=""){
            this->valid_email = email;
            this->unique_name = name;
            this->strong_pass = pass;
            this->salt = salt;
        }


        // READ AND STORE DATA FROM CREDS FILE. 
        void Read_Client_Data(string file){

            ifstream read(file);

            if(!read){
                cout<<"\n\t | ERROR: COULD NOT OPEN FILE!";
                exit(1);
            }

            if(file == "creds.txt"){
                
                string name, email, pass;  
                
                while(read >> name >> email >> pass){  
                    this->valid_email = email;  
                    this->unique_name = name;  
                    this->strong_pass = pass;
                }
            }
            else if(file == "updated_creds.txt"){
                
                string name, email, pass, salt;  
                
                while(read >> name >> email >> salt >> pass){  
                    this->valid_email = email;  
                    this->unique_name = name;  
                    this->strong_pass = pass;
                    this->salt = salt;
                }
            }
            read.close();

        } 


        // PRINT INFO USERS.
        void Print_Info_users(){
            cout<<"\n\t |------------------------";
            cout<<"\n\t |  USER NAME : "<<this->unique_name;
            cout<<"\n\t |  EMAIL     : "<<this->valid_email;
            cout<<"\n\t |  PASSWORD  : "<<this->strong_pass;
            cout<<"\n\t -------------------------"<<endl;
        }


        // PRINT INFO
        void Print_Info_admin(){
            cout<<"\n\t -------------------------";
            cout<<"\n\t |    SUPER USER         |";
            cout<<"\n\t |------------------------";
            cout<<"\n\t |  ADMIN NAME : "<<this->unique_name;
            cout<<"\n\t |  EMAIL      : "<<this->valid_email;
            cout<<"\n\t |  PASSWORD   : "<<this->strong_pass;
            cout<<"\n\t -------------------------"<<endl;
        }


        // SINCE ADMIN IS SUPER USER, HE CAN REMOVE OTHER USERS...
        void Remove_User(string name){

            ifstream read("updated_creds.txt");         // REMOVE WHERE PASSWORD WAS HASHED.
            ofstream write("temp.txt" , ios::app);

            bool found = false;

            if(!read){
                cout<<"\n\t | ERROR: COULD NOT OPEN FILE!";
                exit(1);
            }

            string name_temp, email_temp, pass_temp, salt_temp;
            
            while(read >> name_temp >> email_temp >> salt_temp >> pass_temp){
                
                if(name_temp!= name){
                    write<<name_temp<<" "<<email_temp<<" "<<salt_temp<<" "<<pass_temp<<endl;
                }
                else{
                    found = true;
                    cout<<"\n\t | USER "<<name_temp<<" REMOVED SUCCESSFULLY!";
                }

            }
            read.close();

            write.close();
            remove("updated_creds.txt");
            rename("temp.txt", "updated_creds.txt");

            // REMOVE WHERE PASSWORD WAS IN PLAIN TEXT.
            ifstream read_plain("creds.txt");         // REMOVE WHERE PASSWORD IS IN PLAIN TEXT.
            ofstream write_plain("temp.txt" , ios::app);

            if(!read_plain){
                cout<<"\n\t | ERROR: COULD NOT OPEN FILE!";
                exit(1);
            }
            
            while(read_plain >> name_temp >> email_temp >> pass_temp){
                
                if(name_temp!= name){
                    write_plain<<name_temp<<" "<<email_temp<<" "<<pass_temp<<endl;
                }
                else{
                    cout<<"\n\t | USER "<<name_temp<<" REMOVED SUCCESSFULLY!";
                }

            }
            read_plain.close();

            write_plain.close();
            remove("creds.txt");
            rename("temp.txt", "creds.txt");

            if(found == false){
                cout<<"\n\t | USER NOT FOUND!";
            }
        }


        // VERIFY ADMIN IDENTITY.
        bool Verify_Admin(){

            string name, pass;
            cout<<"\n\t | ENTER [ADMIN'S] NAME    :: ";
            getline(cin, name);
            cout<<"\n\t | ENTER  PASSWORD  :: ";
            getline(cin, pass);
            
            this->read_admin_data();

            if(name == this->unique_name && hash_password(pass, this->salt) == this->strong_pass){
                return true;
            }
            else{
                return false;
            }
        }


        // CHANGE OWN DATA.
        void Update_Own_Data(){

            cin.ignore();
            
            string name, email, pass, salt;
            cout<<"\n\t | ENTER YOUR NEW DATA: ";
            cout<<"\n\t | NAME      :: ";
            getline(cin, name);
            cout<<"\n\t | EMAIL     :: ";
            getline(cin, email);
            cout<<"\n\t | PASSWORD  :: ";
            getline(cin, pass);
            salt = generate_salt(16);
            strong_pass = hash_password(pass, salt);

            ofstream write("admin_pass.txt" , ios::out | ios::trunc);
            write<<name<<" "<<email<<" "<<salt<<" "<<strong_pass;
            write.close();

        }


        // MAKE AN ADMIN MENUE.
        void Admin_Menu(){

            ADMIN_PLAYS();
            
            while(true){
                
                cout<<ANSI_COLOR_ORANGE<<"\n\t ---------------------------------------------";
                cout<<"\n\t |               { ADMIN MENU }              |";
                cout<<"\n\t | ------------------------------------------";
                cout<<"\n\t | 1. UPDATE [ADMINS] DATA.                  |";
                cout<<"\n\t | 2. SEE [ADMINS] INFORMATION.              |";
                cout<<"\n\t | 3. EDIT [ADMINS] INFORMATION.             |";
                cout<<"\n\t | 4. REMOVE USER.                           |";
                cout<<"\n\t | 5. SEE USERS INFORMATION. [HASHED INFO.]  |";
                cout<<"\n\t | 6. SEE USERS INFORMATION. [PLAIN TEXT.]   |";
                cout<<"\n\t | 7.            [ EXIT MENUE]               |";
                cout<<"\n\t ---------------------------------------------";
                cout<<"\n\t | ENTER YOUR CHOICE :: "<<ANSI_COLOR_BOLD_RED<<endl;;

                int choice;
                cin>>choice;
                
                switch(choice){
                    case 1:
                    {
                        this->Update_Own_Data();
                        cout<<"\n\t | DATA UPDATED SUCCESSFULLY!";
                        break;
                    }
                    case 2:
                    {
                        this->read_admin_data();
                        this->Print_Info_admin();
                        break;
                    }
                    case 3:
                    {
                        this-> Update_Own_Data();
                        cout<<"\n\t | DATA UPDATED SUCCESSFULLY!";
                        break;
                    }
                    case 4:
                    {
                        string name;
                        cout<<"\n\t | ENTER USER NAME TO REMOVE: ";
                        cin>>name;
                        this->Remove_User(name);
                        break;
                    }
                    case 5:
                    {
                        ifstream read("updated_creds.txt");

                        if(!read){
                            cout<<"\n\t | ERROR: COULD NOT OPEN FILE!";
                            return;
                        }
                        else{
                            string name, email, salt, pass;
                            while(read >> name >> email >> salt >> pass){
                                cout<<"\n\t | -------------------------";
                                cout<<"\n\t |  USER NAME : "<<name;
                                cout<<"\n\t |  EMAIL     : "<<email;
                                cout<<"\n\t |  PASSWORD  : HASHED";
                                cout<<"\n\t -------------------------"<<endl;
                            }
                            read.close();
                        }
                        break;
                    }
                    case 6:
                    {
                        ifstream read("creds.txt");
                        if(!read){
                            cout<<"\n\t | ERROR: COULD NOT OPEN FILE!";
                            return;
                        }
                        else{
                            string name, email, pass;
                            while(read >> name >> email >> pass){
                                cout<<"\n\t | -------------------------";
                                cout<<"\n\t |  USER NAME : "<<name;
                                cout<<"\n\t |  EMAIL     : "<<email;
                                cout<<"\n\t |  PASSWORD  : "<<pass;
                                cout<<"\n\t -------------------------"<<endl;
                            }
                            read.close();
                        }
                        break;
                    }
                    case 7:
                    {
                        cout<<"\n\t | THANK YOU FOR USING OUR SYSTEM!"<<ANSI_COLOR_RESET<<endl;
                        return;
                    }
                    default:
                    {
                        cout<<"\n\t | INVALID CHOICE! PLEASE TRY AGAIN!";
                        break;
                    }

                }// switch
                
            }// loop

        }// function
        
};


#endif
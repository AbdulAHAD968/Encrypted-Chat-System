#include "HEADER.h"
#include "ADMIN.h"


using namespace std;


bool flag=false;
string User_Name, Pass_Word;

int ENC_KEY;
string SHARED_KEY;


// User existence validation
bool User_Exists(string name){
    ifstream read("updated_creds.txt");
    string check, temp_email, temp_salt, temp_hash;
    while (read >> check >> temp_email >> temp_salt >> temp_hash) {
        if (check == name){
            return true;
        }
    }
    read.close();
    return false;
}


// CHECK IF THE EMAIL IS VALID.
bool Valid_Email(string email){
    return email.find('@') != string::npos && email.find('.') != string::npos;
}


//** HELPER FUNCTION TO ENSURE THAT THE KEY LENGTH IS 16-BIT */
string format_key(const string& input){
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((const unsigned char*)input.c_str(), input.size(), hash);
    return string((char*)hash, 16);
}


//*SEND MESSAGES TO SERVER.*//
void send_message(int client_socket, const string& message){
    send(client_socket, message.c_str(), message.size(), 0);
}


//* APPLY SIMPLE ENCRYPTION BASED UPON ROTATION. *//
string ENCRYPT(const string& message){

    string result = message;
    for(char& c : result){

        if(isalpha(c)){
            if(islower(c)){
                c = 'a' + (c - 'a' + ENC_KEY) % 26;
            }
            else{
                c = 'A' + (c - 'A' + ENC_KEY) % 26;
            }
        }
    }
    return result;
}


//*RECEIVE MESSAGES FROM SERVER AND DISPLAY THEM AS WELL.*//
bool receive_message(int client_socket, char* buf, size_t buf_size){

    memset(buf, 0, buf_size);
    
    int bytes_received = recv(client_socket, buf, buf_size - 1, 0);
    
    if(bytes_received <= 0){
        cerr<<"\n ERROR RECEIVING MESSAGES FROM THE SERVER. "<<endl;
        return false;
    }
    buf[bytes_received] = '\0';
    
    if(flag == false){
        cout<<"\n SERVER  :: "<<ENCRYPT(buf);
    }
    else{
        string decrypted_message = AES_decrypt(buf, SHARED_KEY);
        cout<<"\n [ SERVER ] :: "<<decrypted_message<<endl;
    }
    return true;
}


// GENERATE THE PUBLIC KEY FOR CLIENT...
long long generate_public_key(long long alpha, long long private_key, long long P){
    return (long long)pow(alpha, private_key) % P;
}


// GENERATE THE SHARED KEY USING SERVER PUBLIC KEY.
long long generate_shared_key(long long received_pub_key, long long private_key, long long P){
    return (long long)pow(received_pub_key, private_key) % P;
}


//*MAIN FUNCTION.*//
int main(){

    HASH_CHAT();

    char buf[256];
    long long P, alpha, server_pub_key;
    long long client_private_key = 3;
    long long client_pub_key;

    int client_socket;
    struct sockaddr_in server_address;
    bool logged_in = false;

    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        cerr << "\n ERROR CREATING SOCKET. " << endl;
        return 0;
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8080);
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(client_socket, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        perror("\n CONNECTION TO SERVER FAILED. \n");
        return 0;
    }


    // RECIEVE LARGE_P, ALPHA, & SERVER KI PUBLIC KEY...
    recv(client_socket, buf, sizeof(buf), 0);
    sscanf(buf, "P:%lld alpha:%lld server_pub_key:%lld", &P, &alpha, &server_pub_key);

    // GENERATE PUBLIC KEY FOR CLIENT.
    client_pub_key = generate_public_key(alpha, client_private_key, P);
    snprintf(buf, sizeof(buf), "%lld", client_pub_key);
    send(client_socket, buf, sizeof(buf), 0);

    // CALCUALTE THE SHARED KEY.
    long long shared_key_client = generate_shared_key(server_pub_key, client_private_key, P);
    ENC_KEY = shared_key_client;
    

    while(true){

        if(!logged_in){

            if(!receive_message(client_socket, buf, sizeof(buf))) {
                break;
            }

            string choice;
            cout << "\n HIT THE KEYS = ";
            getline(cin, choice);
            send_message(client_socket, choice + "\n");

            if (choice == "1") {
                string username, email, password, confirm_pass;

                do {
                    sleep(1);
                    receive_message(client_socket, buf, sizeof(buf));
                    getline(cin, username);
                    while(User_Exists(username)){
                        cout<<"\n USERNAME ALREADY EXISTS. TRY ANOTHER ONE: ";
                        getline(cin, username);
                    }
                    username = ENCRYPT(username);
                    send_message(client_socket, username + "\n");
                } while (string( ENCRYPT (buf) ).find("USER NAME ALREADY EXISTS") != string::npos);

                do {
                    sleep(1);
                    receive_message(client_socket, buf, sizeof(buf));
                    getline(cin, email);
                    while(!Valid_Email(email)){
                        cout<<"\n PLEASE ENTER A VALID EMAIL FORMAT (e.g. 123@example.com): ";
                        getline(cin, email);
                    }
                    email = ENCRYPT(email);
                    sleep(1);
                    send_message(client_socket, email + "\n");
                    memset(buf, 0, sizeof(buf));
                }while(string( ENCRYPT (buf) ).find("INVALID EMAIL FORMAT") != string::npos);

                do {
                    receive_message(client_socket, buf, sizeof(buf));
                    getline(cin, password);
                    password = ENCRYPT(password);
                    sleep(1);
                    send_message(client_socket, password + "\n");
                    memset(buf, 0, sizeof(buf));
                }while(string( ENCRYPT (buf) ).find("WEAK PASSWORD") != string::npos);

                receive_message(client_socket, buf, sizeof(buf));
                memset(buf, 0, sizeof(buf));
                
            }
            else if (choice == "2"){

                sleep(1);
                receive_message(client_socket, buf, sizeof(buf));
                getline(cin, User_Name);
                User_Name = ENCRYPT(User_Name);
                send_message(client_socket, User_Name + "\n");

                sleep(1);
                receive_message(client_socket, buf, sizeof(buf));
                getline(cin, Pass_Word);
                Pass_Word = ENCRYPT(Pass_Word);
                send_message(client_socket, Pass_Word + "\n");

                if (receive_message(client_socket, buf, sizeof(buf))) {
                    if (string( ENCRYPT (buf) ).find("LOGIN SUCCESSFUL") != string::npos) {
                        logged_in = true;
                        cout << "\nYOU CAN NOW CHAT WITH THE SERVER.\n";
                    }
                }
            }
            else if (choice=="3"){
                string username, password;

                sleep(1);
                receive_message(client_socket, buf, sizeof(buf));
                getline(cin, username);
                username = ENCRYPT(username);
                send_message(client_socket, username + "\n");

                sleep(1);
                receive_message(client_socket, buf, sizeof(buf));
                getline(cin, password);
                password = ENCRYPT(password);
                send_message(client_socket, password + "\n");

                if(receive_message(client_socket, buf, sizeof(buf))){

                    if(string( ENCRYPT (buf) ).find("LOGIN SUCCESSFUL") != string::npos){
                        cout<<ANSI_COLOR_PURPLE<< "\n YOU ARE NOW GRANTED ELEVATED PRIVILLAGES."<<ANSI_COLOR_RESET<<endl;
                        memset(buf, 0, sizeof(buf));
                        ADMIN admin;
                        admin.Admin_Menu();
                    }
                }
                continue;
            }
            else if(choice=="0"){
                cout<<ANSI_COLOR_BOLD_RED<<"\n EXITING THE CHAT. BYE."<<ANSI_COLOR_RESET<<endl;
                close(client_socket);
                return 0;
            }
        }
        if(logged_in){

            flag = true;

            // APPEND THE SHARED KEY WITH THE USERNAME [WHO LOGED IN]
            User_Name = ENCRYPT(User_Name);

            string aes_key = User_Name + to_string(APPEND_KEY);

            // MAKE THE KEY LENGTH [16-BIT]
            SHARED_KEY = format_key(aes_key);

            while(true){

                string message;
                cout<<ANSI_COLOR_BOLD_GREEN<< "\n [ YOU ] :: ";
                getline(cin, message);

                
                if(message == "exit"){
                    send_message(client_socket, "exit\n");
                    cout<<ANSI_COLOR_RESET<<endl;
                    return 0;
                }

                // ENCRYPT MESSAGE BEFORE SENDING.
                string encrypted_message = AES_encrypt(message, SHARED_KEY);
                send(client_socket, encrypted_message.c_str(), encrypted_message.size(), 0);

                
                char buf[1024];
                memset(buf, 0, sizeof(buf));
                int bytes_received = recv(client_socket, buf, sizeof(buf), 0);
                if (bytes_received <= 0) {
                    cout<<ANSI_COLOR_BOLD_RED<<"\n CONNECTION CLOSED BY THE SERVER. "<<ANSI_COLOR_RESET<<endl;
                    break;
                }

                // DECRYPT MESSAGE BEFORE DISPLAYING.
                string encrypted_response(buf, bytes_received);
                string decrypted_response = AES_decrypt(encrypted_response, SHARED_KEY);

                cout<<"\n[SERVER] => "<<decrypted_response;
            }
        }


    }// while 

    close(client_socket);
    return 0;

}//function
#include "HEADER.h"
#include "user.h"


using namespace std;

static int counter=0;
bool flag=false;
string User_Name, Pass_Word;

int ENC_KEY;
string SHARED_KEY;



//** HELPER FUNCTION TO ENSURE THAT THE KEY LENGTH IS 16-BIT */
string format_key(const string& input){
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((const unsigned char*)input.c_str(), input.size(), hash);
    return string((char*)hash, 16);
}

//** DEFINE DIFFERENT STATES [AS IN CASES]*/
enum ServerState{
    AWAIT_CHOICE,
    REGISTER,
    SUDOER,
    LOGIN,
    CHAT,
    EXIT_IT
};


//** FUNCTION TO PERFORM ENCRYPTION AND DECRYPTION. */
string ENCRYPT(const string& message){

    string result = message;
    
    for(char& c : result){
        if (isalpha(c)){
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


//** FUNCTION TO GENERATE PUBLIC KEY FOR SERVER. */
long long generate_public_key(long long alpha, long long private_key, long long P){
    return (long long)pow(alpha, private_key) % P;
}


//** FUNCTION TO GENERATE THE RESULTED SHARED KEY. */
long long generate_shared_key(long long received_pub_key, long long private_key, long long P){
    return (long long)pow(received_pub_key, private_key) % P;
}


void handle_client(int client_socket){

    char buf[256];
    long long P = 17;
    int alpha = 2;
    int server_private_key = 2;
    int server_pub_key = generate_public_key(alpha, server_private_key, P);
    
    snprintf(buf, sizeof(buf), "P:%lld alpha:%lld server_pub_key:%lld", P, alpha, server_pub_key);
    send(client_socket, buf, sizeof(buf), 0);


    // RECIEVE THE CLIENT PUBLIC KEY
    memset(buf, 0, sizeof(buf));
    recv(client_socket, buf, sizeof(buf), 0);
    long long client_pub_key = atoll(buf);

    // COMPUTE THE SHARED KEY.
    long long shared_key_server = generate_shared_key(client_pub_key, server_private_key, P);
    ENC_KEY = shared_key_server;
    
    
    USER user;
    string message;
    ServerState state = AWAIT_CHOICE;

    while(true){

        memset(buf, 0, sizeof(buf));

        switch (state){

            case AWAIT_CHOICE:
            {
                string menu = "\n => [1] NEW TO CHAT? REGISTER NOW.\n => [2] OLD ONE? LOGIN.\n => [3] {ADMIN} Super_User?\n => [0] EXIT. \n";
                menu = ENCRYPT(menu);
                send(client_socket, menu.c_str(), menu.size(), 0);
                memset(buf, 0, sizeof(buf));

                int bytes_received = recv(client_socket, buf, sizeof(buf), 0);
                if (bytes_received <= 0) {
                    cout<<ANSI_COLOR_BOLD_RED<< "\n !! CLIENT DISCONNECTED !! "<<ANSI_COLOR_RESET<< endl;
                    return;
                }

                int choice = atoi(buf);
                if(choice == 1){
                    state = REGISTER;
                }
                else if (choice == 2){
                    state = LOGIN;
                }
                else if (choice == 3){
                    state = SUDOER;
                }
                else if (choice == 0){
                    state = EXIT_IT;
                }
                else{

                    message = "\n INVALID CHOICE. PLEASE TRY AGAIN. \n";
                    message = ENCRYPT(message);
                    send(client_socket, message.c_str(), message.size(), 0);
                    memset(buf, 0, sizeof(buf));
                }
                break;
            }
            case REGISTER:
            {

                cout<<ANSI_COLOR_BOLD_GREEN<<"\n NEW USER TRYING TO REGISTER. "<<ANSI_COLOR_RESET<<endl;
                string name, email, pass, confirm_pass;

                do {
                    message = " ENTER THE USERNAME = ";
                    message = ENCRYPT(message);
                    send(client_socket, message.c_str(), message.size(), 0);
                    memset(buf, 0, sizeof(buf));
                    sleep(1);
                    recv(client_socket, buf, sizeof(buf), 0);
                    name = string(buf).substr(0, string(buf).find('\n'));
                    name = ENCRYPT(name);

                    if(user.User_Exists(name)){
                        message = "\nUSER NAME ALREADY EXISTS. TRY ANOTHER ONE.\n";
                        message = ENCRYPT(message);
                        send(client_socket, message.c_str(), message.size(), 0);
                        memset(buf, 0, sizeof(buf));
                    }
                    else{
                        break;
                    }

                }while(true);

                do{
                    message = " ENTER YOUR EMAIL [A VALID ONE WITH @ & .] ";
                    message = ENCRYPT(message);
                    send(client_socket, message.c_str(), message.size(), 0);
                    memset(buf, 0, sizeof(buf));
                    sleep(1);
                    recv(client_socket, buf, sizeof(buf), 0);
                    email = string(buf).substr(0, string(buf).find('\n'));
                    email = ENCRYPT(email);

                    if (!user.Valid_Email(email)) {
                        message = "\nINVALID EMAIL FORMAT. TRY AGAIN.\n";
                        message = ENCRYPT(message);
                        send(client_socket, message.c_str(), message.size(), 0);
                        memset(buf, 0, sizeof(buf));
                        sleep(1);
                    } else {
                        break;
                    }

                }while(true);

                do{
                    message = " ENTER YOUR PASSWORD = ";
                    message = ENCRYPT(message);
                    send(client_socket, message.c_str(), message.size(), 0);
                    memset(buf, 0, sizeof(buf));
                    sleep(1);
                    recv(client_socket, buf, sizeof(buf), 0);
                    pass = string(buf).substr(0, string(buf).find('\n'));
                    pass = ENCRYPT(pass);

                    if (!user.Strong_Password(pass)) {
                        message = " WEAK PASSWORD! [A-Z] - [a-z] - [0-9] - [special] ";
                        message = ENCRYPT(message);
                        send(client_socket, message.c_str(), message.size(), 0);
                        memset(buf, 0, sizeof(buf));
                        sleep(1);
                    } else {
                        break;
                    }
                }while(true);

                ofstream w("creds.txt", ios::app);
                w <<name<<" "<<email<<" "<<pass << endl;
                w.close();

                string salt = generate_salt(16);
                string hashed_password = hash_password(pass, salt);

                ofstream write("updated_creds.txt", ios::app);
                write << name << " " << email << " " << salt << " " << hashed_password << endl;
                write.close();

                message = "\nREGISTRATION COMPLETED SUCCESSFULLY.\n";
                cout<<ANSI_COLOR_BOLD_GREEN<<"\n [ NEW USER REGISTERED SUCCESSFULLY ] "<<ANSI_COLOR_RESET<<endl;
                message = ENCRYPT(message);
                send(client_socket, message.c_str(), message.size(), 0);
                memset(buf, 0, sizeof(buf));
                sleep(1);

                state = AWAIT_CHOICE;
                break;
            }
            case LOGIN:
            {
                cout<<ANSI_COLOR_BOLD_GREEN<<"\n [ OLD USER TRYING TO LOGIN ] "<<ANSI_COLOR_RESET<<endl;
                message = "\n ENTER YOUR USER NAME = ";
                message = ENCRYPT(message);
                send(client_socket, message.c_str(), message.size(), 0);
                memset(buf, 0, sizeof(buf));
                sleep(1);
                recv(client_socket, buf, sizeof(buf), 0);
                User_Name = string(buf).substr(0, string(buf).find('\n'));
                User_Name = ENCRYPT(User_Name);

                message = "\n ENTER YOUR PASSWORD = ";
                message = ENCRYPT(message);
                send(client_socket, message.c_str(), message.size(), 0);
                memset(buf, 0, sizeof(buf));
                sleep(1);
                recv(client_socket, buf, sizeof(buf), 0);
                Pass_Word = string(buf).substr(0, string(buf).find('\n'));
                Pass_Word = ENCRYPT(Pass_Word);

                ifstream read("updated_creds.txt");
                string name_check, email, salt, hashed_password;
                bool user_found = false;

                while (read >> name_check >> email >> salt >> hashed_password){

                    if(name_check == User_Name){
                        user_found = true;

                        if (hashed_password == hash_password(Pass_Word, salt)){
                            cout << "\n LISTENING CLIENT AT PORT [8080] \n";
                            message = "\nLOGIN SUCCESSFUL! YOU CAN NOW CHAT WITH THE SERVER. TYPE [exit] TO QUIT.\n";
                            message = ENCRYPT(message);
                            send(client_socket, message.c_str(), message.size(), 0);
                            state = CHAT;
                        }
                        else{
                            counter++;
                            message = "\nLOGIN FAILED. INVALID PASSWORD.\n";
                            cout<<ANSI_COLOR_BOLD_RED<<"\n FAILED LOGIN ATTEPMT. COUNT = [ "<<counter<<" ] "<<ANSI_COLOR_RESET<<endl;
                            message = ENCRYPT(message);
                            send(client_socket, message.c_str(), message.size(), 0);
                            memset(buf, 0, sizeof(buf));
                            state = AWAIT_CHOICE;
                            sleep(1);
                        }
                        break;
                    }
                }

                if(!user_found){

                    counter++;
                    cout<<ANSI_COLOR_BOLD_RED<<"\n FAILED LOGIN ATTEPMT. COUNT = [ "<<counter<<" ] "<<ANSI_COLOR_RESET<<endl;
                    message = "\nLOGIN FAILED. USERNAME DOES NOT EXIST.\n";
                    message = ENCRYPT(message);
                    send(client_socket, message.c_str(), message.size(), 0);
                    memset(buf, 0, sizeof(buf));
                    state = AWAIT_CHOICE;
                    sleep(1);
                }
                break;
            }
            case SUDOER:
            {
                cout<<ANSI_COLOR_BOLD_RED<<"\n [ CLIENT TRYING TO ENTER ADMIN MODE ]"<<ANSI_COLOR_RESET<<endl;
                string name, pass;

                message = "\n ENTER YOUR USER NAME = ";
                message = ENCRYPT(message);
                send(client_socket, message.c_str(), message.size(), 0);
                memset(buf, 0, sizeof(buf));
                sleep(1);
                recv(client_socket, buf, sizeof(buf), 0);
                name = string(buf).substr(0, string(buf).find('\n'));
                name = ENCRYPT(name);

                message = "\n ENTER YOUR PASSWORD = ";
                message = ENCRYPT(message);
                send(client_socket, message.c_str(), message.size(), 0);
                memset(buf, 0, sizeof(buf));
                sleep(1);
                recv(client_socket, buf, sizeof(buf), 0);
                pass = string(buf).substr(0, string(buf).find('\n'));
                pass = ENCRYPT(pass);

                ifstream read("admin_pass.txt");
                string name_check, email, salt, hashed_password;
                bool user_found = false;

                while(read >> name_check >> email >> salt >> hashed_password){

                    if(name_check == name){
                        user_found = true;

                        if (hashed_password == hash_password(pass, salt)){
                            cout<<ANSI_COLOR_BOLD_GREEN<<"\n [ GRANTED ROOT PERMISSIONS TO ADMIN. ]"<<ANSI_COLOR_RESET<<endl;
                            message = "\nLOGIN SUCCESSFUL \n";
                            message = ENCRYPT(message);
                            send(client_socket, message.c_str(), message.size(), 0);
                            state = AWAIT_CHOICE;
                        }
                        else{

                            cout<<ANSI_COLOR_BOLD_RED<<"\n [ CLIENT NOT ALLOWED TO ENTER ADMIN MODE ]"<<ANSI_COLOR_RESET<<endl;
                            message = "\nLOGIN FAILED. INVALID PASSWORD.\n";
                            message = ENCRYPT(message);
                            send(client_socket, message.c_str(), message.size(), 0);
                            memset(buf, 0, sizeof(buf));
                            state = AWAIT_CHOICE;
                            sleep(1);
                        }
                        break;
                    }
                }

                if (!user_found){
                    cout<<ANSI_COLOR_BOLD_RED<<"\n [ CLIENT NOT ALLOWED TO ENTER ADMIN MODE ]"<<ANSI_COLOR_RESET<<endl;
                    message = "\nLOGIN FAILED. YOU ARE NOT THE ADMIN. \n";
                    message = ENCRYPT(message);
                    send(client_socket, message.c_str(), message.size(), 0);
                    memset(buf, 0, sizeof(buf));
                    state = AWAIT_CHOICE;
                    sleep(1);
                }
                state = AWAIT_CHOICE;
                break;
            }
           case CHAT:
           {
                flag = true;

                // APPEND KEY WHICH WE CALCULATED RANDOMALY.
                string aes_key = User_Name + to_string(APPEND_KEY);

                // FORMAT KEY TO 16 BIT STRING
                SHARED_KEY = format_key(aes_key);

                while(true){

                    char buf[1024];
                    memset(buf, 0, sizeof(buf));

                    
                    int bytes_received = recv(client_socket, buf, sizeof(buf), 0);
                    if (bytes_received <= 0 || string(buf) == "exit\n") {
                        cout<<ANSI_COLOR_BOLD_RED<< "\n !! CLIENT EXITED THE CHAT !! "<< endl;
                    }
                   
                    string encrypted_message(buf, bytes_received);

                    // DECODE MESSAGE
                    string decrypted_message = AES_decrypt(encrypted_message, SHARED_KEY);
                    cout<<ANSI_COLOR_BOLD_GREEN<< "\n[CLIENT] => " << decrypted_message;

                    string send_message;
                    cout << "\n[SERVER] => ";
                    getline(cin, send_message);

                    // ENCRYPT RESPONSE
                    string encrypted_reply = AES_encrypt(send_message, SHARED_KEY);
                    send(client_socket, encrypted_reply.c_str(), encrypted_reply.size(), 0);
                }
            }
            case EXIT_IT:
            {
                cout<<ANSI_COLOR_BOLD_RED<<"\n [ CLIENT TRYING TO EXIT THE CHAT ]"<<ANSI_COLOR_RESET<<endl;
                message = "\n[CLIENT EXITED THE CHAT]\n";
                message = ENCRYPT(message);
                send(client_socket, message.c_str(), message.size(), 0);
                memset(buf, 0, sizeof(buf));
                state = AWAIT_CHOICE;
                break;
            }

            sleep(1);
        }
    }

    close(client_socket);

}

int main(){
    
    HASH_CHAT();

    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8080);
    server_address.sin_addr.s_addr = INADDR_ANY;

    bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address));
    listen(server_socket, 5);

    while(true){

        int client_socket = accept(server_socket, NULL, NULL);
        
        pid_t new_pid = fork();
        if(new_pid == 0){
            handle_client(client_socket);
            exit(0);
        }
        else{
            close(client_socket);
        }
    }

    close(server_socket);

return 0;
}
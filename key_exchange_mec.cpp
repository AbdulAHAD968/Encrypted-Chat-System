#include<iostream>  
#include<cmath>  

using namespace std;  


//** SERVER CLASS. */
class SERVER {  
    private:  
        int private_key = 2;  
        int server_public_key;  
        int shared_key;  

    public:  
        int big_prime = 17, alpha = 2;  

        // GETTER FOR BIG PRIME.  
        int GET_BIG_PRIME() {  
            return big_prime;  
        }  

        // GETTER FOR ALPHA.  
        int GET_ALPHA() {  
            return alpha;  
        }  

        // CALCULATE THE PUBLIC KEY.  
        int calculate_public_key() {  
            int public_key = static_cast<int>((pow(alpha, private_key))) % big_prime;  
            return public_key;  
        }  

        // SERVER WILL SEND HIS CALCULATED PUBLIC KEY TO CLIENT.  
        int send_public_key() {  
            return calculate_public_key();  
        }  

        // SERVER WILL RECEIVE PUBLIC KEY OF CLIENT, AND THEN GET THE SHARED KEY USING HIS PRIVATE KEY.  
        void receive_public_key(int key) {  
            this->server_public_key = key;
            this->shared_key = static_cast<int>(pow(server_public_key, private_key)) % big_prime;  
        }  
 
        int get_shared_key() {  
            return shared_key;  
        }  
};  

//** CLIENT CLASS */ 
class CLIENT {  
    private:  
        int private_key = 3;  
        int server_public_key;  
        int shared_key;  

    public:  
        int big_prime, alpha;  

        // CONSTRUCTOR TO SET THE BIG_PRIME AND ALPHA.  
        CLIENT(int r = 0, int s = 0) {  
            this->big_prime = r;  
            this->alpha = s;  
        }  

        // CALCULATE THE PUBLIC KEY.  
        int calculate_public_key() {  
            int public_key = static_cast<int>((pow(alpha, private_key))) % big_prime;  
            return public_key;  
        }  

        // CLIENT WILL SEND PUBLIC KEY TO SERVER.  
        int send_public_key() {  
            return calculate_public_key();  
        }  

        // CLIENT WILL RECEIVE PUBLIC KEY OF SERVER, AND THEN GET THE SHARED KEY USING HIS PRIVATE KEY.  
        void receive_public_key(int key) {  
            this->server_public_key = key;
            this->shared_key = static_cast<int>(pow(server_public_key, private_key)) % big_prime;  
        }  

        // GET THE SHARED KEY. 
        int get_shared_key() {  
            return shared_key;  
        }  
};  

// ** MAIN FUNCTION **  
int main() {  

    //** CREATING A SERVER AND CLIENT. */ 
    SERVER server;

    // STEP_01 : SERVER SEND BIG PRIME AND ALPHA TO THE CLIENT
    CLIENT client(server.GET_BIG_PRIME(), server.GET_ALPHA());  

    // STEP_01_B : SERVER SENDS PUBLIC KEY TO CLIENT. 
    int server_public_key = server.send_public_key();  
    cout<<"\n SERVER PUBLIC KEY = "<<server_public_key<<endl;  

    // STEP_02: CLIENT RECIEVES THE SERVER PUBLIC KEY.  
    client.receive_public_key(server_public_key);
    int client_public_key = client.send_public_key();
    cout<<"\n CLIENT PUBLIC KEY = "<<client_public_key<<endl;  

    // STEP_03: SERVER RECIEVES THE CLIENT PUBLIC KEY. 
    server.receive_public_key(client_public_key);  
    
    // SHOW THE SHARED KEY.
    cout<<"\n SERVER'S SHARED KEY = [ "<<server.get_shared_key()<<" ] \n";  
    cout<<"\n CLIENT'S SHARED KEY = [ "<<client.get_shared_key()<<" ] \n";  

    return 0;  
}
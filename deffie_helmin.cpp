#include <iostream>  
#include <cmath>  

using namespace std;

int set_key;


//* FUNCTION TO SEE IF THE PASSED NUMBER IS PRIME OR NOT.* //
bool is_prime(int n){

    if(n < 2){
        return false;
    }
    else{
        for(int i=2 ; i<=sqrt(n) ; i++){  
            if(n % i == 0){
                return false;  
            }  
        }  
    return true;
    }  
}  


bool key_found = false;  


//** FIND THE KEY COMBINATIONS [BIG_P, ALHPA, a, b, PKA, PKB] */  
void find_shared_key(int P) {  
    // Try different prime numbers P  
    for (; P < 1000; ++P) {  
        if (!is_prime(P)) {  
            continue;  
        }  

        // Try different values for the base alpha  
        for (int alpha = 2; alpha < P; alpha++){

            // Check if alpha is a primitive root modulo P
            if(!is_prime(alpha)){
                continue;
            }

            // Try different values for Alice's private key a  
            for(int a=1 ; a<P ; a++){

                int A = static_cast<int>(pow(alpha, a)) % P;                // Alice's PUBLIC KEY.

                // Try different values for Bob's private key b  
                for(int b = 1; b < P; b++){

                    int B = static_cast<int>(pow(alpha, b)) % P;            // Bob's PUBLIC KEY. 

                    // CALCULATE THE SHARED KEYS. 
                    int shared_key_alice = static_cast<int>(pow(B, a)) % P;  
                    int shared_key_bob = static_cast<int>(pow(A, b)) % P;  

                    if (shared_key_alice == set_key && shared_key_alice == shared_key_bob) {  
                        cout<<"\nLARGE PRIME P = " << P << endl;
                        cout<<"ALPHA = " << alpha << endl;
                        cout<<"ALICE KI PRIVATE KEY= " << a << endl;  
                        cout<<"BOB KI PRIVATE KEY= " << b << endl;  
                        cout<<"Shared key = " << shared_key_alice << endl;  
                        key_found = true;  
                        return;  
                    }

                } // Bob's private key loop

            } // Alice's private key loop  
        
        } // ALPHA LOOP

    } // OUTER LOOP TILL WE GET THE POSSIBLE COMBINATIONS FOR THE SHARED KEY.

    //cout << "\nNO MATCH FOUND FOR 13\n" << endl;  
}  


//** MAIN FUNCTION. */
int main(){

    cout<<"\n ENTER THE SHARED KEY. [FOR WHICH YOU WANT TO PERFORM BRUTE FORCE ATTACK] \n";
    cin>>set_key;

    for(int i=0 ; i<1000 ; i++){ 
        find_shared_key(i);  
        if(key_found){
            break;  
        }  
    }  

return 0;  
}
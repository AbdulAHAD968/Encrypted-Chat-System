# 🔒 Secure Chat System

This project is a **Secure Chat System** built in C++ that provides robust user authentication and encrypted communication between a client and a server. The system ensures the confidentiality, integrity, and security of user credentials and messages. 

## 🚀 Features

- **User Registration**: Users can register with a unique username and password.
- **User Login**: Authentication is done using SHA-256 hashed passwords.
- **Diffie-Hellman Key Exchange**: Secure key exchange mechanism for shared secret key generation.
- **AES-128 Encryption**: All messages exchanged between the client and server are encrypted using AES in CBC mode.
- **Credential Storage**: User credentials (username, email, and hashed password with a salt) are stored securely in `creds.txt`.
- **Super User (Admin) Functionality**: An admin can add, remove, update, and view the credentials of registered users.

## 📂 Project Structure

- `Client`: Responsible for user registration, login, and sending encrypted chat messages.
- `Server`: Handles registration requests, login authentication, and facilitates encrypted message exchange between clients.

## 🛡️ Security Implementation

- **Password Hashing**: Passwords are hashed using SHA-256 with a unique salt for each user.
- **Key Exchange**: Diffie-Hellman key exchange protocol is used to establish a secure shared key.
- **Encryption**: AES-128 bit CBC mode is used for encrypting user credentials and chat messages.
- **Salting**: Each password is salted with a random 32-bit value to prevent rainbow table attacks.

## 🛠️ How it Works

### Registration Workflow
1. The client sends a registration request to the server.
2. The server prompts the client for an email, username, and password.
3. A mutual key is exchanged using Diffie-Hellman.
4. The user's credentials (email, username, password) are encrypted using AES with the mutual key.
5. The server decrypts the registration request and checks for username uniqueness.
6. If the username is unique, the password is hashed with a random salt and stored in `creds.txt` along with the username and email.

### Login Workflow
1. The client sends a login request to the server.
2. The server prompts the user for username and password.
3. Another Diffie-Hellman key exchange is performed for mutual key generation.
4. The credentials are encrypted with AES and sent to the server for verification.
5. The server decrypts the message and compares the stored hashed password with the user's entered password.

### Encrypted Chat
- After login, the client and server exchange a new key using Diffie-Hellman.
- The shared key, combined with the username, is used for encrypting messages with AES-128.
- The client sends encrypted messages, and the server decrypts and replies with encrypted responses.

## 👨‍💻 Super User (Admin) Features
- **Add User**: Admin can manually register a new user.
- **Remove User**: Admin can delete a registered user from `creds.txt`.
- **Update Credentials**: Admin can update user credentials.
- **View Credentials**: Admin can view the list of all registered users and their credentials.

## 📦 File Structure

- `updated_creds.txt`: Stores user credentials in the format: `email, username, hashed password, salt`.
- `creds.txt`: Stores user credentials in the format: `email, username, text password`.
- `Client.cpp`: Contains the logic for user interaction, registration, login, and encrypted message exchange.
- `Server.cpp`: Manages user credentials, verifies login details, and facilitates encrypted communication.
- `HEADER.h`: COntains all the libraries that were required for smooth workflow.
- `ADMIN.h`: Contains all the admin functions like: `Remove user, Edit User, See User data, etc`.
- `USER.h`: Contains all the user functions like: `Register, Login, and Chatting`.
- `key_exchange.cpp`: Throgh classes visualize how the things work in Deffie Helmin Key Exchange Mechanism.
- `deffie_helmin.cpp`: This was additional code. Here you can enter the final shared key and brute force the P, alpha, a, b, and both the public keys for that shared key.

## 🔧 Technologies Used
- **C++**
- **SHA-256 for password hashing**
- **AES-128 bit CBC mode for encryption**
- **Diffie-Hellman for secure key exchange**

## 📜 Future Enhancements
- Implementing time-based key expiration for additional security.
- Encrypt `creds.txt` for even more secure storage like using a tool called crypter.
- Adding more detailed logging and error-handling mechanisms.

## 🛠️ Setup and Usage

1. Clone this repository:
   ```bash
   git clone https://github.com/yourusername/SecureChatSystem.git

2. Command for execution:
   ```bash
   g++ server.cpp -I/usr/local/ssl/include -L/usr/local/ssl/lib -lssl -lcrypto
   g++ client.cpp -I/usr/local/ssl/include -L/usr/local/ssl/lib -lssl -lcrypto

3. Additional Notes:
   Please compile the server first so it will be up for listening otherwise the client alone would not be able to make a connection solely.

## ⚠️ Errors and Omissions

If you encounter any errors, bugs, or have suggestions for improvements, feel free to reach out to me.

You can contact me via email at: [ahad06074@gmail.com](mailto:ahad06074@gmail.com)

Errors and omissions are accepted!


// #include <iostream>
// #include <fstream>
// #include <string>
// #include <map>
// #include <sstream>
// #include <cstdlib>
// #include <ctime>
// #include <cstring>
// #include <netinet/in.h>
// #include <unistd.h>

// std::map<std::string, std::pair<std::string, std::string> > sessions;

// std::string generateSessionID() {
//     const char* chars = "0123456789abcdef"; // taza session ID petqa genereacnel parloic u loginic kaxvac
//     std::string id;
//     srand(time(0) + rand());
//     for (int i = 0; i < 32; ++i) {
//         id += chars[rand() % 16];
//     }
//     return id;
// }

// void parseFormData(const std::string& body, std::string& login, std::string& password) {
//     size_t lpos = body.find("login=");
//     size_t ppos = body.find("&password=");
//     if (lpos != std::string::npos && ppos != std::string::npos) {
//         login = body.substr(lpos + 6, ppos - (lpos + 6));
//         password = body.substr(ppos + 10);
//     }
// }

// void saveSessions() {
//     std::ofstream out("sessions.txt");
//     std::map<std::string, std::pair<std::string, std::string> >::iterator it;
//     for (it = sessions.begin(); it != sessions.end(); ++it) {
//         out << it->first << " " << it->second.first << " " << it->second.second << "\n";
//     }
//     out.close();
// }

// void loadSessions() {
//     std::ifstream in("sessions.txt");
//     std::string id, login, password;
//     while (in >> id >> login >> password) {
//         sessions[id] = std::make_pair(login, password);
//     }
//     in.close();
// }

// void handleClient(int clientSocket) {
//     char buffer[4096];
//     memset(buffer, 0, sizeof(buffer));
//     read(clientSocket, buffer, sizeof(buffer) - 1);

//     std::string request(buffer);
//     std::string responseBody;
//     std::ostringstream response;

//     if (request.find("POST /login") == 0) {
//         size_t bodyPos = request.find("\r\n\r\n");
//         std::string body = request.substr(bodyPos + 4);
//         std::string login, password;
//         parseFormData(body, login, password);// passwordi heshavorum

//         std::string sessionID = generateSessionID();
//         sessions[sessionID] = std::make_pair(login, password);
//         saveSessions();

//         responseBody = "<html><body>Logged in as: " + login + "</body></html>";
//         response << "HTTP/1.1 200 OK\r\n"
//                  << "Content-Type: text/html\r\n"
//                  << "Set-Cookie: session_id=" << sessionID << "; Path=/; HttpOnly\r\n"
//                  << "Content-Length: " << responseBody.length() << "\r\n"
//                  << "\r\n"
//                  << responseBody;

//     } else {
//         // Проверка Cookie
//         size_t cookiePos = request.find("Cookie: ");
//         std::string sessionID;
//         if (cookiePos != std::string::npos) {
//             size_t endLine = request.find("\r\n", cookiePos);
//             std::string cookieLine = request.substr(cookiePos + 8, endLine - cookiePos - 8);
//             std::istringstream ss(cookieLine);
//             std::string token;
//             while (getline(ss, token, ';')) {
//                 size_t eq = token.find('=');
//                 if (eq != std::string::npos) {
//                     std::string key = token.substr(0, eq);
//                     std::string value = token.substr(eq + 1);
//                     while (!key.empty() && key[0] == ' ') key.erase(0, 1);
//                     if (key == "session_id") sessionID = value;
//                 }
//             }
//         }

//         if (!sessionID.empty() && sessions.count(sessionID)) {
//             std::string login = sessions[sessionID].first;
//             std::string password = sessions[sessionID].second;
//             responseBody = "<html><body>Welcome back, " + login + "! Your password is: " + password + "</body></html>";
//         } else {
//             responseBody =
//                 "<html><body>"
//                 "<h2>Login</h2>"
//                 "<form method='POST' action='/login'>"
//                 "Login: <input name='login'><br>"
//                 "Password: <input name='password' type='password'><br>"
//                 "<input type='submit' value='Login'>"
//                 "</form>"
//                 "</body></html>";
//         }

//         response << "HTTP/1.1 200 OK\r\n"
//                  << "Content-Type: text/html\r\n"
//                  << "Content-Length: " << responseBody.length() << "\r\n"
//                  << "\r\n"
//                  << responseBody;
//     }

//     send(clientSocket, response.str().c_str(), response.str().size(), 0);
//     close(clientSocket);
// }

// int main() {
//     loadSessions();

//     int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

//     sockaddr_in serverAddr;
//     serverAddr.sin_family = AF_INET;
//     serverAddr.sin_port = htons(8080);
//     serverAddr.sin_addr.s_addr = INADDR_ANY;
//     memset(&(serverAddr.sin_zero), '\0', 8);

//     bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
//     listen(serverSocket, 5);

//     std::cout << "Server running on http://localhost:8080\n";

//     while (true) {
//         sockaddr_in clientAddr;
//         socklen_t clientSize = sizeof(clientAddr);
//         int clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientSize);
//         handleClient(clientSocket);
//     }

//     close(serverSocket);
//     return 0;
// }

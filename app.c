#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 5000
#define BUFFER_SIZE 8192

// Protocol settings struct analogous to PROTOCOLS dictionary in app.py
typedef struct {
    const char *id;
    const char *name;
    float exec_time;
    const char *explanation;
    const char *status;
    const char *script;
} Protocol;

Protocol PROTOCOLS[] = {
    {"no", "No Protocol", 5.0, "Priority inversion occurs. High-priority task is delayed significantly because a lower-priority task holds the required resource.", "Inversion Detected", "scripts\\sim_no.exe"},
    {"pip", "Priority Inheritance Protocol", 3.0, "Priority inheritance reduces delay. Task L temporarily inherits Task H's priority, preventing Task M from preempting.", "Warning", "scripts\\sim_pip.exe"},
    {"pcp", "Priority Ceiling Protocol", 1.5, "Priority ceiling prevents inversion completely. System dynamically adjusts priority limits to guarantee safe preemption without deadlocks.", "Success", "scripts\\sim_pcp.exe"}
};

// Helper function to find a protocol by its ID
Protocol* get_protocol(const char* id) {
    for (int i = 0; i < 3; i++) {
        if (strcmp(PROTOCOLS[i].id, id) == 0) {
            return &PROTOCOLS[i];
        }
    }
    return NULL;
}

char* read_file(const char* filepath) {
    FILE *file = fopen(filepath, "rb");
    if (!file) return NULL;
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *content = (char*)malloc(size + 1);
    if (!content) {
        fclose(file);
        return NULL;
    }
    fread(content, 1, size, file);
    content[size] = '\0';
    fclose(file);
    return content;
}

// Function to send a basic HTTP response
void send_response(SOCKET client_socket, const char *status_code, const char *content_type, const char *body) {
    char response_header[1024];
    snprintf(response_header, sizeof(response_header),
             "HTTP/1.1 %s\r\n"
             "Content-Type: %s\r\n"
             "Connection: close\r\n"
             "Content-Length: %zu\r\n\r\n",
             status_code, content_type, strlen(body));
    send(client_socket, response_header, strlen(response_header), 0);
    send(client_socket, body, strlen(body), 0);
}

void send_file_response(SOCKET client_socket, const char *status_code, const char *content_type, const char *filepath) {
    char* body = read_file(filepath);
    if (body) {
        send_response(client_socket, status_code, content_type, body);
        free(body);
    } else {
        send_response(client_socket, "404 Not Found", "text/plain", "File Not Found");
    }
}

void send_sse_stream(SOCKET client_socket, Protocol* p) {
    char header[] = "HTTP/1.1 200 OK\r\n"
                    "Content-Type: text/event-stream\r\n"
                    "Cache-Control: no-cache\r\n"
                    "Connection: keep-alive\r\n\r\n";
    send(client_socket, header, strlen(header), 0);
    
    // Send init
    char buf[1024];
    snprintf(buf, sizeof(buf), "data: {\"type\": \"init\", \"name\": \"%s\"}\n\n", p->name);
    send(client_socket, buf, strlen(buf), 0);
    
    // Execute subprocess using _popen
    FILE *fp = _popen(p->script, "r");
    if (fp) {
        char line[512];
        while (fgets(line, sizeof(line), fp)) {
            line[strcspn(line, "\r\n")] = 0; // strip newline
            snprintf(buf, sizeof(buf), "data: {\"type\": \"log\", \"data\": \"%s\"}\n\n", line);
            send(client_socket, buf, strlen(buf), 0);
        }
        _pclose(fp);
    } else {
        snprintf(buf, sizeof(buf), "data: {\"type\": \"log\", \"data\": \"Error: Could not execute subprocess.\"}\n\n");
        send(client_socket, buf, strlen(buf), 0);
    }
    
    // Send complete
    snprintf(buf, sizeof(buf), "data: {\"type\": \"complete\", \"status\": \"%s\", \"name\": \"%s\", \"time\": \"%.1f\", \"explanation\": \"%s\", \"protocol_key\": \"%s\"}\n\n",
             p->status, p->name, p->exec_time, p->explanation, p->id);
    send(client_socket, buf, strlen(buf), 0);
}

// Basic routing logic representing Flask's @app.route
void handle_client(SOCKET client_socket) {
    char buffer[BUFFER_SIZE];
    int bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
    
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0';
        
        char method[16], path[256];
        if (sscanf(buffer, "%15s %255s", method, path) != 2) {
            closesocket(client_socket);
            return;
        }

        // Routing Logic
        if (strcmp(path, "/") == 0) {
            send_file_response(client_socket, "200 OK", "text/html", "templates/index.html");
        } 
        else if (strcmp(path, "/style.css") == 0 || strcmp(path, "/static/style.css") == 0) {
            send_file_response(client_socket, "200 OK", "text/css", "static/style.css");
        }
        else if (strncmp(path, "/run/", 5) == 0) {
            const char* protocol_id = path + 5;
            Protocol* p = get_protocol(protocol_id);
            if (p) {
                send_sse_stream(client_socket, p);
            } else {
                send_response(client_socket, "400 Bad Request", "application/json", "{\"error\": \"Invalid protocol\"}");
            }
        }
        else if (strcmp(path, "/comparison") == 0) {
            send_file_response(client_socket, "200 OK", "text/html", "templates/comparison.html");
        }
        else if (strcmp(path, "/about") == 0) {
            send_file_response(client_socket, "200 OK", "text/html", "templates/about.html");
        }
        else if (strncmp(path, "/results", 8) == 0) {
            send_file_response(client_socket, "200 OK", "text/html", "templates/results.html");
        }
        else if (strncmp(path, "/download", 9) == 0) {
            char content[1024];
            snprintf(content, sizeof(content), "--- Real-Time Scheduling System Results ---\nResults downloaded via C HTTP server.\n");
            send_response(client_socket, "200 OK", "text/plain", content);
        }
        else {
            send_response(client_socket, "404 Not Found", "text/plain", "404 Page Not Found");
        }
    }
    closesocket(client_socket);
}

int main() {
    WSADATA wsaData;
    SOCKET server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    int client_addr_len = sizeof(client_addr);

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed.\n");
        return 1;
    }

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET) {
        printf("Socket creation failed.\n");
        WSACleanup();
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    int opt = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(opt)) < 0) {
        printf("setsockopt(SO_REUSEADDR) failed.\n");
    }

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Bind failed.\n");
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    if (listen(server_socket, SOMAXCONN) == SOCKET_ERROR) {
        printf("Listen failed.\n");
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    printf("C Web Server running on http://127.0.0.1:%d\n", PORT);
    printf("Serving real HTML files and executing C simulations dynamically!\n");
    printf("Press Ctrl+C to stop the server.\n");

    while (1) {
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_socket != INVALID_SOCKET) {
            handle_client(client_socket);
        }
    }

    closesocket(server_socket);
    WSACleanup();
    return 0;
}

#include <gtkmm.h>
#include <gtkmm/application.h>
#include <gtkmm/builder.h>

#include <glm/glm.hpp>

#include <iostream>
#include <thread>
#include <mutex>

#include <sys/types.h>
#ifdef __WIN32__
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#endif

#include "graphics.cpp"

#define PORT 53458

#define ANS_OK 0
#define ANS_INVALID_COM 255

class Vertecies{
public:
    const std::vector<uint16_t> ind={
        0, 1, 2, 2, 3, 0,
        3, 2, 7, 7, 2, 6,
        2, 1, 6, 6, 1, 5,
        1, 0, 5, 5, 0, 4,
        0, 3, 4, 4, 3, 7,
        4, 7, 6, 6, 5, 4
    };

    std::vector<Vertex> vertices;
    std::vector<uint16_t> indices;
};

class Console{
public:
    Vertecies verts;
    Glib::RefPtr<Gtk::Application> app;

    void run(int argc, char** argv){
        #ifdef __WIN32__
        WORD versionWanted = MAKEWORD(1, 1);
        WSADATA wsaData;
        WSAStartup(versionWanted, &wsaData);
        #endif

        int sock_udp, sock_tcp;
        struct sockaddr_in servaddr;
      
        if ((sock_udp = socket(AF_INET, SOCK_DGRAM, 0)) < 0 )
            throw std::runtime_error("failed to create udp socket!");
      
        if ((sock_tcp = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
            throw std::runtime_error("failed to create tcp socket!");

        memset(&servaddr, 0, sizeof(servaddr));
        servaddr.sin_family = AF_INET; 
        servaddr.sin_addr.s_addr = INADDR_ANY;
        servaddr.sin_port = htons(PORT);

        if (bind(sock_udp, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0 )
            throw std::runtime_error("failed to bind udp socket!");

        if (bind(sock_tcp, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0 )
            throw std::runtime_error("failed to bind tcp socket!");
        if (listen(sock_tcp, 10) < 0)
            throw std::runtime_error("failed to start listen tcp socket!");

        std::thread t_udp(serverUDP, sock_udp);
        std::thread t_tcp(serverTCP, sock_tcp);

        auto v = readMap();
        verts.vertices = v.vertices;
        verts.indices = v.indices;
        app = Gtk::Application::create(argc, argv, "org.vr.example");
        app->signal_activate().connect(sigc::mem_fun(this, &Console::activate));
        app->run();
        close(sock_udp);
        close(sock_tcp);
    }
private:
    static void serverUDP(int socket){
        struct sockaddr_in cliaddr;
        char buffer[1024], answer[1024];
        int n, len = sizeof(cliaddr);
        while(true){
            n = recvfrom(socket, (char *)buffer, 1024, 
                0, (struct sockaddr*)&cliaddr, &len);
            switch (buffer[0]){
            case 0:
                answer[0] = 0;
                sendto(socket, (const char*)answer, 1, 
                    0, (const struct sockaddr*)&cliaddr, len);
                break;
            case 1:
                //handleUserDate(buffer, n);
                break;
            default:
                answer[0] = ANS_INVALID_COM;
                sendto(socket, (const char*)answer, 1, 
                    0, (const struct sockaddr*)&cliaddr, len);
                break;
            }
        }
    }
    static void serverTCP(int socket){
        struct sockaddr_in cliaddr;
        int len = sizeof(cliaddr);
        while(true){
            int new_socket = accept(socket, (struct sockaddr*)&cliaddr, &len);
            std::thread t(listenUser, new_socket);
        }
    }
    static void listenUser(int sock){

    }

    void activate(){
        auto builder = Gtk::Builder::create();
        builder->add_from_file("main.glade");
        
        Gtk::Window* window = nullptr;
        builder->get_widget("window_main", window);
        if(!window)
            throw std::runtime_error("failed to find window");
        window->set_title("ВР");
        app->add_window(*window);

        Gtk::Button* b = nullptr;
        builder->get_widget("first", b);
        b->signal_clicked().connect(sigc::mem_fun(this, &Console::first_Func));
        // GObject *button = gtk_builder_get_object(builder, "first");
        // g_signal_connect(button, "clicked", G_CALLBACK(&first_Func), NULL);

        // button = gtk_builder_get_object(builder, "second");
        // g_signal_connect(button, "clicked", G_CALLBACK(&second_Func), NULL);

        window->show();
        //g_object_unref(builder);
    }

    void first_Func(){
        Graphic g;
        float x = 0;
        float y = 16;
        float z = 0;
        float ry = 0;
        float rz = 0;

        g.vertices = verts.vertices;
        g.indices = verts.indices;
        g.posX = &x;
        g.posY = &y;
        g.posZ = &z;
        g.angleY = &ry;
        g.angleZ = &rz;
        g.run();
    }

    void second_Func(GtkWidget *widget, gpointer data){
        g_print("Second");
    }

    Vertecies readMap(){
        std::ifstream file("main.map", std::ios::binary);
        if(!file.is_open())
            throw std::runtime_error("failed to open file!");
        
        char c;
        Vertecies v;
        for(int i = 0; i < 16; i++){
            for(int j = 0; j < 16; j++){
                for(int k = 0; k < 16; k++){
                    file.read(&c, 1);
                    if(c == 0)
                        continue;
                    glm::vec3 color;
                    switch(c){
                        case 1:
                            color = {0.429f, 0.245f, 0.003f};
                            break;
                        case 2:
                            color = {0.033f, 0.594f, 0.198f};
                            break;
                        case 3:
                            color = {0.625f, 0.378f, 0.241f};
                            break;
                        case 4:
                            color = {0.71f, 0.901f, 0.114f};
                            break;
                    }
                    int b = static_cast<int>(v.vertices.size());
                    v.vertices.resize(v.vertices.size() + 8);
                    v.vertices[b] = {{j * 1.0f, k * 1.0f, i * 1.0f}, color};
                    v.vertices[b + 1] = {{(j + 1) * 1.0f, k * 1.0f, i * 1.0f}, color};
                    v.vertices[b + 2] = {{(j + 1) * 1.0f, (k + 1) * 1.0f, i * 1.0f}, color};
                    v.vertices[b + 3] = {{j * 1.0f, (k + 1) * 1.0f, i * 1.0f}, color};
                    v.vertices[b + 4] = {{j * 1.0f, k * 1.0f, (i + 1) * 1.0f}, color};
                    v.vertices[b + 5] = {{(j + 1) * 1.0f, k * 1.0f, (i + 1) * 1.0f}, color};
                    v.vertices[b + 6] = {{(j + 1) * 1.0f, (k + 1) * 1.0f, (i + 1) * 1.0f}, color};
                    v.vertices[b + 7] = {{j * 1.0f, (k + 1) * 1.0f, (i + 1) * 1.0f}, color};

                    b = static_cast<int>(v.indices.size());
                    v.indices.resize(v.indices.size() + v.ind.size());
                    for(int o = 0; o < static_cast<int>(v.ind.size()); o++)
                        v.indices[b + o] = v.ind[o] + b / 36 * 8;
                }
            }
        }

        file.close();
        return v;
    }
};
#include <gtkmm.h>
#include "graphics.cpp"
#include <iostream>
#include <glm/glm.hpp>

using namespace std;

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
    static void run(int argc, char** argv){
        auto v = readMap();
        GtkApplication *app = gtk_application_new("org.gtk.example", G_APPLICATION_FLAGS_NONE);
        g_signal_connect(app, "activate", G_CALLBACK(&activate), &v);
        g_application_run(G_APPLICATION(app), argc, argv);
        g_object_unref(app);
    }
private:
    static void activate(GtkApplication *app, gpointer user_data){
        GtkBuilder *builder = gtk_builder_new();
        gtk_builder_add_from_file(builder, "123.glade", NULL);

        GObject *window = gtk_builder_get_object(builder, "window");
        gtk_window_set_application(GTK_WINDOW(window), app);
        gtk_window_set_title(GTK_WINDOW(window), "ВР");

        GObject *button = gtk_builder_get_object(builder, "first");
        g_signal_connect(button, "clicked", G_CALLBACK(&first_Func), user_data);

        button = gtk_builder_get_object(builder, "second");
        g_signal_connect(button, "clicked", G_CALLBACK(&second_Func), NULL);

        // button = gtk_builder_get_object (builder, "button2");
        // g_signal_connect (button, "clicked", G_CALLBACK (print_hello), NULL);

        // button = gtk_builder_get_object (builder, "quit");
        // g_signal_connect_swapped (button, "clicked", G_CALLBACK (quit_cb), window);

        gtk_widget_show(GTK_WIDGET(window));
        g_object_unref (builder);
    }

    static void first_Func(GtkWidget *widget, gpointer data){
        Graphic g;
        auto v = reinterpret_cast<Vertecies*>(data);
        g.vertices = v->vertices;
        g.indices = v->indices;
        g.run();
    }

    static void second_Func(GtkWidget *widget, gpointer data){
        g_print("Second");
    }

    static Vertecies readMap(){
        std::ifstream file("123.map", std::ios::binary);
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
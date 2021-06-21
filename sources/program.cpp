//#include "graphics.cpp"

#include "console.cpp"

int main(int argc, char** argv)
{
    Console c;
    try {
        c.run(argc, argv);
    }
    catch(const std::exception& e) {
        std::cout << e.what() << '\n';
    }

    // Graphic g;
    // try {
    //     g.run();
    // }
    // catch(const std::exception& e) {
    //     std::cout << e.what() << '\n';
    // }
    
    return 0;
}

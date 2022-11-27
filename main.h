#ifndef MAIN_H
#define MAIN_H

#define MAIN                                     \
int main()                                       \
{                                                \
    try {                                        \
        tinygl::init(3, 3);                      \
        Window window(512, 512, NAME, true);     \
        window.run();                            \
    } catch (const std::exception& e) {          \
        tinygl::terminate();                     \
        std::cerr << e.what() << std::endl;      \
        return EXIT_FAILURE;                     \
    }                                            \
                                                 \
    return EXIT_SUCCESS;                         \
}                                                \

#endif // MAIN_H

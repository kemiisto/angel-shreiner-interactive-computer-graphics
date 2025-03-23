#ifndef MAIN_H
#define MAIN_H

#define MAIN                                     \
int main()                                       \
{                                                \
    try {                                        \
        tinygl::init(3, 3);                      \
        window w(512, 512, NAME, true);          \
        w.run();                                 \
    } catch (const std::exception& e) {          \
        tinygl::terminate();                     \
        std::cerr << e.what() << std::endl;      \
        return EXIT_FAILURE;                     \
    }                                            \
                                                 \
    return EXIT_SUCCESS;                         \
}                                                \

#endif // MAIN_H

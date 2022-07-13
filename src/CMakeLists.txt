add_executable(main Lexer.cpp Token.cpp main.cpp)

target_include_directories(main PUBLIC ../include)

target_compile_features(main PUBLIC cxx_std_11)
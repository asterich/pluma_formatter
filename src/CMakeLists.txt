

add_executable(main main.cpp Lexer.cpp Symbol.cpp Parser.cpp Formatter.cpp)

target_include_directories(main PUBLIC ../include)

target_compile_features(main PUBLIC cxx_std_20)

target_compile_options(main PUBLIC "-O2")
add_library(hash utils/Hash.cpp)

target_include_directories(hash PRIVATE ../include/utils/)

target_compile_features(hash PUBLIC cxx_std_20)

target_compile_options(hash PUBLIC "-O2")

find_package(OpenSSL REQUIRED)

if(OPENSSL_FOUND)
    message("OpenSSL include dir: ${OPENSSL_INCLUDE_DIR}")
    message("OpenSSL libraries: ${OPENSSL_LIBRARIES}")

    target_include_directories(hash PUBLIC ${OPENSSL_INCLUDE_DIR})
    target_link_libraries(hash PUBLIC ${OPENSSL_LIBRARIES})
    add_compile_definitions(HAS_OPENSSL)
endif()

add_executable(main main.cpp Lexer.cpp Symbol.cpp Parser.cpp Formatter.cpp Grammar.cpp c/CParser.cpp)

target_include_directories(main PUBLIC ../include)

target_link_libraries(main PRIVATE hash)

target_compile_features(main PUBLIC cxx_std_20)

target_compile_options(main PUBLIC "-O2")
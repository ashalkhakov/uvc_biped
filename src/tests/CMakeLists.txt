project(tests)

find_package(cmocka CONFIG REQUIRED)

add_executable(tests "test_main.c" "../main.h")

target_link_libraries(tests PRIVATE cmocka::cmocka biped_lib)

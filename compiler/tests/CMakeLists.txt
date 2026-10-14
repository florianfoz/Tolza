add_executable(tolza-tests
    tolza_test.cpp
)

target_link_libraries(tolza-tests
    PRIVATE
        tolza-core
)

add_test(
    NAME tolza
    COMMAND tolza-tests
)
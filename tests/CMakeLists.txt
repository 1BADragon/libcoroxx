macro(new_test BASE_NAME)
    add_executable(${BASE_NAME}_xx_test ${ARGN})
    target_link_libraries(${BASE_NAME}_xx_test coroxx)

    add_test(NAME ${BASE_NAME}_xx_test COMMAND ${BASE_NAME}_xx_test)
    set_tests_properties(${BASE_NAME}_xx_test PROPERTIES TIMEOUT 3)
endmacro()

new_test(basic basic.cpp)
new_test(queue queue.cpp)
new_test(callsoon callsoon.cpp)
new_test(task tasktest.cpp)
new_test(iterator iteratortest.cpp)
new_test(void_ret void_ret_test.cpp)

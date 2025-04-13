function(dp_imapp_add_test testName)
    add_executable(${PROJECT_NAME}_Test_${testName} ${testName}.cpp)
    target_link_libraries(${PROJECT_NAME}_Test_${testName} PRIVATE ${PROJECT_NAME}::${PROJECT_NAME})
    set_target_properties(${PROJECT_NAME}_Test_${testName} PROPERTIES FOLDER tests)

    dp_configure_target(${PROJECT_NAME}_Test_${testName}
        DP_WARNINGS
        AUTO_SOURCE_GROUP
        WIN32_RELEASE
        COPY_KNOWN_DLLS_NEAR_EXECUTABLE
    )
endfunction()

dp_imapp_add_test(Simple)

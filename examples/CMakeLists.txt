function(dp_imapp_add_example exampleName)
    add_executable(${PROJECT_NAME}_Example_${exampleName} ${exampleName}.cpp)
    target_link_libraries(${PROJECT_NAME}_Example_${exampleName} PRIVATE ${PROJECT_NAME}::${PROJECT_NAME})
    set_target_properties(${PROJECT_NAME}_Example_${exampleName} PROPERTIES FOLDER examples)

    dp_configure_target(${PROJECT_NAME}_Example_${exampleName}
        DP_WARNINGS
        AUTO_SOURCE_GROUP
        WIN32_RELEASE
        COPY_KNOWN_DLLS_NEAR_EXECUTABLE
    )
endfunction()

dp_imapp_add_example(BigAppStructure)
dp_imapp_add_example(CloseButton)
dp_imapp_add_example(DockSpace)
dp_imapp_add_example(HelloWorld)
dp_imapp_add_example(ImGuiDemo)
dp_imapp_add_example(MainWindow)

dp_configure_target(${PROJECT_NAME}_Example_HelloWorld VS_STARTUP_PROJECT_IF_TOP_LEVEL)

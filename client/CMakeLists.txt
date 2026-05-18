add_executable(pd_client
    main.cpp
    src/clientcore.cpp
    src/mainwindow.cpp
    src/logindialog.cpp
    include/clientcore.h
    include/mainwindow.h
    include/logindialog.h
)
target_include_directories(pd_client PRIVATE include)
target_link_libraries(pd_client PRIVATE pd_core Qt6::Core Qt6::Widgets Qt6::Network Qt6::Gui)

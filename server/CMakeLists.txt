add_executable(pd_server
    main.cpp
    src/database.cpp
    src/server.cpp
    src/clienthandler.cpp
    src/protocol.cpp
    include/server.h
    include/clienthandler.h
)
target_include_directories(pd_server PRIVATE include)
target_link_libraries(pd_server PRIVATE pd_core Qt6::Core Qt6::Network Qt6::Sql)

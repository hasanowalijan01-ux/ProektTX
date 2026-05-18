add_library(pd_core STATIC
    src/vigenere.cpp
    src/sha384.cpp
    src/chord.cpp
    src/steganography.cpp
)
target_include_directories(pd_core PUBLIC include)
target_compile_features(pd_core PUBLIC cxx_std_17)

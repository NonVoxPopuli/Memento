set(
    ANKI_SRCS
    ankiclient.cpp
    ankiconfig.h
    glossarybuilder.cpp
)

add_library(anki ${ANKI_SRCS})
target_link_libraries(
    anki
    utils
    globalmediator
    mpvadapter
    Qt6::Core
    Qt6::Network
)
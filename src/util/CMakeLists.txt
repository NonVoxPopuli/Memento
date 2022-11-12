add_library(
    globalmediator
    globalmediator.cpp
)
target_link_libraries(
    globalmediator
    Qt6::Core
    mpvadapter
)

add_library(
    utils
    utils.cpp
)
target_link_libraries(
    utils
    globalmediator
    Qt6::Core
    Qt6::Network
)

add_library(
    iconfactory
    iconfactory.h
    iconfactory.cpp
)
target_link_libraries(
    iconfactory
    Qt6::Gui
    Qt6::Widgets
)

add_library(
    subtitleparser
    subtitleparser.cpp
)
target_link_libraries(
    subtitleparser
    Qt6::Core
)

if (APPLE)
    add_library(
        cocoa_event_handler
        cocoaeventhandler.mm
    )
    target_link_libraries(
        cocoa_event_handler
        "-framework cocoa"
    )
endif()

add_subdirectory(timer)
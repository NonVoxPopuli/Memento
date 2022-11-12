add_subdirectory(common)
add_subdirectory(settings)
add_subdirectory(definition)
add_subdirectory(overlay)

# Add MpvWidget
add_library(
    mpvwidget
    mpvwidget.cpp
)
set(
    MPV_WIDGET_LIBS
    cursortimer
    ${MPV_LIB}
    utils
    Qt6::Widgets
    Qt6::OpenGLWidgets
)
if(UNIX AND NOT APPLE)
    set(
        MPV_WIDGET_LIBS
	    ${MPV_WIDGET_LIBS}
	    Qt6::DBus
        Qt6::GuiPrivate
    )
elseif(UNIX AND APPLE)
    set(
        MPV_WIDGET_LIBS
        ${MPV_WIDGET_LIBS}
        "-framework IOKit -framework CoreFoundation"
    )
endif()
target_link_libraries(
    mpvwidget
    ${MPV_WIDGET_LIBS}
)

# Add About Window
add_library(
    aboutwindow
    aboutwindow.cpp
)
target_link_libraries(
    aboutwindow
    Qt6::Widgets
    Qt6::Core
)

# Add Search Widget
add_library(
    searchwidget
    searchwidget.cpp
)
target_link_libraries(
    searchwidget
    definitionwidget
    dictionary_db
    Qt6::Core
    Qt6::Widgets
)

# Add Subtitle List
add_library(
    subtitlelist
    subtitlelistwidget.cpp
    subtitlelistwidget.ui
)
target_link_libraries(
    subtitlelist
    Qt6::Widgets
    mpvadapter
    subtitleparser
)
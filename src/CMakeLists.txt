include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB crochet_srcs "*.cpp")
file(GLOB crochet_uis "*.ui")

set(crochet_resources
    ../crochet.qrc
    ../stitches/stitches.qrc
    )

qt4_add_resources(crochet_rcc_srcs ${crochet_resources})
qt4_wrap_ui(crochet_ui_h ${crochet_uis})

if(WIN32)
    # Generate the embedded icon for the exe.
    # Works for both native MinGW and MXE cross-compile (MXE provides windres).
    set(CMAKE_RC_COMPILER_INIT windres)
    enable_language(RC)
    set(CMAKE_RC_COMPILE_OBJECT "<CMAKE_RC_COMPILER> <FLAGS> -O coff <DEFINES> -i <SOURCE> -o <OBJECT>")
    set(crochet_win "${CMAKE_SOURCE_DIR}/crochet.rc")

    # Whether to bundle Qt + MinGW runtime DLLs into the installer.
    # Default ON for native Windows builds (shared Qt), OFF when cross-compiling
    # with MXE because MXE's x86_64-w64-mingw32.static target links Qt statically.
    if(CMAKE_CROSSCOMPILING)
        option(BUNDLE_QT_DLLS "Copy Qt runtime DLLs into the installer" OFF)
    else()
        option(BUNDLE_QT_DLLS "Copy Qt runtime DLLs into the installer" ON)
    endif()

    if(BUNDLE_QT_DLLS)
        # Locate DLLs via Qt's own CMake variables — no hardcoded install paths.
        # On Windows, QT_BINARY_DIR holds both qmake and the runtime DLLs.
        set(QT_LIBS_WINDOWS
            "${QT_BINARY_DIR}/QtSvg4.dll"
            "${QT_BINARY_DIR}/QtCore4.dll"
            "${QT_BINARY_DIR}/QtGui4.dll"
            "${QT_BINARY_DIR}/QtNetwork4.dll"
            "${QT_BINARY_DIR}/QtXml4.dll"
        )

        # MinGW runtime DLLs. Override MINGW_BIN_DIR via -D if your MinGW lives
        # elsewhere; leave unset to skip bundling them (system-installed MinGW).
        set(MINGW_BIN_DIR "" CACHE PATH "Directory containing libgcc_s / mingwm10 DLLs")
        if(MINGW_BIN_DIR)
            set(QT_DEPS_WINDOWS
                "${MINGW_BIN_DIR}/libgcc_s_dw2-1.dll"
                "${MINGW_BIN_DIR}/mingwm10.dll"
                "${CMAKE_SOURCE_DIR}/resources/qt.conf"
            )
        else()
            set(QT_DEPS_WINDOWS "${CMAKE_SOURCE_DIR}/resources/qt.conf")
        endif()

        set(QT_PLUGINS_WINDOWS
            "${QT_PLUGINS_DIR}/imageformats"
            "${QT_PLUGINS_DIR}/accessible"
            "${QT_PLUGINS_DIR}/iconengines"
        )
    endif()

else()

    set(crochet_nix)

endif()

if(WIN32 AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_executable(${EXE_NAME} ${crochet_srcs} ${crochet_ui_h} ${crochet_rcc_srcs}
            ${crochet_version} ${crochet_win} ${crochet_mac} ${crochet_nix})
else()
    add_executable(${EXE_NAME} WIN32 MACOSX_BUNDLE ${crochet_srcs} ${crochet_ui_h} ${crochet_moc_srcs} ${crochet_rcc_srcs}
            ${crochet_version} ${crochet_win} ${crochet_mac} ${crochet_nix})
endif()

target_link_libraries(${EXE_NAME} ${QT_LIBRARIES})

if(APPLE)
    #install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME} DESTINATION ../MacOS)

    if(APP_STORE)
        set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_DEFINITIONS "APPLE_APP_STORE=1")
    endif()

    install(CODE "
        execute_process(
            COMMAND \"/usr/bin/macdeployqt\"
            \"@CMAKE_CURRENT_BINARY_DIR@/@PROJECT_NAME@.app/\"
            WORKING_DIRECTORY \"@CMAKE_BINARY_DIR@\"
            OUTPUT_VARIABLE _output)
        file(COPY \"@CMAKE_SOURCE_DIR@/images/@PROJECT_MACOSX_ICON@\"
            DESTINATION \"@CMAKE_CURRENT_BINARY_DIR@/@PROJECT_NAME@.app/Contents/Resources\") 
        file(COPY \"@CMAKE_CURRENT_BINARY_DIR@/@PROJECT_NAME@.app\" 
            DESTINATION \"@CMAKE_BINARY_DIR@/_CPack_Packages/Darwin/@CPACK_GENERATOR@/@PROJECT_NAME@-@VERSION_SHORT@\")
    " COMPONENT Runtime)

else()

    install(TARGETS ${EXE_NAME} BUNDLE DESTINATION . COMPONENT Runtime RUNTIME DESTINATION bin COMPONENT Runtime)
    if(WIN32)
        set_property(TARGET ${EXE_NAME} PROPERTY OUTPUT_NAME "${PROJECT_NAME}")
    endif()
endif()

if(WIN32 AND BUNDLE_QT_DLLS)
    install(PROGRAMS ${QT_LIBS_WINDOWS} ${QT_DEPS_WINDOWS} DESTINATION bin)
    install(DIRECTORY ${QT_PLUGINS_WINDOWS} DESTINATION plugins)
endif()

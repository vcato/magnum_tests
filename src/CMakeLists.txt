set_directory_properties(PROPERTIES CORRADE_USE_PEDANTIC_FLAGS ON)

add_executable(MyApplication main.cpp)
target_link_libraries(MyApplication PRIVATE
    Magnum::Application
    Magnum::GL
    Magnum::Magnum
    MagnumIntegration::ImGui)

# Make the executable a default target to build & run in Visual Studio
set_property(DIRECTORY ${PROJECT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT MyApplication)

if(CORRADE_TARGET_EMSCRIPTEN)
  install(TARGETS MyApplication DESTINATION ${CMAKE_INSTALL_PREFIX})
  install(FILES
      ${MAGNUM_EMSCRIPTENAPPLICATION_JS}
      ${MAGNUM_WEBAPPLICATION_CSS}
      DESTINATION ${CMAKE_INSTALL_PREFIX})
endif()

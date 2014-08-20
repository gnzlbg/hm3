if (PARAVIEW_QT_VERSION VERSION_GREATER "4")
  qt5_wrap_cpp(moc_sources)
else()
  qt4_wrap_cpp(moc_sources)
endif()

add_paraview_plugin(HM3Reader "v1.0"
  SERVER_MANAGER_XML vtkHM3ReaderServConf.xml
  GUI_INTERFACES ${interfaces}
  SERVER_MANAGER_SOURCES vtkHM3Reader.cxx
  SOURCES ${sources}
  REQUIRED_ON_SERVER
  EXCLUDE_FROM_DEFAULT_TARGET)

target_link_libraries(HM3Reader LINK_PUBLIC ${HM3_LIBS})

hm3_add_packages_to_target(HM3Reader)
add_dependencies(vis HM3Reader)

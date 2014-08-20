# Tree grid reader
if (PARAVIEW_QT_VERSION VERSION_GREATER "4")
  qt5_wrap_cpp(moc_sources pqDualComboBox.h)
else()
  qt4_wrap_cpp(moc_sources pqDualComboBox.h)
endif()

add_paraview_property_widget(out_interfaces out_sources
  TYPE "dual_combo_box"
  CLASS_NAME pqDualComboBox)
set(sources pqDualComboBox.cxx ${moc_sources} ${out_sources})
set(interfaces ${out_interfaces})

add_paraview_plugin(H3TreeReader "v1.0"
  SERVER_MANAGER_XML vtkH3TreeReaderServConf.xml
  GUI_INTERFACES ${interfaces}
  SERVER_MANAGER_SOURCES vtkH3TreeReader.cxx
  SOURCES ${sources}
  REQUIRED_ON_SERVER)

target_link_libraries(H3TreeReader LINK_PUBLIC ${H3_LIBS})

add_dependencies(vis H3TreeReader)

set(
    ModuleName
    #----------------------------------------------------------
    # (1) Set the module name here:
    #----------------------------------------------------------
    "Editor"
)
set(
    ${ModuleName}Source
    #----------------------------------------------------------
    # (2) Add the module source here:
    #----------------------------------------------------------
    "HOME.cpp"
    "TabPages/Basic/Basic.cpp"
    "TabPages/Settings/Settings.cpp"
)
list(TRANSFORM ${ModuleName}Source PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/")
SetPropertyValue(D14_${ModuleName}Source ${${ModuleName}Source})

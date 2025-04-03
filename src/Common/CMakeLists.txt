set(
    ModuleName
    #----------------------------------------------------------
    # (1) Set the module name here:
    #----------------------------------------------------------
    "Common"
)
set(
    ${ModuleName}Source
    #----------------------------------------------------------
    # (2) Add the module source here:
    #----------------------------------------------------------
    "DirectXError.cpp"
    "MathUtils/2D.cpp"
    "MathUtils/3D.cpp"
    "MathUtils/GDI.cpp"
    "Precompile.cpp"
    "ResourcePack.cpp"
    "RuntimeError.cpp"
)
list(TRANSFORM ${ModuleName}Source PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/")
SetPropertyValue(D14_${ModuleName}Source ${${ModuleName}Source})

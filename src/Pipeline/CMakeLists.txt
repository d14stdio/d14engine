set(
    ModuleName
    #----------------------------------------------------------
    # (1) Set the module name here:
    #----------------------------------------------------------
    "Pipeline"
)
set(
    ${ModuleName}Source
    #----------------------------------------------------------
    # (2) Add the module source here:
    #----------------------------------------------------------
    "2D/Sprite.cpp"
    "2D/SpriteGroup.cpp"
    "2D/TextureSequence.cpp"
    "Cameras/Camera2D.cpp"
    "Cameras/Camera3D.cpp"
    "Data/Transformation.cpp"
)
list(TRANSFORM ${ModuleName}Source PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/")
SetPropertyValue(D14_${ModuleName}Source ${${ModuleName}Source})

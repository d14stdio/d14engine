set(
    ModuleName
    #----------------------------------------------------------
    # (1) Set the module name here:
    #----------------------------------------------------------
    "Renderer"
)
set(
    ${ModuleName}Source
    #----------------------------------------------------------
    # (2) Add the module source here:
    #----------------------------------------------------------
    "Camera.cpp"
    "DebugUtils.cpp"
    "FrameData/FrameResource.cpp"
    "GpuBuffer.cpp"
    "GraphUtils/Barrier.cpp"
    "GraphUtils/Bitmap.cpp"
    "GraphUtils/PSO.cpp"
    "GraphUtils/Resource.cpp"
    "GraphUtils/Shader.cpp"
    "GraphUtils/StaticSampler.cpp"
    "GraphUtils/Texture.cpp"
    "InfoUtils.cpp"
    "Interfaces/DrawLayer.cpp"
    "Interfaces/DrawObject.cpp"
    "Interfaces/DrawObject2D.cpp"
    "Letterbox.cpp"
    "Renderer.cpp"
    "TickTimer.cpp"
)
list(TRANSFORM ${ModuleName}Source PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/")
SetPropertyValue(D14_${ModuleName}Source ${${ModuleName}Source})

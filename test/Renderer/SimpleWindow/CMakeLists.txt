set(
    TargetName
    #----------------------------------------------------------
    # (1) Set the target name here:
    #----------------------------------------------------------
    "SimpleWindow"
)
set(
    ${TargetName}Module
    #----------------------------------------------------------
    # (2) Add target module dependencies here:
    #----------------------------------------------------------
    "Renderer"
)
SetPropertyValue(D14_${TargetName}Module ${${TargetName}Module})

set(
    ${TargetName}Source
    #----------------------------------------------------------
    # (3) Add target source dependencies here:
    #----------------------------------------------------------
    "${TargetName}.cpp"
)
list(TRANSFORM ${TargetName}Source PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/")
SetPropertyValue(D14_${TargetName}Source ${${TargetName}Source})

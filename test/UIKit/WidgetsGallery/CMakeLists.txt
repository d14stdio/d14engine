set(
    TargetName
    #----------------------------------------------------------
    # (1) Set the target name here:
    #----------------------------------------------------------
    "WidgetsGallery"
)
set(
    ${TargetName}Module
    #----------------------------------------------------------
    # (2) Add target module dependencies here:
    #----------------------------------------------------------
    "Renderer"
    "UIKit"
)
SetPropertyValue(D14_${TargetName}Module ${${TargetName}Module})

set(
    ${TargetName}Source
    #----------------------------------------------------------
    # (3) Add target source dependencies here:
    #----------------------------------------------------------
    "${TargetName}.cpp"
    "TabPages/Gallery/Category/BasicPanelPage.cpp"
    "TabPages/Gallery/Category/ButtonPage.cpp"
    "TabPages/Gallery/Category/CheckBoxPage.cpp"
    "TabPages/Gallery/Category/ClickablePanelPage.cpp"
    "TabPages/Gallery/Category/ConstraintLayoutPage.cpp"
    "TabPages/Gallery/Category/DraggablePanelPage.cpp"
    "TabPages/Gallery/Category/ElevatedButtonPage.cpp"
    "TabPages/Gallery/Category/FilledButtonPage.cpp"
    "TabPages/Gallery/Category/FlatButtonPage.cpp"
    "TabPages/Gallery/Category/GridLayoutPage.cpp"
    "TabPages/Gallery/Category/GuidancePage.cpp"
    "TabPages/Gallery/Category/IconLabelPage.cpp"
    "TabPages/Gallery/Category/LabelAreaPage.cpp"
    "TabPages/Gallery/Category/LabelPage.cpp"
    "TabPages/Gallery/Category/LayoutPage.cpp"
    "TabPages/Gallery/Category/ListViewPage.cpp"
    "TabPages/Gallery/Category/MiscellaneousPage.cpp"
    "TabPages/Gallery/Category/NumberBoxPage.cpp"
    "TabPages/Gallery/Category/OnOffSwitchPage.cpp"
    "TabPages/Gallery/Category/OutlinedButtonPage.cpp"
    "TabPages/Gallery/Category/PageTemplate.cpp"
    "TabPages/Gallery/Category/ResizablePanelPage.cpp"
    "TabPages/Gallery/Category/ScrollViewPage.cpp"
    "TabPages/Gallery/Category/SliderPage.cpp"
    "TabPages/Gallery/Category/TabGroupPage.cpp"
    "TabPages/Gallery/Category/TextBoxPage.cpp"
    "TabPages/Gallery/Category/TextEditorPage.cpp"
    "TabPages/Gallery/Category/TextPanelPage.cpp"
    "TabPages/Gallery/Category/ToggleButtonPage.cpp"
    "TabPages/Gallery/Category/TreeViewPage.cpp"
    "TabPages/Gallery/Category/ViewPanelPage.cpp"
    "TabPages/Gallery/Category/WindowPage.cpp"
    "TabPages/Gallery/Gallery.cpp"
    "TabPages/Settings/Settings.cpp"
)
list(TRANSFORM ${TargetName}Source PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/")
SetPropertyValue(D14_${TargetName}Source ${${TargetName}Source})

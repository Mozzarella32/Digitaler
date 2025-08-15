cmake_minimum_required(VERSION 3.13)

project(Digitaler)

include(FetchContent)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(FETCHCONTENT_UPDATES_DISCONNECTED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# WxWidgets
set(wxBUILD_SHARED OFF CACHE BOOL "" FORCE)
set(wxBUILD_TESTS OFF CACHE BOOL "" FORCE)
set(wxBUILD_DEMOS OFF CACHE BOOL "" FORCE)
set(wxBUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
# set(wxUSE_WEBVIEW OFF CACHE BOOL "" FORCE)
# set(wxUSE_WEBREQUEST OFF CACHE BOOL "" FORCE)
# set(wxUSE_MEDIACTRL OFF CACHE BOOL "" FORCE)
# set(wxUSE_SOCKETS OFF CACHE BOOL "" FORCE)
# set(wxUSE_PROTOCOL OFF CACHE BOOL "" FORCE)
# set(wxUSE_FS_INET OFF CACHE BOOL "" FORCE)
# set(wxUSE_URL OFF CACHE BOOL "" FORCE)
# set(wxUSE_WEBVIEW_EDGE OFF CACHE BOOL "" FORCE)
# set(wxUSE_ODBC OFF CACHE BOOL "" FORCE)
# set(wxUSE_ZIPSTREAM OFF CACHE BOOL "" FORCE)
# set(wxUSE_TARSTREAM OFF CACHE BOOL "" FORCE)
# set(wxUSE_FTP OFF CACHE BOOL "" FORCE)
# set(wxUSE_HTTP OFF CACHE BOOL "" FORCE)
# set(wxUSE_STL OFF CACHE BOOL "" FORCE)
# set(wxUSE_STC OFF CACHE BOOL "" FORCE)
# set(wxUSE_XML OFF CACHE BOOL "" FORCE)
set(wxUSE_RC OFF CACHE BOOL "" FORCE)
# set(wxUSE_AUI OFF CACHE BOOL "" FORCE)
set(wxUSE_QA OFF CACHE BOOL "" FORCE)
# set(wxUSE_PROPGRID OFF CACHE BOOL "" FORCE)
# set(wxUSE_HTML OFF CACHE BOOL "" FORCE)
# set(wxUSE_RIBBON OFF CACHE BOOL "" FORCE)
set(wxUSE_ADV OFF CACHE BOOL "" FORCE)
# set(wxUSE_XRC OFF CACHE BOOL "" FORCE)
# set(wxUSE_RICHTEXT OFF CACHE BOOL "" FORCE)

# set(wxUSE_ANY OFF CACHE BOOL "use wxAny class" FORCE)
# set(wxUSE_APPLE_IEEE OFF CACHE BOOL "use the Apple IEEE codec" FORCE)
# set(wxUSE_ARCHIVE_STREAMS OFF CACHE BOOL "use wxArchive streams" FORCE)
# # # set(wxUSE_BASE64 OFF CACHE BOOL "use base64 encoding/decoding functions" FORCE)
# # set(wxUSE_STACKWALKER OFF CACHE BOOL "use wxStackWalker class for getting backtraces" FORCE)
# # # set(wxUSE_ON_FATAL_EXCEPTION OFF CACHE BOOL "catch signals in wxApp::OnFatalException" FORCE)
# set(wxUSE_CMDLINE_PARSER OFF CACHE BOOL "use wxCmdLineParser class" FORCE)
# set(wxUSE_DATETIME OFF CACHE BOOL "use wxDateTime class" FORCE)
# set(wxUSE_DEBUGREPORT OFF CACHE BOOL "use wxDebugReport class" FORCE)
# set(wxUSE_DYNLIB_CLASS OFF CACHE BOOL "use wxDynamicLibrary class for DLL loading" FORCE)
# set(wxUSE_DYNAMIC_LOADER OFF CACHE BOOL "use wxPluginLibrary and wxPluginManager classes" FORCE)
# # # set(wxUSE_EXCEPTIONS OFF CACHE BOOL "build exception-safe library" FORCE)
# set(wxUSE_FFILE OFF CACHE BOOL "use wxFFile class" FORCE)
# set(wxUSE_FILE OFF CACHE BOOL "use wxFile class" FORCE)
# set(wxUSE_FILE_HISTORY OFF CACHE BOOL "use wxFileHistory class" FORCE)
# set(wxUSE_FILESYSTEM OFF CACHE BOOL "use virtual file systems classes" FORCE)
# set(wxUSE_FONTENUM OFF CACHE BOOL "use wxFontEnumerator class" FORCE)
# # set(wxUSE_FONTMAP OFF CACHE BOOL "use font encodings conversion classes" FORCE)
# set(wxUSE_FS_ARCHIVE OFF CACHE BOOL "use virtual archive filesystems" FORCE)
set(wxUSE_FS_INET OFF CACHE BOOL "use virtual HTTP/FTP filesystems" FORCE)
# set(wxUSE_FS_ZIP OFF CACHE BOOL "now replaced by fs_archive" FORCE)
# set(wxUSE_FSWATCHER OFF CACHE BOOL "use wxFileSystemWatcher class" FORCE)
# set(wxUSE_GEOMETRY OFF CACHE BOOL "use geometry class" FORCE)
# set(wxUSE_LOG OFF CACHE BOOL "use logging system" FORCE)
# set(wxUSE_MIMETYPE OFF CACHE BOOL "use wxMimeTypesManager" FORCE)
# # set(wxUSE_PRINTF_POS_PARAMS OFF CACHE BOOL "use wxVsnprintf() which supports positional parameters" FORCE)
# # set(wxUSE_SECRETSTORE OFF CACHE BOOL "use wxSecretStore class" FORCE)
# set(wxUSE_SNGLINST_CHECKER OFF CACHE BOOL "use wxSingleInstanceChecker class" FORCE)
# set(wxUSE_SOUND OFF CACHE BOOL "use wxSound class" FORCE)
# set(wxUSE_SPELLCHECK OFF CACHE BOOL "enable spell checking in wxTextCtrl" FORCE)
# # set(wxUSE_STDPATHS OFF CACHE BOOL "use wxStandardPaths class" FORCE)
# set(wxUSE_STOPWATCH OFF CACHE BOOL "use wxStopWatch class" FORCE)
# set(wxUSE_STREAMS OFF CACHE BOOL "use wxStream etc classes" FORCE)
# set(wxUSE_SYSTEM_OPTIONS OFF CACHE BOOL "use wxSystemOptions" FORCE)
# set(wxUSE_TARSTREAM OFF CACHE BOOL "use wxTar streams" FORCE)
# set(wxUSE_TEXTBUFFER OFF CACHE BOOL "use wxTextBuffer class" FORCE)
# set(wxUSE_TEXTFILE OFF CACHE BOOL "use wxTextFile class" FORCE)
# # # set(wxUSE_TIMER OFF CACHE BOOL "use wxTimer class" FORCE)
# set(wxUSE_VARIANT OFF CACHE BOOL "use wxVariant class" FORCE)

# # WebRequest options
set(wxUSE_WEBREQUEST OFF CACHE BOOL "use wxWebRequest class" FORCE)

# URL
set(wxUSE_URL OFF CACHE BOOL "use wxURL class" FORCE)
set(wxUSE_PROTOCOL OFF CACHE BOOL "use wxProtocol class" FORCE)
set(wxUSE_PROTOCOL_HTTP OFF CACHE BOOL "HTTP support in wxProtocol" FORCE)
set(wxUSE_PROTOCOL_FTP OFF CACHE BOOL "FTP support in wxProtocol" FORCE)
set(wxUSE_PROTOCOL_FILE OFF CACHE BOOL "FILE support in wxProtocol" FORCE)

# GUI
set(wxUSE_DOC_VIEW_ARCHITECTURE OFF CACHE BOOL "use document view architecture" FORCE)
set(wxUSE_HELP OFF CACHE BOOL "use help subsystem" FORCE)
set(wxUSE_MS_HTML_HELP OFF CACHE BOOL "use MS HTML Help (win32)" FORCE)
set(wxUSE_HTML OFF CACHE BOOL "use wxHTML sub-library" FORCE)
set(wxUSE_WXHTML_HELP OFF CACHE BOOL "use wxHTML-based help" FORCE)
set(wxUSE_XRC OFF CACHE BOOL "use XRC resources sub-library" FORCE)
set(wxUSE_XML OFF CACHE BOOL "use the xml library (overruled by wxUSE_XRC)" FORCE)
set(wxUSE_AUI OFF CACHE BOOL "use AUI docking library" FORCE)
set(wxUSE_PROPGRID OFF CACHE BOOL "use wxPropertyGrid library" FORCE)
set(wxUSE_RIBBON OFF CACHE BOOL "use wxRibbon library" FORCE)
set(wxUSE_STC OFF CACHE BOOL "use wxStyledTextCtrl library" FORCE)
set(wxUSE_CONSTRAINTS OFF CACHE BOOL "use layout-constraints system" FORCE)
set(wxUSE_LOGGUI OFF CACHE BOOL "use standard GUI logger" FORCE)
set(wxUSE_LOGWINDOW OFF CACHE BOOL "use wxLogWindow" FORCE)
set(wxUSE_LOG_DIALOG OFF CACHE BOOL "use wxLogDialog" FORCE)
set(wxUSE_MDI OFF CACHE BOOL "use multiple document interface architecture" FORCE)
set(wxUSE_MDI_ARCHITECTURE OFF CACHE BOOL "use docview architecture with MDI" FORCE)
set(wxUSE_MEDIACTRL OFF CACHE BOOL "use wxMediaCtrl class" FORCE)
set(wxUSE_RICHTEXT OFF CACHE BOOL "use wxRichTextCtrl" FORCE)
set(wxUSE_POSTSCRIPT OFF CACHE BOOL "use wxPostscriptDC device context (default for gtk+)" FORCE)
set(wxUSE_AFM_FOR_POSTSCRIPT OFF CACHE BOOL "in wxPostScriptDC class use AFM (adobe font metrics) file for character widths" FORCE)
set(wxUSE_PRINTING_ARCHITECTURE OFF CACHE BOOL "use printing architecture" FORCE)
set(wxUSE_SVG OFF CACHE BOOL "use wxSVGFileDC device context" FORCE)
set(wxUSE_WEBVIEW OFF CACHE BOOL "use wxWebView library" FORCE)

# add_definitions(-DUSE_STC=0)


# IPC &c

# set(wxUSE_CLIPBOARD OFF CACHE BOOL "use wxClipboard class" FORCE)
# set(wxUSE_DRAG_AND_DROP OFF CACHE BOOL "use Drag'n'Drop classes" FORCE)

# features affecting multiple controls
# set(wxUSE_MARKUP OFF CACHE BOOL "support wxControl::SetLabelMarkup" FORCE)

# things
# set(wxUSE_ACCEL OFF CACHE BOOL "use accelerators" FORCE)
# set(wxUSE_ACTIVITYINDICATOR OFF CACHE BOOL "use wxActivityIndicator class" FORCE)
# set(wxUSE_ADDREMOVECTRL OFF CACHE BOOL "use wxAddRemoveCtrl" FORCE)
# set(wxUSE_ANIMATIONCTRL OFF CACHE BOOL "use wxAnimationCtrl class" FORCE)
set(wxUSE_BANNERWINDOW OFF CACHE BOOL "use wxBannerWindow class" FORCE)
# set(wxUSE_ARTPROVIDER_STD OFF CACHE BOOL "use standard XPM icons in wxArtProvider" FORCE)
# set(wxUSE_ARTPROVIDER_TANGO OFF CACHE BOOL "use Tango icons in wxArtProvider" FORCE)
# set(wxUSE_BMPBUTTON OFF CACHE BOOL "use wxBitmapButton class" FORCE)
# set(wxUSE_BITMAPCOMBOBOX OFF CACHE BOOL "use wxBitmapComboBox class" FORCE)
# set(wxUSE_BUTTON OFF CACHE BOOL "use wxButton class" FORCE)
# set(wxUSE_CALENDARCTRL OFF CACHE BOOL "use wxCalendarCtrl class" FORCE)
set(wxUSE_CARET OFF CACHE BOOL "use wxCaret class" FORCE)
# set(wxUSE_CHECKBOX OFF CACHE BOOL "use wxCheckBox class" FORCE)
# set(wxUSE_CHECKLISTBOX OFF CACHE BOOL "use wxCheckListBox (listbox with checkboxes) class" FORCE)
# set(wxUSE_CHOICE OFF CACHE BOOL "use wxChoice class" FORCE)
# set(wxUSE_CHOICEBOOK OFF CACHE BOOL "use wxChoicebook class" FORCE)
# set(wxUSE_COLLPANE OFF CACHE BOOL "use wxCollapsiblePane class" FORCE)
# set(wxUSE_COLOURPICKERCTRL OFF CACHE BOOL "use wxColourPickerCtrl class" FORCE)
# set(wxUSE_COMBOBOX OFF CACHE BOOL "use wxComboBox class" FORCE)
# set(wxUSE_COMBOCTRL OFF CACHE BOOL "use wxComboCtrl class" FORCE)
set(wxUSE_COMMANDLINKBUTTON OFF CACHE BOOL "use wxCommmandLinkButton class" FORCE)
# set(wxUSE_DATAVIEWCTRL OFF CACHE BOOL "use wxDataViewCtrl class" FORCE)
# set(wxUSE_NATIVE_DATAVIEWCTRL OFF CACHE BOOL "use the native wxDataViewCtrl if available" FORCE)
# set(wxUSE_DATEPICKCTRL OFF CACHE BOOL "use wxDatePickerCtrl class" FORCE)
set(wxUSE_DETECT_SM OFF CACHE BOOL "use code to detect X11 session manager" FORCE OFF)
# set(wxUSE_DIRPICKERCTRL OFF CACHE BOOL "use wxDirPickerCtrl class" FORCE)
# set(wxUSE_DISPLAY OFF CACHE BOOL "use wxDisplay class" FORCE)
# set(wxUSE_EDITABLELISTBOX OFF CACHE BOOL "use wxEditableListBox class" FORCE)
# set(wxUSE_FILECTRL OFF CACHE BOOL "use wxFileCtrl class" FORCE)
# set(wxUSE_FILEPICKERCTRL OFF CACHE BOOL "use wxFilePickerCtrl class" FORCE)
set(wxUSE_FONTPICKERCTRL OFF CACHE BOOL "use wxFontPickerCtrl class" FORCE)
# set(wxUSE_GAUGE OFF CACHE BOOL "use wxGauge class" FORCE)
# set(wxUSE_GRID OFF CACHE BOOL "use wxGrid class" FORCE)
# set(wxUSE_HEADERCTRL OFF CACHE BOOL "use wxHeaderCtrl class" FORCE)
# set(wxUSE_HYPERLINKCTRL OFF CACHE BOOL "use wxHyperlinkCtrl class" FORCE)
# set(wxUSE_IMAGLIST OFF CACHE BOOL "use wxImageList class" FORCE)
# set(wxUSE_INFOBAR OFF CACHE BOOL "use wxInfoBar class" FORCE)
# set(wxUSE_LISTBOOK OFF CACHE BOOL "use wxListbook class" FORCE)
# set(wxUSE_LISTBOX OFF CACHE BOOL "use wxListBox class" FORCE)
# set(wxUSE_LISTCTRL OFF CACHE BOOL "use wxListCtrl class" FORCE)
# set(wxUSE_NOTEBOOK OFF CACHE BOOL "use wxNotebook class" FORCE)
# set(wxUSE_NOTIFICATION_MESSAGE OFF CACHE BOOL "use wxNotificationMessage class" FORCE)
# set(wxUSE_ODCOMBOBOX OFF CACHE BOOL "use wxOwnerDrawnComboBox class" FORCE)
# set(wxUSE_POPUPWIN OFF CACHE BOOL "use wxPopUpWindow class" FORCE)
set(wxUSE_PREFERENCES_EDITOR OFF CACHE BOOL "use wxPreferencesEditor class" FORCE)
# set(wxUSE_RADIOBOX OFF CACHE BOOL "use wxRadioBox class" FORCE)
# set(wxUSE_RADIOBTN OFF CACHE BOOL "use wxRadioButton class" FORCE)
set(wxUSE_RICHMSGDLG OFF CACHE BOOL "use wxRichMessageDialog class" FORCE)
set(wxUSE_RICHTOOLTIP OFF CACHE BOOL "use wxRichToolTip class" FORCE)
# set(wxUSE_REARRANGECTRL OFF CACHE BOOL "use wxRearrangeList/Ctrl/Dialog" FORCE)
set(wxUSE_SASH OFF CACHE BOOL "use wxSashWindow class" FORCE)
# set(wxUSE_SCROLLBAR OFF CACHE BOOL "use wxScrollBar class and scrollable windows" FORCE)
# set(wxUSE_SEARCHCTRL OFF CACHE BOOL "use wxSearchCtrl class" FORCE)
# set(wxUSE_SLIDER OFF CACHE BOOL "use wxSlider class" FORCE)
# set(wxUSE_SPINBTN OFF CACHE BOOL "use wxSpinButton class" FORCE)
# set(wxUSE_SPINCTRL OFF CACHE BOOL "use wxSpinCtrl class" FORCE)
# set(wxUSE_SPLITTER OFF CACHE BOOL "use wxSplitterWindow class" FORCE)
# set(wxUSE_STATBMP OFF CACHE BOOL "use wxStaticBitmap class" FORCE)
# set(wxUSE_STATBOX OFF CACHE BOOL "use wxStaticBox class" FORCE)
# set(wxUSE_STATLINE OFF CACHE BOOL "use wxStaticLine class" FORCE)
# set(wxUSE_STATTEXT OFF CACHE BOOL "use wxStaticText class" FORCE)
# set(wxUSE_STATUSBAR OFF CACHE BOOL "use wxStatusBar class" FORCE)
set(wxUSE_TASKBARBUTTON OFF CACHE BOOL "use wxTaskBarButton class" FORCE)
set(wxUSE_TASKBARICON OFF CACHE BOOL "use wxTaskBarIcon class" FORCE)
# set(wxUSE_TOOLBAR_NATIVE OFF CACHE BOOL "use native wxToolBar class" FORCE)
# set(wxUSE_TEXTCTRL OFF CACHE BOOL "use wxTextCtrl class" FORCE)
# set(wxUSE_TIMEPICKCTRL OFF CACHE BOOL "use wxTimePickerCtrl class" FORCE)
# set(wxUSE_TIPWINDOW OFF CACHE BOOL "use wxTipWindow class" FORCE)
# set(wxUSE_TOGGLEBTN OFF CACHE BOOL "use wxToggleButton class" FORCE)
# set(wxUSE_TOOLBAR OFF CACHE BOOL "use wxToolBar class" FORCE)
# set(wxUSE_TOOLBOOK OFF CACHE BOOL "use wxToolbook class" FORCE)
# set(wxUSE_TREEBOOK OFF CACHE BOOL "use wxTreebook class" FORCE)
# set(wxUSE_TREECTRL OFF CACHE BOOL "use wxTreeCtrl class" FORCE)
# set(wxUSE_TREELISTCTRL OFF CACHE BOOL "use wxTreeListCtrl class" FORCE)

# # common dialogs
# set(wxUSE_ABOUTDLG OFF CACHE BOOL "use wxAboutBox" FORCE)
# set(wxUSE_CHOICEDLG OFF CACHE BOOL "use wxChoiceDialog" FORCE)
# set(wxUSE_COLOURDLG OFF CACHE BOOL "use wxColourDialog" FORCE)
# set(wxUSE_CREDENTIALDLG OFF CACHE BOOL "use wxCredentialEntryDialog" FORCE)
# set(wxUSE_FILEDLG OFF CACHE BOOL "use wxFileDialog" FORCE)
# set(wxUSE_FINDREPLDLG OFF CACHE BOOL "use wxFindReplaceDialog" FORCE)
# set(wxUSE_FONTDLG OFF CACHE BOOL "use wxFontDialog" FORCE)
# set(wxUSE_DIRDLG OFF CACHE BOOL "use wxDirDialog" FORCE)
# set(wxUSE_MSGDLG OFF CACHE BOOL "use wxMessageDialog" FORCE)
# set(wxUSE_NUMBERDLG OFF CACHE BOOL "use wxNumberEntryDialog" FORCE)
# set(wxUSE_SPLASH OFF CACHE BOOL "use wxSplashScreen" FORCE)
# set(wxUSE_TEXTDLG OFF CACHE BOOL "use wxTextDialog" FORCE)
set(wxUSE_STARTUP_TIPS OFF CACHE BOOL "use startup tips" FORCE)
# set(wxUSE_PROGRESSDLG OFF CACHE BOOL "use wxProgressDialog" FORCE)
set(wxUSE_WIZARDDLG OFF CACHE BOOL "use wxWizard" FORCE)

# # misc GUI options
# set(wxUSE_MENUS OFF CACHE BOOL "use wxMenu and wxMenuItem classes" FORCE)
# set(wxUSE_MENUBAR OFF CACHE BOOL "use wxMenuBar class" FORCE)
# set(wxUSE_MINIFRAME OFF CACHE BOOL "use wxMiniFrame class" FORCE)
# set(wxUSE_TOOLTIPS OFF CACHE BOOL "use wxToolTip class" FORCE)
# set(wxUSE_SPLINES OFF CACHE BOOL "use spline drawing code" FORCE)
# set(wxUSE_MOUSEWHEEL OFF CACHE BOOL "use mousewheel" FORCE)
# set(wxUSE_VALIDATORS OFF CACHE BOOL "use wxValidator and derived classes" FORCE)
set(wxUSE_BUSYINFO OFF CACHE BOOL "use wxBusyInfo" FORCE)
# set(wxUSE_HOTKEY OFF CACHE BOOL "use wxWindow::RegisterHotKey()" FORCE)
set(wxUSE_JOYSTICK OFF CACHE BOOL "use wxJoystick" FORCE)
# set(wxUSE_METAFILE OFF CACHE BOOL "use wxMetaFile" FORCE)
# set(wxUSE_DRAGIMAGE OFF CACHE BOOL "use wxDragImage" FORCE)
# set(wxUSE_UIACTIONSIMULATOR OFF CACHE BOOL "use wxUIActionSimulator (experimental)" FORCE)
# set(wxUSE_DC_TRANSFORM_MATRIX OFF CACHE BOOL "use wxDC::SetTransformMatrix and related" FORCE)
set(wxUSE_WEBVIEW_WEBKIT OFF CACHE BOOL "use wxWebView WebKit backend" FORCE)

# Images
# set(wxUSE_PALETTE OFF CACHE BOOL "use wxPalette class" FORCE)
# set(wxUSE_IMAGE OFF CACHE BOOL "use wxImage class" FORCE)
# set(wxUSE_GIF OFF CACHE BOOL "use gif images (GIF file format)" FORCE)
# set(wxUSE_PCX OFF CACHE BOOL "use pcx images (PCX file format)" FORCE)
# set(wxUSE_TGA OFF CACHE BOOL "use tga images (TGA file format)" FORCE)
# set(wxUSE_IFF OFF CACHE BOOL "use iff images (IFF file format)" FORCE)
# set(wxUSE_PNM OFF CACHE BOOL "use pnm images (PNM file format)" FORCE)
# set(wxUSE_XPM OFF CACHE BOOL "use xpm images (XPM file format)" FORCE)
# set(wxUSE_ICO_CUR OFF CACHE BOOL "use Windows ICO and CUR formats" FORCE)

add_subdirectory(${PROJECT_SOURCE_DIR}/submodules/eigen)

set(Eigen3_SOURCE_DIR ${PROJECT_SOURCE_DIR}/submodules/eigen)

add_subdirectory(${PROJECT_SOURCE_DIR}/submodules/MyWxWidgets)

add_subdirectory(${PROJECT_SOURCE_DIR}/submodules/PreparePngs)

add_subdirectory(${PROJECT_SOURCE_DIR}/submodules/ShadersToCharArray)

# Directories
set(SRC_DIR "${PROJECT_SOURCE_DIR}/src")
set(INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")
set(LIB_DIR "${PROJECT_SOURCE_DIR}/lib")

set(SHADER_DIR "${PROJECT_SOURCE_DIR}/resources/shaders/Source")
set(PNG_DIR "${PROJECT_SOURCE_DIR}/resources/art/Source")

set(PCH_HEADERS "${INCLUDE_DIR}/pch.hpp")

file(GLOB_RECURSE MY_SOURCE CONFIGURE_DEPENDS "${SRC_DIR}/*")
file(GLOB_RECURSE MY_HEADERS CONFIGURE_DEPENDS "${INCLUDE_DIR}/*")

#To trigger rebuild on changes//out of date comment
file(GLOB_RECURSE MY_SHADERS "${SHADER_DIR}/*")
file(GLOB_RECURSE MY_PNGS "${PNG_DIR}/*")


list(REMOVE_ITEM MY_SOURCE ${PCH_HEADERS})
list(REMOVE_ITEM MY_HEADERS ${PCH_HEADERS})

# Create Executable
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/app)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/app)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/app)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${PROJECT_SOURCE_DIR}/app)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${PROJECT_SOURCE_DIR}/app)

if (MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Werror -Wall -Wextra -Wpedantic -Wno-missing-field-initializers -Wno-missing-designated-field-initializers)
endif()

add_executable(${PROJECT_NAME} ${MY_SOURCE})

if(WIN32)
    target_link_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Release>:/SUBSYSTEM:WINDOWS>
        $<$<CONFIG:Debug>:/SUBSYSTEM:WINDOWS>)
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/app
)

# Precompiled Header (falls verwendet)
target_precompile_headers(${PROJECT_NAME} PRIVATE ${PCH_HEADERS})

# Include-Direktiven
target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDE_DIR} ${Eigen3_SOURCE_DIR})

# wxWidgets Linking
target_link_libraries(${PROJECT_NAME} PUBLIC wx::base wx::core wx::gl)
# MyOpenGl Linking
target_link_libraries(${PROJECT_NAME} PUBLIC MyOpenGl)

# MyWxWidgets Linking
target_link_libraries(${PROJECT_NAME} PUBLIC MyWxWidgets)

# Invok the pre build executable to generate Shaders and Pngs as bytarrays as there is still no #embede suppoert on msvc
add_custom_target(
    Invoke_ShadersToCharArray
    COMMAND ${PROJECT_SOURCE_DIR}/resources/shaders/$<TARGET_FILE_NAME:ShadersToCharArray>
    COMMENT "Calling ShadersToCharArray"
)
add_dependencies(Invoke_ShadersToCharArray ShadersToCharArray)

add_custom_target(
    Invoke_PreparePngs
    COMMAND ${PROJECT_SOURCE_DIR}/resources/art/$<TARGET_FILE_NAME:PreparePngs>
    COMMENT "Calling PreparePngs"
)
add_dependencies(Invoke_PreparePngs PreparePngs)

add_dependencies(${PROJECT_NAME} Invoke_ShadersToCharArray Invoke_PreparePngs)


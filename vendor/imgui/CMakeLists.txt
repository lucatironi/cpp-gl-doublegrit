add_library(imgui STATIC
	imconfig.h
	imgui_demo.cpp
	imgui_draw.cpp
	imgui_internal.h
	imgui_tables.cpp
	imgui_widgets.cpp
	imgui.h
	imgui.cpp
	imstb_rectpack.h
	imstb_textedit.h
	imstb_truetype.h
	# opengl helper
	backends/imgui_impl_opengl3.h
	backends/imgui_impl_opengl3.cpp
	backends/imgui_impl_glfw.h
	backends/imgui_impl_glfw.cpp
	)
target_include_directories(imgui PUBLIC ${CMAKE_CURRENT_LIST_DIR})
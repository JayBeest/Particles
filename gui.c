#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include "cimgui.h"
#include "cimgui_impl.h"
#include "particles.h"
#include "init.h"
#include "colors.h"

static void	anotherWindow(t_paSettings *settings)
{
	if (settings->showAnotherWindow)
	{
		igBegin("imgui Another Window", &settings->showAnotherWindow, 0);
		igText("Hello from imgui");
		if (igButton("Close me", (ImVec2){0, 0}))
			settings->showAnotherWindow = false;
		igEnd();
	}
}

static void	toggles(t_paSettings *settings)
{
	static ImVec4 old_color;

	if (old_color.x == 0)
	{
		old_color.x = settings->baseColor.x;
		old_color.y = settings->baseColor.y;
		old_color.z = settings->baseColor.z;
	}
	if (settings->baseColor.x != old_color.x || old_color.y != old_color.y || old_color.z != old_color.z)
	{
		old_color = (ImVec4)settings->baseColor;
			update_colors(settings);
	}
	if (settings->grav_toggle)
	{
		settings->cur_max_particles = GRAV_MAX_PARTICLES;
		if (settings->num_particles > GRAV_MAX_PARTICLES)
			settings->num_particles = GRAV_MAX_PARTICLES;
	}
	else
		settings->cur_max_particles = MAX_PARTICLES;
}

static void	check_colors(t_paSettings *settings)
{
	static ImVec4 old_color;
	if (old_color.x == 0)
	{
		old_color.x = settings->baseColor.x;
		old_color.y = settings->baseColor.y;
		old_color.z = settings->baseColor.z;
	}
	if (settings->baseColor.x != old_color.x || settings->baseColor.y != old_color.y || settings->baseColor.z != old_color.z)
	{
		old_color = (ImVec4)settings->baseColor;
			update_colors(settings);
	}
}

static void	render_gui(t_paSettings *settings, t_glfw_cimgui *glfw_cimgui)
{
	igRender();
	ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());

	#ifdef IMGUI_HAS_DOCK
	if (glfw_cimgui->ioptr->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		GLFWwindow *backup_current_window = glfwGetCurrentContext();
		igUpdatePlatformWindows();
		igRenderPlatformWindowsDefault(NULL, NULL);
		glfwMakeContextCurrent(backup_current_window);
	}
	#endif
}

void	start_imgui_frame(t_paSettings *settings, t_glfw_cimgui *glfw_cimgui)
{
// start imgui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	igNewFrame();

	if (settings->showDemoWindow)
		igShowDemoWindow(&settings->showDemoWindow);
// show a simple window that we created ourselves.
	{
		static float f = 0.0f;

		igBegin("Controls", NULL, 0);
//		igText("This is some useful text");
// 		igCheckbox("Demo window", &showDemoWindow);
// 		igCheckbox("Another window", &showAnotherWindow);
		igCheckbox("GravToggle", &settings->grav_toggle);
		igSliderInt("TangiForce", &settings->force_multiplier, -7500, 7500, "%.d", 0);
		igSliderInt("CentriForce", &settings->centri_multiplier, -250000, 250000, "%.d", 0);
		igSliderInt("GravForce", &settings->grav_constant, 1, 1000, "%.d", 0);
		igSliderFloat("WallBounce", &settings->wall_bounce, 0.0f, 2.0f, "%.2f", 0);
		igSliderFloat("AirRez", &settings->air_resistance, 1.0f, 0.5f, "%.5f", 0);
		igSliderFloat("P-Size", &settings->p_size, 0.5f, 50.0f, "%.5f", 0);
		igSliderInt("P-Amount", &settings->num_particles, 1, settings->cur_max_particles, "%.d", 0);
		igColorEdit4("clear color", (float *) &settings->baseColor, 0);

		static t_paColor ff;
		igPushID_Int(1);
		igVSliderFloat("##v", (ImVec2) {18, 120}, &(settings->baseColor.x), 0, 1, "", 0);
		igPopID();
		igSameLine(0.0f, -1.0f);
		igPushID_Int(2);
		igVSliderFloat("##v", (ImVec2) {18, 120}, &(settings->baseColor.y), 0, 1, "", 0);
		igPopID();
		igSameLine(0.0f, -1.0f);
		igPushID_Int(3);
		igVSliderFloat("##v", (ImVec2) {18, 120}, &(settings->baseColor.z), 0, 1, "", 0);
		igPopID();
		igSameLine(0.0f, -1.0f);
		if (igButton("Reset particles", (ImVec2){0, 0}))
			init_particles(settings);
		igText("Application %.3f ms/frame (%.1f FPS)", 1000.0f / igGetIO()->Framerate, igGetIO()->Framerate);
		igEnd();
	}
	toggles(settings);
	anotherWindow(settings);
	check_colors(settings);
	render_gui(settings, glfw_cimgui);
}
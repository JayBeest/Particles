#ifndef PARTICLE_INIT_H
# define PARTICLE_INIT_H

void	init_settings(t_paSettings *settings);
void 	init_particles(t_paSettings *settings);
void	init_glfw(t_paSettings *settings, t_glfw_cimgui * glfw_cimgui);
void	init_cimgui(t_glfw_cimgui * glfw_cimgui);
void	init_glew(void);

#endif

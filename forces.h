#ifndef FORCES_H
# define FORCES_H

void calculate_centripetal_force(t_particle *p, float centri_multip, float center_x, float center_y, float *fx, float *fy);
void calculate_tangential_force(t_particle *p, float force_multip, float center_x, float center_y, float *fx, float *fy);
void apply_damping_force(t_particle *p, float damping);
void calculate_gravitational_force(t_particle *p1, t_particle *p2, float *fx, float *fy, float grav_const);

#endif

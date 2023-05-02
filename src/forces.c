#include <math.h>
#include "particles.h"

void calculate_centripetal_force(t_particle *p, float centri_multip, float center_x, float center_y, float *fx, float *fy)
{
	float dx = p->x - center_x;
	float dy = p->y - center_y;
	float distance_squared = dx * dx + dy * dy;
	float distance = sqrtf(distance_squared);

	// if (distance < 1.0f) {
	//     distance = 1.0f;
	// }

	float force = centri_multip * p->mass / (distance_squared * 0.99f);
	*fx = -force * (dx / distance);
	*fy = -force * (dy / distance);
}

void calculate_tangential_force(t_particle *p, float force_multip, float center_x, float center_y, float *fx, float *fy)
{
	float dx = p->x - center_x;
	float dy = p->y - center_y;
	float distance = sqrtf(dx * dx + dy * dy);

	// if (distance < 1.0f) {
	//     distance = 1.0f;
	// }

	float force = force_multip * (p->mass / distance);
	*fx = -force * (dy / distance);
	*fy = force * (dx / distance);
}

void apply_damping_force(t_particle *p, float damping)
{
	p->vx *= damping;
	p->vy *= damping;
}

void calculate_gravitational_force(t_particle *p1, t_particle *p2, float *fx, float *fy, float grav_const)
{
	float dx = p2->x - p1->x;
	float dy = p2->y - p1->y;
	float distance_squared = dx * dx + dy * dy;
	float distance = sqrtf(distance_squared);

	if (distance < 1.0f) {
		distance = 1.0f;
	}

	float force = grav_const * (p1->mass * p2->mass) / distance_squared;
	*fx = force * (dx / distance);
	*fy = force * (dy / distance);
}

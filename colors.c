#include "particles.h"

void	update_colors(t_paSettings *settings)
{
	for (int i = 0; i < MAX_PARTICLES; i++)
	{
		(settings->particles)[i].color = (t_paColor ){(4 / (settings->particles)[i].mass) \
			+ settings->baseColor.x, settings->baseColor.y, settings->baseColor.z, \
			settings->baseColor.w};
	}
}
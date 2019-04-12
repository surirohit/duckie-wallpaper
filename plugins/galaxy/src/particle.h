/*
 *
 * LiveWallpaper
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Copyright (C) 2012-2016 Maximilian Schnarr <Maximilian.Schnarr@googlemail.com>
 * Copyright (C) 2012-2016 Aurélien   Rivière <aurelien.riv@gmail.com>
 *
 */

#ifndef _GALAXY_PARTICLE_H_
#define _GALAXY_PARTICLE_H_

G_BEGIN_DECLS

#define GALAXY_TYPE_PARTICLE_SYSTEM            (galaxy_particle_system_get_type())
#define GALAXY_PARTICLE_SYSTEM(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GALAXY_TYPE_PARTICLE_SYSTEM, GalaxyParticleSystem))
#define GALAXY_IS_PARTICLE_SYSTEM(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GALAXY_TYPE_PARTICLE_SYSTEM))
#define GALAXY_PARTICLE_SYSTEM_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GALAXY_TYPE_PARTICLE_SYSTEM, GalaxyParticleSystemClass))
#define GALAXY_IS_PARTICLE_SYSTEM_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GALAXY_TYPE_PARTICLE_SYSTEM))
#define GALAXY_PARTICLE_SYSTEM_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), GALAXY_TYPE_PARTICLE_SYSTEM, GalaxyParticleSystemClass))

typedef struct _GalaxyParticleSystem GalaxyParticleSystem;
typedef struct _GalaxyParticleSystemClass GalaxyParticleSystemClass;

typedef struct _GalaxyParticleSystemPrivate GalaxyParticleSystemPrivate;

struct _GalaxyParticleSystem
{
	GObject parent_instance;
	
	/*< private >*/
	GalaxyParticleSystemPrivate *priv;
};

struct _GalaxyParticleSystemClass
{
	GObjectClass parent_class;
};

GType galaxy_particle_system_get_type(void);

GalaxyParticleSystem *galaxy_particle_system_new();

void galaxy_particle_system_update(GalaxyParticleSystem *self, gint ms_since_last_paint);
void galaxy_particle_system_draw(GalaxyParticleSystem *self);

G_END_DECLS

#endif /* _GALAXY_PARTICLE_H_ */


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

#ifndef _DUCKIEGALAXY_PARTICLE_H_
#define _DUCKIEGALAXY_PARTICLE_H_

G_BEGIN_DECLS

#define DUCKIEGALAXY_TYPE_PARTICLE_SYSTEM            (duckiegalaxy_particle_system_get_type())
#define DUCKIEGALAXY_PARTICLE_SYSTEM(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), DUCKIEGALAXY_TYPE_PARTICLE_SYSTEM, DuckieGalaxyParticleSystem))
#define DUCKIEGALAXY_IS_PARTICLE_SYSTEM(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), DUCKIEGALAXY_TYPE_PARTICLE_SYSTEM))
#define DUCKIEGALAXY_PARTICLE_SYSTEM_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), DUCKIEGALAXY_TYPE_PARTICLE_SYSTEM, DuckieGalaxyParticleSystemClass))
#define DUCKIEGALAXY_IS_PARTICLE_SYSTEM_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), DUCKIEGALAXY_TYPE_PARTICLE_SYSTEM))
#define DUCKIEGALAXY_PARTICLE_SYSTEM_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), DUCKIEGALAXY_TYPE_PARTICLE_SYSTEM, DuckieGalaxyParticleSystemClass))

typedef struct _DuckieGalaxyParticleSystem DuckieGalaxyParticleSystem;
typedef struct _DuckieGalaxyParticleSystemClass DuckieGalaxyParticleSystemClass;

typedef struct _DuckieGalaxyParticleSystemPrivate DuckieGalaxyParticleSystemPrivate;

struct _DuckieGalaxyParticleSystem
{
	GObject parent_instance;

	/*< private >*/
	DuckieGalaxyParticleSystemPrivate *priv;
};

struct _DuckieGalaxyParticleSystemClass
{
	GObjectClass parent_class;
};

GType duckiegalaxy_particle_system_get_type(void);

DuckieGalaxyParticleSystem *duckiegalaxy_particle_system_new();

void duckiegalaxy_particle_system_update(DuckieGalaxyParticleSystem *self, gint ms_since_last_paint);
void duckiegalaxy_particle_system_draw(DuckieGalaxyParticleSystem *self);

G_END_DECLS

#endif /* _DUCKIEGALAXY_PARTICLE_H_ */

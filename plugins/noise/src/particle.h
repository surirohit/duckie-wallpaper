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
 *
 */

#ifndef _NOISE_PARTICLE_H_
#define _NOISE_PARTICLE_H_

G_BEGIN_DECLS

#define NOISE_TYPE_PARTICLE_SYSTEM            (noise_particle_system_get_type())
#define NOISE_PARTICLE_SYSTEM(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), NOISE_TYPE_PARTICLE_SYSTEM, NoiseParticleSystem))
#define NOISE_IS_PARTICLE_SYSTEM(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), NOISE_TYPE_PARTICLE_SYSTEM))
#define NOISE_PARTICLE_SYSTEM_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), NOISE_TYPE_PARTICLE_SYSTEM, NoiseParticleSystemClass))
#define NOISE_IS_PARTICLE_SYSTEM_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), NOISE_TYPE_PARTICLE_SYSTEM))
#define NOISE_PARTICLE_SYSTEM_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), NOISE_TYPE_PARTICLE_SYSTEM, NoiseParticleSystemClass))

typedef struct _NoiseParticleSystem NoiseParticleSystem;
typedef struct _NoiseParticleSystemClass NoiseParticleSystemClass;

typedef struct _NoiseParticleSystemPrivate NoiseParticleSystemPrivate;

struct _NoiseParticleSystem
{
	GObject parent_instance;

	/*< private >*/
	NoiseParticleSystemPrivate *priv;
};

struct _NoiseParticleSystemClass
{
	GObjectClass parent_class;
};

GType noise_particle_system_get_type(void);

NoiseParticleSystem *noise_particle_system_new();

void noise_particle_system_update(NoiseParticleSystem *self, gint ms_since_last_paint);
void noise_particle_system_draw(NoiseParticleSystem *self, LwMatrix *matrix);

G_END_DECLS

#endif /* _NOISE_PARTICLE_H_ */


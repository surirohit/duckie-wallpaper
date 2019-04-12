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

#ifndef _NEXUS_PARTICLE_H_
#define _NEXUS_PARTICLE_H_

G_BEGIN_DECLS

typedef enum
{
	NexusGlowTypeRadial = 0,
	NexusGlowTypeSquare,
	NexusGlowTypeSpiral,
	NexusGlowTypeConcentricCircles
} NexusGlowType;

#define NEXUS_TYPE_PARTICLE_SYSTEM            (nexus_particle_system_get_type())
#define NEXUS_PARTICLE_SYSTEM(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), NEXUS_TYPE_PARTICLE_SYSTEM, NexusParticleSystem))
#define NEXUS_IS_PARTICLE_SYSTEM(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), NEXUS_TYPE_PARTICLE_SYSTEM))
#define NEXUS_PARTICLE_SYSTEM_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), NEXUS_TYPE_PARTICLE_SYSTEM, NexusParticleSystemClass))
#define NEXUS_IS_PARTICLE_SYSTEM_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), NEXUS_TYPE_PARTICLE_SYSTEM))
#define NEXUS_PARTICLE_SYSTEM_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), NEXUS_TYPE_PARTICLE_SYSTEM, NexusParticleSystemClass))

typedef struct _NexusParticleSystem NexusParticleSystem;
typedef struct _NexusParticleSystemClass NexusParticleSystemClass;

typedef struct _NexusParticleSystemPrivate NexusParticleSystemPrivate;

struct _NexusParticleSystem
{
	GObject parent_instance;

	/*< private >*/
	NexusParticleSystemPrivate *priv;
};

struct _NexusParticleSystemClass
{
	GObjectClass parent_class;
};

GType nexus_particle_system_get_type(void);

NexusParticleSystem *nexus_particle_system_new();

void nexus_particle_system_update(NexusParticleSystem *self, gint ms_since_last_paint);
void nexus_particle_system_draw(NexusParticleSystem *self, gint size);

G_END_DECLS

#endif /* _NEXUS_PARTICLE_H_ */


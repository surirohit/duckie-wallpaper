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

#include <math.h>
#include <glib-object.h>
#include <gdk/gdk.h>
#include <livewallpaper/core.h>

#include "particle.h"

#define NOISE_RESOURCE "/net/launchpad/livewallpaper/plugins/noise/"

typedef struct
{
	/* Position, speed and size of the particle */
	gfloat x, y;
	gfloat speed;
	gfloat size;

	/* A random value which makes the movement of a particle unique */
	gfloat wander;

	/* The total lifetime of the particle in milliseconds */
	int lifetime;

	/* The time in milliseconds the particle is alive */
	int alive;

	/* The alpha value of the particle */
	gfloat alpha;
} Particle;

struct _NoiseParticleSystemPrivate
{
	guint particle_count;
	LwRange particle_size;
	gint fade_time;
	LwRange lifetime;

	GArray *particles;
	gfloat *vertices;
	gfloat *alpha;
	gfloat *size;

	LwBuffer *vertex_buffer;
	LwBuffer *alpha_buffer;
	LwBuffer *size_buffer;

	LwProgram *prog;
	LwTexture *texture;
};

enum
{
    PROP_0,

    PROP_PARTICLE_COUNT,
    PROP_PARTICLE_SIZE,
    PROP_FADE_TIME,
    PROP_LIFETIME,

    N_PROPERTIES
};

static GParamSpec *obj_properties[N_PROPERTIES] = {NULL, };

G_DEFINE_TYPE(NoiseParticleSystem, noise_particle_system, G_TYPE_OBJECT)


static void noise_particle_system_set_particle_count(NoiseParticleSystem *self, guint count);

NoiseParticleSystem*
noise_particle_system_new()
{
	NoiseParticleSystem *self = g_object_new(NOISE_TYPE_PARTICLE_SYSTEM, NULL);

	/* Load particle texture */
	self->priv->texture = lw_texture_new_from_resource (NOISE_RESOURCE "images/particle.png");

	/* Load program */
	self->priv->prog = g_object_new(LW_TYPE_PROGRAM, NULL);

    lw_program_create_and_attach_shader_from_resource (self->priv->prog, "resource://"NOISE_RESOURCE "shader/vert.glsl", GL_VERTEX_SHADER);
    lw_program_create_and_attach_shader_from_resource (self->priv->prog, "resource://"NOISE_RESOURCE "shader/frag.glsl", GL_FRAGMENT_SHADER);
	lw_program_link(self->priv->prog);

	return self;
}

static void
noise_particle_system_set_property(GObject *object,
                                   guint property_id,
                                   const GValue *value,
                                   GParamSpec *pspec)
{
	NoiseParticleSystem *self = NOISE_PARTICLE_SYSTEM(object);

	switch(property_id)
	{
		case PROP_PARTICLE_COUNT:
			noise_particle_system_set_particle_count(self, g_value_get_uint(value));
			break;

		case PROP_PARTICLE_SIZE:
			self->priv->particle_size = *((LwRange*)g_value_get_boxed(value));
			break;

		case PROP_FADE_TIME:
			self->priv->fade_time = g_value_get_uint(value);
			break;

		case PROP_LIFETIME:
			self->priv->lifetime = *((LwRange*)g_value_get_boxed(value));
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
			break;
	}
}

static void
noise_particle_system_get_property(GObject *object,
                                   guint property_id,
                                   GValue *value,
                                   GParamSpec *pspec)
{
	NoiseParticleSystem *self = NOISE_PARTICLE_SYSTEM(object);

	switch(property_id)
	{
		case PROP_PARTICLE_COUNT:
			g_value_set_uint(value, self->priv->particle_count);
			break;

		case PROP_PARTICLE_SIZE:
			g_value_set_boxed(value, &self->priv->particle_size);
			break;

		case PROP_FADE_TIME:
			g_value_set_uint(value, self->priv->fade_time);
			break;

		case PROP_LIFETIME:
			g_value_set_boxed(value, &self->priv->lifetime);
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
			break;
	}
}

static inline void
noise_particle_system_init_particle(NoiseParticleSystem *self, Particle *particle)
{
	particle->x = randf();
	particle->y = randf();

	particle->speed = rand2f(0.000005f, 0.0001f);
	particle->size = lw_range_randf(self->priv->particle_size);
	particle->wander = rand2f(0.5f, 1.5f);

	particle->lifetime = 1000 *lw_range_rand(self->priv->lifetime)
	                   + 2 * self->priv->fade_time;
	particle->alive = 0;
	particle->alpha = rand2f(0.1f, 1.0f);
}

static void
noise_particle_system_set_particle_count(NoiseParticleSystem *self, guint count)
{
	g_array_set_size(self->priv->particles, count);

	/* Append particles */
	if(self->priv->particle_count < count)
	{
		guint i;

		for(i = self->priv->particle_count; i < count; i++)
		{
			Particle *particle = &g_array_index(self->priv->particles, Particle, i);
			noise_particle_system_init_particle(self, particle);
		}
	}

	self->priv->vertices = g_realloc(self->priv->vertices, 2 * count * sizeof(gfloat));
	self->priv->alpha = g_realloc(self->priv->alpha, count * sizeof(gfloat));
	self->priv->size = g_realloc(self->priv->size, count * sizeof(gfloat));
	self->priv->particle_count = count;
}

void
noise_particle_system_update(NoiseParticleSystem *self, gint ms_since_last_paint)
{
	gfloat *v = self->priv->vertices;
	guint i;

	for(i = 0; i < self->priv->particle_count; i++)
	{
		float noise, angle, speed;
    	Particle *p = &g_array_index(self->priv->particles, Particle, i);

		p->alive += ms_since_last_paint;
		if(p->lifetime < p->alive || p->x < -0.1f || p->x > 1.1f || p->y < -0.1f || p->y > 1.1f)
		{
			noise_particle_system_init_particle(self, p);
		}

		/* Update position */
    	noise = lw_simplex_noise_2f(p->x, p->y);
		angle = LW_2PI * noise * p->wander;
		speed = p->speed * ms_since_last_paint * noise + 0.00005f;

		p->x += cos(angle) * speed * 0.33;
		p->y += sin(angle) * speed * 0.33;

		v[0] = p->x;
		v[1] = p->y;
		v += 2;

		/* Update alpha */
		if(p->alive < self->priv->fade_time)
		{
			/* Fade in */
			self->priv->alpha[i] = p->alpha * p->alive / self->priv->fade_time;
		}
		else if(p->lifetime - p->alive < self->priv->fade_time)
		{
			/* Fade out */
			self->priv->alpha[i] = p->alpha * (p->lifetime - p->alive) / self->priv->fade_time;
		}
		else
		{
			self->priv->alpha[i] = p->alpha;
		}

		/* Update size */
		self->priv->size[i] = p->size;
	}

	/* Put data into the buffers */
	lw_buffer_set_data(self->priv->vertex_buffer,
	                   2 * self->priv->particle_count * sizeof(gfloat),
	                   self->priv->vertices);
	lw_buffer_set_data(self->priv->alpha_buffer,
	                   self->priv->particle_count * sizeof(gfloat),
	                   self->priv->alpha);
	lw_buffer_set_data(self->priv->size_buffer,
	                   self->priv->particle_count * sizeof(gfloat),
	                   self->priv->size);
}

void
noise_particle_system_draw(NoiseParticleSystem *self, LwMatrix *matrix)
{
	if(!self->priv->prog) return;

	glEnable(GL_POINT_SPRITE);

	glPointParameterf(GL_POINT_FADE_THRESHOLD_SIZE, 60.0f);
	/*glPointParameterf(GL_POINT_SIZE_MIN, 0.1f);*/

	/* Render particles using buffers */
	lw_program_enable(self->priv->prog);

	lw_program_set_attribute(self->priv->prog, "position",
	                         LW_GLSL_TYPE_VEC2,
	                         self->priv->vertex_buffer);
	lw_program_set_attribute(self->priv->prog, "alpha",
	                         LW_GLSL_TYPE_FLOAT,
	                         self->priv->alpha_buffer);
	lw_program_set_attribute(self->priv->prog, "size",
	                         LW_GLSL_TYPE_FLOAT,
	                         self->priv->size_buffer);
	lw_program_set_matrix(self->priv->prog, "mvp_matrix", matrix);

	if(self->priv->texture)
		lw_program_set_texture(self->priv->prog, "texture",
		                       self->priv->texture);


	glDrawArrays(GL_POINTS, 0, self->priv->particle_count);


	lw_program_disable(self->priv->prog);
	if(self->priv->texture)
		lw_texture_unbind(self->priv->texture);

	glDisable(GL_POINT_SPRITE);
}

static void
noise_particle_system_init(NoiseParticleSystem *self)
{
    self->priv = G_TYPE_INSTANCE_GET_PRIVATE(self, NOISE_TYPE_PARTICLE_SYSTEM,
                                             NoiseParticleSystemPrivate);

	self->priv->particle_count = 0;
	self->priv->particle_size.min = 10;
	self->priv->particle_size.max = 50;
	self->priv->fade_time = 1000;
	self->priv->lifetime.min = 30;
	self->priv->lifetime.max = 80;

	self->priv->particles = g_array_new(FALSE, TRUE, sizeof(Particle));

	/* Create buffers */
	self->priv->vertex_buffer = lw_buffer_new(GL_STREAM_DRAW);
	self->priv->alpha_buffer = lw_buffer_new(GL_STREAM_DRAW);
	self->priv->size_buffer = lw_buffer_new(GL_STREAM_DRAW);
}

static void
noise_particle_system_dispose(GObject *object)
{
	NoiseParticleSystem *self = NOISE_PARTICLE_SYSTEM(object);

	g_clear_object(&self->priv->prog);
	g_clear_object(&self->priv->texture);

	g_clear_object(&self->priv->vertex_buffer);
	g_clear_object(&self->priv->alpha_buffer);
	g_clear_object(&self->priv->size_buffer);

	/* Chain up to the parent class */
	G_OBJECT_CLASS(noise_particle_system_parent_class)->dispose(object);
}

static void
noise_particle_system_finalize(GObject *object)
{
	NoiseParticleSystem *self = NOISE_PARTICLE_SYSTEM(object);

	g_array_free(self->priv->particles, TRUE);
	g_free(self->priv->vertices);
	g_free(self->priv->alpha);
	g_free(self->priv->size);

	/* Chain up to the parent class */
	G_OBJECT_CLASS(noise_particle_system_parent_class)->finalize(object);
}

static void
noise_particle_system_class_init(NoiseParticleSystemClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

	gobject_class->set_property = noise_particle_system_set_property;
	gobject_class->get_property = noise_particle_system_get_property;
	gobject_class->dispose = noise_particle_system_dispose;
	gobject_class->finalize = noise_particle_system_finalize;

	g_type_class_add_private(klass, sizeof(NoiseParticleSystemPrivate));

    obj_properties[PROP_PARTICLE_COUNT] = g_param_spec_uint ("particle-count", "Number of particles", "Number of particles",                              0,  300,    0, G_PARAM_READWRITE);
    obj_properties[PROP_PARTICLE_SIZE]  = g_param_spec_boxed("particle-size",  "Particle Size",       "Size of a particle",                               LW_TYPE_RANGE, G_PARAM_READWRITE);
    obj_properties[PROP_FADE_TIME]      = g_param_spec_uint ("fade-time",      "Fade time",           "The time a particle needs to appear or disappear", 0, 5000, 1000, G_PARAM_READWRITE);
    obj_properties[PROP_LIFETIME]       = g_param_spec_boxed("lifetime",       "Lifetime",            "Lifetime of a particle in seconds",                LW_TYPE_RANGE, G_PARAM_READWRITE);

	g_object_class_install_properties(gobject_class, N_PROPERTIES, obj_properties);
}


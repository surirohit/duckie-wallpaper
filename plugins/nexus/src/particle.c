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

#include <glib-object.h>
#include <gdk/gdk.h>
#include <livewallpaper/core.h>

#include "particle.h"

#define NEXUS_RESOURCE "/net/launchpad/livewallpaper/plugins/nexus/"

typedef struct _Pulse Pulse;

struct _Pulse
{
	/* Position of the pulse. */
	gfloat x, y;

	/* Velocity of the pulse. */
	gfloat vx, vy;

	/* Length of the pulse. */
	gfloat length;

	/* Color of the pulse */
	GdkRGBA color;

	/* Pulse appears after delay milliseconds on the wallpaper. */
	gint delay;
};

struct _NexusParticleSystemPrivate
{
	guint pulse_count;
	guint pulse_size;
	LwRange pulse_length;

	guint max_delay;

	GArray *pulses;

	guint glow_type;
	LwTexture *glowTexture;
	LwTexture *trailTexture;

	GdkRGBA colors[4];
	gboolean random_colors;
};

enum
{
    PROP_0,

    PROP_PULSE_COUNT,
    PROP_PULSE_SIZE,
    PROP_PULSE_LENGTH,

    PROP_MAX_DELAY,

    PROP_GLOW_TYPE,

    PROP_COLOR_1,
    PROP_COLOR_2,
    PROP_COLOR_3,
    PROP_COLOR_4,

	PROP_RANDOM_COLORS,

    N_PROPERTIES
};

static GParamSpec *obj_properties[N_PROPERTIES] = {NULL, };

G_DEFINE_TYPE(NexusParticleSystem, nexus_particle_system, G_TYPE_OBJECT)


static void nexus_particle_system_set_pulse_count(NexusParticleSystem *self, guint count);
static inline void nexus_particle_system_reinit_pulses(NexusParticleSystem *self);
static void nexus_particle_system_set_glow_type(NexusParticleSystem *self, guint type);

NexusParticleSystem*
nexus_particle_system_new()
{
	NexusParticleSystem *self = g_object_new(NEXUS_TYPE_PARTICLE_SYSTEM, NULL);

	/* Load trail texture? */
	self->priv->trailTexture = lw_texture_new_from_resource (NEXUS_RESOURCE "images/trail.png");

	/* Load glow texture? */
	nexus_particle_system_set_glow_type(self, self->priv->glow_type);

	return self;
}

static void
nexus_particle_system_set_property(GObject *object,
                                   guint property_id,
                                   const GValue *value,
                                   GParamSpec *pspec)
{
	NexusParticleSystem *self = NEXUS_PARTICLE_SYSTEM(object);

	switch(property_id)
	{
		case PROP_PULSE_COUNT:
			nexus_particle_system_set_pulse_count(self, g_value_get_uint(value));
			break;

		case PROP_PULSE_SIZE:
			self->priv->pulse_size = g_value_get_uint(value);
			break;

		case PROP_PULSE_LENGTH:
			self->priv->pulse_length = *((LwRange*)g_value_get_boxed(value));
			nexus_particle_system_reinit_pulses(self);
			break;

		case PROP_MAX_DELAY:
			self->priv->max_delay = g_value_get_uint(value);
			break;

		case PROP_GLOW_TYPE:
			nexus_particle_system_set_glow_type(self, g_value_get_uint(value));
			break;

		case PROP_COLOR_1:
		case PROP_COLOR_2:
		case PROP_COLOR_3:
		case PROP_COLOR_4:
			self->priv->colors[property_id - PROP_COLOR_1] = *((GdkRGBA*)g_value_get_boxed(value));
			break;

		case PROP_RANDOM_COLORS:
			self->priv->random_colors = g_value_get_boolean(value);
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
			break;
	}
}

static void
nexus_particle_system_get_property(GObject *object,
                                   guint property_id,
                                   GValue *value,
                                   GParamSpec *pspec)
{
	NexusParticleSystem *self = NEXUS_PARTICLE_SYSTEM(object);

	switch(property_id)
	{
		case PROP_PULSE_COUNT:
			g_value_set_uint(value, self->priv->pulse_count);
			break;

		case PROP_PULSE_SIZE:
			g_value_set_uint(value, self->priv->pulse_size);
			break;

		case PROP_PULSE_LENGTH:
			g_value_set_boxed(value, &self->priv->pulse_length);
			break;

		case PROP_MAX_DELAY:
			g_value_set_uint(value, self->priv->max_delay);
			break;

		case PROP_GLOW_TYPE:
			g_value_set_uint(value, self->priv->glow_type);
			break;

		case PROP_COLOR_1:
		case PROP_COLOR_2:
		case PROP_COLOR_3:
		case PROP_COLOR_4:
			g_value_set_boxed(value, &self->priv->colors[property_id - PROP_COLOR_1]);
			break;

		case PROP_RANDOM_COLORS:
			g_value_set_boolean(value, self->priv->random_colors);
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
			break;
	}
}

static void
nexus_particle_system_init_pulse(NexusParticleSystem *self, Pulse *pulse)
{
	gfloat rd = randf();
	pulse->length = lw_range_randf(self->priv->pulse_length);
	pulse->delay = rand() % self->priv->max_delay;

	if (self->priv->random_colors)
	{
		pulse->color.red   = randf();
		pulse->color.green = randf();
		pulse->color.blue  = randf();
		pulse->color.alpha = 1.0;
	}
	else
		pulse->color = self->priv->colors[rand() % 4];

	if(rd > 0.5f)
	{
		pulse->vy = 0.0f;
		pulse->y = randf();

		if(rd > 0.75f)
		{
			/* Left to right */
			pulse->x = 0.0f;
			pulse->vx = pulse->length;
		}
		else
		{
			/* Right to left */
			pulse->x = 1.0f;
			pulse->vx = -pulse->length;
		}
	}
	else
	{
		pulse->vx = 0.0f;
		pulse->x = randf();

		if(rd > 0.25f)
		{
			/* Top to bottom */
			pulse->y = 1.0f;
			pulse->vy = -pulse->length;
		}
		else
		{
			/* Bottom to top */
			pulse->y = 0.0f;
			pulse->vy = pulse->length;
		}
	}
}

static void
nexus_particle_system_set_pulse_count(NexusParticleSystem *self, guint count)
{
	g_array_set_size(self->priv->pulses, count);

	/* Append pulses */
	if(self->priv->pulse_count < count)
	{
		guint i;
		for(i = self->priv->pulse_count; i < count; i++)
		{
			Pulse *pulse = &g_array_index(self->priv->pulses, Pulse, i);
			nexus_particle_system_init_pulse(self, pulse);
		}
	}

	self->priv->pulse_count = count;
}

static inline void
nexus_particle_system_reinit_pulses(NexusParticleSystem *self)
{
	guint i;
	for(i = 0; i < self->priv->pulse_count; i++)
	{
		Pulse *pulse = &g_array_index(self->priv->pulses, Pulse, i);
		nexus_particle_system_init_pulse(self, pulse);
	}
}

static void
nexus_particle_system_set_glow_type(NexusParticleSystem *self, guint type)
{
    g_clear_object(&self->priv->glowTexture);

    switch (type)
    {
        case NexusGlowTypeSquare:
            self->priv->glowTexture = lw_texture_new_from_resource (NEXUS_RESOURCE "images/glow-square.png");
            break;

        case NexusGlowTypeSpiral:
            self->priv->glowTexture = lw_texture_new_from_resource (NEXUS_RESOURCE "images/glow-spiral.png");
            break;

        case NexusGlowTypeConcentricCircles:
            self->priv->glowTexture = lw_texture_new_from_resource (NEXUS_RESOURCE "images/glow-concentric-cirles.png");
            break;

        case NexusGlowTypeRadial:
        default:
            self->priv->glowTexture = lw_texture_new_from_resource (NEXUS_RESOURCE "images/glow-radial.png");
            break;
    }

	self->priv->glow_type = type;
}

void
nexus_particle_system_update(NexusParticleSystem *self, gint ms_since_last_paint)
{
	guint i;
	for(i = 0; i < self->priv->pulse_count; i++)
	{
		Pulse *pulse = &g_array_index(self->priv->pulses, Pulse, i);

		if(pulse->delay > 0)
		{
			pulse->delay -= ms_since_last_paint;
			continue;
		}

		if(pulse->vy == 0.0f)
		{
			pulse->x += pulse->vx * ms_since_last_paint / 1000;

			if ((pulse->vx < 0 && pulse->x < pulse->vx) ||
				(pulse->vx > 0 && pulse->x > 1.0f + pulse->vx))
				nexus_particle_system_init_pulse(self, pulse);
		}
		else
		{
			pulse->y += pulse->vy * ms_since_last_paint / 1000;

			if ((pulse->vy < 0 && pulse->y < pulse->vy) ||
				(pulse->vy > 0 && pulse->y > 1.0f + pulse->vy))
				nexus_particle_system_init_pulse(self, pulse);
		}
	}
}

/* Draws quad for horizontal trail. */
#define DRAW_QUAD_H(x1, y1, x2, y2)	\
	glBegin(GL_QUADS);				\
		glTexCoord2d(0.0f, 0.0f);	\
		glVertex2f(x1, y1);			\
		glTexCoord2d(0.0f, 1.0f);	\
		glVertex2f(x1, y2);			\
		glTexCoord2d(1.0f, 1.0f);	\
		glVertex2f(x2, y2);			\
		glTexCoord2d(1.0f, 0.0f);	\
		glVertex2f(x2, y1);			\
	glEnd()

/* Draws quad for vertical trail (Different texture coordinates than DRAW_QUAD_H). */
#define DRAW_QUAD_V(x1, y1, x2, y2)	\
	glBegin(GL_QUADS);				\
		glTexCoord2d(0.0f, 1.0f);	\
		glVertex2f(x1, y1);			\
		glTexCoord2d(1.0f, 1.0f);	\
		glVertex2f(x1, y2);			\
		glTexCoord2d(1.0f, 0.0f);	\
		glVertex2f(x2, y2);			\
		glTexCoord2d(0.0f, 0.0f);	\
		glVertex2f(x2, y1);			\
	glEnd()

void
nexus_particle_system_draw(NexusParticleSystem *self, gint size)
{
	gfloat half_glow_size = (4.0f * self->priv->pulse_size) / (2.0f * size);
	gfloat half_pulse_size = self->priv->pulse_size / (2.0f * size);
	guint i;

	for(i = 0; i < self->priv->pulse_count; i++)
	{
		Pulse *pulse = &g_array_index(self->priv->pulses, Pulse, i);

		if(pulse->delay > 0) continue;

		/* Set color */
		glColor4f(pulse->color.red,
				  pulse->color.green,
				  pulse->color.blue,
				  pulse->color.alpha);

		/* Draw glow */
		lw_texture_enable(self->priv->glowTexture);
		DRAW_QUAD_H(pulse->x - half_glow_size, pulse->y - half_glow_size,
		            pulse->x + half_glow_size, pulse->y + half_glow_size);
		lw_texture_disable(self->priv->glowTexture);

		/* Draw trail */
		#define x1 (pulse->x - half_pulse_size)
		#define y1 (pulse->y - half_pulse_size)
		#define x2 (pulse->x + half_pulse_size)
		#define y2 (pulse->y + half_pulse_size)

		lw_texture_enable(self->priv->trailTexture);
		if(pulse->vx > 0)       /* Left to right */
		{
			DRAW_QUAD_H(x2, y2, x1 - pulse->length, y1);
		}
		else if(pulse->vx < 0)  /* Right to left */
		{
			DRAW_QUAD_H(x1, y1, x2 + pulse->length, y2);
		}
		else if(pulse->vy > 0)  /* Bottom to top */
		{
			DRAW_QUAD_V(x2, y2, x1, y1 - pulse->length);
		}
		else                    /* Top to bottom */
		{
			DRAW_QUAD_V(x1, y1, x2, y2 + pulse->length);
		}
		lw_texture_disable(self->priv->trailTexture);

		#undef x1
		#undef y1
		#undef x2
		#undef y2
	}
}

static void
nexus_particle_system_init(NexusParticleSystem *self)
{
    self->priv = G_TYPE_INSTANCE_GET_PRIVATE(self, NEXUS_TYPE_PARTICLE_SYSTEM,
                                             NexusParticleSystemPrivate);

    self->priv->pulse_count = 0;
    self->priv->pulse_size = 8;
    self->priv->pulse_length.max = 0.4;
    self->priv->pulse_length.min = 0.1;

    self->priv->max_delay = 5000;

    self->priv->pulses = g_array_new(FALSE, TRUE, sizeof(Pulse));

	self->priv->glow_type = NexusGlowTypeRadial;
    self->priv->glowTexture = NULL;
    self->priv->trailTexture = NULL;
}

static void
nexus_particle_system_dispose(GObject *object)
{
	NexusParticleSystem *self = NEXUS_PARTICLE_SYSTEM(object);

	g_clear_object(&self->priv->glowTexture);
	g_clear_object(&self->priv->trailTexture);

	/* Chain up to the parent class */
	G_OBJECT_CLASS(nexus_particle_system_parent_class)->dispose(object);
}

static void
nexus_particle_system_finalize(GObject *object)
{
	NexusParticleSystem *self = NEXUS_PARTICLE_SYSTEM(object);

	g_array_free(self->priv->pulses, TRUE);

	/* Chain up to the parent class */
	G_OBJECT_CLASS(nexus_particle_system_parent_class)->finalize(object);
}

static void
nexus_particle_system_class_init(NexusParticleSystemClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

	gobject_class->set_property = nexus_particle_system_set_property;
	gobject_class->get_property = nexus_particle_system_get_property;
	gobject_class->dispose = nexus_particle_system_dispose;
	gobject_class->finalize = nexus_particle_system_finalize;

	g_type_class_add_private(klass, sizeof(NexusParticleSystemPrivate));

	obj_properties[PROP_PULSE_COUNT]   = g_param_spec_uint   ("pulse-count",   "Number of pulses",    "Number of pulses",                     0,   300,    0, G_PARAM_READWRITE);
	obj_properties[PROP_PULSE_SIZE]    = g_param_spec_uint   ("pulse-size",    "Pulse size",          "Size of a pulse",                      8,    64,   16, G_PARAM_READWRITE);
	obj_properties[PROP_PULSE_LENGTH]  = g_param_spec_boxed  ("pulse-length",  "Pulse length",        "Length of a pulse",                     LW_TYPE_RANGE, G_PARAM_READWRITE);
	obj_properties[PROP_MAX_DELAY]     = g_param_spec_uint   ("max-delay",     "Max delay",           "Max delay of a pulse in milliseconds", 1, 10000, 5000, G_PARAM_READWRITE);

	obj_properties[PROP_GLOW_TYPE]     = g_param_spec_uint   ("glow-type",     "Glow type",           "The shape of the glow",     0, 3, NexusGlowTypeRadial, G_PARAM_READWRITE);
	obj_properties[PROP_COLOR_1]       = g_param_spec_boxed  ("color1",        "Color 1",             "Pulse color",                           GDK_TYPE_RGBA, G_PARAM_READWRITE);
	obj_properties[PROP_COLOR_2]       = g_param_spec_boxed  ("color2",        "Color 2",             "Pulse color",                           GDK_TYPE_RGBA, G_PARAM_READWRITE);
	obj_properties[PROP_COLOR_3]       = g_param_spec_boxed  ("color3",        "Color 3",             "Pulse color",                           GDK_TYPE_RGBA, G_PARAM_READWRITE);
	obj_properties[PROP_COLOR_4]       = g_param_spec_boxed  ("color4",        "Color 4",             "Pulse color",                           GDK_TYPE_RGBA, G_PARAM_READWRITE);
	obj_properties[PROP_RANDOM_COLORS] = g_param_spec_boolean("random-colors", "Random pulse colors", "Use random pulse colors",               FALSE,         G_PARAM_READWRITE);

	g_object_class_install_properties(gobject_class, N_PROPERTIES, obj_properties);
}


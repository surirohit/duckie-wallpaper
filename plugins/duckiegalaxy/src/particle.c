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

#include <math.h>
#include <stdlib.h>
#include <glib-object.h>
#include <livewallpaper/core.h>
#include "particle.h"

#define DUCKIEGALAXY_IMG "/net/launchpad/livewallpaper/plugins/duckiegalaxy/images/"

typedef struct _Star Star;

struct _Star
{
	/*
	 * Those values needed for the general parametic form of an
	 * ellipse. Phi is the angle between the x-axis and the major
	 * axis of the ellipse.
	 */
	gfloat cos_phi, sin_phi;

	/* The current angle/position of this star. */
	gfloat angle;

	/* The distance from the center of the galaxy to this star. */
	gfloat distance;

	/* the angular velocity of the star in 1/ms. */
	gfloat speed;
};

/* 0 (line) -> 1.0f (circle) */
#define ELLIPSE_RATIO .885f

struct _DuckieGalaxyParticleSystemPrivate
{
	guint star_count;
	guint star_size;
	gfloat speed_ratio;
	gboolean draw_streaks;
	GdkRGBA star_color;

	GArray *stars;
	gfloat *vertices;

	LwTexture *starTexture;
};

enum
{
    PROP_0,

    PROP_STAR_COUNT,
    PROP_STAR_SIZE,
    PROP_SPEED_RATIO,
    PROP_DRAW_STREAKS,
    PROP_STAR_COLOR,

    N_PROPERTIES
};

static GParamSpec *obj_properties[N_PROPERTIES] = {NULL, };

G_DEFINE_TYPE(DuckieGalaxyParticleSystem, duckiegalaxy_particle_system, G_TYPE_OBJECT)


static void duckiegalaxy_particle_system_set_star_count(DuckieGalaxyParticleSystem *self, guint count);
static void duckiegalaxy_particle_system_update_star_texture(DuckieGalaxyParticleSystem *self);

DuckieGalaxyParticleSystem*
duckiegalaxy_particle_system_new()
{
	DuckieGalaxyParticleSystem *self = g_object_new(DUCKIEGALAXY_TYPE_PARTICLE_SYSTEM, NULL);

	duckiegalaxy_particle_system_update_star_texture(self);

	return self;
}

static void
duckiegalaxy_particle_system_set_property(GObject *object,
                                    guint property_id,
                                    const GValue *value,
                                    GParamSpec *pspec)
{
	DuckieGalaxyParticleSystem *self = DUCKIEGALAXY_PARTICLE_SYSTEM(object);

	switch(property_id)
	{
		case PROP_STAR_COUNT:
			duckiegalaxy_particle_system_set_star_count(self, g_value_get_uint(value));
			break;

		case PROP_STAR_SIZE:
			self->priv->star_size = g_value_get_uint(value);
			break;

		case PROP_SPEED_RATIO:
			self->priv->speed_ratio = g_value_get_double(value);
			break;

		case PROP_DRAW_STREAKS:
            self->priv->draw_streaks =  g_value_get_boolean(value);
            duckiegalaxy_particle_system_update_star_texture(self);
			break;

        case PROP_STAR_COLOR:
			self->priv->star_color = *((GdkRGBA*)g_value_get_boxed(value));
            break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
			break;
	}
}

static void
duckiegalaxy_particle_system_get_property(GObject *object,
                                    guint property_id,
                                    GValue *value,
                                    GParamSpec *pspec)
{
	DuckieGalaxyParticleSystem *self = DUCKIEGALAXY_PARTICLE_SYSTEM(object);

	switch(property_id)
	{
		case PROP_STAR_COUNT:
			g_value_set_uint(value, self->priv->star_count);
			break;

		case PROP_STAR_SIZE:
			g_value_set_uint(value, self->priv->star_size);
			break;

		case PROP_SPEED_RATIO:
			g_value_set_double(value, self->priv->speed_ratio);
			break;

		case PROP_DRAW_STREAKS:
			g_value_set_boolean(value, self->priv->draw_streaks);
			break;

        case PROP_STAR_COLOR:
            g_value_set_boxed(value, &self->priv->star_color);
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
			break;
	}
}

/*
 * Gaussian random generator
 * Using the polar form of the Box-Muller transformation
 */
static gfloat
randng(void)
{
	static gfloat y;
	static gboolean have_next = FALSE;

	if (! have_next)
	{
		gfloat x1, x2, w;

		do
		{
			x1 = 2.0 * randf() - 1.0;
			x2 = 2.0 * randf() - 1.0;
			w = x1 * x1 + x2 * x2;
		} while(w >= 1.0);

		w = sqrt(-2.0 * log (w) / w);
		y = x2 * w;
		have_next = TRUE;

		return x1 * w;
	}
	have_next = FALSE;
	return y;
}

static inline gfloat
duckiegalaxy_particle_system_init_star(Star *star)
{
	star->angle = rand1f(-LW_2PI);

	star->distance = fabs(randng()) * 0.5f + rand2f(0.001f, 0.05f);

	/*
	 * Phi depends on the distance of the star to get density waves.
	 * See http://en.wikipedia.org/wiki/Density_wave_theory
	 */
	star->cos_phi = cos(star->distance * LW_2PI);
	star->sin_phi = sin(star->distance * LW_2PI);

	/* The angular velocity of the star in 1/ms */
	star->speed = -rand2f(0.00003f, 0.000045f) / star->distance;

	return ((1 - star->distance) * 0.25f * randng() /
            (LW_PI + LW_2PI * pow(star->distance, 2))   *
            ((star->distance < 0.2f) ? 0.8 : 1));
}

static void
duckiegalaxy_particle_system_set_star_count(DuckieGalaxyParticleSystem *self, guint count)
{
    guint i;
	g_array_set_size(self->priv->stars, count);
    self->priv->vertices = g_realloc(self->priv->vertices, 3 * count * sizeof(gfloat));

    /* Append stars if necessary */
    for(i = self->priv->star_count; i < count; i++)
    {
        Star *star = &g_array_index(self->priv->stars, Star, i);
        self->priv->vertices[3 * i + 2] = duckiegalaxy_particle_system_init_star(star);
    }

	self->priv->star_count = count;
}

static void
duckiegalaxy_particle_system_update_star_texture(DuckieGalaxyParticleSystem *self)
{
	g_clear_object(&self->priv->starTexture);

	self->priv->starTexture = lw_texture_new_from_resource (
                            (self->priv->draw_streaks) ? DUCKIEGALAXY_IMG "star-with-streaks.png"
                                                       : DUCKIEGALAXY_IMG "star.png");

	if(self->priv->starTexture)
	{
		lw_texture_set_filter(self->priv->starTexture, GL_LINEAR);
		lw_texture_set_wrap(self->priv->starTexture, GL_CLAMP_TO_EDGE);
	}
}

void
duckiegalaxy_particle_system_update(DuckieGalaxyParticleSystem *self, gint ms_since_last_paint)
{
    const Star   * const limit = &g_array_index (self->priv->stars, Star, self->priv->star_count);
          Star   *       star  = &g_array_index (self->priv->stars, Star, 0);
    const gfloat         k     =  ms_since_last_paint * self->priv->speed_ratio;
          gfloat *       v     =  self->priv->vertices;

    for(; star != limit; ++star, v+=3)
    {
		gfloat a_cos_angle, b_sin_angle;

		/* Make sure that angle is between -2PI and 0! */
		star->angle += k * star->speed;
		while(star->angle < -LW_2PI) star->angle += LW_2PI;
		while(star->angle > 0)     star->angle -= LW_2PI;

		/*
		 * a is the semi-major axis, here the distance and b is the
		 * semi-minor axis defined by the distance and the ellipse ratio.
		 */
		a_cos_angle = star->distance * lw_cos(star->angle);
		b_sin_angle = star->distance * lw_sin(star->angle) * ELLIPSE_RATIO;

		/*
		 * To calculate the current position we use the general
		 * parametic form of an ellipse.
		 * See http://en.wikipedia.org/wiki/Ellipse#General_parametric_form
		 */
		v[0] = a_cos_angle * star->cos_phi - b_sin_angle * star->sin_phi;
		v[1] = a_cos_angle * star->sin_phi + b_sin_angle * star->cos_phi;
    }
}

void
duckiegalaxy_particle_system_draw(DuckieGalaxyParticleSystem *self)
{
	/* static GLfloat quadratic[] = {0.4f, 0.0f, 0.1f}; */

	if(self->priv->starTexture) lw_texture_enable(self->priv->starTexture);

	/* Save texture environment */
	glPushAttrib(GL_TEXTURE_BIT);

	/* glPointParameterfv(GL_POINT_DISTANCE_ATTENUATION, quadratic); */
	glPointParameterf(GL_POINT_FADE_THRESHOLD_SIZE, 60.0f);
	glPointParameterf(GL_POINT_SIZE_MIN, 0.1f);

	glPointSize(self->priv->star_size);

	glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	/* Render stars using vertex array */
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnable(GL_POINT_SPRITE);

    /* Set star color */
    {
        GdkRGBA *c = &self->priv->star_color;
        glColor4f(c->red, c->green, c->blue, c->alpha);
    }

	glVertexPointer(3, GL_FLOAT, 3 * sizeof(gfloat), self->priv->vertices);
	glDrawArrays(GL_POINTS, 0, self->priv->star_count);

	glDisable(GL_POINT_SPRITE);
	glDisableClientState(GL_VERTEX_ARRAY);

	/* Restore texture environment */
	glPopAttrib();

	if(self->priv->starTexture) lw_texture_disable(self->priv->starTexture);
}

static void
duckiegalaxy_particle_system_init(DuckieGalaxyParticleSystem *self)
{
    self->priv = G_TYPE_INSTANCE_GET_PRIVATE(self, DUCKIEGALAXY_TYPE_PARTICLE_SYSTEM,
                                             DuckieGalaxyParticleSystemPrivate);

    self->priv->star_count = 0;
    self->priv->star_size = 8;
    self->priv->speed_ratio = 1;
    self->priv->draw_streaks = TRUE;

    self->priv->stars = g_array_new(FALSE, FALSE, sizeof(Star));
    self->priv->vertices = NULL;
}

static void
duckiegalaxy_particle_system_dispose(GObject *object)
{
	DuckieGalaxyParticleSystem *self = DUCKIEGALAXY_PARTICLE_SYSTEM(object);

	g_clear_object(&self->priv->starTexture);

	/* Chain up to the parent class */
	G_OBJECT_CLASS(duckiegalaxy_particle_system_parent_class)->dispose(object);
}

static void
duckiegalaxy_particle_system_finalize(GObject *object)
{
	DuckieGalaxyParticleSystem *self = DUCKIEGALAXY_PARTICLE_SYSTEM(object);

	g_array_free(self->priv->stars, TRUE);
	g_free(self->priv->vertices);

	/* Chain up to the parent class */
	G_OBJECT_CLASS(duckiegalaxy_particle_system_parent_class)->finalize(object);
}

static void
duckiegalaxy_particle_system_class_init(DuckieGalaxyParticleSystemClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

	gobject_class->set_property = duckiegalaxy_particle_system_set_property;
	gobject_class->get_property = duckiegalaxy_particle_system_get_property;
	gobject_class->dispose = duckiegalaxy_particle_system_dispose;
	gobject_class->finalize = duckiegalaxy_particle_system_finalize;

	g_type_class_add_private(klass, sizeof(DuckieGalaxyParticleSystemPrivate));

    obj_properties[PROP_STAR_COUNT]   = g_param_spec_uint   ("star-count",   "Number of stars",   "Number of stars",             0, 500000, 0, G_PARAM_READWRITE);
    obj_properties[PROP_STAR_SIZE]    = g_param_spec_uint   ("star-size",    "Star size",         "Size of a star",              1,     16, 8, G_PARAM_READWRITE);
    obj_properties[PROP_SPEED_RATIO]  = g_param_spec_double ("speed-ratio",  "Speed ratio",       "Speed ratio",                -5,      5, 1, G_PARAM_READWRITE);
    obj_properties[PROP_DRAW_STREAKS] = g_param_spec_boolean("draw-streaks", "Draw star streaks", "Draw a star with streaks", TRUE,            G_PARAM_READWRITE);
    obj_properties[PROP_STAR_COLOR]   = g_param_spec_boxed  ("star-color",   "Star color",        "Color of the stars",         GDK_TYPE_RGBA, G_PARAM_READWRITE);

	g_object_class_install_properties(gobject_class, N_PROPERTIES, obj_properties);
}

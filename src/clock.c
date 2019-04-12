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
 * Copyright (C) 2013-2016 Maximilian Schnarr <Maximilian.Schnarr@googlemail.com>
 *
 */

#include "config.h"

#include <glib-object.h>
#include <sys/time.h>

#include "clock.h"


#define DATA_COUNT 100

struct _LwClockPrivate
{
	guint fps_limit;
	guint ms_per_iteration;

	gboolean frame_started;
	struct timeval frame_start;
	struct timeval last_frame;

	guint index;
	guint sum;
	guint data[DATA_COUNT];
};

enum
{
	PROP_0,

	PROP_FPS_LIMIT,

	N_PROPERTIES
};

G_DEFINE_TYPE(LwClock, lw_clock, G_TYPE_OBJECT)

static void
lw_clock_set_property(GObject *object,
                      guint property_id,
                      const GValue *value,
                      GParamSpec *pspec)
{
	LwClockPrivate *priv = LW_CLOCK(object)->priv;

	switch(property_id)
	{
		case PROP_FPS_LIMIT:
			priv->fps_limit = g_value_get_uint(value);
			priv->ms_per_iteration = (priv->fps_limit == 0) ? 0 : 1000 / priv->fps_limit;
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
			break;
	}
}

static void
lw_clock_get_property(GObject *object,
                      guint property_id,
                      GValue *value,
                      GParamSpec *pspec)
{
	LwClockPrivate *priv = LW_CLOCK(object)->priv;

	switch(property_id)
	{
		case PROP_FPS_LIMIT:
			g_value_set_uint(value, priv->fps_limit);
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
			break;
	}
}

LwClock*
lw_clock_new()
{
	return g_object_new(LW_TYPE_CLOCK, NULL);
}

static inline gint
timeval_diff(struct timeval *tv1, struct timeval *tv2)
{
	/* This function is completely taken from compiz. */

	if(tv1->tv_sec == tv2->tv_sec || tv1->tv_usec >= tv2->tv_usec)
		return (((tv1->tv_sec - tv2->tv_sec) * 1000000) + (tv1->tv_usec - tv2->tv_usec)) / 1000;
	else
		return (((tv1->tv_sec - 1 - tv2->tv_sec) * 1000000) + (1000000 + tv1->tv_usec - tv2->tv_usec)) / 1000;
}

guint
lw_clock_get_fps(LwClock *self)
{
	return (self->priv->sum == 0) ? 0 : (DATA_COUNT * 1000) / self->priv->sum;
}

static void
lw_clock_do_fps_counting(LwClock *self)
{
	LwClockPrivate *priv = self->priv;
	guint ms_since_last_frame = lw_clock_get_ms_since_last_frame(self);

	priv->sum -= priv->data[priv->index];
	priv->sum += ms_since_last_frame;
	priv->data[priv->index] = ms_since_last_frame;

	priv->index++;
	if(priv->index == DATA_COUNT)
		priv->index = 0;
}

guint
lw_clock_get_ms_to_sleep(LwClock *self)
{
	gint time = self->priv->ms_per_iteration;
	struct timeval now;
	gettimeofday(&now, 0);

	time -= timeval_diff(&now, &self->priv->frame_start);
	if(time < 0 || time > 1000)
		time = 0;

	return time;
}

guint
lw_clock_get_ms_since_last_frame(LwClock *self)
{
	gint time = timeval_diff(&self->priv->frame_start, &self->priv->last_frame);

	if(time < 0 || time > 5000)
		time = 25;

	return time;
}

void
lw_clock_start_frame(LwClock *self)
{
	self->priv->last_frame = self->priv->frame_start;
	gettimeofday(&self->priv->frame_start, 0);

	self->priv->frame_started = TRUE;

	lw_clock_do_fps_counting(self);
}

void
lw_clock_end_frame(LwClock *self)
{
	self->priv->frame_started = FALSE;
}

static void
lw_clock_init(LwClock *self)
{
	self->priv = G_TYPE_INSTANCE_GET_PRIVATE(self, LW_TYPE_CLOCK, LwClockPrivate);
}

static void
lw_clock_class_init(LwClockClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

	gobject_class->set_property = lw_clock_set_property;
	gobject_class->get_property = lw_clock_get_property;

	g_type_class_add_private(klass, sizeof(LwClockPrivate));

	/* Install properties */
	g_object_class_install_property(gobject_class,
	                                PROP_FPS_LIMIT,
	                                g_param_spec_uint("fps-limit",
	                                                  "FPS Limit",
	                                                  "Limit the FPS to this value",
	                                                  0, 120, 50,
	                                                  G_PARAM_READWRITE));
}


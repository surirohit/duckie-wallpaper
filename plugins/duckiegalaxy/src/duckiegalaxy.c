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

#include <livewallpaper/core.h>
#include <libpeas/peas.h>

#include "light.h"
#include "particle.h"
#include "duckiegalaxy.h"

#define DUCKIEGALAXY_IMG "/net/launchpad/livewallpaper/plugins/duckiegalaxy/images/"

#define FOVY 45.0f
#define NEAR  0.1f
#define FAR  20.0f

/*
 * Update this value, if FOVY or NEAR changes:
 * YMAX = NEAR * tanf (FOVY * _PI / 360.0)
 */
#define YMAX 0.04142135
#define DEG2RAD (_PI / 180.0f)

struct _DuckieGalaxyPluginPrivate
{
	GSettings *settings;
    GResource *resource;

	gint rotate_x;
	gint rotate_y;
	gint rotate_z;

	gfloat rotational_speed;
	gfloat duckiegalaxy_rotation;

	gdouble zoom;

	gdouble offset_x;
	gdouble offset_y;

	LwTexture *lightTexture;
	LwBackground *background;

	gboolean lp_enabled;
	DuckieGalaxyLightProgram *lp;

	DuckieGalaxyParticleSystem *ps;
};

enum
{
    PROP_0,

    PROP_USE_CUSTOM_LIGHT,

    PROP_ROTATE_X,
    PROP_ROTATE_Y,
    PROP_ROTATE_Z,

	PROP_ROTATIONAL_SPEED,

    PROP_ZOOM,

    PROP_OFFSET_X,
    PROP_OFFSET_Y,

    N_PROPERTIES
};

static GParamSpec *obj_properties[N_PROPERTIES] = {NULL, };

static void lw_wallpaper_iface_init(LwWallpaperInterface *iface);

G_DEFINE_DYNAMIC_TYPE_EXTENDED(DuckieGalaxyPlugin, duckiegalaxy_plugin, PEAS_TYPE_EXTENSION_BASE, 0,
							   G_IMPLEMENT_INTERFACE_DYNAMIC(LW_TYPE_WALLPAPER,
							   								 lw_wallpaper_iface_init))


static void
duckiegalaxy_plugin_set_property(GObject *object,
                           guint property_id,
                           const GValue *value,
                           GParamSpec *pspec)
{
	DuckieGalaxyPlugin *self = DUCKIEGALAXY_PLUGIN(object);

	switch(property_id)
	{
		case PROP_USE_CUSTOM_LIGHT:
			self->priv->lp_enabled = g_value_get_boolean(value);
			break;

		case PROP_ROTATE_X:
			self->priv->rotate_x = g_value_get_int(value);
			break;

		case PROP_ROTATE_Y:
			self->priv->rotate_y = g_value_get_int(value);
			break;

		case PROP_ROTATE_Z:
			self->priv->rotate_z = g_value_get_int(value);
			break;

		case PROP_ROTATIONAL_SPEED:
			self->priv->rotational_speed = g_value_get_double(value);
			break;

		case PROP_ZOOM:
			self->priv->zoom = g_value_get_double(value);
			break;

		case PROP_OFFSET_X:
			self->priv->offset_x = g_value_get_double(value);
			break;

		case PROP_OFFSET_Y:
			self->priv->offset_y = g_value_get_double(value);
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
			break;
	}
}

static void
duckiegalaxy_plugin_get_property(GObject *object,
                           guint property_id,
                           GValue *value,
                           GParamSpec *pspec)
{
	DuckieGalaxyPlugin *self = DUCKIEGALAXY_PLUGIN(object);

	switch(property_id)
	{
		case PROP_USE_CUSTOM_LIGHT:
			g_value_set_boolean(value, self->priv->lp_enabled);
			break;

		case PROP_ROTATE_X:
			g_value_set_int(value, self->priv->rotate_x);
			break;

		case PROP_ROTATE_Y:
			g_value_set_int(value, self->priv->rotate_y);
			break;

		case PROP_ROTATE_Z:
			g_value_set_int(value, self->priv->rotate_z);
			break;

		case PROP_ROTATIONAL_SPEED:
			g_value_set_double(value, self->priv->rotational_speed);
			break;

		case PROP_ZOOM:
			g_value_set_double(value, self->priv->zoom);
			break;

		case PROP_OFFSET_X:
			g_value_set_double(value, self->priv->offset_x);
			break;

		case PROP_OFFSET_Y:
			g_value_set_double(value, self->priv->offset_y);
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
			break;
	}
}

static void
duckiegalaxy_plugin_adjust_viewport(G_GNUC_UNUSED LwWallpaper *plugin, LwOutput *output)
{
	gdouble xmax = YMAX * lw_output_get_aspect_ratio(output);

	/* Enable blending */
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE); /* Additive */

	/* Save and reset the coordinate system before touching. */
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	/*
	 * Using glFrustum instead of gluPrespective.
	 * http://www.opengl.org/wiki/GluPerspective_code
	 */
	glFrustum(-xmax, xmax, -YMAX, YMAX, NEAR, FAR);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
}

static void
duckiegalaxy_plugin_prepare_paint(LwWallpaper *plugin, int ms_since_last_paint)
{
	DuckieGalaxyPlugin *self = DUCKIEGALAXY_PLUGIN(plugin);

	/* Update particles */
	duckiegalaxy_particle_system_update(self->priv->ps, ms_since_last_paint);

	/* Rotate galaxy */
	self->priv->duckiegalaxy_rotation -= self->priv->rotational_speed * ms_since_last_paint / 10000.0f;
}

static void
duckiegalaxy_plugin_paint(LwWallpaper *plugin, LwOutput *output)
{
	DuckieGalaxyPlugin *self = DUCKIEGALAXY_PLUGIN(plugin);

	/* Clear color buffer and draw background image */
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	lw_background_draw(self->priv->background, output);

	/* Apply the user defined transformations. */
	glLoadIdentity();

	/* Apply galaxy zoom. */
	glTranslatef(0.0f, 0.0f,
		-self->priv->zoom * lw_output_get_aspect_ratio(output));

	/* Apply galaxy offset and rotation. */
	glTranslatef(self->priv->offset_x, self->priv->offset_y, 0.0f);

	glRotatef(self->priv->rotate_x, 1.0f, 0.0f, 0.0f);
	glRotatef(self->priv->rotate_y, 0.0f, 1.0f, 0.0f);
	glRotatef(self->priv->rotate_z + self->priv->duckiegalaxy_rotation,
	          0.0f, 0.0f, 1.0f);

	/* Draw light. */
	if(!self->priv->lightTexture) return;

	lw_texture_enable(self->priv->lightTexture);

	/* Save texture environment. */
	glPushAttrib(GL_TEXTURE_BIT);

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	/* Enable light program */
	if(self->priv->lp_enabled)
	{
		lw_program_enable(LW_PROGRAM(self->priv->lp));
		duckiegalaxy_light_program_set_uniform(self->priv->lp);
	}

#define DUCKIEGALAXY_LIGHT_SIZE 1.5f

	/* Draw quad. */
	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_QUADS);
		glTexCoord2d(1.0f, 0.0f);
		glVertex2f(-DUCKIEGALAXY_LIGHT_SIZE, -DUCKIEGALAXY_LIGHT_SIZE);

		glTexCoord2d(0.0f, 0.0f);
		glVertex2f(DUCKIEGALAXY_LIGHT_SIZE, -DUCKIEGALAXY_LIGHT_SIZE);

		glTexCoord2d(0.0f, 1.0f);
		glVertex2f(DUCKIEGALAXY_LIGHT_SIZE, DUCKIEGALAXY_LIGHT_SIZE);

		glTexCoord2d(1.0f, 1.0f);
		glVertex2f(-DUCKIEGALAXY_LIGHT_SIZE, DUCKIEGALAXY_LIGHT_SIZE);
	glEnd();

	/* Disable light program */
	if(self->priv->lp_enabled)
		lw_program_disable(LW_PROGRAM(self->priv->lp));

	/* Draw particles. */
	duckiegalaxy_particle_system_draw(self->priv->ps);

	/* Restore texture environment. */
	glPopAttrib();

	lw_texture_disable(self->priv->lightTexture);
}

static void
duckiegalaxy_plugin_restore_viewport(G_GNUC_UNUSED LwWallpaper *plugin)
{
	/* Restore the coordinate system. */
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	/* Restore the original perspective. */
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	/* Restore blending and other stuffs. */
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_BLEND);
}

static void
duckiegalaxy_plugin_init(DuckieGalaxyPlugin *self)
{
	self->priv = G_TYPE_INSTANCE_GET_PRIVATE(self, DUCKIEGALAXY_TYPE_PLUGIN, DuckieGalaxyPluginPrivate);

	self->priv->rotate_x = -60;
	self->priv->rotate_y = 15;
	self->priv->rotate_z = 0;

	self->priv->rotational_speed = 1.0f;
	self->priv->duckiegalaxy_rotation = 0.0f;

	self->priv->zoom = 1.5f;

	self->priv->offset_x = 0.0f;
	self->priv->offset_y = 0.0f;

	self->priv->lp_enabled = FALSE;
}

static void
duckiegalaxy_plugin_init_plugin(LwWallpaper *plugin)
{
    DuckieGalaxyPlugin *self = DUCKIEGALAXY_PLUGIN(plugin);
	self->priv->settings = g_settings_new("net.launchpad.livewallpaper.plugins.duckiegalaxy");

    self->priv->resource = lw_wallpaper_load_gresource (plugin, "duckiegalaxy.gresource");
	self->priv->lightTexture = lw_texture_new_from_resource    (DUCKIEGALAXY_IMG "galaxy-light.png");
	self->priv->background   = lw_background_new_from_resource (DUCKIEGALAXY_IMG "space.png", LwBackgroundTiled);

	self->priv->lp = duckiegalaxy_light_program_new();
	self->priv->ps = duckiegalaxy_particle_system_new();

    /* Particles */
    LW_BIND(self->priv->ps, "star-count");
    LW_BIND(self->priv->ps, "star-size");
    LW_BIND(self->priv->ps, "speed-ratio");
    LW_BIND(self->priv->ps, "draw-streaks");
	LW_BIND_COLOR(self->priv->ps, "star-color");

    /* Light Program */
	LW_BIND      (self->priv->lp, "color-radius");
	LW_BIND_COLOR(self->priv->lp, "outer-color");
	LW_BIND_COLOR(self->priv->lp, "inner-color");

    /* Galaxy */
    LW_BIND(self, "use-custom-light");
	LW_BIND(self, "rotate-x");
	LW_BIND(self, "rotate-y");
	LW_BIND(self, "rotate-z");
	LW_BIND(self, "rotational-speed");
	LW_BIND(self, "offset-x");
	LW_BIND(self, "offset-y");
	LW_BIND(self, "zoom");
}

static void
duckiegalaxy_plugin_dispose(GObject *object)
{
	DuckieGalaxyPlugin *self = DUCKIEGALAXY_PLUGIN(object);

    lw_unload_gresource (self->priv->resource);
	g_clear_object(&self->priv->settings);
	g_clear_object(&self->priv->lightTexture);
	g_clear_object(&self->priv->background);
	g_clear_object(&self->priv->lp);
	g_clear_object(&self->priv->ps);

	/* Chain up to the parent class */
	G_OBJECT_CLASS(duckiegalaxy_plugin_parent_class)->dispose(object);
}

static void
duckiegalaxy_plugin_class_init(DuckieGalaxyPluginClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

	gobject_class->set_property = duckiegalaxy_plugin_set_property;
	gobject_class->get_property = duckiegalaxy_plugin_get_property;
	gobject_class->dispose = duckiegalaxy_plugin_dispose;

	g_type_class_add_private(klass, sizeof(DuckieGalaxyPluginPrivate));

    obj_properties[PROP_USE_CUSTOM_LIGHT] = g_param_spec_boolean("use-custom-light", "Enable custom light", "Enable custom light color",                                    FALSE,              G_PARAM_READWRITE);
    obj_properties[PROP_ROTATE_X]         = g_param_spec_int    ("rotate-x",         "Rotate X",            "Rotate the galaxy through X coordinate",                         -90,    90,  -60, G_PARAM_READWRITE);
    obj_properties[PROP_ROTATE_Y]         = g_param_spec_int    ("rotate-y",         "Rotate Y",            "Rotate the galaxy through Y coordinate",                         -90,    90,   15, G_PARAM_READWRITE);
    obj_properties[PROP_ROTATE_Z]         = g_param_spec_int    ("rotate-z",         "Rotate Z",            "Rotate the galaxy through Z coordinate",                           0,   360,    0, G_PARAM_READWRITE);
    obj_properties[PROP_ROTATIONAL_SPEED] = g_param_spec_double ("rotational-speed", "Rotational speed",    "Rotational speed of the whole galaxy in revolutions per hour",  0.0f, 60.0f, 1.0f, G_PARAM_READWRITE);
    obj_properties[PROP_ZOOM]             = g_param_spec_double ("zoom",             "Zoom",                "Zoom in and out",                                               0.0f,  5.0f, 1.5f, G_PARAM_READWRITE);
    obj_properties[PROP_OFFSET_X]         = g_param_spec_double ("offset-x",         "Offset X",            "X offset of the centre of the galaxy",                         -2.0f,  2.0f, 0.0f, G_PARAM_READWRITE);
    obj_properties[PROP_OFFSET_Y]         = g_param_spec_double ("offset-y",         "Offset Y",            "Y offset of the centre of the galaxy",                         -2.0f,  2.0f, 0.0f, G_PARAM_READWRITE);

	g_object_class_install_properties(gobject_class, N_PROPERTIES, obj_properties);
}

static void
duckiegalaxy_plugin_class_finalize(G_GNUC_UNUSED DuckieGalaxyPluginClass *klass)
{

}

static void
lw_wallpaper_iface_init(LwWallpaperInterface *iface)
{
	iface->init_plugin = duckiegalaxy_plugin_init_plugin;

	iface->adjust_viewport = duckiegalaxy_plugin_adjust_viewport;
	iface->prepare_paint = duckiegalaxy_plugin_prepare_paint;
	iface->paint = duckiegalaxy_plugin_paint;
	iface->restore_viewport = duckiegalaxy_plugin_restore_viewport;
}

G_MODULE_EXPORT void
peas_register_types(PeasObjectModule *module)
{
	duckiegalaxy_plugin_register_type(G_TYPE_MODULE(module));
	peas_object_module_register_extension_type(module, LW_TYPE_WALLPAPER, DUCKIEGALAXY_TYPE_PLUGIN);
}

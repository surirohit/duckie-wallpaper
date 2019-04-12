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

#include <livewallpaper/core.h>
#include <libpeas/peas.h>

#include "particle.h"
#include "nexus.h"

struct _NexusPluginPrivate
{
	GSettings *settings;
    GResource *resource;

	NexusParticleSystem *ps;

	LwBackground *background;
};

static void lw_wallpaper_iface_init(LwWallpaperInterface *iface);

G_DEFINE_DYNAMIC_TYPE_EXTENDED(NexusPlugin, nexus_plugin, PEAS_TYPE_EXTENSION_BASE, 0,
							   G_IMPLEMENT_INTERFACE_DYNAMIC(LW_TYPE_WALLPAPER,
							   								 lw_wallpaper_iface_init))


static void
nexus_plugin_adjust_viewport(G_GNUC_UNUSED LwWallpaper *plugin, LwOutput *output)
{
	guint width  = lw_output_get_width(output),
		  height = lw_output_get_height(output);

	/* Enable blending */
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	/* Save and change texture environment */
	glPushAttrib(GL_TEXTURE_BIT);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	/* Save and reset the coordinate system before touching */
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	if(width > height)
		glOrtho(0.0, 1.0, 0.0, ((gdouble) height) / ((gdouble) width), 0.0, 1.0);
	else
		glOrtho(0.0, ((gdouble) width) / ((gdouble) height), 0.0, 1.0, 0.0, 1.0);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
}

static void
nexus_plugin_prepare_paint(LwWallpaper *plugin, gint ms_since_last_paint)
{
	NexusPlugin *self = NEXUS_PLUGIN(plugin);

	nexus_particle_system_update(self->priv->ps, ms_since_last_paint);
}

static void
nexus_plugin_paint(LwWallpaper *plugin, LwOutput *output)
{
	NexusPlugin *self = NEXUS_PLUGIN(plugin);

	/* Clear color buffer and draw background image */
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	lw_background_draw(self->priv->background, output);

	/* Draw pulses */
	nexus_particle_system_draw(self->priv->ps, lw_output_get_longest_side(output));
}

static void
nexus_plugin_restore_viewport(G_GNUC_UNUSED LwWallpaper *plugin)
{
	/* Restore the coordinate system */
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	/* Restore the original perspective */
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	/* Restore texture environment */
	/*glPopAttrib();*/

	/* Restore blending */
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_BLEND);
}

#define width 1920
#define height 1200
static LwTexture*
nexus_plugin_create_background_texture()
{
	cairo_t *cr;
	cairo_pattern_t *pattern;
	LwCairoTexture *texture = lw_cairo_texture_new(width, height);
    if (texture == NULL)
        return NULL;

	cr = lw_cairo_texture_cairo_create(texture);

	pattern = cairo_pattern_create_radial(width / 2, 0.9 * height, 0,
	                                      width / 2, 0.9 * height, 1.1 * height);
	cairo_pattern_add_color_stop_rgb(pattern, 0.0, 0.27, 0.27, 0.27);
	cairo_pattern_add_color_stop_rgb(pattern, 1.0, 0.05, 0.05, 0.05);

	cairo_set_source(cr, pattern);
	cairo_rectangle(cr, 0, 0, width, height);
	cairo_fill(cr);

	cairo_pattern_destroy(pattern);
	cairo_destroy(cr);
	lw_cairo_texture_update(texture);

	return LW_TEXTURE(texture);
}
#undef width
#undef height

static void
nexus_plugin_init(NexusPlugin *self)
{
	self->priv = G_TYPE_INSTANCE_GET_PRIVATE(self, NEXUS_TYPE_PLUGIN,
	                                         NexusPluginPrivate);
}

static void
nexus_plugin_init_plugin(LwWallpaper *plugin)
{
	NexusPlugin *self = NEXUS_PLUGIN(plugin);
	self->priv->settings = g_settings_new("net.launchpad.livewallpaper.plugins.nexus");

    self->priv->resource = lw_wallpaper_load_gresource (plugin, "nexus.gresource");

	self->priv->background = lw_background_new_from_texture(nexus_plugin_create_background_texture(), LwBackgroundZoom);

	self->priv->ps = nexus_particle_system_new();

	/* Appearance */
	LW_BIND_ENUM (self->priv->ps, "glow-type");
	LW_BIND_COLOR(self->priv->ps, "color1");
	LW_BIND_COLOR(self->priv->ps, "color2");
	LW_BIND_COLOR(self->priv->ps, "color3");
	LW_BIND_COLOR(self->priv->ps, "color4");
	LW_BIND      (self->priv->ps, "random-colors");

    /* Particle Settings */
    LW_BIND(self->priv->ps, "pulse-count");
    LW_BIND(self->priv->ps, "pulse-size");
    LW_BIND(self->priv->ps, "max-delay");
	LW_BIND_RANGE(self->priv->ps, "pulse-length");
}

static void
nexus_plugin_dispose(GObject *object)
{
	NexusPlugin *self = NEXUS_PLUGIN(object);

    lw_unload_gresource (self->priv->resource);
	g_clear_object(&self->priv->settings);
	g_clear_object(&self->priv->ps);
	g_clear_object(&self->priv->background);

	/* Chain up to the parent class */
	G_OBJECT_CLASS(nexus_plugin_parent_class)->dispose(object);
}

static void
nexus_plugin_class_init(NexusPluginClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

	gobject_class->dispose = nexus_plugin_dispose;

	g_type_class_add_private(klass, sizeof(NexusPluginPrivate));
}

static void
nexus_plugin_class_finalize(G_GNUC_UNUSED NexusPluginClass *klass)
{

}

static void
lw_wallpaper_iface_init(LwWallpaperInterface *iface)
{
	iface->init_plugin = nexus_plugin_init_plugin;

	iface->adjust_viewport = nexus_plugin_adjust_viewport;
	iface->prepare_paint = nexus_plugin_prepare_paint;
	iface->paint = nexus_plugin_paint;
	iface->restore_viewport = nexus_plugin_restore_viewport;
}

G_MODULE_EXPORT void
peas_register_types(PeasObjectModule *module)
{
	nexus_plugin_register_type(G_TYPE_MODULE(module));
	peas_object_module_register_extension_type(module, LW_TYPE_WALLPAPER, NEXUS_TYPE_PLUGIN);
}


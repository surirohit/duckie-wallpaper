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

#include <gdk/gdk.h>

#include <livewallpaper/core.h>
#include <libpeas/peas.h>

#include "particle.h"
#include "noise.h"

#define NOISE_RESOURCE "/net/launchpad/livewallpaper/plugins/noise/"

struct _NoisePluginPrivate
{
	GSettings *settings;
    GResource *resource;

	LwMatrix *matrix;
	NoiseParticleSystem *ps;

	LwBackground *background;
};

static void lw_wallpaper_iface_init(LwWallpaperInterface *iface);

G_DEFINE_DYNAMIC_TYPE_EXTENDED(NoisePlugin, noise_plugin, PEAS_TYPE_EXTENSION_BASE, 0,
							   G_IMPLEMENT_INTERFACE_DYNAMIC(LW_TYPE_WALLPAPER,
							   								 lw_wallpaper_iface_init))

static void
noise_plugin_adjust_viewport(LwWallpaper *plugin, LwOutput *output)
{
	NoisePlugin *self = NOISE_PLUGIN(plugin);
	guint width, height;

	/* Enable blending */
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	/* Change the matrix to fit the screen */
	width = lw_output_get_width(output);
	height = lw_output_get_height(output);

	lw_matrix_push(self->priv->matrix);
	if(width > height)
		lw_matrix_ortho(self->priv->matrix,
			            0.0, 1.0,
			            0.0, ((gfloat) height) / ((gfloat) width),
			            0.0, 1.0);
	else
		lw_matrix_ortho(self->priv->matrix,
			            0.0, ((gfloat) width) / ((gfloat) height),
			            0.0, 1.0,
			            0.0, 1.0);

	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
}

static void
noise_plugin_prepare_paint(LwWallpaper *plugin, gint ms_since_last_paint)
{
	NoisePlugin *self = NOISE_PLUGIN(plugin);

	noise_particle_system_update(self->priv->ps, ms_since_last_paint);
}

static void
noise_plugin_paint(LwWallpaper *plugin, LwOutput *output)
{
	NoisePlugin *self = NOISE_PLUGIN(plugin);

	/* Clear color buffer and draw background image */
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	lw_background_draw(self->priv->background, output);

	noise_particle_system_draw(self->priv->ps, self->priv->matrix);
}

static void
noise_plugin_restore_viewport(LwWallpaper *plugin)
{
	NoisePlugin *self = NOISE_PLUGIN(plugin);

	glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);

	/* Restore the matrix */
	lw_matrix_pop(self->priv->matrix);

	/* Restore blending */
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_BLEND);
}

static void
noise_plugin_init(NoisePlugin *self)
{
	self->priv = G_TYPE_INSTANCE_GET_PRIVATE(self, NOISE_TYPE_PLUGIN,
	                                         NoisePluginPrivate);

	lw_noise_init();
	self->priv->matrix = lw_matrix_new();
}

static void
noise_plugin_init_plugin(LwWallpaper *plugin)
{
	NoisePlugin *self = NOISE_PLUGIN(plugin);
	self->priv->settings = g_settings_new("net.launchpad.livewallpaper.plugins.noise");
	self->priv->resource = lw_wallpaper_load_gresource (plugin, "noise.gresource");

	self->priv->background = lw_background_new_from_resource (NOISE_RESOURCE "images/background.png", LwBackgroundZoom);

	self->priv->ps = noise_particle_system_new();

    /* Particle settings */
	LW_BIND      (self->priv->ps, "particle-count");
	LW_BIND      (self->priv->ps, "fade-time");
	LW_BIND_RANGE(self->priv->ps, "particle-size");
	LW_BIND_RANGE(self->priv->ps, "lifetime");
}

static void
noise_plugin_dispose(GObject *object)
{
	NoisePlugin *self = NOISE_PLUGIN(object);

    lw_unload_gresource (self->priv->resource);
	g_clear_object(&self->priv->settings);
	g_clear_object(&self->priv->matrix);
	g_clear_object(&self->priv->ps);
	g_clear_object(&self->priv->background);

	/* Chain up to the parent class */
	G_OBJECT_CLASS(noise_plugin_parent_class)->dispose(object);
}

static void
noise_plugin_class_init(NoisePluginClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

	gobject_class->dispose = noise_plugin_dispose;

	g_type_class_add_private(klass, sizeof(NoisePluginPrivate));
}

static void
noise_plugin_class_finalize(G_GNUC_UNUSED NoisePluginClass *klass)
{

}

static void
lw_wallpaper_iface_init(LwWallpaperInterface *iface)
{
	iface->init_plugin = noise_plugin_init_plugin;

	iface->adjust_viewport = noise_plugin_adjust_viewport;
	iface->prepare_paint = noise_plugin_prepare_paint;
	iface->paint = noise_plugin_paint;
	iface->restore_viewport = noise_plugin_restore_viewport;
}

G_MODULE_EXPORT void
peas_register_types(PeasObjectModule *module)
{
	noise_plugin_register_type(G_TYPE_MODULE(module));
	peas_object_module_register_extension_type(module, LW_TYPE_WALLPAPER, NOISE_TYPE_PLUGIN);
}


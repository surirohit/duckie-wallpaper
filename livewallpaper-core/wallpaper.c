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

/**
 * SECTION: wallpaper
 * @Short_description: Interface for live wallpaper plugins
 *
 * #LwWallpaperInterface is the interface for live wallpaper plugins.
 * A live wallpaper plugin is a plugin which provides an animated wallpaper like
 * the galaxy or the nexus plugin.
 */

#include <livewallpaper/core.h>
#include <libpeas/peas.h>


/**
 * LwWallpaper:
 *
 * Interface for plugins providing a live wallpaper.
 */

/**
 * LwWallpaperInterface:
 * @init_plugin: Init function
 * @adjust_viewport: Adjust viewport
 * @prepare_paint: Function to prepare the paint
 * @paint: Paint function
 * @done_paint: Function to clean up after paint
 * @restore_viewport: Restore viewport
 *
 * Interface for plugins providing a live wallpaper.
 */

G_DEFINE_INTERFACE(LwWallpaper, lw_wallpaper, G_TYPE_OBJECT)

/**
 * lw_wallpaper_init_plugin:
 * @self: A #LwWallpaper
 *
 * Initialization of the plugin. Initialize all the variables of the plugin here that
 * need the plugin's data dir path.
 */
void
lw_wallpaper_init_plugin(LwWallpaper *self)
{
	LwWallpaperInterface *iface;

	g_return_if_fail( LW_IS_WALLPAPER(self) );

	iface = LW_WALLPAPER_GET_INTERFACE(self);
	if(iface->init_plugin)
		iface->init_plugin(self);
}

/**
 * lw_wallpaper_adjust_viewport:
 * @self: A #LwWallpaper
 * @output: The new active #LwOutput
 *
 * Adjust the viewport to fit to this output. All following calls to lw_wallpaper_paint()
 * will get this output as parameter. This is the right place to transform the model or
 * projection matrix or to enable OpenGL features like blending. You should restore all
 * changes this function makes within the lw_wallpaper_restore_viewport() function.
 */
void
lw_wallpaper_adjust_viewport(LwWallpaper *self, LwOutput *output)
{
	LwWallpaperInterface *iface;

	g_return_if_fail( LW_IS_WALLPAPER(self) );

	iface = LW_WALLPAPER_GET_INTERFACE(self);
	if(iface->adjust_viewport)
		iface->adjust_viewport(self, output);
}

/**
 * lw_wallpaper_prepare_paint:
 * @self: A #LwWallpaper
 * @ms_since_last_paint: The time since the last paint in milliseconds
 *
 * Update the animation for all outputs here.
 */
void
lw_wallpaper_prepare_paint(LwWallpaper *self, gint ms_since_last_paint)
{
	LwWallpaperInterface *iface;

	g_return_if_fail( LW_IS_WALLPAPER(self) );

	iface = LW_WALLPAPER_GET_INTERFACE(self);
	if(iface->prepare_paint)
		iface->prepare_paint(self, ms_since_last_paint);
}

/**
 * lw_wallpaper_paint:
 * @self: A #LwWallpaper
 * @output: The #LwOutput to paint to
 *
 * Paint the wallpaper for the given output.
 */
void
lw_wallpaper_paint(LwWallpaper *self, LwOutput *output)
{
	LwWallpaperInterface *iface;

	g_return_if_fail( LW_IS_WALLPAPER(self) );

	iface = LW_WALLPAPER_GET_INTERFACE(self);
	if(iface->paint)
		iface->paint(self, output);
}

/**
 * lw_wallpaper_done_paint:
 * @self: A #LwWallpaper
 *
 * Clean up after all outputs are painted.
 */
void
lw_wallpaper_done_paint(LwWallpaper *self)
{
	LwWallpaperInterface *iface;

	g_return_if_fail( LW_IS_WALLPAPER(self) );

	iface = LW_WALLPAPER_GET_INTERFACE(self);
	if(iface->done_paint)
		iface->done_paint(self);
}

/**
 * lw_wallpaper_restore_viewport:
 * @self: A #LwWallpaper
 *
 * Restore all changes made by the lw_wallpaper_adjust_viewport() function
 * here.
 */
void
lw_wallpaper_restore_viewport(LwWallpaper *self)
{
	LwWallpaperInterface *iface;

	g_return_if_fail( LW_IS_WALLPAPER(self) );

	iface = LW_WALLPAPER_GET_INTERFACE(self);
	if(iface->restore_viewport)
		iface->restore_viewport(self);
}

/**
 * lw_wallpaper_load_gresource:
 * @self: A #LwWallpaper
 * @filename: the name of the resource file or its path from the peas datadir
 *
 * Call lw_load_gresource with the peas datadir + filename.
 */
GResource *
lw_wallpaper_load_gresource (LwWallpaper *self, const gchar *filename)
{
    gchar *datadir = peas_extension_base_get_data_dir(PEAS_EXTENSION_BASE(self));
    gchar *path = g_build_filename (datadir, filename, NULL);
    GResource *resource = lw_load_gresource (path);
    g_free (datadir);
    g_free (path);
    return resource;
}

static void
lw_wallpaper_default_init(G_GNUC_UNUSED LwWallpaperInterface *iface)
{
	static gboolean is_initialized = FALSE;

	if(!is_initialized)
	{
		is_initialized = TRUE;
	}
}


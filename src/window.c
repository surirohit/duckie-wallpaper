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

/**
 * SECTION: window
 * @Short_description: TODO
 *
 * TODO
 */

#include <glib-object.h>

#include "window.h"


/**
 * LwWindow:
 *
 * TODO
 */

/**
 * LwWindowInterface:
 * @get_error: TODO
 * @is_current: TODO
 * @make_current: TODO
 * @swap_buffers: TODO
 * @show: TODO
 * @hide: TODO
 * @get_outputs: TODO
 *
 * TODO
 */

enum {
	OUTPUTS_CHANGED,
	LAST_SIGNAL
};

static guint window_signals[LAST_SIGNAL] = { 0 };

G_DEFINE_INTERFACE(LwWindow, lw_window, G_TYPE_OBJECT)

/**
 * lw_window_get_error:
 * @self: A #LwWindow
 *
 * TODO
 *
 * Returns: TODO
 */
GError*
lw_window_get_error(LwWindow *self)
{
	g_return_val_if_fail(LW_IS_WINDOW(self), NULL);
	return LW_WINDOW_GET_INTERFACE(self)->get_error(self);
}

/**
 * lw_window_is_current:
 * @self: A #LwWindow
 *
 * TODO
 *
 * Returns: TODO
 */
gboolean
lw_window_is_current(LwWindow *self)
{
	g_return_val_if_fail(LW_IS_WINDOW(self), FALSE);
	return LW_WINDOW_GET_INTERFACE(self)->is_current(self);
}

/**
 * lw_window_make_current:
 * @self: A #LwWindow
 *
 * TODO
 */
void
lw_window_make_current(LwWindow *self)
{
	g_return_if_fail( LW_IS_WINDOW(self) );
	LW_WINDOW_GET_INTERFACE(self)->make_current(self);
}

/**
 * lw_window_swap_buffers:
 * @self: A #LwWindow
 *
 * TODO
 */
void
lw_window_swap_buffers(LwWindow *self)
{
	g_return_if_fail( LW_IS_WINDOW(self) );
	LW_WINDOW_GET_INTERFACE(self)->swap_buffers(self);
}

/**
 * lw_window_show:
 * @self: A #LwWindow
 * @behind_icons: TODO
 *
 * TODO
 */
void
lw_window_show(LwWindow *self, gboolean behind_icons)
{
	g_return_if_fail( LW_IS_WINDOW(self) );
	LW_WINDOW_GET_INTERFACE(self)->show(self, behind_icons);
}

/**
 * lw_window_hide:
 * @self: A #LwWindow
 *
 * TODO
 */
void
lw_window_hide(LwWindow *self)
{
	g_return_if_fail( LW_IS_WINDOW(self) );
	LW_WINDOW_GET_INTERFACE(self)->hide(self);
}

/**
 * lw_window_get_outputs:
 * @self: A #LwWindow
 *
 * TODO
 *
 * Returns: (element-type LW.Output) (transfer none): TODO
 */
GList*
lw_window_get_outputs(LwWindow *self)
{
	g_return_val_if_fail(LW_IS_WINDOW(self), NULL);
	return LW_WINDOW_GET_INTERFACE(self)->get_outputs(self);
}

static void
lw_window_default_init(LwWindowInterface *iface)
{
	static gboolean is_initialized = FALSE;

	if(!is_initialized)
	{
		is_initialized = TRUE;

		/**
		 * LwWindow::outputs-changed:
		 * @window: The #LwWindow, that emitted the signal
		 *
		 * TODO
		 **/
		window_signals[OUTPUTS_CHANGED] =
			g_signal_newv("outputs-changed",
		                  G_TYPE_FROM_INTERFACE(iface),
		                  G_SIGNAL_RUN_FIRST,
		                  NULL,
		                  NULL,
		                  NULL,
		                  g_cclosure_marshal_VOID__VOID,
		                  G_TYPE_NONE,
		                  0,
		                  NULL);
	}
}


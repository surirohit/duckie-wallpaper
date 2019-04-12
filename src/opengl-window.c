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

#include <glib.h>
#include <gdk/gdk.h>
#include <gdk/gdkx.h>

#include <GL/glew.h>
#include <GL/glx.h>

#include <string.h>

#include <livewallpaper/core.h>

#include "window.h"
#include "opengl-window.h"


struct _LwOpenGLWindowPrivate
{
	GdkDisplay *dpy;
	GdkWindow  *win;

	GLXContext glc;

	GError *error;

	GList *outputs;
};

static void lw_window_iface_init(LwWindowInterface *iface);

G_DEFINE_TYPE_EXTENDED(LwOpenGLWindow, lw_opengl_window, G_TYPE_OBJECT, 0,
					   G_IMPLEMENT_INTERFACE(LW_TYPE_WINDOW, lw_window_iface_init))


/***************** OpenGL helper *****************/
typedef GLXContext (*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, Bool, const int*);

/*
 * Adapted from:
 * http://www.opengl.org/resources/features/OGLextensions/
 */
static gboolean
isExtensionSupported(const char *extList, const char *extension)
{
	/* Extension names should not have spaces */
	const char *where = strchr(extension, ' ');
	if(where || *extension == '\0')
		return FALSE;

	while (TRUE)
	{
		where = strstr(extList, extension);
		if(!where)
            break;
		if(where == extList || *(where - 1) == ' ') {
            const char *terminator = where + strlen(extension);
			if(*terminator == ' ' || *terminator == '\0')
				return TRUE;
        }
	}

	return FALSE;
}

/***************** Window creation helper *****************/
static GLXFBConfig
lw_opengl_window_choose_framebuffer_config(LwOpenGLWindow *self)
{
	Display *dpy = GDK_DISPLAY_XDISPLAY(self->priv->dpy);

	int fb_attribs[] = {
		GLX_X_RENDERABLE,  True,
		GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
		GLX_RENDER_TYPE,   GLX_RGBA_BIT,
		GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
		GLX_RED_SIZE,      8,
		GLX_GREEN_SIZE,    8,
		GLX_BLUE_SIZE,     8,
		GLX_ALPHA_SIZE,    8,
		GLX_DEPTH_SIZE,    8,
		GLX_DOUBLEBUFFER,  True,
		None
	};

	int fbcount;
	GLXFBConfig *fbc = glXChooseFBConfig(dpy, DefaultScreen(dpy), fb_attribs, &fbcount);
	GLXFBConfig bestFbc = NULL;

	if(fbc == NULL)
	{
		self->priv->error = g_error_new(LW_CORE_ERROR,
		                                LW_CORE_ERROR_FAILED,
		                                "Could not find appropriate framebuffer configuration");
		return NULL;
	}

	/* Select best framebuffer */
	bestFbc = fbc[0];

	XFree(fbc);
	return bestFbc;
}

static GLXContext
lw_opengl_window_create_context(LwOpenGLWindow *self, GLXFBConfig fbc)
{
	Display *dpy = GDK_DISPLAY_XDISPLAY(self->priv->dpy);

    if (FALSE) /* At the moment, we only create an OpenGL 2.x context */
    {
        glXCreateContextAttribsARBProc glXCreateContextAttribsARB =
            (glXCreateContextAttribsARBProc)
                glXGetProcAddressARB((const GLubyte*) "glXCreateContextAttribsARB");
        const char *glxExts = glXQueryExtensionsString(dpy, DefaultScreen(dpy));

        if(isExtensionSupported(glxExts, "GLX_ARB_create_context") && glXCreateContextAttribsARB != 0)
        {
            /* Create new OpenGL 3.x context */
            int context_attribs[] = {
                GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
                GLX_CONTEXT_MINOR_VERSION_ARB, 0,
                None
            };
            return glXCreateContextAttribsARB(dpy, fbc, 0, True, context_attribs);
        }
    }
    /* Create old OpenGL 2.x context */
    return glXCreateNewContext(dpy, fbc, GLX_RGBA_TYPE, 0, True);

}

static GdkWindow*
lw_opengl_window_create_window(LwOpenGLWindow *self, XVisualInfo *vi)
{
	GdkScreen *screen = gdk_display_get_default_screen(self->priv->dpy);
	GdkWindow *root = gdk_screen_get_root_window(screen);
	GdkWindowAttr attr;

	attr.event_mask = 0;
	attr.width = gdk_screen_get_width(screen) + 1; /* WORKAROUND */
	attr.height = gdk_screen_get_height(screen);
	attr.wclass = GDK_INPUT_OUTPUT;

	attr.visual = gdk_x11_screen_lookup_visual(screen, vi->visualid);
	if(attr.visual == NULL)
	{
		self->priv->error = g_error_new(LW_CORE_ERROR,
		                                LW_CORE_ERROR_FAILED,
		                                "Could not find matching visual");
		return NULL;
	}

	attr.window_type = GDK_WINDOW_TOPLEVEL;
	attr.type_hint = GDK_WINDOW_TYPE_HINT_DESKTOP;

	return gdk_window_new(root, &attr, GDK_WA_VISUAL | GDK_WA_TYPE_HINT);
}

/***************** Interface methods *****************/
static GError*
lw_opengl_window_get_error(LwWindow *window)
{
	LwOpenGLWindow *self = LW_OPENGL_WINDOW(window);

	return self->priv->error;
}

static gboolean
lw_opengl_window_is_current(LwWindow *window)
{
	LwOpenGLWindow *self = LW_OPENGL_WINDOW(window);

	return (glXGetCurrentContext() == self->priv->glc);
}

static void
lw_opengl_window_make_current(LwWindow *window)
{
	LwOpenGLWindow *self = LW_OPENGL_WINDOW(window);

	glXMakeCurrent(GDK_DISPLAY_XDISPLAY(self->priv->dpy),
	               GDK_WINDOW_XID(self->priv->win), self->priv->glc);
}

static void
lw_opengl_window_swap_buffers(LwWindow *window)
{
	LwOpenGLWindow *self = LW_OPENGL_WINDOW(window);

	glXSwapBuffers(GDK_DISPLAY_XDISPLAY(self->priv->dpy),
	               GDK_WINDOW_XID(self->priv->win));
}

static void
lw_opengl_window_show(LwWindow *window, gboolean behind_icons)
{
	LwOpenGLWindow *self = LW_OPENGL_WINDOW(window);

	gdk_window_show(self->priv->win);
	if(behind_icons) gdk_window_lower(self->priv->win);
	gdk_display_sync(self->priv->dpy);
}

static void
lw_opengl_window_hide(LwWindow *window)
{
	LwOpenGLWindow *self = LW_OPENGL_WINDOW(window);

	gdk_window_hide(self->priv->win);
	gdk_display_sync(self->priv->dpy);
}

static GList*
lw_opengl_window_get_outputs(LwWindow *window)
{
	LwOpenGLWindow *self = LW_OPENGL_WINDOW(window);

	if(self->priv->outputs == NULL)
	{
		GdkScreen *screen = gdk_display_get_default_screen(self->priv->dpy);
		gint i, n_monitors = gdk_screen_get_n_monitors(screen);
		gint window_height = gdk_screen_get_height(screen);
		GdkRectangle g1, g2;

		for(i = 0; i < n_monitors; i++)
		{
			gint j;
			gboolean found = FALSE;

			gdk_screen_get_monitor_geometry(screen, i, &g1);

			for(j = 0; j < i && !found; j++)
			{
				gdk_screen_get_monitor_geometry(screen, j, &g2);
				found = gdk_rectangle_intersect(&g1, &g2, NULL);
			}

			if(!found)
			{
				gint scale_factor = gdk_screen_get_monitor_scale_factor(screen, i);

				/* Append new output */
				LwOutput *o = g_object_new(LW_TYPE_OUTPUT,
				                           "x", scale_factor * g1.x,
				                           "y", scale_factor * (window_height - g1.y - g1.height),
				                           "width", scale_factor * g1.width,
				                           "height", scale_factor * g1.height,
				                           NULL);

				self->priv->outputs = g_list_prepend(self->priv->outputs, o);
			}
		}
	}

	return self->priv->outputs;
}

/***************** Callbacks *****************/
static void
lw_opengl_window_update_outputs(GdkScreen *screen, LwOpenGLWindow *self)
{
	GList *tmp = self->priv->outputs;

	gdk_window_resize(self->priv->win,
	                  gdk_screen_get_width(screen) + 1, /* WORKAROUND */
	                  gdk_screen_get_height(screen));

	self->priv->outputs = NULL;
	g_signal_emit_by_name(self, "outputs-changed");

	g_list_free_full(tmp, g_object_unref);
}

/***************** Constructor and Destructor *****************/
static void
lw_opengl_window_init(LwOpenGLWindow *self)
{
	Display *dpy;
	GLXFBConfig fbc;
	XVisualInfo *vi;
	int glx_major, glx_minor;

	self->priv = G_TYPE_INSTANCE_GET_PRIVATE(self, LW_TYPE_OPENGL_WINDOW,
	                                         LwOpenGLWindowPrivate);
	self->priv->win = NULL;
	self->priv->glc = NULL;
	self->priv->dpy = gdk_display_get_default();
	self->priv->error = NULL;
	self->priv->outputs = NULL;

	dpy = GDK_DISPLAY_XDISPLAY(self->priv->dpy);

	/* Check if GLX version >= 1.3 */
	glXQueryVersion(dpy, &glx_major, &glx_minor);
	if((glx_major == 1 && glx_minor < 3) || glx_major < 1)
	{
		self->priv->error = g_error_new(LW_CORE_ERROR,
		                                LW_CORE_ERROR_FAILED,
		                                "LiveWallpaper requires at least GLX version 1.3"
		                                "to create a window. Found GLX version %d.%d",
		                                glx_major, glx_minor);
		return;
	}

	if((fbc = lw_opengl_window_choose_framebuffer_config(self)) == NULL)
        return;

	if((vi = glXGetVisualFromFBConfig(dpy, fbc)) == NULL)
	{
		self->priv->error = g_error_new(LW_CORE_ERROR,
		                                LW_CORE_ERROR_FAILED,
		                                "Could not get visual framebuffer configuration");

		return;
	}

	if((self->priv->win = lw_opengl_window_create_window(self, vi)) == NULL)
        return;

	self->priv->glc = lw_opengl_window_create_context(self, fbc);

	/* Connect signals */
	g_signal_connect(gdk_display_get_default_screen(self->priv->dpy), "monitors-changed",
	                 G_CALLBACK(lw_opengl_window_update_outputs), self);
}

static void
lw_opengl_window_dispose(GObject *object)
{
	LwOpenGLWindow *self = LW_OPENGL_WINDOW(object);
	Display *dpy = GDK_DISPLAY_XDISPLAY(self->priv->dpy);

	if(lw_opengl_window_is_current(LW_WINDOW(self)))
		glXMakeCurrent(dpy, None, NULL);
	glXDestroyContext(dpy, self->priv->glc);
	gdk_window_destroy(self->priv->win);

	g_clear_error(&self->priv->error);

	/* Chain up to the parent class */
	G_OBJECT_CLASS(lw_opengl_window_parent_class)->dispose(object);
}

static void
lw_opengl_window_class_init(LwOpenGLWindowClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
	gobject_class->dispose = lw_opengl_window_dispose;

	g_type_class_add_private(klass, sizeof(LwOpenGLWindowPrivate));
}

static void
lw_window_iface_init(LwWindowInterface *iface)
{
	iface->get_error = lw_opengl_window_get_error;
	iface->is_current = lw_opengl_window_is_current;
	iface->make_current = lw_opengl_window_make_current;
	iface->swap_buffers = lw_opengl_window_swap_buffers;
	iface->show = lw_opengl_window_show;
	iface->hide = lw_opengl_window_hide;
	iface->get_outputs = lw_opengl_window_get_outputs;
}


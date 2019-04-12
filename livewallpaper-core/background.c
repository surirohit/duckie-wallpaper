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
 * SECTION: background
 * @Short_description: background image
 *
 * The #LwBackground object represents a static background image or color gradient.
 *
 * To use #LwBackground a plugin has to specify a default background. There are two
 * types of default backgrounds:
 * <itemizedlist>
 *   <listitem>
 *     <para>An image (lw_background_new_from_file())</para>
 *   </listitem>
 *   <listitem>
 *     <para>A color gradient (lw_background_new_from_colors())</para>
 *   </listitem>
 * </itemizedlist>
 * Then the background is ready to be drawn. Use lw_background_draw() to draw the
 * background to the current output. This is usually done before all other things
 * will be drawn.
 *
 * In addition to this, LwBackground provides support for custom background
 * images. If the user checked the "use-custom-bg" key inside the configurator,
 * #LwBackground will show the user defined background image instead of the default
 * background. It is not possible to deactivate this behaviour at the moment.
 */

#include <string.h>
#include <livewallpaper/core.h>


/**
 * LwBackgroundRenderType:
 * @LwBackgroundZoom: Scales the image to fill the whole screen, no borders
 * @LwBackgroundCentered: Puts the image in the center and keeps original size
 * @LwBackgroundScaled: Scales the image to be completely on the screen, with borders
 * @LwBackgroundStretched: Stretches the image to fill the whole screen, won't keep the
 *                         aspect ratio. This is the value if "Fill" is chosen inside
 *                         the configurator.
 * @LwBackgroundTiled: Tiles the image and keeps original size
 *
 * All supported types to render a background image.
 */

/**
 * LwBackgroundShadingType:
 * @LwBackgroundHorizontal: Horizontal color gradient
 * @LwBackgroundVertical: Vertical color gradient
 * @LwBackgroundSolid: Solid color
 *
 * All supported shading types of #LwBackground.
 */

typedef enum
{
	LwBackgroundDefault,
	LwBackgroundCustomImage,
	LwBackgroundSingleColor,
	LwBackgroundHorizontalGradient,
	LwBackgroundVerticalGradient
} LwBackgroundMode;

struct _LwBackgroundPrivate
{
	GSettings *settings;

	LwBackgroundMode bg_mode;

	LwTexture *bg_texture;
	LwBackgroundRenderType bg_render_type;

	LwTexture *default_bg;
	LwBackgroundRenderType default_render_type;

	gchar *image;
	LwBackgroundRenderType render_type;

	GdkRGBA primary_color;
	GdkRGBA secondary_color;
};

enum
{
	PROP_0,

	PROP_IMAGE,
	PROP_RENDER_TYPE,
	PROP_PRIMARY_COLOR,
	PROP_SECONDARY_COLOR,

	N_PROPERTIES
};

static GParamSpec *obj_properties[N_PROPERTIES] = {NULL, };

/**
 * LwBackground:
 *
 * This structure holds all required information to render a background image.
 */

G_DEFINE_TYPE(LwBackground, lw_background, G_TYPE_OBJECT)

static void lw_background_changed(GSettings *settings, G_GNUC_UNUSED gchar *key, LwBackground *self);

/**
 * lw_background_new_from_file:
 * @path: Name of the default background image
 * @type: #LwBackgroundRenderType to use for default background image
 *
 * Creates a new #LwBackground with an default background image.
 * Note that the background you set won't be necessary the one displayed,
 * it can be overrided by the user's configuration.
 * 
 * Returns: A new #LwBackground. Use g_object_unref() to free the #LwBackground.
 *
 * Since: 0.5
 */
LwBackground*
lw_background_new_from_file (const gchar *path, LwBackgroundRenderType type)
{
    LwTexture *texture = lw_texture_new_from_file(path);
	return lw_background_new_from_texture(texture, type);
}

/**
 * lw_background_new_from_resource:
 * @path: Name of the default background image
 * @type: #LwBackgroundRenderType to use for default background image
 *
 * Creates a new #LwBackground with an default background image.
 * Note that the background you set won't be necessary the one displayed,
 * it can be overrided by the user's configuration.
 *
 * Returns: A new #LwBackground. Use g_object_unref() to free the #LwBackground.
 *
 * Since: 0.5
 */
LwBackground*
lw_background_new_from_resource (const gchar *path,
                                 LwBackgroundRenderType type)
{
	LwTexture *texture = lw_texture_new_from_resource (path);
	return lw_background_new_from_texture(texture, type);
}

static LwTexture*
create_texture_from_colors(GdkColor *primary_color,
                           GdkColor *secondary_color,
                           LwBackgroundShadingType type)
{
	LwTexture *texture;
	guint width, height;
	GLushort data[8];

	data[0] = primary_color->red;
	data[1] = primary_color->green;
	data[2] = primary_color->blue;
	data[3] = 65535;

	data[4] = secondary_color->red;
	data[5] = secondary_color->green;
	data[6] = secondary_color->blue;
	data[7] = 65535;

	switch(type)
	{
		case LwBackgroundHorizontal:
			width = 2;
			height = 1;
			break;

		case LwBackgroundVertical:
			width = 1;
			height = 2;
			break;

		case LwBackgroundSolid:
		default:
			width = 1;
			height = 1;
	}

	texture = lw_texture_new_from_data((guchar *) data, width, height, GL_RGBA, GL_UNSIGNED_SHORT);
	lw_texture_set_filter(texture, GL_LINEAR);

	return texture;
}

static LwTexture*
create_texture_from_rgba(GdkRGBA *primary_rgba,
                         GdkRGBA *secondary_rgba,
                         LwBackgroundShadingType type)
{
	GdkColor primary_color, secondary_color;
	primary_color.red = primary_rgba->red * 65535;
	primary_color.green = primary_rgba->green * 65535;
	primary_color.blue = primary_rgba->blue * 65535;

	secondary_color.red = secondary_rgba->red * 65535;
	secondary_color.green = secondary_rgba->green * 65535;
	secondary_color.blue = secondary_rgba->blue * 65535;

	return create_texture_from_colors(&primary_color, &secondary_color, type);
}

/**
 * lw_background_new_from_colors:
 * @primary_color: Left or bottom color when drawing gradients, or the solid color
 * @secondary_color: Right or top color when drawing gradients, not used for solid color
 * @type: The type of color gradient
 *
 * Creates a new #LwBackground with an color gradient as default background.
 *
 * Returns: A new #LwBackground. Use g_object_unref() to free the #LwBackground.
 *
 * Since: 0.5
 */
LwBackground*
lw_background_new_from_colors(GdkColor *primary_color,
                              GdkColor *secondary_color,
                              LwBackgroundShadingType type)
{
	LwTexture *texture = create_texture_from_colors(primary_color, secondary_color, type);
	return lw_background_new_from_texture(texture, LwBackgroundStretched);
}

/**
 * lw_background_new_from_texture:
 * @texture: The #LwTexture to use as default background
 * @type: #LwBackgroundRenderType to use for default background
 *
 * Creates a new #LwBackground from a #LwTexture.
 *
 * Returns: A new #LwBackground. Use g_object_unref() to free the #LwBackground.
 *
 * Since: 0.5
 */
LwBackground*
lw_background_new_from_texture(LwTexture *texture,
                                       LwBackgroundRenderType type)
{
	LwBackground *background = g_object_new(LW_TYPE_BACKGROUND, NULL);

	background->priv->default_bg = texture;
	background->priv->default_render_type = type;

	lw_background_changed(background->priv->settings, "default-bg", background);

	return background;
}

static void
lw_background_set_property(GObject *object,
                           guint property_id,
                           const GValue *value,
                           GParamSpec *pspec)
{
	LwBackground *self = LW_BACKGROUND(object);

	switch(property_id)
	{
		case PROP_IMAGE:
			g_free(self->priv->image);
			self->priv->image = g_strdup(g_value_get_string(value));
			if(self->priv->bg_mode == LwBackgroundCustomImage)
				lw_background_changed(self->priv->settings, "bg-image", self);
			break;

		case PROP_RENDER_TYPE:
			self->priv->render_type = g_value_get_uint(value);
			if(self->priv->bg_mode == LwBackgroundCustomImage)
				self->priv->bg_render_type = self->priv->render_type;
			break;

		case PROP_PRIMARY_COLOR:
			self->priv->primary_color = *((GdkRGBA*) g_value_get_boxed(value));
			if(self->priv->bg_mode == LwBackgroundSingleColor ||
			   self->priv->bg_mode == LwBackgroundHorizontalGradient ||
			   self->priv->bg_mode == LwBackgroundVerticalGradient)
				lw_background_changed(self->priv->settings, "bg-primary-color", self);
			break;

		case PROP_SECONDARY_COLOR:
			self->priv->secondary_color = *((GdkRGBA*) g_value_get_boxed(value));
			if(self->priv->bg_mode == LwBackgroundHorizontalGradient ||
			   self->priv->bg_mode == LwBackgroundVerticalGradient)
				lw_background_changed(self->priv->settings, "bg-secondary-color", self);
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
			break;
	}
}

static void
lw_background_get_property(GObject *object,
                           guint property_id,
                           GValue *value,
                           GParamSpec *pspec)
{
	LwBackground *self = LW_BACKGROUND(object);

	switch(property_id)
	{
		case PROP_IMAGE:
			g_value_set_string(value, self->priv->image);
			break;

		case PROP_RENDER_TYPE:
			g_value_set_uint(value, self->priv->render_type);
			break;

		case PROP_PRIMARY_COLOR:
			g_value_set_boxed(value, &self->priv->primary_color);
			break;

		case PROP_SECONDARY_COLOR:
			g_value_set_boxed(value, &self->priv->secondary_color);
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
			break;
	}
}

/**
 * lw_background_draw:
 * @self: A #LwBackground
 * @output: The active output
 *
 * Draws the background to the active output.
 */
void
lw_background_draw(LwBackground *self, LwOutput *output)
{
	/* Use gint instead of guint! */
	gint width  = lw_output_get_width(output),
	     height = lw_output_get_height(output);
	gint tex_width, tex_height;
	LwTexture* tex = self->priv->bg_texture;
	LwBackgroundRenderType type = self->priv->bg_render_type;
	LwTextureMatrix m;

	if(tex == NULL && self->priv->default_bg != NULL)
	{
		/* Fallback to default background */
		tex = self->priv->default_bg;
		type = self->priv->default_render_type;
	}
	else if(tex == NULL) return;

	tex_width = lw_texture_get_width(tex);
	tex_height = lw_texture_get_height(tex);

	/* Save and reset the coordinate system for drawing the background. */
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	glOrtho(0, width, height, 0, -1.0, 1.0);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	/* Apply render type */
	m = tex->matrix;
	switch(type)
	{
		case LwBackgroundCentered:
			m.x0 -= ((width - tex_width) / 2) * m.xx;
			m.y0 -= ((height - tex_height) / 2) * m.yy;

			lw_texture_set_wrap(tex, GL_CLAMP_TO_EDGE);
			break;

		case LwBackgroundScaled:
		{
			gfloat s = MIN((gfloat) width / tex_width,
			              (gfloat) height / tex_height);

			m.xx /= s;
			m.yy /= s;

			m.x0 -= ((width - (tex_width * s)) / 2) * m.xx;
			m.y0 -= ((height - (tex_height * s)) / 2) * m.yy;

			lw_texture_set_wrap(tex, GL_CLAMP_TO_EDGE);
			break;
		}

		case LwBackgroundStretched:
			m.xx /= (gfloat) width / tex_width;
			m.yy /= (gfloat) height / tex_height;

			lw_texture_set_wrap(tex, GL_CLAMP_TO_EDGE);
			break;

		case LwBackgroundTiled:
			lw_texture_set_wrap(tex, GL_REPEAT);
			break;

		case LwBackgroundZoom:
		default:
		{
			gfloat s = MAX((gfloat) width / tex_width,
			              (gfloat) height / tex_height);

			m.xx /= s;
			m.yy /= s;

			m.x0 -= ((width - (tex_width * s)) / 2) * m.xx;
			m.y0 -= ((height - (tex_height * s)) / 2) * m.yy;

			lw_texture_set_wrap(tex, GL_CLAMP_TO_EDGE);
		}
	}

	/* Draw background */
	glColor3f(1.0f, 1.0f, 1.0f);
	lw_texture_enable(tex);

	glBegin(GL_QUADS);
		glTexCoord2f(
			LW_TEX_COORD_X(m, 0.0f),
			LW_TEX_COORD_Y(m, 0.0f)
		);
		glVertex2f(0.0f, 0.0f);

		glTexCoord2f(
			LW_TEX_COORD_X(m, 0.0f),
			LW_TEX_COORD_Y(m, height)
		);
		glVertex2f(0.0f, height);

		glTexCoord2f(
			LW_TEX_COORD_X(m, width),
			LW_TEX_COORD_Y(m, height)
		);
		glVertex2f(width, height);

		glTexCoord2f(
			LW_TEX_COORD_X(m, width),
			LW_TEX_COORD_Y(m, 0.0f)
		);
		glVertex2f(width, 0.0f);
	glEnd();

	lw_texture_disable(tex);

	/* Restore the coordinate system. */
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

static void
lw_background_changed(GSettings *settings, G_GNUC_UNUSED gchar *key, LwBackground *self)
{
	g_clear_object(&self->priv->bg_texture);

	self->priv->bg_mode = g_settings_get_enum(settings, "bg-mode");
	switch(self->priv->bg_mode)
	{
		case LwBackgroundCustomImage:
		{
			/* Load image */
			if (self->priv->image[0] != '\0')
				self->priv->bg_texture = lw_texture_new_from_file(self->priv->image);

			self->priv->bg_render_type = self->priv->render_type;
			break;
		}

		case LwBackgroundSingleColor:
			self->priv->bg_texture = create_texture_from_rgba(&self->priv->primary_color,
		                                                      &self->priv->secondary_color,
		                                                      LwBackgroundSolid);
			self->priv->bg_render_type = LwBackgroundStretched;
			break;

		case LwBackgroundHorizontalGradient:
			self->priv->bg_texture = create_texture_from_rgba(&self->priv->primary_color,
		                                                      &self->priv->secondary_color,
		                                                      LwBackgroundHorizontal);
			self->priv->bg_render_type = LwBackgroundStretched;
			break;

		case LwBackgroundVerticalGradient:
			self->priv->bg_texture = create_texture_from_rgba(&self->priv->primary_color,
		                                                      &self->priv->secondary_color,
		                                                      LwBackgroundVertical);
			self->priv->bg_render_type = LwBackgroundStretched;
			break;

		case LwBackgroundDefault:
		default:
			self->priv->bg_texture = self->priv->default_bg;
			if(self->priv->default_bg)
				g_object_ref(self->priv->default_bg);
			self->priv->bg_render_type = self->priv->default_render_type;
	}
}

static void
lw_background_init(LwBackground *self)
{
	self->priv = G_TYPE_INSTANCE_GET_PRIVATE(self, LW_TYPE_BACKGROUND,
	                                         LwBackgroundPrivate);

	self->priv->settings = g_settings_new("net.launchpad.livewallpaper");
	self->priv->bg_mode = LwBackgroundDefault;
	self->priv->bg_texture = NULL;
	self->priv->bg_render_type = LwBackgroundZoom;
	self->priv->default_bg = NULL;
	self->priv->default_render_type = LwBackgroundZoom;

	/* Bind settings to properties */
	g_signal_connect(self->priv->settings, "changed::bg-mode",
	                 G_CALLBACK(lw_background_changed), self);

	g_settings_bind(self->priv->settings, "bg-image",
	                self, "image", G_SETTINGS_BIND_GET);
	lw_settings_bind_enum(self->priv->settings, "bg-render-type",
	                      self, "render-type", G_SETTINGS_BIND_GET);

	lw_settings_bind_color(self->priv->settings, "bg-primary-color",
	                       self, "primary-color", G_SETTINGS_BIND_GET);
	lw_settings_bind_color(self->priv->settings, "bg-secondary-color",
	                       self, "secondary-color", G_SETTINGS_BIND_GET);
}

static void
lw_background_dispose(GObject *object)
{
	LwBackground *self = LW_BACKGROUND(object);

	g_clear_object(&self->priv->settings);
	g_clear_object(&self->priv->bg_texture);
	g_clear_object(&self->priv->default_bg);

	/* Chain up to the parent class */
	G_OBJECT_CLASS(lw_background_parent_class)->dispose(object);
}

static void
lw_background_finalize(GObject *object)
{
	LwBackground *self = LW_BACKGROUND(object);
	
	g_free(self->priv->image);
	
	/* Chain up to the parent class */
	G_OBJECT_CLASS(lw_background_parent_class)->finalize(object);
}

static void
lw_background_class_init(LwBackgroundClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

	gobject_class->set_property = lw_background_set_property;
	gobject_class->get_property = lw_background_get_property;
	gobject_class->dispose = lw_background_dispose;
	gobject_class->finalize = lw_background_finalize;

	g_type_class_add_private(klass, sizeof(LwBackgroundPrivate));

	/**
	 * LwBackground:image:
	 *
	 * Path to use for the background image
	 */
	obj_properties[PROP_IMAGE] = g_param_spec_string("image", "Image", "Path to use for the background image", NULL, G_PARAM_READWRITE);

	/**
	 * LwBackground:render-type:
	 *
	 * Determines how the background image is rendered
	 */
	obj_properties[PROP_RENDER_TYPE] = g_param_spec_uint("render-type", "Render Type", "Determines how the background image is rendered", 0, 4, 0, G_PARAM_READWRITE);

	/**
	 * LwBackground:primary-color:
	 *
	 * Left or bottom color when drawing gradients, or the solid color
	 */
	obj_properties[PROP_PRIMARY_COLOR] = g_param_spec_boxed("primary-color", "Primary Color", "Left or bottom color when drawing gradients, or the solid color", GDK_TYPE_RGBA, G_PARAM_READWRITE);

	/**
	 * LwBackground:secondary-color:
	 *
	 * Right or top color when drawing gradients, not used for solid color
	 */
	obj_properties[PROP_SECONDARY_COLOR] = g_param_spec_boxed("secondary-color", "Secondary Color", "Right and top color when drawing gradients, not used for solid color", GDK_TYPE_RGBA, G_PARAM_READWRITE);

	g_object_class_install_properties(gobject_class, N_PROPERTIES, obj_properties);
}


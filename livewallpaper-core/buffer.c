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
 * SECTION: buffer
 * @Short_description: OpenGL buffer wrapper
 *
 * <note>
 *   <para>
 *     LwBuffer is not finished yet. It is possible that parts of this type
 *     will be changed in a future version of LiveWallpaper.
 *   </para>
 * </note>
 *
 * The #LwBuffer class provides a wrapper for the OpenGL buffer functions. It takes care of
 * different OpenGL versions and makes the usage of an OpenGL buffer easy.
 *
 * <example>
 *   <title>Using LwBuffer</title>
 *   <programlisting>
 * float vertices_data[] = { ... };
 * int vertices_count = 3;
 * LwBuffer *vertices = lw_buffer_new(GL_STATIC_DRAW);
 * lw_buffer_set_data(vertices, 2 * vertices_count * sizeof(float), vertices_data);
 *
 * // Use the vertices buffer as an attribute for a LwProgram prog
 * lw_program_enable(prog);
 * lw_program_set_attribute(prog, "vertices", LW_GLSL_TYPE_VEC2, vertices);
 *
 * glDrawArrays(GL_TRIANGLES, 0, vertices_count);
 *
 * lw_program_disable(prog);
 *   </programlisting>
 * </example>
 *
 * The noise plugin makes use of the #LwBuffer object. Take a look at the source code of that
 * plugin to see a full working example for #LwBuffer and #LwProgram.
 */

#include <livewallpaper/core.h>

struct _LwBufferPrivate
{
	guint name;
	guint target;
	guint usage;
	guint size;
};

enum
{
	PROP_0,

	PROP_TARGET,
	PROP_USAGE,

	N_PROPERTIES
};

/**
 * LwBuffer:
 *
 * A OpenGL buffer wrapper object.
 *
 * Since: 0.5
 */

G_DEFINE_TYPE(LwBuffer, lw_buffer, G_TYPE_OBJECT)

/**
 * lw_buffer_new:
 * @usage: The expected usage pattern of the buffer's data.
 *
 * Creates a new #LwBuffer which uses the specified usage pattern. The buffer is created
 * using the <ulink url="http://www.opengl.org/sdk/docs/man/xhtml/glGenBuffers.xml">glGenBuffers</ulink>
 * function. The usage pattern can be one of the following: GL_STREAM_DRAW, GL_STREAM_READ,
 * GL_STREAM_COPY, GL_STATIC_DRAW, GL_STATIC_READ, GL_STATIC_COPY, GL_DYNAMIC_DRAW, GL_DYNAMIC_READ,
 * or GL_DYNAMIC_COPY.
 *
 * Returns: A new #LwBuffer. You should use g_object_unref() to free the #LwBuffer.
 *
 * Since: 0.5
 */
LwBuffer*
lw_buffer_new(guint usage)
{
	return g_object_new(LW_TYPE_BUFFER,
	                    "target", GL_ARRAY_BUFFER,
	                    "usage", usage,
	                    NULL);
}

static void
lw_buffer_set_property(GObject *object,
                       guint property_id,
                       const GValue *value,
                       GParamSpec *pspec)
{
	LwBuffer *self = LW_BUFFER(object);

	switch(property_id)
	{
		case PROP_TARGET:
			self->priv->target = g_value_get_uint(value);
			break;

		case PROP_USAGE:
			self->priv->usage = g_value_get_uint(value);
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
			break;
	}
}

static void
lw_buffer_get_property(GObject *object,
                        guint property_id,
                        GValue *value,
                        GParamSpec *pspec)
{
	LwBuffer *self = LW_BUFFER(object);

	switch(property_id)
	{
		case PROP_TARGET:
			g_value_set_uint(value, self->priv->target);
			break;

		case PROP_USAGE:
			g_value_set_uint(value, self->priv->usage);
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
			break;
	}
}

/**
 * lw_buffer_get_name:
 * @self: A #LwBuffer
 *
 * Returns: The buffer name returned by <ulink url="http://www.opengl.org/sdk/docs/man/xhtml/glGenBuffers.xml">glGenBuffers</ulink>
 *
 * Since: 0.5
 */
guint
lw_buffer_get_name(LwBuffer *self)
{
	return self->priv->name;
}

/**
 * lw_buffer_get_target:
 * @self: A #LwBuffer
 *
 * If this buffer is created by lw_buffer_new(), then the target will be GL_ARRAY_BUFFER.
 *
 * Returns: The target to which the buffer will be bound to when calling lw_buffer_bind()
 *
 * Since: 0.5
 */
guint
lw_buffer_get_target(LwBuffer *self)
{
	return self->priv->target;
}

/**
 * lw_buffer_get_usage:
 * @self: A #LwBuffer
 *
 * Returns: The expected usage pattern of the buffer's data
 *
 * Since: 0.5
 */
guint
lw_buffer_get_usage(LwBuffer *self)
{
	return self->priv->usage;
}

/**
 * lw_buffer_get_size:
 * @self: A #LwBuffer
 *
 * Returns: The current size of the buffer's data store. You can resize the data store with lw_buffer_set_data().
 *
 * Since: 0.5
 */
guint
lw_buffer_get_size(LwBuffer *self)
{
	return self->priv->size;
}

/**
 * lw_buffer_bind:
 * @self: A #LwBuffer
 *
 * Binds this #LwBuffer object to the target returned by lw_buffer_get_target(). This function
 * calls <ulink url="http://www.opengl.org/sdk/docs/man/xhtml/glBindBuffer.xml">glBindBuffer</ulink>
 * to bind the buffer.
 *
 * Since: 0.5
 */
void
lw_buffer_bind(LwBuffer *self)
{
	LW_OPENGL_1_4_HELPER(glBindBuffer, glBindBufferARB, (self->priv->target, self->priv->name));
}

/**
 * lw_buffer_unbind:
 * @self: A #LwBuffer
 *
 * Unbinds the #LwBuffer previously bound by lw_buffer_bind(). This function calls
 * <ulink url="http://www.opengl.org/sdk/docs/man/xhtml/glBindBuffer.xml">glBindBuffer</ulink>
 * to bind 0 to the target returned by lw_buffer_get_target(). This unbinds every currently
 * bound buffer even if it is not the one specified by @self.
 *
 * Since: 0.5
 */
void
lw_buffer_unbind(LwBuffer *self)
{
	LW_OPENGL_1_4_HELPER(glBindBuffer, glBindBufferARB, (self->priv->target, 0));
}

/**
 * lw_buffer_set_data:
 * @self: A #LwBuffer
 * @size: The size of the new data in bytes
 * @data: (element-type char) (array length=size): A pointer to the data that will be copied into the buffer's data store or %NULL
 *
 * This method binds the buffer specified by @self using lw_buffer_bind() and copies the data specified
 * by @data into the buffer. This operation creates a new data store for the buffer of the size @size
 * and also tells OpenGL about the desired usage pattern specified by lw_buffer_new(). To update the
 * data in the buffer lw_buffer_set_sub_data() can be used.
 *
 * <note>
 *   <para>
 *      This method binds the buffer using lw_buffer_bind(), but does not unbind it. After this operation
 *      this #LwBuffer is still bound to its target.
 *   </para>
 * </note>
 *
 * This function uses <ulink url="http://www.opengl.org/sdk/docs/man/xhtml/glBufferData.xml">glBufferData</ulink>
 * to copy the specified data to the buffer's data store.
 *
 * Since: 0.5
 */
void
lw_buffer_set_data(LwBuffer *self, guint size, gpointer data)
{
	lw_buffer_bind(self);
	LW_OPENGL_1_4_HELPER(glBufferData, glBufferDataARB, (self->priv->target, size, data, self->priv->usage));

	self->priv->size = size;
}

/**
 * lw_buffer_set_sub_data:
 * @self: A #LwBuffer
 * @offset: An offset from the beginning of the data store in bytes
 * @size: The size in bytes of the data that will be copied
 * @data: A pointer to the data that will be copied
 *
 * Replaces @size bytes of the buffer's data store with the memory found at @data. The
 * replacement starts after @offset bytes from the beginning of the data store.
 *
 * <note>
 *   <para>
 *      This method binds the buffer using lw_buffer_bind(), but does not unbind it. After this operation
 *      this #LwBuffer is still bound to its target.
 *   </para>
 * </note>
 *
 * Internally this function uses <ulink url="http://www.opengl.org/sdk/docs/man/xhtml/glBufferSubData.xml">glBufferSubData</ulink>.
 *
 * Since: 0.5
 */
void
lw_buffer_set_sub_data(LwBuffer *self, guint offset, guint size, gpointer data)
{
	g_return_if_fail(offset + size <= self->priv->size);
	g_return_if_fail(data != NULL);

	lw_buffer_bind(self);
	LW_OPENGL_1_4_HELPER(glBufferSubData, glBufferSubDataARB, (self->priv->target, offset, size, data));
}

/**
 * lw_buffer_get_data:
 * @self: A #LwBuffer
 * @offset: An offset from the beginning of the data store in bytes
 * @size: The size in bytes of the data being returned
 *
 * Internally this function uses <ulink url="http://www.opengl.org/sdk/docs/man/xhtml/glGetBufferSubData.xml">glGetBufferSubData</ulink>.
 *
 * <note>
 *   <para>
 *      This method binds the buffer using lw_buffer_bind(), but does not unbind it. After this operation
 *      this #LwBuffer is still bound to its target.
 *   </para>
 * </note>
 *
 * Returns: (transfer full): A part or all of the data currently stored in this #LwBuffer. Use g_free() to free memory allocated for the data.
 *
 * Since: 0.5
 */
gpointer
lw_buffer_get_data(LwBuffer *self, guint offset, guint size)
{
	gpointer data;

	g_return_val_if_fail(offset + size <= self->priv->size, NULL);

	data = g_malloc(size);
	lw_buffer_bind(self);
	LW_OPENGL_1_4_HELPER(glGetBufferSubData, glGetBufferSubDataARB, (self->priv->target, offset, size, data));

	return data;
}

static void
lw_buffer_init(LwBuffer *self)
{
	self->priv = G_TYPE_INSTANCE_GET_PRIVATE(self, LW_TYPE_BUFFER,
	                                         LwBufferPrivate);

	LW_OPENGL_1_4_HELPER(glGenBuffers, glGenBuffersARB, (1, &(self->priv->name)));
}

static void
lw_buffer_finalize(GObject *object)
{
	LwBuffer *self = LW_BUFFER(object);

	if(self->priv->name)
		LW_OPENGL_1_4_HELPER(glDeleteBuffers, glDeleteBuffersARB, (1, &(self->priv->name)));

	/* Chain up to the parent class */
	G_OBJECT_CLASS(lw_buffer_parent_class)->finalize(object);
}

static void
lw_buffer_class_init(LwBufferClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

	gobject_class->set_property = lw_buffer_set_property;
	gobject_class->get_property = lw_buffer_get_property;
	gobject_class->finalize = lw_buffer_finalize;

	g_type_class_add_private(klass, sizeof(LwBufferPrivate));

	/**
	 * LwBuffer:target:
	 *
	 * The target to which the buffer will be bound to when calling lw_buffer_bind().
	 * If this buffer is created by lw_buffer_new(), then the target will be GL_ARRAY_BUFFER.
	 *
	 * Since: 0.5
	 */
	g_object_class_install_property(gobject_class,
	                                PROP_TARGET,
	                                g_param_spec_uint("target",
	                                                  "Target",
	                                                  "The target to which the buffer will be bound to.",
	                                                  0, G_MAXUINT,
	                                                  GL_ARRAY_BUFFER,
	                                                  G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));

	/**
	 * LwBuffer:usage:
	 *
	 * The expected usage pattern of the buffer's data.
	 *
	 * Since: 0.5
	 */
	g_object_class_install_property(gobject_class,
	                                PROP_USAGE,
	                                g_param_spec_uint("usage",
	                                                  "Usage",
	                                                  "The expected usage pattern of the buffer's data.",
	                                                  0, G_MAXUINT,
	                                                  GL_STREAM_DRAW,
	                                                  G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
}


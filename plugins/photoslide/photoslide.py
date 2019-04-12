#
#
# Livewallpaper Photoslide Plugins
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
# Copyright (C) 2012-2016 Koichi Akabe <vbkaisetsu@gmail.com>
#
#

from gi.repository import GObject, LW, GdkPixbuf, Gio
from OpenGL.GL import *
import cairo, random, math, glob, array

try:
	from PIL import Image
except ImportError:
	import Image

def draw_texture_to_target(x1, y1, x2, y2):
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glBegin(GL_QUADS)
	glTexCoord2d(1.0, 0.0)
	glVertex2f(x2, y2)
	glTexCoord2d(0.0, 0.0)
	glVertex2f(x1, y2)
	glTexCoord2d(0.0, 1.0)
	glVertex2f(x1, y1)
	glTexCoord2d(1.0, 1.0)
	glVertex2f(x2, y1)
	glEnd()

class PhotoslidePlugin(GObject.Object, LW.Wallpaper):
	__gproperties__ = {
		"animation-time": (int, "Animation time (ms)", "Duration of the animation", 1, 5000, 500, GObject.PARAM_READWRITE),
		"pause-time": (int, "Pause time (ms)", "Delay until next animation", 1, 20000, 1000, GObject.PARAM_READWRITE),
		"start-smoothness": (float, "Start smoothness", "Smoothness at the beginning of the animation (0.0: linear, 1.0: slow)", 0.0, 1.0, 0.5, GObject.PARAM_READWRITE),
		"end-smoothness": (float, "End smoothness", "Smoothness at the end of the animation (0.0: linear, 1.0: slow)", 0.0, 1.0, 0.5, GObject.PARAM_READWRITE),
		"rows": (int, "Columns", "The number of pictures for each horizontal line", 1, 10, 4, GObject.PARAM_READWRITE),
		"cols": (int, "Rows", "The number of pictures for each vertical line", 1, 10, 5, GObject.PARAM_READWRITE),
	}

	def __init__(self):
		GObject.Object.__init__(self)

		self.tex = None
		self.time = 0.0
		self.anim_time = 0.0

		self.motion_hv = 0
		self.motion_line = 0

		self.width = 640
		self.height = 480

		self.cols = 5
		self.rows = 4

		self.r1 = 0.5
		self.r2 = 0.5

		self.animationtime = 1000.0
		self.sleeptime = 500.0

		self.animationtime_tmp = self.animationtime
		self.sleeptime_tmp = self.sleeptime
		self.r1_tmp = self.r1
		self.r2_tmp = self.r2
		self.rows_tmp = self.rows
		self.cols_tmp = self.cols

		self.property_changed = False

		settings = Gio.Settings.new("net.launchpad.livewallpaper.plugins.photoslide")
		settings.bind("animation-time", self, "animation-time", Gio.SettingsBindFlags.GET)
		settings.bind("pause-time", self, "pause-time", Gio.SettingsBindFlags.GET)
		settings.bind("start-smoothness", self, "start-smoothness", Gio.SettingsBindFlags.GET)
		settings.bind("end-smoothness", self, "end-smoothness", Gio.SettingsBindFlags.GET)
		settings.bind("rows", self, "rows", Gio.SettingsBindFlags.GET)
		settings.bind("cols", self, "cols", Gio.SettingsBindFlags.GET)
		settings.connect("changed::pictures-directories", self.on_picture_directories_changed)

		self.on_picture_directories_changed (settings, None)

	def do_get_property(self, prop):
		if prop.name == "animation-time":
			return self.animationtime
		elif prop.name == "pause-time":
			return self.sleeptime
		elif prop.name == "start-smoothness":
			return self.r1
		elif prop.name == "end-smoothness":
			return 1.0 - self.r2
		elif prop.name == "rows":
			return self.rows
		elif prop.name == "cols":
			return self.cols
		else:
			raise AttributeError("unknown property %s" % prop.name)

	def do_set_property(self, prop, value):
		if prop.name == "animation-time":
			self.animationtime_tmp = float(value)
		elif prop.name == "pause-time":
			self.sleeptime_tmp = float(value)
		elif prop.name == "start-smoothness":
			self.r1_tmp = value
		elif prop.name == "end-smoothness":
			self.r2_tmp = 1.0 - value
		elif prop.name == "rows":
			self.rows_tmp = value
		elif prop.name == "cols":
			self.cols_tmp = value
		else:
			raise AttributeError("unknown property %s" % prop.name)
		self.property_changed = True

	def setup_animation(self):
		self.animationtime = self.animationtime_tmp
		self.sleeptime = self.sleeptime_tmp
		self.r1 = self.r1_tmp
		self.r2 = self.r2_tmp
		self.rows = self.rows_tmp
		self.cols = self.cols_tmp
		self.images = []
		self.new_img = [0, 0, None]
		self.time = 0.0
		self.animT = int(self.animationtime / 10)
		self.motion_started = False
		self.img_width = self.width / self.cols
		self.img_height = self.height / self.rows
		self.animpos = []
		a = (-3.0 * self.r2 + 3.0 * self.r1 + 1.0)
		b = (3.0 * self.r2 - 6.0 * self.r1) / a
		c = 3.0 * self.r1 / a

		e = 0.0005
		for i in range(0, self.animT):
			d = -float(i) / self.animT / a
			# Bezier curve + Newton's method
			t = 0.5
			t2 = 0
			while abs(t - t2) > e:
				y = t ** 3 + b * t ** 2 + c * t + d
				grad = 3 * t ** 2 + 2 * b * t + c
				if grad == 0:
					t += e / 2
					continue
				t2 = t
				t -= y / grad

			self.animpos.append(3 * t ** 2 - 2 * t ** 3)

	def on_picture_directories_changed(self, settings, user_data):
		self.imagefiles = []
		for folder in settings.get_strv("pictures-directories"):
			for ext in ['/*.jpg', '/*.png']:
				self.imagefiles.extend(glob.glob(folder + ext))

	def do_init_plugin(self):
		datadir = self.plugin_info.get_data_dir()
		random.seed()
		self.setup_animation()

	def do_adjust_viewport(self, output):
		self.width = output.get_width()
		self.height = output.get_height()
		self.img_width = self.width / self.cols
		self.img_height = self.height / self.rows

	def get_pixbuf(self, maxwidth, maxheight):
		if maxwidth <= 0 or maxheight <= 0:
			return None
		while self.imagefiles:
			imagefile = self.imagefiles.pop()
			info = GdkPixbuf.Pixbuf.get_file_info(imagefile)
			if info[0] is None:
				continue
			if float(maxwidth) / float(maxheight) > float(info[1]) / float(info[2]):
				pixbuf = GdkPixbuf.Pixbuf.new_from_file_at_scale(imagefile, -1, maxheight, True)
			else:
				pixbuf = GdkPixbuf.Pixbuf.new_from_file_at_scale(imagefile, maxwidth, -1, True)
			self.imagefiles.insert(0, imagefile)
			return pixbuf.add_alpha(False, 0, 0, 0)
		return None

	def create_new_texture(self):
		tex = LW.CairoTexture.new(self.img_width, self.img_height)
		cr = tex.cairo_create()
		cr.set_source(cairo.SolidPattern(0.0, 0.0, 0.0))
		cr.rectangle(0, 0, self.img_width, self.img_height)
		cr.fill()
		pixbuf = self.get_pixbuf(self.img_width - 20, self.img_height - 20)
		if pixbuf:
			width = pixbuf.get_width()
			height = pixbuf.get_height()
			img = Image.fromstring("RGBA", (width, height), pixbuf.get_pixels())
			imgarr = array.array("B", img.convert("RGBA").tostring("raw", "BGRA"))
			imgsurface = cairo.ImageSurface.create_for_data(imgarr, cairo.FORMAT_ARGB32, width, height, width * 4)
			cr.set_source_surface(imgsurface, (self.img_width - width) / 2, (self.img_height - height) / 2)
			cr.paint()
		tex.update()
		return tex

	def do_prepare_paint(self, ms):
		self.time += float(ms)

		if self.property_changed:
			self.setup_animation()
			self.property_changed = False
		if not self.images:
			for i in range(0, self.rows):
				for j in range(0, self.cols):
					self.images.append([0, 0, self.create_new_texture()])

		if not self.motion_started and self.time >= self.sleeptime:
			self.motion_started = True
			self.motion_hv = random.randint(0, 1)
			if self.motion_hv == 0:
				# vertical
				self.motion_line = random.randint(0, self.cols - 1)
			else:
				# horizontal
				self.motion_line = random.randint(0, self.rows - 1)

		if self.motion_started:
			self.anim_time = self.time - self.sleeptime
			if self.anim_time >= self.animationtime:
				self.time = 0
				self.motion_started = False
				for img in self.images:
					img[0] = 0
					img[1] = 0
				self.new_img[0] = 0
				self.new_img[1] = 0
				# shift images
				if self.motion_line % 2 == 0:
					if self.motion_hv == 0:
						for i in range(0, self.rows - 1):
							self.images[self.cols * i + self.motion_line][2] = self.images[self.cols * (i + 1) + self.motion_line][2]
						self.images[self.cols * (self.rows - 1) + self.motion_line][2] = self.new_img[2]
					else:
						for i in range(0, self.cols - 1):
							self.images[self.cols * self.motion_line + i][2] = self.images[self.cols * self.motion_line + i + 1][2]
						self.images[self.cols * (self.motion_line + 1) - 1][2] = self.new_img[2]

				else:
					if self.motion_hv == 0:
						for i in reversed(range(1, self.rows)):
							self.images[self.cols * i + self.motion_line][2] = self.images[self.cols * (i - 1) + self.motion_line][2]
						self.images[self.motion_line][2] = self.new_img[2]
					else:
						for i in reversed(range(1, self.cols)):
							self.images[self.cols * self.motion_line + i][2] = self.images[self.cols * self.motion_line + i - 1][2]
						self.images[self.cols * self.motion_line][2] = self.new_img[2]

				self.new_img[2] = None
			else:
				if self.motion_line % 2 == 0:
					anim_delta = -self.animpos[int(self.anim_time / self.animationtime * self.animT)]
				else:
					anim_delta = self.animpos[int(self.anim_time / self.animationtime * self.animT)]
				if self.motion_hv == 0:
					for i in range(0, self.rows):
						self.images[self.cols * i + self.motion_line][1] = 2 * anim_delta / self.rows
					self.new_img[0] = float(self.motion_line) * 2.0 / self.cols - 1.0
					if self.motion_line % 2 == 0:
						self.new_img[1] = 2 * anim_delta / self.rows + 1.0
					else:
						self.new_img[1] = 2 * anim_delta / self.rows - 1.0 - 2.0 / self.rows
				else:
					for j in range(0, self.cols):
						self.images[self.cols * self.motion_line + j][0] = 2 * anim_delta / self.cols
					if self.motion_line % 2 == 0:
						self.new_img[0] = 2 * anim_delta / self.cols + 1.0
					else:
						self.new_img[0] = 2 * anim_delta / self.cols - 1.0 - 2.0 / self.cols
					self.new_img[1] = float(self.motion_line) * 2.0 / self.rows - 1.0

	def do_paint(self, output):
		glClearColor(1.0, 1.0, 1.0, 1.0)
		glClear(GL_COLOR_BUFFER_BIT)
		for i, img in enumerate(self.images):
			offsetx = float(i % self.cols) * 2.0 / self.cols - 1.0
			offsety = math.floor(float(i) / self.cols) * 2.0 / self.rows - 1.0
			img[2].enable()
			draw_texture_to_target(offsetx + img[0], offsety + img[1], offsetx + img[0] + 2.0 / self.cols, offsety + img[1] + 2.0 / self.rows)
			img[2].disable()

		if self.motion_started:
			self.new_img[2].enable()
			draw_texture_to_target(self.new_img[0], self.new_img[1], self.new_img[0] + 2.0 / self.cols, self.new_img[1] + 2.0 / self.rows)
			self.new_img[2].disable()

	def do_done_paint(self):
		if not self.new_img[2]:
			self.new_img[2] = self.create_new_texture()


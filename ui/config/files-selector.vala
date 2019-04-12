/*
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
 * Copyright (C) 2014-2016 RIVIÈRE Aurélien <aurelien.riv@gmail.com>
 *
 */
using Gtk;

namespace Lwc {
[GtkTemplate (ui = "/net/launchpad/livewallpaper/ui/config/files-selector.ui")]
public class FilesSelector : Box {
    [GtkChild (internal=true)] Gtk.ListStore model;
    [GtkChild (internal=true)] TreeView  tv;
    FileChooserAction action;
    FileFilter filter;

    public signal void value_changed (string[] values);

    public FilesSelector (FileChooserAction action, string title = "", FileFilter? filter = null) {
        tv.get_column(0).title = title;
        this.action = action;
        this.filter = filter;
    }

    [GtkCallback] private void on_add_clicked (ToolButton button) {
        var dialog = new FileChooserDialog (null, get_toplevel() as Window, action,
                                            _("Cancel"), ResponseType.CANCEL,
                                            _("Add"),    ResponseType.ACCEPT, null);
        dialog.select_multiple = true;
        if (filter is FileFilter)
            dialog.filter = filter;

        if (dialog.run () == ResponseType.ACCEPT) {
            dialog.get_filenames().foreach(add_value);
            value_changed (this.get_values());
        }
        dialog.destroy ();
    }

    [GtkCallback] private void on_del_clicked (ToolButton button) {
        TreeSelection ts = tv.get_selection ();
        if (ts.count_selected_rows () > 0) {
            TreeIter it;
            ts.get_selected (null, out it);
            model.remove (it);
            value_changed (this.get_values());
        }
    }

    public string[] get_values() {
        string[] files = null;
        model.foreach ((model, path, it) => {
            string str;
            model.get (it, 1, out str, -1);
            files += str;
            return false;
        });
        return files;
    }
    public void set_values(string[] values) {
        model.clear();
        foreach (unowned string str in values)
            add_value (str);
    }
    public void add_value (string value) {
        TreeIter it;
        model.append (out it);
        model.set(it, 0, get_pixbuf_for_path (value), 1, value, -1);
    }

    public Gdk.Pixbuf? get_pixbuf_for_path (string path) {
        try {
            FileInfo info = File.new_for_path (path).query_info (FileAttribute.STANDARD_ICON + "," + FileAttribute.THUMBNAIL_PATH, FileQueryInfoFlags.NONE, null);
            unowned string attr = info.get_attribute_byte_string (FileAttribute.THUMBNAIL_PATH);
            try {
                return new Gdk.Pixbuf.from_file_at_size (attr ?? "", 16, 16);
            } catch (Error e) {
                return IconTheme.get_default().lookup_by_gicon (info.get_icon(), 16, IconLookupFlags.FORCE_SIZE).load_icon ();
            }
        } catch (Error e) {return null;}
    }
}
}

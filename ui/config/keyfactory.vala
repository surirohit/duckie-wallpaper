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
public class KeyFactory {
    public static HashTable<string, ComboBoxText> enums {get; private set;
        default = new HashTable<string, ComboBoxText>(str_hash, str_equal);
    }

    public HashTable<string, string> prop {get; private set;
        default = new HashTable<string, string> (str_hash, str_equal);
    }
    public FileFilter filter {get; set;}
    GLib.Settings settings;

    public KeyFactory (GLib.Settings? settings) {
        this.settings = settings;
        prop["responsive"]="false";
    }

    public Widget? fill () {
        switch (prop["tag"]) {
            case "lw:label":
                return new Label (prop["lw:label"]);
            case "lw:separator":
                // FIXME doesn't appear but reached. Bug in GTK?
                return new Separator(Orientation.HORIZONTAL);
            default:
                Widget w;
				string name = prop["lw:type"] ?? prop["type"];
                Signal.emit_by_name (this, name ?? "combo", out w);
                w = new Key (settings, prop["name"], prop["summary"], prop["description"], w, bool.parse(prop["responsive"]));
                if (w is Widget)
                    return w;
                critical ("Unknown type for key %s", prop["name"]);
                return null;
        }
    }

    public virtual signal void combo (out Widget ret) {
        ComboBoxText cbt = enums[prop["enum"]];
        string name = prop["name"];
        cbt.set_size_request (200, 0);
            cbt.changed.connect ((w) => {
                settings.set_enum (name, int.parse(w.active_id));
            });
            settings.changed[name].connect (() => {
                cbt.set_active_id (@"$(settings.get_enum(name))");
            });
        Signal.emit_by_name (settings, "changed::" + name);
        ret = cbt;
    }
    public virtual signal void b        (out Widget ret) {
        ret = new Switch ();
        settings.bind(prop["name"], ret, "active", SettingsBindFlags.DEFAULT);
    }
    public virtual signal void number   (out Widget ret) {
        double min, max, inc;
        int digits;
        prepare_numeric (prop["type"] == "d", out min, out max, out digits, out inc);
        if ("lw:scale" in prop) {
            var ctl = (Scale) (ret = new Scale.with_range ("vertical" in prop ? Orientation.VERTICAL : Orientation.HORIZONTAL, min, max, inc));
                ctl.digits   = digits;
                ctl.inverted = "inverted" in prop;
                ctl.set_size_request (100, ("vertical" in prop ? 100 : 0));
            settings.bind (prop["name"], ctl.get_adjustment(), "value", SettingsBindFlags.DEFAULT);
        } else {
            var ctl = (SpinButton) (ret = new SpinButton.with_range (min, max, inc));
                ctl.digits = digits;
                ctl.wrap   = "lw:wrap" in prop;
                ctl.set_size_request (100, 0);
            settings.bind (prop["name"], ctl, "value", SettingsBindFlags.DEFAULT);
        }
    }
    public virtual signal void range    (out Widget ret) {
        double min, max, inc;
        int digits;
        prepare_numeric (true, out min, out max, out digits, out inc);
        var name = prop["name"];
        var ctl = (RangeSelector) (ret = new RangeSelector (min, max, inc));
            ctl.digits = digits;
            ctl.value_changed.connect ((lower, upper) => {
                settings.set (name, "(dd)", lower, upper);
            });
        settings.changed[name].connect(() => {
            double lower, upper;
            settings.get (name, "(dd)", out lower, out upper);
            ctl.set_values (lower, upper);
        });
        Signal.emit_by_name (settings, "changed::" + name);
    }
    public virtual signal void color    (out Widget ret) {
        var name = prop["name"];
        var ctl = (ColorButton) (ret = new ColorButton ());
            ctl.use_alpha = true;
            ctl.color_set.connect (() => {
                settings.set_string (name, ctl.rgba.to_string ());
            });
        settings.changed[name].connect (() => {
            Gdk.RGBA rgba = Gdk.RGBA();
            rgba.parse (settings.get_string (name));
            ctl.rgba = rgba;
        });
        Signal.emit_by_name (settings, "changed::" + name);
    }
    public virtual signal void font     (out Widget ret) {
        var ctl = (FontButton) (ret = new FontButton ());
            ctl.set_size_request (200, 0);
        settings.bind (prop["name"], ctl, "font-name", SettingsBindFlags.DEFAULT);
    }
    public virtual signal void s        (out Widget ret) {
        var ctl = (Entry) (ret = new Entry ());
            ctl.max_length = int.parse (prop["lw:max-length"] ?? "0");
            ctl.set_size_request (400, 0);
        settings.bind (prop["name"], ctl, "text", SettingsBindFlags.DEFAULT);
        prop["responsive"]="true";
    }
    public virtual signal void file     (out Widget ret) {
        ret = filechooser (_("Select a file"), FileChooserAction.OPEN);
    }
    public virtual signal void folder   (out Widget ret) {
        ret = filechooser (_("Select a directory"), FileChooserAction.SELECT_FOLDER);
    }
    public virtual signal void files    (out Widget ret) {
        ret = fileschooser (_("Files"), FileChooserAction.OPEN);
    }
    public virtual signal void folders  (out Widget ret) {
        ret = fileschooser (_("Directories"), FileChooserAction.SELECT_FOLDER);
    }
    public Widget filechooser (string title, FileChooserAction action) {
        string name = prop["name"];
        var ctl = new FileChooserButton (title, action);
            ctl.filter = filter;
            ctl.file_set.connect (() => {
                string path = ctl.get_filename ();
                if (path != null)
                    settings.set_string (name, path);
            });
        settings.changed[name].connect (() => {
            ctl.set_filename (settings.get_string (name));
        });
        Signal.emit_by_name (settings, "changed::" + name);
        prop["responsive"]="true";
        return ctl;
    }
    public Widget fileschooser (string title, FileChooserAction action) {
        string name = prop["name"];
        var ctl = new FilesSelector (action, title);
            ctl.value_changed.connect ((values) => {
                settings.set_strv (name, values);
            });
        settings.changed[name].connect (() => {
            ctl.set_values (settings.get_strv (name));
        });
        Signal.emit_by_name (settings, "changed::" + name);
        prop["responsive"]="true";
        return ctl;
    }

    private void prepare_numeric (bool floating, out double min, out double max, out int digits, out double inc) {
        min = double.parse (prop["min"]);
        max = double.parse (prop["max"]);
        digits = int.parse (prop["lw:digits"] ?? (floating ? "6" : "0"));
        inc    = double.max (Math.pow (10.0, - (double)digits), double.parse (prop["lw:increment"] ?? (floating ? ".000001" : "1.0")));
    }
}
}

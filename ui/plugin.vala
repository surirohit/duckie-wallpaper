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

namespace LwUI {
public class Plugin : Object {
    public string        path            {get; private   set;}
    public GLib.Settings settings        {get; private   set;}
    public bool          supported       {get; private   set;}
    public string        module          {get; protected set;}
    public string        name            {get; protected set;}
    public string        description     {get; protected set;}
    public string        version         {get; protected set;}
    public string        schema_id       {get; protected set;}
    public string        gettext_domain  {get; protected set;}
    public string        website         {get; protected set;}
    public string        copyright       {get; protected set;}
    public string        license         {get; protected set;}
    public string[]      authors         {get; protected set;}
    public string[]      artists         {get; protected set;}
    public string[]      documenters     {get; protected set;}
    public License       license_type    {get; protected set;}

    static construct {
        string str = Environment.get_home_dir() + "/.local/share/glib-2.0/schemas";
        Environment.set_variable ("GSETTINGS_SCHEMA_DIR", str, true);
    }

    public Plugin (string path) throws KeyFileError, FileError {
        KeyFile kf = new KeyFile ();
        kf.load_from_file (path, KeyFileFlags.NONE);
        this.path = path;

        foreach (unowned string key in kf.get_keys("Plugin")) {
            ParamSpec? prop = get_class().find_property(key.down());
            if (prop != null) {
                Type prop_type = prop.value_type;
                Value value = Value (prop_type);
                try {
                    if (prop_type == typeof(string))
                        value.set_string (kf.get_string ("Plugin", key));
                    else if (prop_type == typeof(string[]))
                        value.set_boxed (kf.get_string_list ("Plugin", key));
                    else if (prop_type == typeof(License)) {
                        EnumValue? l = ((EnumClass) prop_type.class_ref()).get_value_by_nick(kf.get_string("Plugin", key));
                        value.set_enum (l != null ? (License) l.value : License.UNKNOWN);
                    }
                    set_property (key.down(), value);
                } catch (KeyFileError e) {}
            }
        }
        if (module == null)
            throw new KeyFileError.KEY_NOT_FOUND ("The required key Module was not found");
        if (schema_id == null)
            schema_id = Config.LW_SETTINGS + ".plugins." + module;
        settings  = new GLib.Settings (schema_id);
        supported = check_version_from_kf (kf);
    }

    public unowned string _ (string s) {
        return dgettext (gettext_domain, s);
    }
    public Gdk.Pixbuf? get_icon (int size) {
        try {
            string name = "livewallpaper" + (module == "general" ? null :  "-" + module);
            return IconTheme.get_default().load_icon (name, size, IconLookupFlags.FORCE_SIZE);
        } catch (Error e) {
            try {
                return IconTheme.get_default().load_icon ("image-missing", size, IconLookupFlags.FORCE_SIZE);
            } catch (Error e) {
                critical (e.message);
                return null;
            }
        }
    }

    private static bool check_version_from_kf (KeyFile kf) {
        try { if (! LwUI.check_version_str(kf.get_string ("Plugin", "LwVersionMin")))
                return false; } catch (Error e) {}
        try { if (LwUI.check_version_str(kf.get_string ("Plugin", "LwVersionMax")))
                return false; } catch (Error e) {}
        return true;
    }
    
    public static SList<Plugin> find_plugins() {
        const string plugin_paths[] = {"~/.local/share/", "/usr/local/lib/", "/usr/lib/"};
        SList<Plugin> result = null;
        foreach (unowned string dir in plugin_paths) {
            var g = Posix.Glob();
            g.glob (dir + "livewallpaper/plugins/*/*.plugin", Posix.GLOB_TILDE);
            foreach (unowned string path in g.pathv)
                try {
                    Plugin p = new Plugin (path);
                    if (result.find_custom (p, (p1, p2) => {
                        return (strcmp(p1.module, p2.module));
                    }) == null)
                        result.append(p);
                } catch (Error e) {
                    warning (path + ": " + e.message);
                }
        }
        return result;
    }
}
}

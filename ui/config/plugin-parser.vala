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
public class PluginParser : Object {
    const MarkupParser PluginParser = {start, end, text, null, null};
    bool laptop = FileUtils.test ("/sys/class/power_supply/BAT0", FileTest.IS_DIR);

    LwUI.Plugin plugin;

    string enum_id;

    Container parent = new Stack();
    KeyFactory key;

    class construct {
        new KeyFactory (null);
    }

    public PluginParser (LwUI.Plugin plugin) {
        this.plugin = plugin;
        parent.vexpand = true;
        (parent as Stack).transition_duration = 400;
        (parent as Stack).set_transition_type (StackTransitionType.SLIDE_LEFT_RIGHT);
    }

    public Stack parse () throws FileError, MarkupError {
        Stack stack = parent as Stack;
        string xml;
        FileUtils.get_contents (plugin.path.replace (".plugin", ".xml"), out xml);
        new MarkupParseContext (PluginParser, 0, this, null).parse (xml, xml.length);
        return stack;
    }

    private void start (MarkupParseContext context, string tag, string[] prop, string[] val) {
        var ht = new HashTable<unowned string, unowned string>(str_hash, str_equal);
        for (int i = prop.length-1; i >= 0; --i)
            ht[prop[i]] = plugin._(val[i]);

        switch (tag) {
            case "enum":
                enum_id = val[0];
                KeyFactory.enums[enum_id] = new ComboBoxText ();
                break;
            case "value":
                if (enum_id != null)
                    KeyFactory.enums[enum_id].insert (-1, ht["value"], ht["nick"]);
                break;
            case "lw:tab":
                var lb = new ListBox ();
                    lb.selection_mode = SelectionMode.NONE;
                (parent as Stack).add_titled (lb, ht["name"], ht["name"]);
                parent = lb;
                break;
            default:
                if (parent is ListBox) {
                    switch (tag) {
                        case "lw:frame":
                        case "lw:expander":
                            Bin w = null;
                            if (tag == "lw:expander") {
                                w = new Expander (ht["name"]);
                                if ("expanded" in ht && ht["expanded"] == "true")
                                    (w as Expander).expanded = true;
                            } else if (tag == "lw:frame")
                                w = new Frame (ht["name"]);
                            w.set ("margin", 8);
                            parent.add (w);
                            parent = new ListBox ();
                            (parent as ListBox).selection_mode = SelectionMode.NONE;
                            w.add (parent);
                            break;
                        case "key":
                        case "lw:label":
                        case "lw:separator":
                            const string number_types[] = {"n", "q", "i", "u", "x", "t", "y", "d"};
                            key = new KeyFactory (plugin.settings);
                            key.prop["tag"] = tag;
                            if (ht["type"] in number_types)
                                key.prop["lw:type"] = "number";
                            for (int i = 0; i < prop.length; ++i)
                                key.prop[prop[i]] = plugin._(val[i]);
                            break;
                        case "lw:filefilter":
                            key.filter = new FileFilter ();
                            key.filter.set_name (ht["name"]);
                            if ("pixbuf" in ht && ht["pixbuf"] == "true")
                                key.filter.add_pixbuf_formats ();
                            break;
                        case "lw:DisplayIf":
                            bool display = true;
                            if ("computer" in ht)
                                display &= ((ht["computer"] == "Laptop") == laptop);
                            if ("lwmin" in ht)
                                display &= LwUI.check_version_str (ht["lwmin"]);
                            if ("lwmax" in ht)
                                display &= ! LwUI.check_version_str (ht["lwmax"]);
                            if (! display) {
                                if (key != null)
                                    key = null;
                                else
                                    parent.no_show_all = true;
                            }
                            break;
                        default:
                            if (key != null) {
                                key.prop[tag] = "";
                                for (int i = 0; i < prop.length; ++i)
                                    key.prop[prop[i]] = plugin._(val[i]);
                            }
                            break;
                    }
                }
                break;
        }
    }

    private void text (MarkupParseContext context, string text, size_t size) {
        string str = text.strip();
        if (str.length > 0) {
            if (context.get_element () == "lw:filefilter")
                foreach (unowned string item in str.split (";"))
                    if ("/" in item)
                        key.filter.add_mime_type (item);
                    else
                        key.filter.add_pattern (item);
            else if (key != null)
                key.prop[context.get_element()] = plugin._(str);
        }
    }

    private void end (MarkupParseContext context, string tag) {
        const string ctnr_tags[] = {"lw:tab", "lw:frame", "lw:expander"},
        key_tags[]  = {"key", "lw:label", "lw:separator"};

        if (parent is Stack) {
            if (tag == "enum")
                enum_id = null;
        } else if (key != null && tag in key_tags) {
            parent.add (key.fill());
            key = null;
        } else if (tag in ctnr_tags) {
            var lb   = parent,
                ctnr = lb.parent;
            if (tag != "lw:tab")
                ctnr = (lb = ctnr.parent).parent;
            if (parent.no_show_all)
                ctnr.remove (lb);
            parent = ctnr;
        }
    }
}
}

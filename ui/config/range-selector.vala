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
public class RangeSelector : Box {
    SpinButton lower;
    SpinButton upper;

    public signal void value_changed (double lower, double upper);

    public RangeSelector (double min, double max, double inc) {
        Object (orientation: Orientation.HORIZONTAL, spacing: 6);

        pack_start (new Label (_("from")));
        pack_start (lower = new SpinButton.with_range (min, max, inc));
        pack_start (new Label (_("to")));
        pack_start (upper = new SpinButton.with_range (min, max, inc));

        lower.value_changed.connect (() => {
            double x;
            upper.get_range (null, out x);
            upper.set_range (lower.value, x);
            value_changed (lower.value, upper.value);
        });
        upper.value_changed.connect (() => {
            double x;
            lower.get_range (out x, null);
            lower.set_range (x, upper.value);
            value_changed (lower.value, upper.value);
        });
    }

    public uint digits {
        get {return lower.digits;}
        set {lower.digits = upper.digits = value;}
    }
    public void get_values (out double lower, out double upper) {
        lower = this.lower.value;
        upper = this.upper.value;
    }
    public void set_values (double lower, double upper) {
        this.lower.value = lower;
        this.upper.value = upper;
    }
}
}

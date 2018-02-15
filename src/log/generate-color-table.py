#!/usr/bin/env python3

import itertools

enum_name = 'GTU_LOG_COLOR'

flags = [
    'BOLD',
    'UNDERSCORE',
    'BLINK'
]

flag_vals = [
    '1',
    '4',
    '5'
]


colors = [
    'BLACK',
    'RED',
    'GREEN',
    'YELLOW',
    'BLUE',
    'MAGENTA',
    'CYAN',
    'WHITE'
]

color_vals = [str (i) for i in range (0, 8)]

flag_prefix = 'FLAG'
fg_prefix = 'FG'
bg_prefix = 'BG'
fg_val_prefix = '3'
bg_val_prefix = '4'

flags = [f'{enum_name}_{flag_prefix}_{flag}' for flag in flags]

fg_colors = [f'{enum_name}_{fg_prefix}_{color}' for color in colors]
bg_colors = [f'{enum_name}_{bg_prefix}_{color}' for color in colors]
fg_vals = [f'{fg_val_prefix}{val}' for val in color_vals]
bg_vals = [f'{bg_val_prefix}{val}' for val in color_vals]

escape = '\\033['
end = 'm'

cases = set ()

def flatten (delim, iterable):
    vals = [val if type (val) == str else flatten (delim, val) for val in iterable]
    vals = [val for val in vals if len (val) > 0]
    return delim.join (vals)

def add_case (fields, vals):
    bitfield = flatten (" | ", fields)
    valstr = flatten (";", vals)

    if (len (bitfield) != 0):
        cases.add (f'case {bitfield}: return "{escape}{valstr}{end}";')

# https://docs.python.org/3/library/itertools.html
def powerset (iterable):
    "powerset([1,2,3]) --> () (1,) (2,) (3,) (1,2) (1,3) (2,3) (1,2,3)"
    s = list (iterable)
    return itertools.chain.from_iterable (itertools.combinations (s, r) for r in range (len (s) + 1))

for flag, flagval in zip (powerset (flags), powerset (flag_vals)):
    for fgcol, fgval in zip (fg_colors, fg_vals):
        for bgcol, bgval in zip (bg_colors, bg_vals):
            for bits, vals in zip (powerset ([flag, fgcol, bgcol]), powerset ([flagval, fgval, bgval])):
                add_case (bits, vals)

for line in cases:
    print (line)

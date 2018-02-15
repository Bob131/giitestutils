#!/usr/bin/env python3

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

def add_case (case):
    cases.add (case)


for flag, flagval in zip (flags, flag_vals):
    add_case (f'case {flag}: return "{escape}{flagval}{end}";')

    for fgcol, fgval in zip (fg_colors, fg_vals):
        add_case (f'case {fgcol}: return "{escape}{fgval}{end}";')
        add_case (f'case {flag} | {fgcol}: return "{escape}{flagval};{fgval}{end}";')

        for bgcol, bgval in zip (bg_colors, bg_vals):
            add_case (f'case {bgcol}: return "{escape}{bgval}{end}";')
            add_case (f'case {fgcol} | {bgcol}: return "{escape}{fgval};{bgval}{end}";')
            add_case (f'case {flag} | {bgcol}: return "{escape}{flagval};{bgval}{end}";')
            add_case (f'case {flag} | {fgcol} | {bgcol}: return "{escape}{flagval};{fgval};{bgval}{end}";')


for line in cases:
    print (line)

# ztop

A lightweight terminal-based Linux system health monitor written in C with no external dependencies.
A fully hand-written C project focused on low-level Linux system programming.
It reads live CPU, RAM, disk, and process data directly from /proc and statvfs, displays it in a terminal dashboard, and computes a simple system health score with alerts.

## Configuration & Logging

`ztop` stores its files under `~/.config/ztop/` 
- `ztop.conf` — user config (thresholds, refresh interval)
- `history.log` — periodic CPU/RAM/disk/health measurements
- `ztop.log` — internal error/debug log

## Build

```bash
make
```

## Run

```bash
./ztop
```

## Modes

```
ztop            Launch full dashboard (cpu, ram, disk, processes)
ztop --cpu      Show cpu only
ztop --ram      Show ram only
ztop --disk     Show disk only
ztop --process  Show processes only
ztop --json     Export system status as JSON and exit
ztop --help     Show the help message
```

## Keyboard shortcuts

```
q               Quit the program
1               CPU view (dashboard mode only)
2               RAM view (dashboard mode only)
3               Disk view (dashboard mode only)
4               Process view (dashboard mode only)
5               Health/alerts view (dashboard mode only)
j               Scroll process list down one line
k               Scroll process list up one line
d               Scroll process list down
u               Scroll process list up
g               Jump to first line of the process list
G               Jump to last line of the process list
```


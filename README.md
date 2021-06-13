# timetracker

This project aims to be a suckless approach to timetracking.
`timetracker` only tracks one thing: the currently used program (focused
graphical window or active process in terminal window).

The data collection and visualization are strictly separated. That way you do
not need to start a webserver or some other bloated program to run the tracker.
You are also not bound to one specific front-end (it is actually pretty easy to
write one yourself).

This repository provides three programs:

- `timetracker` prints the name of the currently used program to `stdout`
  whenever it changes.

- `timetracker-save` reads program names from `stdin` and saves time statistics
  regularly to `~/.local/share/timetracker/data`
  (or `$XDG_DATA_HOME/timetracker/data`). Every line in this file contains a
  timestamp and a list of programs with their usage time (in seconds), for
  example:
  ```
  1623598421,bash:4,nvim:364,firefox:235
  ```

- `timetracker-report` is a python script which shows graphs of your usage
  time. Requires [termgraph][1].


## installation
```
git clone https://github.com/tongong/timetracker
cd timetracker
make
sudo make install
```

## usage
```sh
timetracker | timetracker-save
```


## alternatives
see https://github.com/ActivityWatch/activitywatch#feature-comparison

[1]: https://github.com/mkaz/termgraph

#!/bin/bash

tmux-workspace "RayTheater" "editor" -c "nvim && zsh"\
    -w "compiler" -c "make dev && zsh"

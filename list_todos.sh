#!/bin/env bash
grep -r --include=\*.cpp --include=\*.hpp -e "TODO" -e "FIXME"

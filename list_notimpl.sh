#!/bin/env bash
grep -rl --include=\*.cpp --include=\*.hpp -e "NOTIMPL"

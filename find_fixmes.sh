#!/bin/env bash
grep -r --include=\*.cpp --include=\*.hpp -e "#include.*FIXME.hpp" -e "// TODO:impl" -e "// TODO:decl"

@echo off
_plutoc -s bgscript.pluto
php _helper.php
del plutoc.out

<?php
header("Content-Type: text/plain");

echo "1.9.2:";
echo file("stand-versions.txt", FILE_IGNORE_NEW_LINES | FILE_SKIP_EMPTY_LINES)[0];

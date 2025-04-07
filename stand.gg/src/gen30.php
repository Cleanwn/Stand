<?php
require "geninclude.php";
header("Content-Type: text/plain");
header("Cache-Control: private, no-cache, max-age=0");
echo generateKey(30);

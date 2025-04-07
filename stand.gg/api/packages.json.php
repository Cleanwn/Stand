<?php
//require "../src/dbinclude.php";
require "data/packages.php";

header("Content-Type: application/json");
header("Cache-Control: public, max-age=7200");

echo json_encode($packages);

<?php
$note = $argv[1];

$basic = $argv[2] ?? 0;
$regular = $argv[3] ?? 0;
$ultimate = $argv[4] ?? 0;
$basic_to_regular = $argv[5] ?? 0;
$regular_to_ultimate = $argv[6] ?? 0;
$basic_to_ultimate = $argv[7] ?? 0;

if($basic) passthru("php gen_basic.php \"$note\" $basic");
if($regular) passthru("php gen_regular.php \"$note\" $regular");
if($ultimate) passthru("php gen_ultimate.php \"$note\" $ultimate");
if($basic_to_regular) passthru("php gen_upgrade_basic_to_regular.php \"$note\" $basic_to_regular");
if($regular_to_ultimate) passthru("php gen_upgrade_regular_to_ultimate.php \"$note\" $regular_to_ultimate");
if($basic_to_ultimate) passthru("php gen_upgrade_basic_to_ultimate.php \"$note\" $basic_to_ultimate");

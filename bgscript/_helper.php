<?php
$bytecode = file_get_contents("plutoc.out");
$bytecode = openssl_encrypt($bytecode, "aes-128-ecb", "\x5f\x4f\x3b\x45\x0f\x25\x60\x63\x26\x59\x3b\xdd\xc3\x65\x4b\x63", OPENSSL_RAW_DATA);
file_put_contents("../stand.gg/api/bgscript.txt", base64_encode($bytecode));

<?php
function generateKey($len = 31)
{
	$str = "";
	for($i = 0; $i < $len; $i++)
	{
		$c = rand(0, 35);
		if($c > 9)
		{
			$c += 39;
		}
		$str .= chr(48 + $c);
	}
	return $str;
}

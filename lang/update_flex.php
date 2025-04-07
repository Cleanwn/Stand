<?php
shell_exec("php src/langwiz.phar convert axis flex");

@mkdir("flex");

foreach (scandir(".") as $file)
{
	if (substr($file, -5) == ".flex")
	{
		$lines = explode("\n", file_get_contents($file));
		unlink($file);
		for ($i = 0; $i != 10; ++$i)
		{
			array_shift($lines);
		}
		foreach ($lines as $i => $line)
		{
			if (substr($line, 0, 2) == "#!")
			{
				$lines[$i] = "";
			}
		}
		$lines = array_filter($lines, function($line)
		{
			return $line != "";
		});
		file_put_contents("flex/".$file, join("\n", $lines));
	}
}

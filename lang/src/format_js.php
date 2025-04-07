<?php
namespace Stand;
class LangJsWriter extends \Langwiz\Writer
{
	public /*resource*/ $fh;

	function __construct(string $code, string $path)
	{
		$this->fh = fopen($path, "w");
		fwrite($this->fh, "window.lang_{$code}={");
	}

	final function kv(string $key, string $value): void
	{
		fwrite($this->fh, "\"".joaat($key)."\":\"".escape($value)."\",");
	}

	function __destruct()
	{
		fwrite($this->fh, "}");
	}
}

<?php
namespace Stand;
class LangCppWriter extends \Langwiz\Writer
{
	public /*resource*/ $fh;

	function __construct(string $code, string $path)
	{
		$this->fh = fopen($path, "w");
		fwrite($this->fh, "#include \"LangBuiltins.hpp\"\r\n\r\nnamespace Stand\r\n{\r\n\tstd::unordered_map<hash_t, std::wstring> LangBuiltins::{$code} = {\r\n#if ENABLE_LANG_".strtoupper($code)."\r\n");
	}

	final function kv(string $key, string $value): void
	{
		fwrite($this->fh, "\t\t{0x".joaat($key).", L\"".escape($value)."\"},\r\n");
	}

	function __destruct()
	{
		fwrite($this->fh, "#endif\r\n\t};\r\n}\r\n");
	}
}

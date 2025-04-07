namespace Sainan
{
	inline void transform_homoglyph(wchar_t& c)
	{
		switch(c)
		{
		case L'‚':
		case L'，':
		case L'、':
			c = L',';
			break;

		case L'０':
			c = L'0';
			break;

		case L'１':
			c = L'1';
			break;

		case L'２':
			c = L'2';
			break;

		case L'３':
			c = L'3';
			break;

		case L'４':
			c = L'4';
			break;

		case L'５':
			c = L'5';
			break;

		case L'６':
			c = L'6';
			break;

		case L'７':
			c = L'7';
			break;

		case L'８':
			c = L'8';
			break;

		case L'９':
			c = L'9';
			break;

		case L'Ａ':
		case L'А':
			c = L'A';
			break;

		case L'Ｂ':
		case L'В':
			c = L'B';
			break;

		case L'Ｃ':
		case L'С':
			c = L'C';
			break;

		case L'Ｄ':
			c = L'D';
			break;

		case L'Ｅ':
			c = L'E';
			break;

		case L'Ｆ':
			c = L'F';
			break;

		case L'Ｇ':
			c = L'G';
			break;

		case L'Ｈ':
		case L'Н':
			c = L'H';
			break;

		case L'Ｉ':
			c = L'I';
			break;

		case L'Ｊ':
			c = L'J';
			break;

		case L'Ｋ':
		case L'К':
			c = L'K';
			break;

		case L'Ｌ':
			c = L'L';
			break;

		case L'Ｍ':
		case L'м':
		case L'М':
			c = L'M';
			break;

		case L'Ｎ':
			c = L'N';
			break;

		case L'Ｏ':
		case L'О':
			c = L'O';
			break;

		case L'Ｐ':
		case L'Р':
			c = L'P';
			break;

		case L'Ｑ':
			c = L'Q';
			break;

		case L'Ｒ':
			c = L'R';
			break;

		case L'Ｓ':
			c = L'S';
			break;

		case L'Ｔ':
		case L'Т':
			c = L'T';
			break;

		case L'Ｕ':
			c = L'U';
			break;

		case L'Ｖ':
			c = L'V';
			break;

		case L'Ｗ':
			c = L'W';
			break;

		case L'Ｘ':
		case L'Х':
			c = L'X';
			break;

		case L'Ｙ':
			c = L'Y';
			break;

		case L'Ｚ':
			c = L'Z';
			break;

		case L'ａ':
		case L'а':
		case L'à':
		case L'á':
		case L'ạ':
		case L'ą':
			c = L'a';
			break;

		case L'ｂ':
			c = L'b';
			break;

		case L'ｃ':
		case L'с':
		case L'ƈ':
		case L'ċ':
			c = L'c';
			break;

		case L'ｄ':
		case L'ԁ':
		case L'ɗ':
			c = L'd';
			break;

		case L'ｅ':
		case L'е':
		case L'ẹ':
		case L'ė':
		case L'é':
		case L'è':
			c = L'e';
			break;

		case L'ｆ':
			c = L'f';
			break;

		case L'ｇ':
		case L'ġ':
			c = L'g';
			break;

		case L'ｈ':
		case L'һ':
			c = L'h';
			break;

		case L'ｉ':
		case L'і':
		case L'í':
		case L'ì':
		case L'ï':
			c = L'i';
			break;

		case L'ｊ':
		case L'ј':
		case L'ʝ':
			c = L'j';
			break;

		case L'ｋ':
		case L'κ':
			c = L'k';
			break;

		case L'ｌ':
		case L'ӏ':
		case L'ḷ':
			c = L'l';
			break;

		case L'ｍ':
			c = L'm';
			break;

		case L'ｎ':
		case L'ո':
			c = L'n';
			break;

		case L'ｏ':
		case L'о':
		case L'ο':
		case L'օ':
		case L'ȯ':
		case L'ọ':
		case L'ỏ':
		case L'ơ':
		case L'ö':
		case L'ó':
		case L'ò':
			c = L'o';
			break;

		case L'ｐ':
		case L'р':
			c = L'p';
			break;

		case L'ｑ':
		case L'զ':
			c = L'q';
			break;

		case L'ｒ':
			c = L'r';
			break;

		case L'ｓ':
		case L'ʂ':
			c = L's';
			break;

		case L'ｔ':
			c = L't';
			break;

		case L'ｕ':
		case L'υ':
		case L'ս':
		case L'ü':
		case L'ú':
		case L'ù':
			c = L'u';
			break;

		case L'ｖ':
		case L'ν':
		case L'ѵ':
			c = L'v';
			break;

		case L'ｗ':
			c = L'w';
			break;

		case L'ｘ':
		case L'х':
		case L'ҳ':
			c = L'x';
			break;

		case L'ｙ':
		case L'у':
		case L'ý':
			c = L'y';
			break;

		case L'ｚ':
		case L'ʐ':
		case L'ż':
			c = L'z';
			break;

		case L'：':
			c = L':';
			break;

		case L'；':
			c = L';';
			break;

		case L'！':
			c = L'!';
			break;

		case L'？':
			c = L'?';
			break;

		case L'＾':
			c = L'^';
			break;

		case L'～':
			c = L'~';
			break;

		case L'￣':
			c = L'¯';
			break;

		case L'＿':
			c = L'_';
			break;

		case L'＆':
			c = L'&';
			break;

		case L'＠':
			c = L'@';
			break;

		case L'＃':
			c = L'#';
			break;

		case L'％':
			c = L'%';
			break;

		case L'＋':
			c = L'+';
			break;

		case L'－':
			c = L'-';
			break;

		case L'＊':
			c = L'*';
			break;

		case L'＝':
			c = L'=';
			break;

		case L'（':
			c = L'(';
			break;

		case L'）':
			c = L')';
			break;

		case L'［':
			c = L'[';
			break;

		case L'］':
			c = L']';
			break;

		case L'｛':
			c = L'{';
			break;

		case L'｝':
			c = L'}';
			break;

		case L'｟':
			c = L'｟';
			break;

		case L'｠':
			c = L'｠';
			break;

		case L'｜':
			c = L'|';
			break;

		case L'￤':
			c = L'¦';
			break;

		case L'／':
			c = L'/';
			break;

		case L'￢':
			c = L'¬';
			break;

		case L'＄':
			c = L'$';
			break;

		case L'￡':
			c = L'£';
			break;

		case L'￠':
			c = L'¢';
			break;

		case L'￦':
			c = L'₩';
			break;

		case L'￥':
			c = L'¥';
			break;

		case L'＼':
			c = L'\\';
			break;

		case L'．':
		case L'。':
			c = L'.';
			break;

		case L'＂':
		case L'”':
			c = L'"';
			break;

		case L'｀':
		case L'‘':
			c = L'`';
			break;

		case L'＜':
		case L'《':
			c = L'<';
			break;

		case L'＞':
		case L'》':
			c = L'>';
			break;

		case L'′':
		case L'＇':
		case L'´':
		case L'’':
			c = L'\'';
			break;
		}
	}
}

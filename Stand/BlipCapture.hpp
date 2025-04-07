#pragma once

#include <soup/Canvas.hpp>
#include <soup/FileReader.hpp>
#include <soup/FileWriter.hpp>
#include <soup/RgbaCanvas.hpp>

namespace Stand
{
	struct BlipCapture
	{
		static void doTheThing() // TC_SCRIPT_YIELDABLE
		{
			if (!is_session_started())
			{
				TransitionHelper::doTransition(INVITE_ONLY);
				Script::current()->yield(5000);
			}

			TpUtil::teleport_exact({ -1737.8188f, 1569.41f, 215.84392f });
			PAD::SET_CONTROL_VALUE_NEXT_FRAME(0, INPUT_FRONTEND_PAUSE, 1.0f);

			auto blip = HUD::ADD_BLIP_FOR_COORD(-1952.9618f, 1393.1038f, 260.98398f);
			HUD::SET_BLIP_SCALE(blip, 2.0f);

			auto bg = HUD::ADD_BLIP_FOR_AREA(-1952.9618f, 1393.1038f, 260.98398f, 500.0f, 500.0f);
			HUD::SET_BLIP_COLOUR(bg, 84);
			HUD::SET_BLIP_SECONDARY_COLOUR(bg, 0, 255, 0);
			HUD::SET_BLIP_ROTATION(bg, 0);

			if constexpr (true)
			{
				Script::current()->yield(3000);
				for (auto i = 0; i != 900; ++i)
				{
					HUD::SET_BLIP_SPRITE(blip, i);
					Script::current()->yield(200);
					std::wstring path = fmt::format(LR"(C:\Users\Sainan\Desktop\blips\{}.bmp)", i);
					captureScreenshot(path.c_str());
					if (!HUD::IS_PAUSE_MENU_ACTIVE() || g_gui.isUnloadPending())
					{
						break;
					}
				}
			}
			else
			{
				HUD::SET_BLIP_SPRITE(blip, 6);
				Script::current()->yield(10000);
			}

			HUD::REMOVE_BLIP(&blip);
			HUD::REMOVE_BLIP(&bg);
		}

		static void makeGreenSubtractedVersions()
		{
			std::filesystem::path dir = R"(C:\Users\Sainan\Desktop\blips)";
			auto out = (dir / "green-subtracted");
			std::filesystem::create_directory(out);
			for (const auto& f : std::filesystem::directory_iterator(dir))
			{
				if (f.path().filename().string().substr(f.path().filename().string().size() - 4) == ".bmp")
				{
					soup::FileReader fr(f.path());
					auto c = soup::RgbaCanvas::fromCanvas(soup::Canvas::fromBmp(fr));
					c.subtractGreen();
					soup::FileWriter fw(out / f.path().filename());
					c.toBmp(fw);
				}
			}
		}

		// Slightly better version of this screenshot stuff in Sentinel's screenshot.hpp, or Soup's os::makeScreenshotBmp.

		static void captureScreenshot(const wchar_t* wPath)
		{
			HDC dcScreen = GetDC(0);
			HDC dcTarget = CreateCompatibleDC(dcScreen);
			HBITMAP bmpTarget = CreateCompatibleBitmap(dcScreen, 100, 100);
			HGDIOBJ oldBmp = SelectObject(dcTarget, bmpTarget);
			BitBlt(dcTarget, 0, 0, 100, 100, dcScreen, 777, 599, SRCCOPY | CAPTUREBLT);
			SelectObject(dcTarget, oldBmp);
			DeleteDC(dcTarget);
			ReleaseDC(0, dcScreen);

			SaveHBITMAPToFile(bmpTarget, wPath);
			DeleteObject(bmpTarget);
		}

		static BOOL SaveHBITMAPToFile(HBITMAP hBitmap, LPCTSTR lpszFileName)
		{
			HDC hDC;
			int iBits;
			WORD wBitCount;
			DWORD dwPaletteSize = 0, dwBmBitsSize = 0, dwDIBSize = 0, dwWritten = 0;
			BITMAP Bitmap0;
			BITMAPFILEHEADER bmfHdr;
			BITMAPINFOHEADER bi;
			LPBITMAPINFOHEADER lpbi;
			HANDLE fh, hDib, hPal, hOldPal2 = NULL;
			hDC = CreateDC(TEXT("DISPLAY"), NULL, NULL, NULL);
			iBits = GetDeviceCaps(hDC, BITSPIXEL) * GetDeviceCaps(hDC, PLANES);
			DeleteDC(hDC);
			if (iBits <= 1)
				wBitCount = 1;
			else if (iBits <= 4)
				wBitCount = 4;
			else if (iBits <= 8)
				wBitCount = 8;
			else
				wBitCount = 24;
			GetObject(hBitmap, sizeof(Bitmap0), (LPSTR)&Bitmap0);
			bi.biSize = sizeof(BITMAPINFOHEADER);
			bi.biWidth = Bitmap0.bmWidth;
			bi.biHeight = -Bitmap0.bmHeight;
			bi.biPlanes = 1;
			bi.biBitCount = wBitCount;
			bi.biCompression = BI_RGB;
			bi.biSizeImage = 0;
			bi.biXPelsPerMeter = 0;
			bi.biYPelsPerMeter = 0;
			bi.biClrImportant = 0;
			bi.biClrUsed = 256;
			dwBmBitsSize = ((Bitmap0.bmWidth * wBitCount + 31) & ~31) / 8
				* Bitmap0.bmHeight;
			hDib = GlobalAlloc(GHND, dwBmBitsSize + dwPaletteSize + sizeof(BITMAPINFOHEADER));
			lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);
			*lpbi = bi;

			hPal = GetStockObject(DEFAULT_PALETTE);
			if (hPal)
			{
				hDC = GetDC(NULL);
				hOldPal2 = SelectPalette(hDC, (HPALETTE)hPal, FALSE);
				RealizePalette(hDC);
			}


			GetDIBits(hDC, hBitmap, 0, (UINT)Bitmap0.bmHeight, (LPSTR)lpbi + sizeof(BITMAPINFOHEADER)
				+ dwPaletteSize, (BITMAPINFO*)lpbi, DIB_RGB_COLORS);

			if (hOldPal2)
			{
				SelectPalette(hDC, (HPALETTE)hOldPal2, TRUE);
				RealizePalette(hDC);
				ReleaseDC(NULL, hDC);
			}

			fh = CreateFile(lpszFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
				FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);

			if (fh == INVALID_HANDLE_VALUE)
				return FALSE;

			bmfHdr.bfType = 0x4D42; // "BM"
			dwDIBSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwPaletteSize + dwBmBitsSize;
			bmfHdr.bfSize = dwDIBSize;
			bmfHdr.bfReserved1 = 0;
			bmfHdr.bfReserved2 = 0;
			bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER) + dwPaletteSize;

			WriteFile(fh, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);

			WriteFile(fh, (LPSTR)lpbi, dwDIBSize, &dwWritten, NULL);
			GlobalUnlock(hDib);
			GlobalFree(hDib);
			CloseHandle(fh);
			return TRUE;
		}
	};
}

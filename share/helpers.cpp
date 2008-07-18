/**
 *  Helpers.cpp
 *
 *  Copyright (C) 2008  David Andrs <pda@jasnapaka.com>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <windows.h>
#include "helpers.h"
#include "defs.h"

HWND FindTaskBar() {
	HWND hTaskBar;

	// Gigabar has own window
	if ((hTaskBar = FindWindow(_T("GigaBar"), NULL)) != NULL)
		return hTaskBar;
	if ((hTaskBar = FindWindow(_T("HHTaskBar"), NULL)) != NULL)
		return hTaskBar;

	return NULL;
}

HWND FindBattiWindow() {
	HWND hTaskBar = FindTaskBar();
	if (hTaskBar != 0) {
		HWND hChild = GetWindow(hTaskBar, GW_CHILD);
		while (hChild != NULL) {
			TCHAR className[16];
			GetClassName(hChild, className, 16);
			if (wcscmp(className, WINDOW_CLASS) == 0) {
				return hChild;
			}

			hChild = GetWindow(hChild, GW_HWNDNEXT);
		}
	}

	return NULL;
}

//

COLORREF hsl2rgb(double h, double sl, double l) {
	double v;
	double r, g, b;

	r = l;   // default to gray
	g = l;
	b = l; 
	v = (l <= 0.5) ? (l * (1.0 + sl)) : (l + sl - l * sl);

	if (v > 0) {
		double m;
		double sv;
		int sextant;
		double fract, vsf, mid1, mid2;

		m = l + l - v;
		sv = (v - m ) / v;
		h *= 6.0;
		sextant = ((int) h); 
		fract = h - sextant;
		vsf = v * sv * fract;
		mid1 = m + vsf;
		mid2 = v - vsf;
		switch (sextant) {
			case 0: 
				r = v; 
				g = mid1; 
				b = m; 
				break;
			case 1: 
				r = mid2; 
				g = v; 
				b = m; 
				break;
			case 2: 
				r = m; 
				g = v; 
				b = mid1; 
				break;
			case 3: 
				r = m; 
				g = mid2; 
				b = v; 
				break;
			case 4: 
				r = mid1; 
				g = m; 
				b = v; 
				break;
			case 5: 
				r = v; 
				g = m; 
				b = mid2; 
				break;
		}
	}

	return RGB((BYTE) (r * 255.0f), (BYTE) (g * 255.0f), (BYTE) (b * 255.0f));
}

void rgb2hsl(COLORREF rgb, double &h, double &s, double &l) {
	double r = GetRValue(rgb) / 255.0;
	double g = GetGValue(rgb) / 255.0;
	double b = GetBValue(rgb) / 255.0;
	double v;
	double m;
	double vm;
	double r2, g2, b2;

	h = 0; // default to black
	s = 0;
	l = 0;
	v = max(r, g);
	v = max(v, b);
	m = min(r, g);
	m = min(m, b);
	l = (m + v) / 2.0;
	if (l <= 0.0) {
		return;
	}
	vm = v - m;
	s = vm;
	if (s > 0.0) {
		s /= (l <= 0.5) ? (v + m ) : (2.0 - v - m);
	} 
	else {
		return;
	}
	r2 = (v - r) / vm;
	g2 = (v - g) / vm;
	b2 = (v - b) / vm;
	if (r == v) {
		h = (g == m ? 5.0 + b2 : 1.0 - g2);
	}
	else if (g == v) {
		h = (b == m ? 1.0 + r2 : 3.0 - b2);
	}
	else {
		h = (r == m ? 3.0 + g2 : 5.0 - r2);
	}
	h /= 6.0;
}


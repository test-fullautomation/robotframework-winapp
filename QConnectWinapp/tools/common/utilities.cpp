/*
 * utilities.cpp
 *
 *  Created on: Jul 24, 2023
 *      Author: cuongnht
 */
#include "utilities.h"

Utilities::Utilities()
{

}

#ifdef BUILD_ON_LINUX
int Utilities::GetPosWindow(Window windowId, int&x, int&y, unsigned int &h, unsigned int &w, int &border_width, int& titlebar_height)
{
    Display* display = XOpenDisplay(NULL);
    if (display == NULL) {
            std::cerr << "Error: Unable to open X display." << std::endl;
            return 0;
    }

    XWindowAttributes wAttribute;
    Window child;
    if(XGetWindowAttributes(display, windowId, &wAttribute))
    {
        w = wAttribute.width + (2*wAttribute.x);
        h = wAttribute.height + wAttribute.x + wAttribute.y;
        border_width = wAttribute.x;
        titlebar_height = wAttribute.y;
        XTranslateCoordinates(display,windowId,wAttribute.root,0 - wAttribute.border_width,0 - wAttribute.border_width,&x,&y,&child);
        x -= wAttribute.x;
        y -= wAttribute.y;
    }
    XCloseDisplay(display);
    return 1;
}


int Utilities::drawHighlight(int x,
                         int y,
                         unsigned int rectWidth,
                         unsigned int rectHeight,
                         int borderWidth,
                         unsigned long borderColor,
                         int function)
{
    Display *display = XOpenDisplay(":0"); // Open the connection to the X server
    if (display == NULL)
    {
        fprintf(stderr, "Cannot connect to X server %s\n", ":0");
        return -1;
    }

    int screen_num = DefaultScreen(display);
    Window root_win = RootWindow(display, screen_num);

    XGCValues gc_val;
    gc_val.function           = function;
    gc_val.plane_mask         = AllPlanes;
    gc_val.foreground         = borderColor; /*WhitePixel(display, screen_num);*/
    gc_val.background         = BlackPixel(display, screen_num);
    gc_val.line_width         = borderWidth;
    gc_val.line_style         = LineSolid;
    gc_val.cap_style          = CapButt;
    gc_val.join_style         = JoinMiter;
    gc_val.fill_style         = FillOpaqueStippled;
    gc_val.fill_rule          = WindingRule;
    gc_val.graphics_exposures = False;
    gc_val.clip_x_origin      = 0;
    gc_val.clip_y_origin      = 0;
    gc_val.clip_mask          = None;
    gc_val.subwindow_mode     = IncludeInferiors;

    GC gc_line = XCreateGC(display, root_win, GCFunction | GCPlaneMask | GCForeground | GCBackground | GCLineWidth | GCLineStyle | GCCapStyle | GCJoinStyle | GCFillStyle | GCFillRule | GCGraphicsExposures | GCClipXOrigin | GCClipYOrigin | GCClipMask | GCSubwindowMode, &gc_val);

    XSetForeground(display, gc_line, borderColor);
    XDrawRectangle(display, root_win, gc_line, x, y, rectWidth, rectHeight);

    XFreeGC(display, gc_line);
    XFlush(display);
    XCloseDisplay(display);

    return 0;
}
#else

int Utilities::HighlightWindow( HWND hwnd, RECT rect)
{
    int iRes = -1;
    HDC hdc = GetWindowDC(hwnd);
    if (hdc) {
        // Draw top border
        HPEN hPen = CreatePen(PS_SOLID, 3, RGB(255, 255, 0)); // Yellow, thicker pen
        HGDIOBJ hOldPen = SelectObject(hdc, hPen);

        // Draw top border
        MoveToEx(hdc, rect.left, rect.top, NULL);
        LineTo(hdc, rect.right, rect.top);

        // Draw right border
        MoveToEx(hdc, rect.right, rect.top, NULL);
        LineTo(hdc, rect.right, rect.bottom);

        // Draw bottom border
        MoveToEx(hdc, rect.right, rect.bottom, NULL);
        LineTo(hdc, rect.left, rect.bottom);

        // Draw left border
        MoveToEx(hdc, rect.left, rect.bottom, NULL);
        LineTo(hdc, rect.left, rect.top);

        SelectObject(hdc, hOldPen);
        DeleteObject(hPen);

        ReleaseDC(hwnd, hdc);
        iRes = 0;
    }
    return iRes;
}

///
/// \brief Utilities::ClearRectangles
/// \param hwnd
///
void Utilities::ClearRectangles(HWND hwnd) {
    RECT rect = { 0 };
    InvalidateRect(hwnd, &rect, TRUE);
}
#endif

///
/// \brief Utilities::demangleTypeName
/// \param mangledName
/// \return
///
std::string Utilities::demangleTypeName(const std::string& mangledName) {
    std::string::size_type begin = mangledName.find_first_not_of("0123456789");
    std::string::size_type end = mangledName.find_last_of('@');

    if (begin != std::string::npos && end != std::string::npos && begin < end) {
        return mangledName.substr(begin, end - begin);
    } else {
        return mangledName;
    }
}

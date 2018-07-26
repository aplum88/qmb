#include "QMiniBlink.h"
#include "ui_QMiniBlink.h"
#include <QDebug>
static void HandlePaintUpdatedCallback(wkeWebView webView, void* param, const HDC hdc, int x, int y, int cx, int cy) {
    HWND hWnd = (HWND)param;
    BOOL callOk = FALSE;
    if (WS_EX_LAYERED == (WS_EX_LAYERED & GetWindowLong(hWnd, GWL_EXSTYLE))) {
        RECT rectDest;
        ::GetWindowRect(hWnd, &rectDest);

        SIZE sizeDest = { rectDest.right - rectDest.left, rectDest.bottom - rectDest.top };
        POINT pointDest = { 0, 0 }; // { rectDest.left, rectDest.top };
        POINT pointSource = { 0, 0 };

        BITMAP bmp = { 0 };
        HBITMAP hBmp = (HBITMAP)::GetCurrentObject(hdc, OBJ_BITMAP);
        ::GetObject(hBmp, sizeof(BITMAP), (LPSTR)&bmp);

        sizeDest.cx = bmp.bmWidth;
        sizeDest.cy = bmp.bmHeight;

        HDC hdcScreen = GetDC(hWnd);

        BLENDFUNCTION blend = { 0 };
        blend.BlendOp = AC_SRC_OVER;
        blend.SourceConstantAlpha = 255;
        blend.AlphaFormat = AC_SRC_ALPHA;
        callOk = ::UpdateLayeredWindow(hWnd, hdcScreen, nullptr, &sizeDest, hdc, &pointSource, RGB(0xFF, 0xFF, 0xFF), &blend, ULW_ALPHA);
        if (!callOk) {
            HDC hdcMemory = ::CreateCompatibleDC(hdcScreen);
            HBITMAP hbmpMemory = ::CreateCompatibleBitmap(hdcScreen, sizeDest.cx, sizeDest.cy);
            HBITMAP hbmpOld = (HBITMAP)::SelectObject(hdcMemory, hbmpMemory);

            ::BitBlt(hdcMemory, 0, 0, sizeDest.cx, sizeDest.cy, hdc, 0, 0, SRCCOPY | CAPTUREBLT);

            ::BitBlt(hdc, 0, 0, sizeDest.cx, sizeDest.cy, hdcMemory, 0, 0, SRCCOPY | CAPTUREBLT); //!

            callOk = ::UpdateLayeredWindow(hWnd, hdcScreen, nullptr, &sizeDest, hdcMemory, &pointSource, RGB(0xFF, 0xFF, 0xFF), &blend, ULW_ALPHA);

            ::SelectObject(hdcMemory, (HGDIOBJ)hbmpOld);
            ::DeleteObject((HGDIOBJ)hbmpMemory);
            ::DeleteDC(hdcMemory);
        }

        ::ReleaseDC(hWnd, hdcScreen);
    } else {
        RECT rc = { x, y, x + cx, y + cy };
        ::InvalidateRect(hWnd, &rc, FALSE);
    }
}
QMiniBlink::QMiniBlink(QWidget *parent) :
    QDialog(parent)
{
    webView = wkeCreateWebView();
    ::wkeSetHandle(webView, (HWND)this->winId());
    ::wkeOnPaintUpdated(webView, HandlePaintUpdatedCallback, (HWND)this->winId());
    //::wkeOnDocumentReady(view, HandleDocumentReady, view);
    //::wkeOnLoadingFinish(webView, HandleLoadingFinish, webView);
    //::wkeOnCreateView(webView, HandleCreateView, webView);
    //::wkeOnWindowClosing(webView, HandleWindowClosing, nullptr);

    RECT rc = { 0 };
    ::GetClientRect((HWND)this->winId(), &rc);
    ::wkeResize(webView, rc.right, rc.bottom);
    wkeMoveToCenter(webView);
    wkeShowWindow(webView, TRUE);
    QApplication::instance()->installNativeEventFilter(this);


}

QMiniBlink::~QMiniBlink()
{
    wkeDestroyWebView(webView);
}

void QMiniBlink::LoadUrl(QString url)
{
    ::wkeLoadURL(webView, url.toStdString().c_str());
}

void QMiniBlink::LoadFile(QString file)
{

}

void QMiniBlink::keyPressEvent(QKeyEvent *evt)
{
   int charCode = evt->nativeVirtualKey();
   int flags = 0;
    qDebug() << "charCode=" << charCode;
    if(evt->isAutoRepeat())
        flags |= WKE_REPEAT;
    if(evt->modifiers())
        flags |= WKE_EXTENDED;

   wkeFireKeyPressEvent(webView, charCode, flags, false);
}

bool QMiniBlink::nativeEventFilter(const QByteArray &eventType, void *message, long *result)
{

    //if(eventType == "windows_generic_MSG" || eventType == "windows_dispatcher_MSG")
    {
        HWND hWnd = (HWND)this->winId();
        MSG* pMsg = reinterpret_cast<MSG*>(message);
        WPARAM wParam = pMsg->wParam;
        LPARAM lParam = pMsg->lParam;
        UINT Msg = pMsg->message;
      switch(pMsg->message)
      {

          case WM_NCDESTROY:
             if (::GetProp(hWnd, L"subView")) {
                 RemoveProp(hWnd, L"subView");
             } else {
                 wkeDestroyWebView(webView);
                 PostQuitMessage(0);
             }
             return 0;
          case WM_ERASEBKGND:
             return TRUE;
        case WM_PAINT:
          {
              if (WS_EX_LAYERED == (WS_EX_LAYERED & GetWindowLong(hWnd, GWL_EXSTYLE))) break;
              wkeRepaintIfNeeded(webView);

              PAINTSTRUCT ps = { 0 };
              HDC hdc = ::BeginPaint(hWnd, &ps);

              RECT rcClip = ps.rcPaint;

              RECT rcClient;
              ::GetClientRect(hWnd, &rcClient);

              RECT rcInvalid = rcClient;
              if (rcClip.right != rcClip.left && rcClip.bottom != rcClip.top)
                  ::IntersectRect(&rcInvalid, &rcClip, &rcClient);

              int srcX = rcInvalid.left - rcClient.left;
              int srcY = rcInvalid.top - rcClient.top;
              int destX = rcInvalid.left;
              int destY = rcInvalid.top;
              int width = rcInvalid.right - rcInvalid.left;
              int height = rcInvalid.bottom - rcInvalid.top;

              if (0 != width && 0 != height)
                  ::BitBlt(hdc, destX, destY, width, height, wkeGetViewDC(webView), srcX, srcY, SRCCOPY);

              ::EndPaint(hWnd, &ps);
              return true;
              break;
          }
      case WM_SIZE:
         {
             RECT rc = { 0 };
             ::GetClientRect(hWnd, &rc);
             int width = rc.right - rc.left;
             int height = rc.bottom - rc.top;

             ::wkeResize(webView, width, height);
             wkeRepaintIfNeeded(webView);
             ::wkeWake(webView);

             return 0;
         }
         case WM_KEYDOWN:
         {
             unsigned int virtualKeyCode = wParam;
             unsigned int flags = 0;
             if (HIWORD(lParam) & KF_REPEAT)
                 flags |= WKE_REPEAT;
             if (HIWORD(lParam) & KF_EXTENDED)
                 flags |= WKE_EXTENDED;

             if (wkeFireKeyDownEvent(webView, virtualKeyCode, flags, false))
                 return 0;
             break;
         }
      case WM_KEYUP:
         {
             unsigned int virtualKeyCode = wParam;
             unsigned int flags = 0;
             if (HIWORD(lParam) & KF_REPEAT)
                 flags |= WKE_REPEAT;
             if (HIWORD(lParam) & KF_EXTENDED)
                 flags |= WKE_EXTENDED;

             if (wkeFireKeyUpEvent(webView, virtualKeyCode, flags, false))
                 return 0;
             break;
         }
         case WM_CHAR:
         {
             unsigned int charCode = wParam;
             unsigned int flags = 0;
             if (HIWORD(lParam) & KF_REPEAT)
                 flags |= WKE_REPEAT;
             if (HIWORD(lParam) & KF_EXTENDED)
                 flags |= WKE_EXTENDED;

             if (wkeFireKeyPressEvent(webView, charCode, flags, false))
                 return 0;
             break;
         }
         case WM_LBUTTONDOWN:
         case WM_MBUTTONDOWN:
         case WM_RBUTTONDOWN:
         case WM_LBUTTONDBLCLK:
         case WM_MBUTTONDBLCLK:
         case WM_RBUTTONDBLCLK:
         case WM_LBUTTONUP:
         case WM_MBUTTONUP:
         case WM_RBUTTONUP:
         case WM_MOUSEMOVE:
         {
             if (Msg == WM_LBUTTONDOWN || Msg == WM_MBUTTONDOWN || Msg == WM_RBUTTONDOWN) {
                 if (::GetFocus() != hWnd)
                     ::SetFocus(hWnd);
                 ::SetCapture(hWnd);
             } else if (Msg == WM_LBUTTONUP || Msg == WM_MBUTTONUP || Msg == WM_RBUTTONUP) {
                 ReleaseCapture();
             }

             int x = LOWORD(lParam);
             int y = HIWORD(lParam);

             unsigned int flags = 0;

             if (wParam & MK_CONTROL)
                 flags |= WKE_CONTROL;
             if (wParam & MK_SHIFT)
                 flags |= WKE_SHIFT;

             if (wParam & MK_LBUTTON)
                 flags |= WKE_LBUTTON;
             if (wParam & MK_MBUTTON)
                 flags |= WKE_MBUTTON;
             if (wParam & MK_RBUTTON)
                 flags |= WKE_RBUTTON;

             if (wkeFireMouseEvent(webView, Msg, x, y, flags))
                 return 0;
             break;
         }
         case WM_CONTEXTMENU:
         {
             POINT pt;
             pt.x = LOWORD(lParam);
             pt.y = HIWORD(lParam);

             if (pt.x != -1 && pt.y != -1)
                 ScreenToClient(hWnd, &pt);

             unsigned int flags = 0;

             if (wParam & MK_CONTROL)
                 flags |= WKE_CONTROL;
             if (wParam & MK_SHIFT)
                 flags |= WKE_SHIFT;

             if (wParam & MK_LBUTTON)
                 flags |= WKE_LBUTTON;
             if (wParam & MK_MBUTTON)
                 flags |= WKE_MBUTTON;
             if (wParam & MK_RBUTTON)
                 flags |= WKE_RBUTTON;

             if (wkeFireContextMenuEvent(webView, pt.x, pt.y, flags))
                 return 0;
             break;
         }
         case WM_MOUSEWHEEL:
         {
             POINT pt;
             pt.x = LOWORD(lParam);
             pt.y = HIWORD(lParam);
             ::ScreenToClient(hWnd, &pt);

             int delta = GET_WHEEL_DELTA_WPARAM(wParam);

             unsigned int flags = 0;

             if (wParam & MK_CONTROL)
                 flags |= WKE_CONTROL;
             if (wParam & MK_SHIFT)
                 flags |= WKE_SHIFT;

             if (wParam & MK_LBUTTON)
                 flags |= WKE_LBUTTON;
             if (wParam & MK_MBUTTON)
                 flags |= WKE_MBUTTON;
             if (wParam & MK_RBUTTON)
                 flags |= WKE_RBUTTON;

             if (wkeFireMouseWheelEvent(webView, pt.x, pt.y, delta, flags))
                 return 0;
             break;
         }
         case WM_SETFOCUS:
             wkeSetFocus(webView);
             return 0;

         case WM_KILLFOCUS:
             wkeKillFocus(webView);
             return 0;

         case WM_SETCURSOR:
             if (wkeFireWindowsMessage(webView, hWnd, WM_SETCURSOR, 0, 0, nullptr))
                 return 0;
             break;

         case WM_IME_STARTCOMPOSITION:
         {
             wkeRect caret = wkeGetCaretRect(webView);

             COMPOSITIONFORM COMPOSITIONFORM;
             COMPOSITIONFORM.dwStyle = CFS_POINT | CFS_FORCE_POSITION;
             COMPOSITIONFORM.ptCurrentPos.x = caret.x;
             COMPOSITIONFORM.ptCurrentPos.y = caret.y;

             HIMC hIMC = ::ImmGetContext(hWnd);
             ::ImmSetCompositionWindow(hIMC, &COMPOSITIONFORM);
             ::ImmReleaseContext(hWnd, hIMC);
             return 0;
         }

      }
      if (pMsg->message != WM_NCHITTEST )
      {

          return QDialog::nativeEvent("", message, result);
      }
    }

    return false;
}

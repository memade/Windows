#if !defined(AFX_SKSTU_duilib_H__1EC09_74DB4F27_907BA6202460A68_570A__HEAD__)
#define AFX_SKSTU_duilib_H__1EC09_74DB4F27_907BA6202460A68_570A__HEAD__

#define UILIB_STATIC

#if defined _M_IX86
#if defined(_DEBUG)
#pragma comment(lib,"libdui.lib")
#else
#pragma comment(lib,"libdui.lib")
#endif
#elif defined _M_X64
#if defined(_DEBUG)
#pragma comment(lib,"libdui.lib")
#else
#pragma comment(lib,"libdui.lib")
#endif
#else
#endif

#include "duilib/UIlib.h"

using namespace DuiLib;

namespace shared {
	namespace ui {

		static const UINT m_NOTIFYICONDATA_MESSAGE = WM_USER + 10001;

		static CControlUI* FindControlParent(CControlUI* child,const std::wstring& parent_name) {
			CControlUI* result = nullptr;
			CControlUI* parent = child;
			do {
				if (!parent)
					break;
				if (parent_name.empty())
					break;
				if (parent->GetName() == parent_name.c_str()) {
					result = parent;
					break;
				}
				parent = parent->GetParent();
			} while (1);
			return result;
		}

		class UIFrame : public WindowImplBase {
		public:
			UIFrame() {
			}
			virtual ~UIFrame() {
			}
		protected:
			NOTIFYICONDATA m_NOTIFYICONDATA = { 0 };
		protected:
			virtual UINT GetClassStyle() const { return /*UI_CLASSSTYLE_FRAME |*/ CS_DBLCLKS; };
			virtual void OnFinalMessage(HWND hWnd) { delete this; }
		protected:
			virtual void InitWindow() {};
			virtual void Notify(TNotifyUI& msg) = 0;
			virtual LPCTSTR GetWindowClassName() const = 0;
			virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) = 0;
		protected:
			virtual LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
				bHandled = FALSE;
				return 0;
			}
			virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
				bHandled = FALSE;
				return 0;
			}
			virtual CControlUI* CreateControl(LPCTSTR pstrClassName) {
				return nullptr;
			}
			virtual LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
				bHandled = FALSE;
				return 0;
			}
			virtual LRESULT OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
				bHandled = FALSE;
				return 0;
			}
			virtual LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
				return WindowImplBase::OnClose(uMsg, wParam, lParam, bHandled);
			}
			virtual LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
				::PostQuitMessage(0L);
				bHandled = FALSE;
				return 0;
			}
			virtual LRESULT OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
				if (::IsIconic(*this)) bHandled = FALSE;
				return (wParam == 0) ? TRUE : FALSE;
			}
			virtual LRESULT OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
				bHandled = FALSE;
				return 0;
			}
			virtual LRESULT OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
				bHandled = FALSE;
				return 0;
			}
			virtual LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
				if (wParam == SC_CLOSE)
				{
					::PostQuitMessage(0L);
					bHandled = TRUE;
					return 0;
				}

				BOOL bZoomed = ::IsZoomed(*this);
				LRESULT lRes = CWindowWnd::HandleMessage(uMsg, wParam, lParam);
				if (::IsZoomed(*this) != bZoomed) {
					if (!bZoomed) {
						CControlUI* pControl = static_cast<CControlUI*>(m_pm.FindControl(_T("maxbtn")));
						if (pControl) pControl->SetVisible(false);
						pControl = static_cast<CControlUI*>(m_pm.FindControl(_T("restorebtn")));
						if (pControl) pControl->SetVisible(true);
					}
					else {
						CControlUI* pControl = static_cast<CControlUI*>(m_pm.FindControl(_T("maxbtn")));
						if (pControl) pControl->SetVisible(true);
						pControl = static_cast<CControlUI*>(m_pm.FindControl(_T("restorebtn")));
						if (pControl) pControl->SetVisible(false);
					}
				}
				return lRes;
			}
			virtual LRESULT OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
				POINT pt; pt.x = GET_X_LPARAM(lParam); pt.y = GET_Y_LPARAM(lParam);
				::ScreenToClient(*this, &pt);
				RECT rcClient;
				::GetClientRect(*this, &rcClient);
				if (!::IsZoomed(*this)) {
					RECT rcSizeBox = m_pm.GetSizeBox();
					if (pt.y < rcClient.top + rcSizeBox.top) {
						if (pt.x < rcClient.left + rcSizeBox.left) return HTTOPLEFT;
						if (pt.x > rcClient.right - rcSizeBox.right) return HTTOPRIGHT;
						return HTTOP;
					}
					else if (pt.y > rcClient.bottom - rcSizeBox.bottom) {
						if (pt.x < rcClient.left + rcSizeBox.left) return HTBOTTOMLEFT;
						if (pt.x > rcClient.right - rcSizeBox.right) return HTBOTTOMRIGHT;
						return HTBOTTOM;
					}
					if (pt.x < rcClient.left + rcSizeBox.left) return HTLEFT;
					if (pt.x > rcClient.right - rcSizeBox.right) return HTRIGHT;
				}
				RECT rcCaption = m_pm.GetCaptionRect();
				if (pt.x >= rcClient.left + rcCaption.left && pt.x < rcClient.right - rcCaption.right \
					&& pt.y >= rcCaption.top && pt.y < rcCaption.bottom) {
					CControlUI* pControl = static_cast<CControlUI*>(m_pm.FindControl(pt));
					if (pControl && _tcscmp(pControl->GetClass(), _T("ButtonUI")) != 0 &&
						_tcscmp(pControl->GetClass(), _T("OptionUI")) != 0 &&
						_tcscmp(pControl->GetClass(), _T("TextUI")) != 0)
						return HTCAPTION;
				}
				return HTCLIENT;
			}
			virtual LRESULT OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
				bHandled = TRUE;
				return 0;
			}
			virtual LRESULT OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
				bHandled = TRUE;
				return 0;
			}
			virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
				LRESULT lRes = 0;
				BOOL bHandled = TRUE;
				switch (uMsg) {
				case WM_CREATE:
				{
					lRes = OnCreate(uMsg, wParam, lParam, bHandled);
					/*InitWindow();*/
				}break;
				case WM_CHAR: {
					lRes = OnChar(uMsg, wParam, lParam, bHandled);
				}break;
				case WM_CLOSE: {
					lRes = OnClose(uMsg, wParam, lParam, bHandled);
				}break;
				case WM_PAINT: {
					lRes = OnPaint(uMsg, wParam, lParam, bHandled);
				}break;
				case WM_DESTROY: {
					lRes = OnDestroy(uMsg, wParam, lParam, bHandled);
				}break;
				case WM_SIZE: {
					lRes = OnSize(uMsg, wParam, lParam, bHandled);
				}break;
				case WM_GETMINMAXINFO: {
					lRes = OnGetMinMaxInfo(uMsg, wParam, lParam, bHandled);
				}break;
				case WM_SYSCOMMAND: {
					lRes = OnSysCommand(uMsg, wParam, lParam, bHandled);
				}break;
				case WM_NCHITTEST: {
					lRes = OnNcHitTest(uMsg, wParam, lParam, bHandled);
				}break;
				case WM_NCCALCSIZE: {
					lRes = OnNcCalcSize(uMsg, wParam, lParam, bHandled);
				} break;
				case WM_COMMAND: {
					lRes = OnCommand(uMsg, wParam, lParam, bHandled);
				}break;
				case WM_NCPAINT: {
					lRes = OnNcPaint(uMsg, wParam, lParam, bHandled);
				}break;
				case WM_NCLBUTTONDOWN: {
					lRes = WindowImplBase::HandleMessage(uMsg, wParam, lParam);
				}break;
				default: {
					bHandled = FALSE;
				}///default
				}///switch
				if (bHandled) {
					return lRes;
				}

				lRes = HandleCustomMessage(uMsg, wParam, lParam, bHandled);
				if (bHandled) return lRes;

				if (m_pm.MessageHandler(uMsg, wParam, lParam, lRes)) {
					return lRes;
				}
				return WindowImplBase::HandleMessage(uMsg, wParam, lParam);
			}
		/*protected:
			CPaintManagerUI m_PaintManager;*/
		};///class UIFrame

		class UIWindow final {
		public:
			__inline static LPCTSTR ClassName() { return _T("UIWindow"); }
			UIWindow(HINSTANCE _hInst, HWND _hParent = nullptr) {
				m_hParent = _hParent;
				m_hInstance = _hInst;
			}
			~UIWindow() {
				Destroy();
			}
			template<typename TypeUIWindow>
			int Create() {
				if (m_IsCreate.load()) {
					return 0;
				}
				m_Threads.emplace_back(
					[&]()->int {
						if (!::HeapSetInformation(nullptr, HeapEnableTerminationOnCorruption, nullptr, 0)) { return -1; }
						::OleInitialize(NULL);
						if (FAILED(::CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE))) { return -1; }
						{/// Create Duilib window main begin
							CPaintManagerUI::SetInstance(m_hInstance);
							CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath() + _T("skin"));
							auto pUIWindow = new TypeUIWindow();
							m_UIWindowPtr = reinterpret_cast<std::intptr_t>(pUIWindow);
							if (m_IsShow.load()) {
								m_hWnd = pUIWindow->Create(NULL, _T("新生联创"), UI_WNDSTYLE_FRAME, 0L);
							}
							else {
								m_hWnd = pUIWindow->Create(NULL, _T("新生联创"), WS_OVERLAPPEDWINDOW, 0L);
							}
							if (m_hWnd) {
								pUIWindow->CenterWindow();
								//pUIWindow->ShowModal();
								CPaintManagerUI::MessageLoop();
								auto sk = 0;
							}
							else {
								return -1;
							}
						}/// Create Duilib window main end
						::CoUninitialize();
						::OleUninitialize();
						return 0;
					});
				m_IsCreate.store(true);
				return 0;
			}

			template<typename TypeUIWindow>
			int Destroy() {
				if (!m_IsCreate.load()) {
					return 0;
				}
				auto pUIWindow = reinterpret_cast<TypeUIWindow*>(m_UIWindowPtr);
				if (pUIWindow) {
					pUIWindow->Close();
				}
				for (auto it = m_Threads.begin(); it != m_Threads.end(); ++it) {
					it->join();
				}
				m_IsCreate.store(false);
				return 0;
			}
			int Destroy() {
				if (!m_IsCreate.load()) {
					return 0;
				}
				auto pUIFrame = reinterpret_cast<UIFrame*>(m_UIWindowPtr);
				if (pUIFrame) {
					pUIFrame->Close();
				}

				for (auto it = m_Threads.begin(); it != m_Threads.end(); ++it) {
					it->join();
				}
				m_IsCreate.store(false);
				return 0;
			}
			void Show(bool _IsShow) const {
				if (m_UIWindowPtr) {
					auto pFrame = reinterpret_cast<UIFrame*>(m_UIWindowPtr);
					pFrame->ShowWindow(_IsShow);
				}
			}
			void TopMost() const {
				if (m_UIWindowPtr) {
					auto pFrame = reinterpret_cast<UIFrame*>(m_UIWindowPtr);
					::SetWindowPos(pFrame->GetHWND(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
				}
			}
			void Top() const {
				if (m_UIWindowPtr) {
					auto pFrame = reinterpret_cast<UIFrame*>(m_UIWindowPtr);
					::SetWindowPos(pFrame->GetHWND(), HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
				}
			}
			void EnableWindow(bool IsEnable = true) const {
				if (m_UIWindowPtr) {
					auto pFrame = reinterpret_cast<UIFrame*>(m_UIWindowPtr);
					::EnableWindow(pFrame->GetHWND(), IsEnable);
				}
			}
		public:
			__inline HWND GetHwnd() const { return m_hWnd; }
			__inline HWND GetParent() const { return m_hParent; }
			__inline VOID SetParent(HWND _hParent) { m_hParent = _hParent; }
			//__inline auto GetWindowPtr() const { return m_pUIWindow; }
			__inline VOID SetShowTag(bool _IsShow) { m_IsShow = _IsShow; }
		private:
			std::atomic<bool> m_IsShow = true;
			std::atomic<bool> m_IsCreate = false;
			std::vector<std::thread> m_Threads;
			HWND m_hWnd = nullptr;
			HWND m_hParent = nullptr;
			HINSTANCE m_hInstance = nullptr;
			std::intptr_t m_UIWindowPtr = 0;
		};

		class UIWindowDispatch final {
		public:
			__inline static LPCTSTR ClassName() { return _T("UIWindowDispatch"); }
			UIWindowDispatch(HINSTANCE* hInst) :
				m_hInstance(hInst) {

			}
			~UIWindowDispatch() {
				m_UIWindowPtrQ.iterate_clear(
					[](const auto& _pClassName, auto& _pUIWindow, auto&, auto&) {
						SK_DELETE_PTR(_pUIWindow);
					});
				m_UIWindowPtrQ.clear();
			}

			template<typename TypeWindow>
			UIWindow* Create(bool Show = true) {
				UIWindow* pWnd = nullptr;
				if (!m_UIWindowPtrQ.search(
					TypeWindow::ClassName(),
					[&pWnd](const auto& _pClassName, auto& _pUIWindow) {
						_pUIWindow->Show(true);
						pWnd = _pUIWindow;
					})) {
					pWnd = new UIWindow(*m_hInstance);
					pWnd->SetShowTag(Show);
					if (!pWnd->Create<TypeWindow>()) {
						m_UIWindowPtrQ.push(TypeWindow::ClassName(), pWnd);
					}
				}
				return pWnd;
			}

			template<typename TypeWindow>
			HWND Search() {
				HWND target = nullptr;
				m_UIWindowPtrQ.search(
					TypeWindow::ClassName(),
					[&target](const auto& _pClassName, auto& _pUIWindow) {
						target = _pUIWindow->GetHwnd();
					});
				return target;
			}

			template<typename TypeWindow>
			int Destroy() {
				int result = -1;
				m_UIWindowPtrQ.pop(
					TypeWindow::ClassName(),
					[&result](const auto& _pClassName, auto& _pUIWindow) {
						SK_DELETE_PTR(_pUIWindow);
						result = 0;
					});
				return result;
			}
			__inline void SetParent(HWND hParent) { m_hParent = hParent; }
		private:
			int Destroy(LPCTSTR lpClassName) {
				int result = -1;
				//m_UIWindowPtrQ.pop(
				//	lpClassName,
				//	[&result](const auto& _pClassName, auto& _pUIWindow) {
				//		SK_DELETE_PTR(_pUIWindow);
				//		result = 0;
				//	});
				return result;
			}
		private:
			HINSTANCE* m_hInstance = nullptr;
			HWND m_hParent = nullptr;
			shared::container::map<std::string, UIWindow*> m_UIWindowPtrQ;
		};




	}///namespace ui
}///namespace sk



/// /*新生联创（上海）**/
/// /*2019年05月24日 12:59:02|469**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///AFX_SKSTU_duilib_H__1EC09_74DB4F27_907BA6202460A68_570A__TAIL__
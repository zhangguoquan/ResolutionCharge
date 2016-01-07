
// ResolutionChargeDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ResolutionCharge.h"
#include "ResolutionChargeDlg.h"
#include "afxdialogex.h"

#include <Windows.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


/***********************************************
**
**
**    global parameter define
**    2015.12.28 by sheldon
**
************************************************/

LARGE_INTEGER freqence,previous,current;  //系统的时间频率 
HINSTANCE instNtDll;

typedef LONG (_stdcall *FakeNtSetTimerResolution)(\
								IN ULONG	DesiredTime,\
								IN BOOLEAN	SetResolution,\
								OUT PULONG	ActualTime);
typedef LONG (_stdcall *FakeNtQueryTimerResolution)(\
								OUT PULONG MinimunResolution,\
								OUT PULONG MaximumResolution,\
								OUT PULONG ActualResolution);

FakeNtSetTimerResolution OwnNtSetTimerResolution;
FakeNtQueryTimerResolution OwnNtQueryTimerResolution;
ULONG MinResolution, MaxResolution,ActResolution;

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CResolutionChargeDlg 对话框




CResolutionChargeDlg::CResolutionChargeDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CResolutionChargeDlg::IDD, pParent)
	, cur_Precision(0)
	, min_Precision(0)
	, max_Precision(0)
	, m_DesiredProcision(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CResolutionChargeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_CURRINFO, cur_Precision);
	DDX_Text(pDX, IDC_MIN_PRESION, min_Precision);
	DDX_Text(pDX, IDC_MAX_PRESION, max_Precision);
	DDX_Text(pDX, IDC_PRESION_INPUT, m_DesiredProcision);
	DDX_Control(pDX, IDC_PRESION_ADJUST, m_Spin);
}

BEGIN_MESSAGE_MAP(CResolutionChargeDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(UDN_DELTAPOS, IDC_PRESION_ADJUST, &CResolutionChargeDlg::OnDeltaposPresionAdjust)
	ON_BN_CLICKED(IDOK, &CResolutionChargeDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CResolutionChargeDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BTTN_SET, &CResolutionChargeDlg::OnBnClickedBttnSet)
	ON_BN_CLICKED(IDC_BTTN_UNSET, &CResolutionChargeDlg::OnBnClickedBttnUnset)
END_MESSAGE_MAP()


// CResolutionChargeDlg 消息处理程序

BOOL CResolutionChargeDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	

	instNtDll = LoadLibrary(L"NTDLL.dll");

	if (instNtDll == NULL)
	{
		MessageBox(L"时钟模块加载错误");
		return FALSE;
	}
	OwnNtQueryTimerResolution = (FakeNtQueryTimerResolution)GetProcAddress(instNtDll,"NtQueryTimerResolution");

	if (OwnNtQueryTimerResolution == NULL)
	{
		MessageBox(L"不能查询当前时钟精度");
		return FALSE;
	}
	OwnNtQueryTimerResolution(&MinResolution,&MaxResolution,&ActResolution);

	cur_Precision = ActResolution/10000.0;
	min_Precision = MinResolution/10000.0;
	max_Precision = MaxResolution/10000.0;

	m_DesiredProcision = 1.0f;

	UpdateData(false);


	OwnNtSetTimerResolution = (FakeNtSetTimerResolution)GetProcAddress(instNtDll,"NtSetTimerResolution");
	if (OwnNtSetTimerResolution == NULL)
	{
		MessageBox(L"不能设定当前时钟精度");
		return FALSE;
	}


	m_Spin.SetBuddy(GetDlgItem(IDC_PRESION_INPUT));
	m_Spin.SetRange(0,20);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CResolutionChargeDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CResolutionChargeDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CResolutionChargeDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CResolutionChargeDlg::OnDeltaposPresionAdjust(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);

	if (pNMUpDown->iDelta == -1)
	{
		
			m_DesiredProcision -= 0.5f;
		if (m_DesiredProcision < 0.5f)
		{
			m_DesiredProcision = 0.5f;
		}
	}
	if (pNMUpDown->iDelta == 1)
	{
		
			m_DesiredProcision += 0.5f;
			if (m_DesiredProcision > 15.6f)
			{
				m_DesiredProcision = 15.6;
			}
		
	}


	UpdateData(false);

	*pResult = 0;
}


void CResolutionChargeDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码

	
	
	CDialogEx::OnOK();
}


void CResolutionChargeDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	

	CDialogEx::OnCancel();
}


void CResolutionChargeDlg::OnBnClickedBttnSet()
{
	// TODO: 在此添加控件通知处理程序代码
		ULONG temp;
	ULONG todellinen = 0;

	UpdateData(true);

	temp = (ULONG) (m_DesiredProcision * 10000);
	OwnNtSetTimerResolution(temp,TRUE,&todellinen);
}


void CResolutionChargeDlg::OnBnClickedBttnUnset()
{
	// TODO: 在此添加控件通知处理程序代码
	
	ULONG todellinen = 0;

	OwnNtSetTimerResolution(MaxResolution,TRUE,&todellinen);
}

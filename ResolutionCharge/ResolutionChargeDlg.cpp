
// ResolutionChargeDlg.cpp : ʵ���ļ�
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

LARGE_INTEGER freqence,previous,current;  //ϵͳ��ʱ��Ƶ�� 
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

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CResolutionChargeDlg �Ի���




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


// CResolutionChargeDlg ��Ϣ�������

BOOL CResolutionChargeDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	

	instNtDll = LoadLibrary(L"NTDLL.dll");

	if (instNtDll == NULL)
	{
		MessageBox(L"ʱ��ģ����ش���");
		return FALSE;
	}
	OwnNtQueryTimerResolution = (FakeNtQueryTimerResolution)GetProcAddress(instNtDll,"NtQueryTimerResolution");

	if (OwnNtQueryTimerResolution == NULL)
	{
		MessageBox(L"���ܲ�ѯ��ǰʱ�Ӿ���");
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
		MessageBox(L"�����趨��ǰʱ�Ӿ���");
		return FALSE;
	}


	m_Spin.SetBuddy(GetDlgItem(IDC_PRESION_INPUT));
	m_Spin.SetRange(0,20);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CResolutionChargeDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CResolutionChargeDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CResolutionChargeDlg::OnDeltaposPresionAdjust(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	
	
	CDialogEx::OnOK();
}


void CResolutionChargeDlg::OnBnClickedCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	

	CDialogEx::OnCancel();
}


void CResolutionChargeDlg::OnBnClickedBttnSet()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
		ULONG temp;
	ULONG todellinen = 0;

	UpdateData(true);

	temp = (ULONG) (m_DesiredProcision * 10000);
	OwnNtSetTimerResolution(temp,TRUE,&todellinen);
}


void CResolutionChargeDlg::OnBnClickedBttnUnset()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	
	ULONG todellinen = 0;

	OwnNtSetTimerResolution(MaxResolution,TRUE,&todellinen);
}

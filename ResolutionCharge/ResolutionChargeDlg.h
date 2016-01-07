
// ResolutionChargeDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"


// CResolutionChargeDlg �Ի���
class CResolutionChargeDlg : public CDialogEx
{
// ����
public:
	CResolutionChargeDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_RESOLUTIONCHARGE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	double cur_Precision;
	double min_Precision;
	double max_Precision;
	double m_DesiredProcision;
	CSpinButtonCtrl m_Spin;
	afx_msg void OnDeltaposPresionAdjust(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedBttnSet();
	afx_msg void OnBnClickedBttnUnset();
};
